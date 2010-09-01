#ifndef MessageFacility_ParameterSet_NewParameterSet_h
#define MessageFacility_ParameterSet_NewParameterSet_h

//
// New ParameterSet
//

#include "boost/any.hpp"

#include <string>
#include <vector>
#include <map>

#include <iostream>
#include <stdexcept>

namespace mf {

class ParameterSet
{
public:
  ParameterSet() : PSetMap() {}
  ~ParameterSet() {}

private:

  template <typename T>
  bool insertEntry(std::string const & name, T const & val, bool overwrite=true);
  void insertEntryObj(std::pair<std::string, boost::any> const & pair);

  boost::any * getParameterObjPtr(std::string const & name, bool bInsert);
  boost::any   getParameterObj(std::string const & name);

  static void printElement(boost::any const &, int indent=0);

public:

  typedef std::vector<int>          vint;
  typedef std::vector<double>       vdouble;
  typedef std::vector<std::string>  vstring;
  typedef std::vector<ParameterSet> vParameterSet;


  // Print out the ParameterSet object
  void print(int indent=0) const;
 
  // Check if empty
  bool empty() const { return PSetMap.empty(); }

 
  // Get methods
  bool getBool ( std::string const &
               , const bool def=false) const;

  int getInt ( std::string const &
             , const int  def=0) const;

  vint getVInt ( std::string const &
               , const vint & def=vint()) const;

  double getDouble ( std::string const &
                   , const double def=0.0) const;

  vdouble getVDouble ( std::string const &
                     , const vdouble & def=vdouble()) const;

  std::string getString ( std::string const &
                        , const std::string & def=std::string()) const;

  vstring getVString ( std::string const &
                     , const vstring & def=vstring()) const;

  ParameterSet getPSet ( std::string const &
                        , const ParameterSet & def=ParameterSet()) const;

  ParameterSet getParameterSet ( std::string const &
                               , const ParameterSet & def=ParameterSet()) const;

  vParameterSet getVPSet ( std::string const &
                         , const vParameterSet & def=vParameterSet()) const;

  vParameterSet getVParameterSet ( std::string const &
                         , const vParameterSet & def=vParameterSet()) const;


  // Methods for inserting parameters
  bool addBool ( std::string const & name, bool val
               , bool overwrite=true) 
       { return insertEntry(name, val, overwrite); }

  bool addInt ( std::string const & name, int  val
              , bool overwrite=true)
       { return insertEntry(name, val, overwrite); }

  bool addDouble ( std::string const & name, double val
                 , bool overwrite=true)
       { return insertEntry(name, val, overwrite); }

  bool addString ( std::string const & name, const std::string & val
                 , bool overwrite=true)
       { return insertEntry(name, val, overwrite); }

  bool addVInt ( std::string const & name, const vint & val
               , bool overwrite=true)
       { return insertEntry(name, val, overwrite); }

  bool addVDouble ( std::string const & name, const vdouble & val
                  , bool overwrite=true)
       { return insertEntry(name, val, overwrite); }

  bool addPSet ( std::string const & name, const ParameterSet & val
               , bool overwrite=true)
       { return insertEntry(name, val, overwrite); }

  bool addParameterSet ( std::string const & name, const ParameterSet & val
                       , bool overwrite=true)
       { return insertEntry(name, val, overwrite); }

  bool addVPSet ( std::string const & name, const vParameterSet & val
                , bool overwrite=true)
       { return insertEntry(name, val, overwrite); }

  bool addVParameterSet( std::string const & name, const vParameterSet & val
                       , bool overwrite=true)
       { return insertEntry(name, val, overwrite); }

  // returns the list of parameter names
  vstring getNameList     () const;
  vstring getPSetNameList () const;
  
private:
  typedef std::map<const std::string, boost::any>   valuemap;

  valuemap  PSetMap;
  static boost::any nil_obj;

  // Make the PSetParser class friend to allow the access of private members
  template<typename Iterator> friend class PSetParser;

};

}


#endif
