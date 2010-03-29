


#include "ParameterSet/interface/ParameterSet.h"

#include "boost/lexical_cast.hpp"

namespace mf {

typedef std::map<std::string, boost::any>   valuemap;
typedef std::vector<int>                    vint;
typedef std::vector<double>                 vdouble;
typedef std::vector<std::string>            vstring;
typedef std::vector<ParameterSet>           vParameterSet;

bool ParameterSet::getBool(
    std::string const & name, 
    bool const & def) const
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
    int const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      int t = boost::any_cast<int>(it->second);
      return t;
    }
    catch(const boost::bad_any_cast &)
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

      std::vector<int> v;

      for(std::vector<boost::any>::iterator it=va.begin(); it!=va.end(); ++it)
      {
        int t = boost::any_cast<int>(*it);
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

double ParameterSet::getDouble(
    std::string const & name, 
    double const & def) const
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


} // namespace
