

#include <QtGui>

#include "mvdlg.h"

msgViewerDlg::msgViewerDlg(QDialog * parent)
: BUFFER_SIZE ( 1000 )  // size of the circular buffer for received messages
, idx         ( 0 )      // index of the head position in the circular buffer
, mfmessages  ( std::vector<mf::MessageFacilityMsg>(BUFFER_SIZE) )
, nMsgs       ( 0 )
, hostFilter  ( "##DEADBEAF##" )
, appFilter   ( "##DEADBEAF##" )
, catFilter   ( "##DEADBEAF##" )
, hostmap     ( )
, appmap      ( )
, catmap      ( )
, qtdds       ( )
{
  setupUi(this);

  connect( btnPause, SIGNAL( clicked() ), this, SLOT(pause()) );
  connect( btnExit,  SIGNAL( clicked() ), this, SLOT(exit())  );
  connect( btnSwitchChannel, SIGNAL( clicked() ), this, SLOT(switchChannel()) );

  connect( btnFilter, SIGNAL( clicked() ), this, SLOT( setFilter() ) );
  connect( btnReset,  SIGNAL( clicked() ), this, SLOT( resetFilter() ) );

  connect( lwApplication, SIGNAL( currentRowChanged(int) ), this, SLOT( filterApp(int) ) );

  connect( vsSeverity
         , SIGNAL( valueChanged(int) )
         , this
         , SLOT(changeSeverity(int)) );

  connect( &qtdds
         , SIGNAL(newMessage(mf::MessageFacilityMsg const & ))
         , this
         , SLOT(onNewMsg(mf::MessageFacilityMsg const & )) );

  connect( &qtdds
         , SIGNAL(newSysMessage(mf::QtDDSReceiver::SysMsgCode, std::string const & ))
         , this
         , SLOT(onNewSysMsg(mf::QtDDSReceiver::SysMsgCode, std::string const & )) );

  label_Partition->setText("Partition 0");

}

void msgViewerDlg::filterApp(int row) {
	//std::cout << row << "\n";
}

void msgViewerDlg::onNewMsg(mf::MessageFacilityMsg const & mfmsg) {

	bool updateApp = false;
	bool updateCat = false;
	bool updateHost = false;

	// Check if the buffer is full
	if(!mfmessages[idx].empty()) {
		std::string host = mfmessages[idx].hostname();
		host = host.substr(0, host.find_first_of('.'));

		updateApp = deleteFromMap(appmap, mfmessages[idx].application());
		updateCat = deleteFromMap(catmap, mfmessages[idx].category());
		updateHost = deleteFromMap(hostmap, host);
	}

	// push the message into buffer
	mfmessages[idx] = mfmsg;

	std::string host = mfmsg.hostname();
	std::string cat  = mfmsg.category();
	std::string app  = mfmsg.application();

	host = host.substr(0, host.find_first_of('.'));

	bool resetFilter = false;

	if(updateMap(appmap, app) || updateApp) {
		updateList(lwApplication, appmap);
	}

	if(updateMap(catmap, cat) || updateCat) {
		updateList(lwCategory, catmap);
	}

	if(updateMap(hostmap, host) || updateHost) {
		updateList(lwHost, hostmap);
	}

	bool hostAny = ( hostFilter == "##DEADBEAF##" );
	bool appAny  = ( appFilter  == "##DEADBEAF##" );
	bool catAny  = ( catFilter  == "##DEADBEAF##" );

	bool hostMatch = ( hostFilter == host );
	bool appMatch  = ( appFilter  == app  );
	bool catMatch  = ( catFilter  == cat  );

	if( (hostAny || hostMatch) && (appAny || appMatch) && (catAny || catMatch) )
		displayMsg(mfmsg);

	// finally revolve the idx forward by one
	idx = (++idx) % BUFFER_SIZE;
}

void msgViewerDlg::onNewSysMsg(mf::QtDDSReceiver::SysMsgCode syscode, std::string const & msg) {

	if(syscode == mf::DDSReceiver::NEW_MESSAGE) {
		++nMsgs;
		lcdMsgs->display( nMsgs );
	}
	else {
		QString qtmsg = "<b><font color=\"#000080\">SYSTEM:"
			  + QString(msg.c_str()) + "</font></b><br>";

		txtMessages->append(qtmsg);
	}
}

void msgViewerDlg::displayMsg(mf::MessageFacilityMsg const & mfmsg) {

    std::string sev = mfmsg.severity();

    mf::QtDDSReceiver::SeverityCode sevid = mf::QtDDSReceiver::getSeverityCode(mfmsg.severity());

    //if(sevid < severityThreshold)
    //    continue;

    std::ostringstream ss;
    ss << "<font face=\"New Courier\" ";

    if(sevid==mf::DDSReceiver::ERROR)         ss << "color='#FF0000'>";
    else if(sevid==mf::DDSReceiver::WARNING)  ss << "color='#E08000'>";
    else if(sevid==mf::DDSReceiver::INFO)     ss << "color='#008000'>";
    else                                      ss << "color='#505050'>";

    ss << "<table border=0 width=95%>";

    ss << "<tr><td width=18%>severity:</td><td><b>"
       << mfmsg.severity()
       << "</b></td></tr>";

    ss << "<tr><td>category:</td><td><b>"
       << mfmsg.category()
       << "</b></td></tr>";

    ss << "<tr><td>date:</td><td>"
       << mfmsg.timestr()
       << "</td></tr>";

    ss << "<tr><td>host:</td><td>"
       << mfmsg.hostname() << " (" << mfmsg.hostaddr() << ")"
       << "</td></tr>";

    ss << "<tr><td>process:</td><td>"
       << mfmsg.process()  << " (" << mfmsg.pid()      << ")"
       << "<td></tr>";

    ss << "<tr><td>context:</td><td>"
       << mfmsg.application() << " / "
       << mfmsg.module()      << " / "
       << mfmsg.context()
       << "</td></tr>";

    ss << "<tr><td>file:</td><td>"
       << mfmsg.file()  << " (" << mfmsg.line() << ")"
       << "</td></tr>";

    ss << "<tr><td>message:</td><td><i>"
       << mfmsg.message()
       << "</i></td></tr>";

    ss << "</table></font><br>";

	txtMessages->append(QString(ss.str().c_str()));
}

bool msgViewerDlg::updateMap(std::map<std::string, std::list<int> > & map, std::string const & key) {

	std::map<std::string, std::list<int> >::iterator it = map.find(key);

	if(it != map.end()) {
		// the key already exists
		it->second.push_back(idx);
		return false;
	} else {
		// its a new key
		std::list<int> idxlist;
		idxlist.push_back(idx);
		map.insert( std::make_pair( key, idxlist ) );
		return true;
	}
}

bool msgViewerDlg::deleteFromMap(std::map<std::string, std::list<int> > & map, std::string const & key) {
	std::map<std::string, std::list<int> >::iterator it = map.find(key);
	if(it != map.end()) {
		it->second.pop_front();
		if(it->second.empty()) {
			map.erase(it);
			return true;
		}
	}
	return false;
}

bool msgViewerDlg::updateList(QListWidget * lw, std::map<std::string, std::list<int> > & map) {

	bool nonSelectedBefore = ( lw->currentRow() == -1 );
	bool nonSelectedAfter = true;

	QString item = nonSelectedBefore ? "" : lw->currentItem()->text();

	lw -> clear();
	int row = 0;
	std::map<std::string, std::list<int> >::const_iterator it = map.begin();

	while(it!=map.end()) {
		lw -> addItem(it->first.c_str());

		if(!nonSelectedBefore && nonSelectedAfter) {
			if(item == it->first.c_str()) {
				lw ->setCurrentRow(row);
				nonSelectedAfter = false;
			}
		}

		++row;
		++it;
	}

	if(!nonSelectedBefore && nonSelectedAfter)  return true;

	return false;
}

std::list<int> msgViewerDlg::findCommonInLists(
		  std::list<int> const & l1
		, std::list<int> const & l2
		, std::list<int> const & l3 )
{
	bool e1 = l1.empty();
	bool e2 = l2.empty();
	bool e3 = l3.empty();

	if( !e1 &&  e2 &&  e3 )
		return l1;

	if(  e1 && !e2 &&  e3 )
		return l2;

	if(  e1 &&  e2 && !e3 )
		return l3;

	if( !e1 && !e2 &&  e3 )
		return findCommonInLists(l1, l2);

	if( !e1 &&  e2 && !e3 )
		return findCommonInLists(l1, l3);

	if(  e1 && !e2 && !e3 )
		return findCommonInLists(l2, l3);

	// start finding commons
	std::list<int>::const_iterator it1 = l1.begin();
	std::list<int>::const_iterator it2 = l2.begin();
	std::list<int>::const_iterator it3 = l3.begin();

	std::list<int> r;

	while( (it1!=l1.end()) && (it2 != l2.end()) && (it3!=l3.end())) {

		if( (*it1 == *it2) && (*it1 == *it3) ) {
			r.push_back(*it1);
			++it1;
			++it2;
			++it3;
		}
		else
			++ ( (*it1 < *it2)
					? ( (*it1 < *it3) ? it1 : it3 )
					: ( (*it2 < *it3) ? it2 : it3 ) );
	}

	return r;
}

std::list<int> msgViewerDlg::findCommonInLists(
		  std::list<int> const & l1
		, std::list<int> const & l2 )
{
	std::list<int>::const_iterator it1 = l1.begin();
	std::list<int>::const_iterator it2 = l2.begin();

	std::list<int> r;

	while( (it1!=l1.end()) && (it2 != l2.end()) )
	{
		if(*it1 == *it2)
		{
			r.push_back(*it1);
			++it1;
			++it2;
		}
		else if(*it1 < *it2)
			++it1;
		else
			++it2;
	}

	return r;
}

void msgViewerDlg::setFilter() {

	hostFilter = (lwHost->currentItem()) ? lwHost->currentItem()->text().toStdString() : "##DEADBEAF##";
	appFilter = (lwApplication->currentItem()) ? lwApplication->currentItem()->text().toStdString() : "##DEADBEAF##";
	catFilter = (lwCategory->currentItem()) ? lwCategory->currentItem()->text().toStdString() : "##DEADBEAF##";

	std::list<int> result;
	std::list<int> empty;

	std::map<std::string, std::list<int> >::const_iterator it;

	result = findCommonInLists(
			  ( (it = hostmap.find(hostFilter)) == hostmap.end() ) ? empty : it->second
			, ( (it = appmap.find(appFilter)) == appmap.end() ) ? empty : it->second
			, ( (it = catmap.find(catFilter)) == catmap.end() ) ? empty : it->second );

	// Update the view
	txtMessages->clear();
	std::list<int>::const_iterator lit = result.begin();
	while(lit!=result.end()) {
		displayMsg(mfmessages[*lit]);
		++lit;
	}
}

void msgViewerDlg::resetFilter() {

	hostFilter = "##DEADBEAF##";
	appFilter = "##DEADBEAF##";
	catFilter = "##DEADBEAF##";

	int i = (idx+1) % BUFFER_SIZE;
	while (i!=idx) {
		if(!mfmessages[i].empty())
			displayMsg(mfmessages[i]);
		i = (++i) % BUFFER_SIZE;
	}
}

void msgViewerDlg::pause()
{
  QMessageBox::about(this, "About MsgViewer", "pausing...");
}

void msgViewerDlg::printMessage(const QString & s)
{
  txtMessages->append(s);
  //txtMessages->append("<hr width=95%><br>\n");
}

void msgViewerDlg::printSysMessage(const QString & s)
{
  QString msg = "<b><font color=\"#000080\">SYSTEM:" 
      + s + "</font></b><br>";
  txtMessages->append(msg);
}

void msgViewerDlg::exit()
{
  close();
}

void msgViewerDlg::changeSeverity(int sev)
{
  qtdds.setSeverityThreshold(mf::QtDDSReceiver::getSeverityCode(sev));
}

void msgViewerDlg::switchChannel()
{
  bool ok;
  int partition = QInputDialog::getInteger(this, 
    "Partition", 
    "Please enter a partition number:",
    qtdds.getPartition(),
    -1, 9, 1, &ok);

  if(ok)
  {
    qtdds.switchPartition(partition);

    QString partStr = "Partition " + QString::number(qtdds.getPartition());
    label_Partition->setText(partStr);
  }

}

void msgViewerDlg::closeEvent(QCloseEvent *event)
{
	qtdds.stop();
	event->accept();
}
