#include "ccpp_dds_dcps.h"
#include "Extensions/DDSdest_bld/ccpp_MessageFacility.h"
#include "Extensions/interface/CheckStatus.h"

#include "MessageLogger/interface/MessageLogger.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <string>

using namespace DDS;
using namespace MessageFacility;

namespace po = boost::program_options;

// required by all threads
static GuardCondition_var       cmdline;
static GuardCondition_var       escape;

// boost::thread
boost::thread                   trigger;
boost::thread                   cmdHandler;
 
// Generic DDS entities
DomainParticipantFactory_var    dpf;
DomainParticipant_var           participant;
Topic_var                       MFMessageTopic;
Subscriber_var                  MFSubscriber;
DataReader_ptr                  parentReader;
  
WaitSet_var                     serverWS;
ReadCondition_var               newMsg;
StatusCondition_var             newStatus;
ConditionSeq                    guardList;

// Type-specific DDS entities
MFMessageTypeSupport_var        MFMessageTS;
MFMessageDataReader_var         reader;
MFMessageSeq_var                msgSeq = new MFMessageSeq();
SampleInfoSeq_var               infoSeq = new SampleInfoSeq();

// QoSPolicy holders
TopicQos                        reliable_topic_qos;
SubscriberQos                   sub_qos;
DataReaderQos                   message_qos;

// DDS Identifiers
DomainId_t                      domain = NULL;
ReturnCode_t                    status;

// Others
char                          * MFMessageTypeName = NULL;
bool                            bConnected = false;


bool bDisplayMsg = true;
int  PartitionNumber = 0;
int  z = 0;

// Methods
bool switchChannel( const std::string & channelName);
void cmdLineTrigger();
void cmdLineInterface();

void cmdLineTrigger()
{
  ReturnCode_t status;

  std::string input;
  getline(std::cin, input);

  status = cmdline->set_trigger_value(TRUE);
  checkStatus(status, "triggering command line interface");

  return;
}

void cmdLineInterface()
{
  std::string cmd;

  while(true)
  {
    std::cout << "> ";
    getline(std::cin, cmd);

    if(cmd == "quit" || cmd == "q")
    {
      escape -> set_trigger_value(TRUE);
      return;
    }
    else if(cmd == "resume" || cmd == "r")
    {
      std::cout << "Resumed to message monitoring mode.\n";
      bDisplayMsg = true;

      // listening for the trigger
      trigger = boost::thread(cmdLineTrigger);

      return;
    }
    else if(cmd == "stat" || cmd == "s")
    {
      std::cout << "Currently listening in partition "
                << "\"Partition" << PartitionNumber << "\".\n"
                << "Total " << z << " messages has been received.\n";
    }
    else if(cmd == "partition" || cmd == "p")
    {
      std::cout << "Please enter a partition number (0-4): ";

      getline(std::cin, cmd);
      std::istringstream ss(cmd);

      if( ss >> PartitionNumber )
      {
        std::string partition("Partition"+cmd);
        if( switchChannel(partition) )
          std::cout << "Switched to partition \"" << partition << "\"\n";
        else
          std::cout << "Failed to switched partition!\n";
      }
      else
      {
        std::cout << "Please use an integer value for the partition!\n";
      }
    }
    else if(cmd == "help" || cmd == "h")
    {
      std::cout << "MessageFacility DDS server available commands:\n"
                << "  (h)elp          display this help message\n"
                << "  (s)tat          summary of received messages\n"
                << "  (r)esume        resume to message listening mode\n"
                << "  (p)artition     listen to a new partition\n"
                << "  (q)uit          exit MessageFacility DDS server\n"
                << "  ... more interactive commands on the way.\n";
    }
    else if(cmd.empty())
    {
    }
    else
    {
      std::cout << "Command " << cmd << " not found. "
                << "Type \"help\" for a list of available commands.\n";
    }
  }

  return;
}


// SwitchChannel -> switch partition in the same dds domain
bool switchChannel( const std::string & channelName) 
{
  if(!bConnected)  return false;

  status = participant -> get_default_subscriber_qos (sub_qos);
  checkStatus(status, "get_default_subscriber_qos()");
  sub_qos.partition.name.length(1);
  sub_qos.partition.name[0]=channelName.c_str();

  status = MFSubscriber -> set_qos(sub_qos);
  checkStatus(status, "set_qos()");

  return true;
}


int main(int argc, char * argv[])
{
  // checking options
  std::string filename;

  try {
    po::options_description cmdopt("Allowed options");
    cmdopt.add_options()
      ("help,h", "display help message")
      ("partition,p", 
        po::value<int>(&PartitionNumber)->default_value(0), 
        "Partition number the msgserver will listen to (0 - 4)")
      ("filename,f",
        po::value<std::string>(&filename)->default_value("msgarchive"),
        "specify the message archive file name");

    po::options_description desc;
    desc.add(cmdopt);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    if(vm.count("help"))
    {
      std::cout << "Usage: msglogger [options] <message text>\n";
      std::cout << cmdopt;
      return 0;
    }
  } 
  catch(std::exception & e)
  {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    std::cerr << "Exception of unknown type!\n";
    return 1;
  }

  // Generate the Partition name string
  std::stringstream ss;
  ss << "Partition" << PartitionNumber;

  // Create DomainParticipantFactory and a Participant
  dpf = DomainParticipantFactory::get_instance();
  checkHandle(dpf.in(), "DomainParticipantFactory::get_instance()");

  participant = dpf -> create_participant (
     domain,
     PARTICIPANT_QOS_DEFAULT,
     NULL,
     ANY_STATUS);
  checkHandle(dpf.in(), "create participant()");

  // Register the required datatype for MFMessage
  MFMessageTS = new MFMessageTypeSupport();
  checkHandle(MFMessageTS.in(), "MFMessageTypeSupport()");

  MFMessageTypeName = MFMessageTS -> get_type_name();
  status = MFMessageTS -> register_type (
      participant.in(),
      MFMessageTypeName);
  checkStatus(status, "register_type()");

  // Set the ReliabilityQosPolicy to RELIABLE
  status = participant -> get_default_topic_qos( reliable_topic_qos );
  checkStatus(status, "get_default_topic_qos()");
  reliable_topic_qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;

  // make the tailored Qos the new default
  status = participant -> set_default_topic_qos( reliable_topic_qos );
  checkStatus(status, "set_default_topic_qos()");

  // Use the changed poilicy when defining the MFMessage topic
  MFMessageTopic = participant -> create_topic (
      "MessageFacility_Message",
      MFMessageTypeName,
      reliable_topic_qos,
      NULL,
      ANY_STATUS);
  checkHandle(MFMessageTopic.in(), "create_topic()");

  // Adapt the default SubscriberQos to read from the partition
  status = participant -> get_default_subscriber_qos( sub_qos );
  checkStatus(status, "get_default_subscriber_qos()");
  sub_qos.partition.name.length(1);
  sub_qos.partition.name[0] = ss.str().c_str();

  // Create a subscriber for the MF
  MFSubscriber = participant -> create_subscriber (
      sub_qos, NULL, ANY_STATUS);
  checkHandle(MFSubscriber.in(), "create_subscriber()");

  // Adapt the DataReaderQoS to keep track of all messages
  status = MFSubscriber -> get_default_datareader_qos( message_qos );
  checkStatus(status, "get_default_datareader_qos()");
  status = MFSubscriber -> copy_from_topic_qos (
      message_qos, reliable_topic_qos );
  checkStatus(status, "copy_from_topic_qos()");
  message_qos.history.kind = KEEP_ALL_HISTORY_QOS;

  // Create a DataReader for the topic
  parentReader = MFSubscriber -> create_datareader (
      MFMessageTopic.in(),
      message_qos,
      NULL,
      ANY_STATUS);
  checkHandle(parentReader, "create_datereader()");

  // Narrow the abstract parent into its typed representative
  reader = MessageFacility::MFMessageDataReader::_narrow(parentReader);
  checkHandle(reader.in(), "narrow()");

  // Indicate Server is up...
  std::cout << "MessageFacility DDS server is up and listening for messages "
            << "in partition " << PartitionNumber << "\n"
            << "(press enter then \"quit\" to exit listening)...\n";

  // indicating the connection has be established
  bConnected = true;

  //-----------------------------------------------------------------
  // Blocked receive using wait-condition
  //-----------------------------------------------------------------

  // Create ReadCondition on arrival of new messages
  newMsg = reader -> create_readcondition (
      NOT_READ_SAMPLE_STATE,
      ANY_VIEW_STATE,
      ANY_INSTANCE_STATE);
  checkHandle(newMsg, "create_readcondition()");

  // Create a waitset and add the condition
  //newStatus = reader -> get_statuscondition();
  //newStatus -> set_enabled_statuses (DATA_AVAILABLE_STATUS);
  //checkHandle(newStatus, "get_statuscondition()");

  // Create a command line guard that will be used to trigger the 
  // command line interface
  cmdline = new GuardCondition();

  // Create a guard that will be used for exit
  escape  = new GuardCondition();

  // Attaching guardians
  serverWS = new WaitSet();
  status = serverWS -> attach_condition( newMsg.in() );
  checkStatus(status, "attach_condition( newMsg )");
  //status = serverWS -> attach_condition( newStatus.in() );
  //checkStatus(status, "attach_condition( newStatus )");
  status = serverWS -> attach_condition( cmdline.in() );
  checkStatus(status, "attach_condition( cmdline )");
  status = serverWS -> attach_condition( escape.in() );
  checkStatus(status, "attach_condition( escape )");

  // Initialize the guardList to obtain the triggered conditions
  guardList.length(3);

  // Start MessageFacility Service
  mf::StartMessageFacility(
      mf::MessageFacilityService::MultiThread,
      mf::MessageFacilityService::logArchive("msg_archive"));

  // Start the thread for triggering the command line interface
  trigger = boost::thread(cmdLineTrigger);

  // Read messages from the reader
  bDisplayMsg = true;
  z = 0;

  bool terminated = false;
  while (!terminated)
  {
    status = serverWS -> wait(guardList, DURATION_INFINITE);
    checkStatus(status, "WaitSet::wait()");

    // walk over all guards
    for(CORBA::ULong gi = 0; gi < guardList.length(); ++gi)
    {
      if(guardList[gi] == escape.in())
      {
        terminated = true;
      }
      else if(guardList[gi] == cmdline.in())
      {
        // set to no message display on stdout
        // logging to file still going on...
        bDisplayMsg = false;

        // reset the trigger
        status = cmdline->set_trigger_value(FALSE);
        checkStatus(status, "reset trigger..");
        
        // spawn command handling thread
        cmdHandler = boost::thread(cmdLineInterface);
      }
      else if(guardList[gi] == newMsg.in())
      {
        // new message coming in
        status = reader -> take (
            msgSeq,
            infoSeq,
            LENGTH_UNLIMITED,
            ANY_SAMPLE_STATE,
            ANY_VIEW_STATE,
            ANY_INSTANCE_STATE );
        checkStatus(status, "take()");

        z += msgSeq->length();

        for(CORBA::ULong i = 0; i < msgSeq->length(); ++i)
        {
          if(!infoSeq[i].valid_data)
          {
            --z;
            //std::cout << "One logger exits.\n";
            continue;
          }

          MFMessage * msg = &(msgSeq[i]);

          if(bDisplayMsg)
          {
            std::cout << "severity:       " << msg->severity_   << "\n";
            std::cout << "timestamp:      " << msg->timestamp_  << "\n";
            std::cout << "hostname:       " << msg->hostname_   << "\n";
            std::cout << "hostaddr(ip):   " << msg->hostaddr_   << "\n";
            std::cout << "process:        " << msg->process_    << "\n";
            std::cout << "porcess_id:     " << msg->pid_        << "\n";
            std::cout << "application:    " << msg->application_<< "\n";
            std::cout << "module:         " << msg->module_     << "\n";
            std::cout << "context:        " << msg->context_    << "\n";
            std::cout << "category(id):   " << msg->id_         << "\n";
            std::cout << "file:           " << msg->file_       << "\n";
            std::cout << "line:           " << msg->line_       << "\n";
            std::cout << "message:        " << msg->items_      << "\n";
            //std::cout << "idOverflow:   " << msg->idOverflow_ << "\n";
            //std::cout << "subroutine:   " << msg->subroutine_ << "\n";
            std::cout << std::endl;

            //std::cout <<"sample_state =   "<< infoSeq[i].sample_state << "\n";
            //std::cout <<"view_state =     "<< infoSeq[i].view_state << "\n";
            //std::cout <<"instance_state = "
            //          << infoSeq[i].instance_state << "\n";

          }

          // Re-construct the ErrorObject
          mf::ErrorObj * eo_p = new mf::ErrorObj(
              mf::ELseverityLevel(std::string(msg->severity_)), 
              std::string(msg->id_) );

          eo_p -> setHostName   ( std::string(msg->hostname_)    );
          eo_p -> setHostAddr   ( std::string(msg->hostaddr_)    );
          eo_p -> setProcess    ( std::string(msg->process_)     );
          eo_p -> setPID        (      (long)(msg->pid_)         );
          eo_p -> setApplication( std::string(msg->application_) );
          eo_p -> setModule     ( std::string(msg->module_)      );
          eo_p -> setContext    ( std::string(msg->context_)     );
          eo_p -> setSubroutine ( std::string(msg->subroutine_)  );

          (*eo_p) << " " << msg->file_
                  << ":" << msg->line_
                  << "\n" << msg->items_;

          mf::LogErrorObj(eo_p);
        }

        if(bDisplayMsg)
            std::cout << "Recevied messages " << z << "\n\n";

        status = reader -> return_loan(msgSeq, infoSeq);
        checkStatus(status, "return_loan()");
     
      }
    }
  }

  // Remove conditions from the waitset
  status = serverWS -> detach_condition( newMsg.in() );
  checkStatus(status, "detach_condition( newMsg )");
  //status = serverWS -> detach_condition( newStatus.in() );
  //checkStatus(status, "detach_condition( newStatus )");
  status = serverWS -> detach_condition( cmdline.in() );
  checkStatus(status, "detach_condition( cmdline )");
  status = serverWS -> detach_condition( escape.in() );
  checkStatus(status, "detach_condition( escape )");

  // De-allocate type-names
  CORBA::string_free(MFMessageTypeName);

  // Free all resources
  status = participant -> delete_contained_entities();
  checkStatus(status, "delete_contained_entities()");

  // Remove the DomainParticipant
  status = dpf -> delete_participant( participant.in() );
  checkStatus(status, "delete_participant()");

  bConnected = false;

  return 0;
}
