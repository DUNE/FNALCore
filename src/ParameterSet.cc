#include <iostream> // for print(). will be removed in future
#include <sstream>


#include "ParameterSet.h"

#include "boost/lexical_cast.hpp"
#include "boost/numeric/conversion/cast.hpp"

namespace fhicl {

typedef std::map<std::string, boost::any>   valuemap;
typedef std::vector<int>                    vint;
typedef std::vector<unsigned int>           vuint;
typedef std::vector<double>                 vdouble;
typedef std::vector<std::string>            vstring;
typedef std::vector<ParameterSet>           vParameterSet;

boost::any ParameterSet::nil_obj;

namespace {

  bool isBool(boost::any const & obj)
       { return obj.type() == typeid(bool); }

  bool isPSet(boost::any const & obj)
       { return obj.type() == typeid(ParameterSet); }

  bool isPrimitive(boost::any const & obj)
       { return obj.type() == typeid(std::string); }

  bool isVector(boost::any const & obj)
       { return obj.type() == typeid(std::vector<boost::any>); }

  void tab(std::string & out, int indent) 
       { for(int i=0;i<indent;++i) out.append(" ");}


  void printElement(boost::any const & obj, 
                                std::string & out, 
                                int indent=0)
  {
    if(obj.empty())
      out.append("nil");

    else if (isBool(obj))
      out.append(boost::any_cast<bool>(obj) ? "true" : "false");

    else if (isPrimitive(obj))
      out.append(boost::any_cast<std::string>(obj));

    else if (isVector(obj))
    {
      std::vector<boost::any> v = boost::any_cast<std::vector<boost::any> >(obj);
      std::vector<boost::any>::const_iterator it = v.begin();

      out.append("[");

      for(; it!=v.end(); ++it)
      {
        printElement(*it, out);
        if(it<v.end()-1)
          out.append(", ");
      }

      out.append("]");
    }
    else if (isPSet(obj))
    {
      out.append("\n");
      tab(out, indent);
      out.append("{\n");
      boost::any_cast<ParameterSet>(obj).print(out, indent+2);
      tab(out, indent);
      out.append("}");
    }
  }

} // end of namespace


// Print the ParameterSet Object
void ParameterSet::print(std::string & out, int indent) const
{
  valuemap::const_iterator it = PSetMap.begin();

  for(; it!=PSetMap.end(); ++it)
  {
    tab(out, indent);

    out.append(it->first);
    out.append(": ");

    boost::any obj = it->second;
    printElement(obj, out, indent);

    out.append("\n");
  }
}


bool ParameterSet::insertEntryObj( 
       std::pair<std::string, boost::any> const & pair
     , bool overwrite)
{
  if (overwrite)
  {
    PSetMap.insert(pair);
    return true;
  }

  valuemap::iterator it = PSetMap.find(pair.first);

  if(it!=PSetMap.end())
    return false;

  PSetMap.insert(pair);
  return true;
}

boost::any * 
ParameterSet::getParameterObjPtr(std::string const & name, bool bInsert)
{
  valuemap::iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
    return &(it->second);

  if(bInsert)
  {
    insertEntryObj(std::make_pair(name, boost::any()));
    return getParameterObjPtr(name, false);
  }
  else
  {
    throw std::runtime_error("Entry " + name + " not found!");
  }
}

boost::any  
ParameterSet::getParameterObj(std::string const & name)
{
  valuemap::iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
    return it->second;

  return nil_obj;
}
    
template <typename T>
bool ParameterSet::insertEntry(std::string const & name, T const & val, bool overwrite) 
{
  boost::any obj = val;

  if (overwrite)
  {
    PSetMap.insert(std::make_pair(name, obj));
    return true;
  }

  valuemap::iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
    return false;

  PSetMap.insert(std::make_pair(name, obj));
  return true;
}

vstring ParameterSet::getNameList() const
{
  vstring names;

  valuemap::const_iterator it = PSetMap.begin();
  while(it!=PSetMap.end())
  {
    names.push_back(it->first);
    ++it;
  }

  return names;    
}

vstring ParameterSet::getPSetNameList() const
{
  vstring names;

  valuemap::const_iterator it = PSetMap.begin();
  while(it!=PSetMap.end())
  {
    if(isPSet(it->second))  names.push_back(it->first);
    ++it;
  }

  return names;
}


bool ParameterSet::getBool(
    std::string const & name, 
    bool const def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      bool t = boost::any_cast<bool>(it->second);
      return t;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
  }

  return def;
}

int ParameterSet::getInt(
    std::string const & name, 
    int const def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::string t = boost::any_cast<std::string>(it->second);
      int v = boost::lexical_cast<int>(t);
      return v;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
    catch(const boost::bad_lexical_cast &)
    {
      return def;
    }
  }

  return def;
}

vint ParameterSet::getVInt(
    std::string const & name, 
    vint const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::vector<boost::any> va 
          = boost::any_cast<std::vector<boost::any> >(it->second);

      std::vector<int> vi;

      for(std::vector<boost::any>::iterator it=va.begin(); it!=va.end(); ++it)
      {
        std::string t = boost::any_cast<std::string>(*it);
        int v = boost::lexical_cast<int>(t);
        vi.push_back(v);
      }

      return vi;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
    catch(const boost::bad_lexical_cast &)
    {
      return def;
    }
  }

  return def;
}

unsigned int ParameterSet::getUInt(
    std::string const & name, 
    unsigned int const def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::string t = boost::any_cast<std::string>(it->second);
      unsigned int v = boost::numeric_cast<unsigned int>(
                         boost::lexical_cast<int>(t));
      return v;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
    catch(const boost::bad_lexical_cast &)
    {
      return def;
    }
    catch(...)
    {
      return def;
    }
  }

  return def;
}

vuint ParameterSet::getVUInt(
    std::string const & name, 
    vuint const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::vector<boost::any> va 
          = boost::any_cast<std::vector<boost::any> >(it->second);

      std::vector<unsigned int> vi;

      for(std::vector<boost::any>::iterator it=va.begin(); it!=va.end(); ++it)
      {
        std::string t = boost::any_cast<std::string>(*it);
        unsigned int v = boost::numeric_cast<unsigned int>(
                           boost::lexical_cast<int>(t));
        vi.push_back(v);
      }

      return vi;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
    catch(const boost::bad_lexical_cast &)
    {
      return def;
    }
    catch(...)
    {
      return def;
    }
  }

  return def;
}


double ParameterSet::getDouble(
    std::string const & name, 
    double const def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::string t = boost::any_cast<std::string>(it->second);
      double v = boost::lexical_cast<double>(t);
      return v;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
    catch(const boost::bad_lexical_cast &)
    {
      return def;
    }
  }

  return def;
}

vdouble ParameterSet::getVDouble(
    std::string const & name, 
    vdouble const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::vector<boost::any> va 
          = boost::any_cast<std::vector<boost::any> >(it->second);

      std::vector<double> vd;

      for(std::vector<boost::any>::iterator it=va.begin(); it!=va.end(); ++it)
      {
        std::string t = boost::any_cast<std::string>(*it);
        double  v = boost::lexical_cast<double>(t);
        vd.push_back(v);
      }

      return vd;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
    catch(const boost::bad_lexical_cast &)
    {
      return def;
    }
  }

  return def;
}

std::string ParameterSet::getString (
    std::string const & name, 
    std::string const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::string t = boost::any_cast<std::string>(it->second);
      return t;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
  }

  return def;
}

vstring ParameterSet::getVString(
    std::string const & name, 
    vstring const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::vector<boost::any> va 
          = boost::any_cast<std::vector<boost::any> >(it->second);

      std::vector<std::string> v;

      for(std::vector<boost::any>::iterator it=va.begin(); it!=va.end(); ++it)
      {
        std::string t = boost::any_cast<std::string>(*it);
        v.push_back(t);
      }

      return v;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
  }

  return def;
}

ParameterSet ParameterSet::getParameterSet(
    std::string const & name, 
    ParameterSet const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      ParameterSet t = boost::any_cast<ParameterSet>(it->second);
      return t;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
  }

  return def;
}

ParameterSet ParameterSet::getPSet(
    std::string const & name,
    ParameterSet const & def) const
{
  return getParameterSet(name, def);
}

vParameterSet ParameterSet::getVParameterSet(
    std::string const & name, 
    vParameterSet const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::vector<boost::any> va 
          = boost::any_cast<std::vector<boost::any> >(it->second);

      std::vector<ParameterSet> v;

      for(std::vector<boost::any>::iterator it=va.begin(); it!=va.end(); ++it)
      {
        ParameterSet t = boost::any_cast<ParameterSet>(*it);
        v.push_back(t);
      }

      return v;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
  }

  return def;
}

vParameterSet ParameterSet::getVPSet(
    std::string const & name,
    vParameterSet const & def) const
{
  return getVParameterSet(name, def);
}


// Add methods

bool ParameterSet::addBool ( 
       std::string const & name
     , bool val
     , bool overwrite) 
{ 
  return insertEntryObj(std::make_pair(name, boost::any(val)), overwrite); 
}

bool ParameterSet::addInt ( 
       std::string const & name
     , int  val
     , bool overwrite)
{ 
  std::stringstream ss;
  ss << val;

  return insertEntryObj(std::make_pair(name, boost::any(ss.str())), overwrite); 
}

bool ParameterSet::addUInt ( 
       std::string const & name
     , unsigned int  val
     , bool overwrite)
{ 
  std::stringstream ss;
  ss << val;

  return insertEntryObj(std::make_pair(name, boost::any(ss.str())), overwrite); 
}

bool ParameterSet::addDouble ( 
       std::string const & name
     , double val
     , bool overwrite)
{ 
  std::stringstream ss;
  ss << val;

  return insertEntryObj(std::make_pair(name, boost::any(ss.str())), overwrite); 
}

bool ParameterSet::addString ( 
       std::string const & name
     , const std::string & val
     , bool overwrite)
{ 
  return insertEntryObj(std::make_pair(name, boost::any(val)), overwrite); 
}

bool ParameterSet::addVInt ( 
       std::string const & name
     , const vint & val
     , bool overwrite)
{ 
  std::vector<boost::any> v;
  vint::const_iterator it = val.begin();

  std::stringstream ss;

  while(it != val.end())
  {
    ss.str("");
    ss << *it;
    v.push_back(boost::any(ss.str()));
    ++it;
  }

  return insertEntryObj(std::make_pair(name, boost::any(v)), overwrite); 
}

bool ParameterSet::addVUInt ( 
       std::string const & name
     , const vuint & val
     , bool overwrite)
{ 
  std::vector<boost::any> v;
  vuint::const_iterator it = val.begin();

  std::stringstream ss;

  while(it != val.end())
  {
    ss.str("");
    ss << *it;
    v.push_back(boost::any(ss.str()));
    ++it;
  }

  return insertEntryObj(std::make_pair(name, boost::any(v)), overwrite); 
}

bool ParameterSet::addVDouble ( 
       std::string const & name
     , const vdouble & val
     , bool overwrite)
{ 
  std::vector<boost::any> v;
  vdouble::const_iterator it = val.begin();

  std::stringstream ss;

  while(it != val.end())
  {
    ss.str("");
    ss << *it;
    v.push_back(boost::any(ss.str()));
    ++it;
  }

  return insertEntryObj(std::make_pair(name, boost::any(v)), overwrite); 
}

bool ParameterSet::addVString ( 
       std::string const & name
     , const vstring & val
     , bool overwrite)
{ 
  std::vector<boost::any> v;
  vstring::const_iterator it = val.begin();

  while(it != val.end())
  {
    v.push_back(boost::any(*it));
    ++it;
  }

  return insertEntryObj(std::make_pair(name, boost::any(v)), overwrite); 
}

bool ParameterSet::addPSet ( 
       std::string const & name
     , const ParameterSet & val
     , bool overwrite)
{ 
  return insertEntryObj(std::make_pair(name, boost::any(val)), overwrite); 
}

bool ParameterSet::addParameterSet ( 
       std::string const & name
     , const ParameterSet & val
     , bool overwrite)
{ 
  return insertEntryObj(std::make_pair(name, boost::any(val)), overwrite); 
}

bool ParameterSet::addVPSet ( 
       std::string const & name
     , const vParameterSet & val
     , bool overwrite)
{ 
  std::vector<boost::any> v;
  vParameterSet::const_iterator it = val.begin();
  while(it != val.end())
  {
    v.push_back(boost::any(*it));
    ++it;
  }

  return insertEntryObj(std::make_pair(name, boost::any(v)), overwrite); 
}

bool ParameterSet::addVParameterSet( 
       std::string const & name
     , const vParameterSet & val
     , bool overwrite)
{ 
  std::vector<boost::any> v;
  vParameterSet::const_iterator it = val.begin();
  while(it != val.end())
  {
    v.push_back(boost::any(*it));
    ++it;
  }

  return insertEntryObj(std::make_pair(name, boost::any(v)), overwrite); 
}

} // namespace
