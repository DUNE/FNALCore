// ----------------------------------------------------------------------
// definition of type encoding/decoding functions
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// prerequisite source files and headers
// ----------------------------------------------------------------------

#include "ParameterSet/interface/types.h"

#include "boost/lexical_cast.hpp"
#include "ParameterSet/interface/split.h"
#include "Utilities/interface/Parse.h"
#include <cctype>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <cassert>

using namespace mf;


// ----------------------------------------------------------------------
// utility functions
// ----------------------------------------------------------------------

static char
  to_hex(unsigned i)
{
  return i + (i < 10u ? '0' : ('A'-10));
}

// ----------------------------------------------------------------------

static unsigned
  from_hex(char c)
{
  switch(c)  {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      return c - '0';
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      return 10 + c - 'a';
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      return 10 + c - 'A';
    default:
      return 0;
  }
}  // from_hex()


static std::string
  to_hex_rep(unsigned c)
{
  char rep[] = "xy";
  rep[0] = to_hex(c / 16u);
  rep[1] = to_hex(c % 16u);

  return rep;
}  // to_hex_rep()


// ----------------------------------------------------------------------
// Bool
// ----------------------------------------------------------------------

bool
  mf::decode(bool & to, std::string const& from)
{
  if     (from == "true")  { to = true ; return true; }
  else if(from == "false")  { to = false; return true; }
  else                        return false;
}  // decode to bool

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, bool from)
{
  to = from ? "true" : "false";
  return true;
}  // encode from bool


// ----------------------------------------------------------------------
// vBool
// ----------------------------------------------------------------------

bool
  mf::decode(std::vector<bool> & to, std::string const& from)
{
  std::vector<std::string> temp;
  if(! split(std::back_inserter(temp), from, '{', ',', '}'))
    return false;

  to.clear();
  for(std::vector<std::string>::const_iterator  b = temp.begin()
                                             ,  e = temp.end()
      ; b != e ; ++b)
  {
    bool  val;
    if(! decode(val, *b))
      return false;
    to.push_back(val);
  }

  return true;
}  // decode to vector<bool>

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, std::vector<bool> const& from)
{
  to = "{";

  std::string  converted;
  for(std::vector<bool>::const_iterator b = from.begin()
                                       , e = from.end()
     ; b != e ; ++b)
  {
    if(! encode(converted, *b))
      return false;

    if(b != from.begin())
      to += ",";
    to += converted;
  }

  to += '}';
  return true;
}  // encode from vector<bool>


// ----------------------------------------------------------------------
// Int32
// ----------------------------------------------------------------------

bool
  mf::decode(int & to, std::string const& from)
{
  std::string::const_iterator  b = from.begin()
                            ,  e = from.end();

  if(*b != '+' && *b != '-')
    return false;
  int  sign = (*b == '+') ? +1 : -1;

  to = 0;
  while(++b != e)  {
    if(! std::isdigit(*b))
      return false;
    to = 10 * to + (*b - '0');
  }
  to *= sign;

  return true;
}  // decode to int

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, int from)
{
  bool is_negative = (from < 0);
  if(is_negative)
    from = - from;  // TODO: work around this for most negative integer

  to.clear();
  do  {
    to = static_cast<char>(from % 10 + '0') + to;
    from /= 10;
  }  while(from > 0);
  to = (is_negative ? '-' : '+') + to;

  return true;
}  // encode from int

// ----------------------------------------------------------------------
// Int64
// ----------------------------------------------------------------------

bool
  mf::decode(long long & to, std::string const& from)
{
  std::string::const_iterator  b = from.begin()
                            ,  e = from.end();

  if(*b != '+' && *b != '-')
    return false;
  int  sign = (*b == '+') ? +1 : -1;

  to = 0;
  while(++b != e)  {
    if(! std::isdigit(*b))
      return false;
    to = 10 * to + (*b - '0');
  }
  to *= sign;

  return true;
}  // decode to int

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, long long from)
{
  bool is_negative = (from < 0);
  if(is_negative)
    from = - from;  // TODO: work around this for most negative integer

  to.clear();
  do  {
    to = static_cast<char>(from % 10 + '0') + to;
    from /= 10;
  }  while(from > 0);
  to = (is_negative ? '-' : '+') + to;

  return true;
}  // encode from int

// ----------------------------------------------------------------------
// vInt32
// ----------------------------------------------------------------------

bool
  mf::decode(std::vector<int> & to, std::string const& from)
{
  std::vector<std::string> temp;
  if(! split(std::back_inserter(temp), from, '{', ',', '}'))
    return false;

  to.clear();
  for(std::vector<std::string>::const_iterator  b = temp.begin()
                                             ,  e = temp.end()
      ; b != e ; ++b)
  {
    int  val;
    if(! decode(val, *b))
      return false;
    to.push_back(val);
  }

  return true;
}  // decode to vector<int>

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, std::vector<int> const& from)
{
  to = "{";

  std::string  converted;
  for(std::vector<int>::const_iterator b = from.begin()
                                      , e = from.end()
     ; b != e ; ++b)
  {
    if(! encode(converted, *b))
      return false;

    if(b != from.begin())
      to += ",";
    to += converted;
  }

  to += '}';
  return true;
}  // encode from vector<int>

// ----------------------------------------------------------------------
// vInt64
// ----------------------------------------------------------------------

bool
  mf::decode(std::vector<long long> & to, std::string const& from)
{
  std::vector<std::string> temp;
  if(! split(std::back_inserter(temp), from, '{', ',', '}'))
    return false;

  to.clear();
  for(std::vector<std::string>::const_iterator  b = temp.begin()
                                             ,  e = temp.end()
      ; b != e ; ++b)
  {
    long long val;
    if(! decode(val, *b))
      return false;
    to.push_back(val);
  }

  return true;
}  // decode to vector<int>

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, std::vector<long long> const& from)
{
  to = "{";

  std::string  converted;
  for(std::vector<long long>::const_iterator b = from.begin()
                                      , e = from.end()
     ; b != e ; ++b)
  {
    if(! encode(converted, *b))
      return false;

    if(b != from.begin())
      to += ",";
    to += converted;
  }

  to += '}';
  return true;
}  // encode from vector<int>

// ----------------------------------------------------------------------
// Uint32
// ----------------------------------------------------------------------

bool
  mf::decode(unsigned & to, std::string const& from)
{
  std::string::const_iterator  b = from.begin()
                            ,  e = from.end();

  to = 0u;
  for(; b != e; ++b)  {
    if(*b == 'u' || *b == 'U')
      return true;
    if(! std::isdigit(*b))
      return false;
    to = 10u * to + (*b - '0');
  }

  return true;
}  // decode to unsigned

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, unsigned from)
{
  to.clear();
  do  {
    to = static_cast<char>(from % 10 + '0') + to;
    from /= 10u;
  }  while(from > 0u);

  return true;
}  // encode from unsigned

// ----------------------------------------------------------------------
// Uint64
// ----------------------------------------------------------------------

bool
  mf::decode(unsigned long long & to, std::string const& from)
{
  std::string::const_iterator  b = from.begin()
                            ,  e = from.end();

  to = 0u;
  for(; b != e; ++b)  {
    if(*b == 'u' || *b == 'U')
      return true;
    if(! std::isdigit(*b))
      return false;
    to = 10u * to + (*b - '0');
  }

  return true;
}  // decode to unsigned

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, unsigned long long from)
{
  to.clear();
  do  {
    to = static_cast<char>(from % 10 + '0') + to;
    from /= 10u;
  }  while(from > 0u);

  return true;
}  // encode from unsigned

// ----------------------------------------------------------------------
// vUint32
// ----------------------------------------------------------------------

bool
  mf::decode(std::vector<unsigned> & to, std::string const& from)
{
  std::vector<std::string> temp;
  if(! split(std::back_inserter(temp), from, '{', ',', '}'))
    return false;

  to.clear();
  for(std::vector<std::string>::const_iterator  b = temp.begin()
                                             ,  e = temp.end()
      ; b != e ; ++b)
  {
    unsigned  val;
    if(! decode(val, *b))
      return false;
    to.push_back(val);
  }

  return true;
}  // decode to vector<unsigned>

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, std::vector<unsigned> const& from)
{
  to = "{";

  std::string  converted;
  for(std::vector<unsigned>::const_iterator b = from.begin()
                                           , e = from.end()
     ; b != e ; ++b)
  {
    if(! encode(converted, *b))
      return false;

    if(b != from.begin())
      to += ",";
    to += converted;
  }

  to += '}';
  return true;
}  // encode from vector<unsigned>

// ----------------------------------------------------------------------
// vUint64
// ----------------------------------------------------------------------

bool
  mf::decode(std::vector<unsigned long long> & to, std::string const& from)
{
  std::vector<std::string> temp;
  if(! split(std::back_inserter(temp), from, '{', ',', '}'))
    return false;

  to.clear();
  for(std::vector<std::string>::const_iterator  b = temp.begin()
                                             ,  e = temp.end()
      ; b != e ; ++b)
  {
    unsigned long long val;
    if(! decode(val, *b))
      return false;
    to.push_back(val);
  }

  return true;
}  // decode to vector<unsigned>

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, std::vector<unsigned long long> const& from)
{
  to = "{";

  std::string  converted;
  for(std::vector<unsigned long long>::const_iterator b = from.begin()
                                           , e = from.end()
     ; b != e ; ++b)
  {
    if(! encode(converted, *b))
      return false;

    if(b != from.begin())
      to += ",";
    to += converted;
  }

  to += '}';
  return true;
}  // encode from vector<unsigned>


// ----------------------------------------------------------------------
// Double
// ----------------------------------------------------------------------

bool
  mf::decode(double & to, std::string const& from)
{
  if(from == "NaN")
    to = std::numeric_limits<double>::quiet_NaN();

  else if(from == "+inf" || from == "inf")
  {
    to = std::numeric_limits<double>::has_infinity
       ? std::numeric_limits<double>::infinity()
       : std::numeric_limits<double>::max();
  }
  else if(from == "-inf")
  {
    to = std::numeric_limits<double>::has_infinity
       ? -std::numeric_limits<double>::infinity()
       : -std::numeric_limits<double>::max();
  }

  else  {
    try  {
      // std::cerr << "from:" << from << std::endl;
      to = boost::lexical_cast<double>(from);
      // std::cerr << "to:" << to << std::endl;
    }
    catch(boost::bad_lexical_cast &)  {
      return false;
    }
  }

  return true;
}

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, double from)
{
  std::ostringstream ost;
  ost.precision(std::numeric_limits<double>::digits10+1);
  ost << from;
  if(!ost) return false;
  to=ost.str();
  return true;
}


// ----------------------------------------------------------------------
// vDouble
// ----------------------------------------------------------------------

bool
  mf::decode(std::vector<double> & to, std::string const& from)
{
  std::vector<std::string> temp;
  if(! split(std::back_inserter(temp), from, '{', ',', '}'))
    return false;

  to.clear();
  for(std::vector<std::string>::const_iterator  b = temp.begin()
                                             ,  e = temp.end()
      ; b != e ; ++b)
  {
    double  val;
    if(! decode(val, *b))
      return false;
    to.push_back(val);
  }

  return true;
}  // decode to vector<double>

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, std::vector<double> const& from)
{
  to = "{";

  std::string  converted;
  for(std::vector<double>::const_iterator b = from.begin()
                                         , e = from.end()
     ; b != e ; ++b)
  {
    if(! encode(converted, *b))
      return false;

    if(b != from.begin())
      to += ",";
    to += converted;
  }

  to += '}';
  return true;
}  // encode from vector<double>


// ----------------------------------------------------------------------
// String
// ----------------------------------------------------------------------

bool
  mf::decode(std::string & to, std::string const& from)
{
  /*std::cerr << "Decoding: " << from << '\n'; //DEBUG*/
  std::string::const_iterator  b = from.begin()
                            ,  e = from.end();

  to = "";
  to.reserve((e-b)/2);
  char  c = '\0';
  for(bool  even_pos = true
     ;  b != e ; ++b, even_pos = ! even_pos)
  {
    if(even_pos)  {
      /*std::cerr << "Even: |"
                << *b
                << "|   giving "
                << from_hex(*b)
                << "\n"; //DEBUG*/
      c = static_cast<char>(from_hex(*b));
    }
    else  {
      /*std::cerr << "Odd:  |"
                << *b
                << "|   giving "
                << from_hex(*b)
                << "\n"; //DEBUG*/
      c = static_cast<char>(c * 16 + from_hex(*b));
      //      if(std::isalnum(c))  {
        /*std::cerr << "Ans:  |" << c << "|\n"; //DEBUG*/
        to += c;
	//}
	//else  {  // keep all special chars encoded
        //to += "\\x";
        //to += to_hex_rep(c);
	//}
    }
  }
  /*std::cerr << "Decoded: " << to << '\n'; //DEBUG*/
  return true;
}  // decode to String


// ----------------------------------------------------------------------
// FileInPath
// ----------------------------------------------------------------------

bool
  mf::decode(FileInPath& to, std::string const& from)
{
  std::istringstream is(from);
  FileInPath temp;
  is >> temp;
  if (!is) return false;
  to = temp;
  return true;
}  // decode to FileInPath



bool
  mf::encode(std::string& to, const FileInPath& from)
{
  std::ostringstream ost;
  ost << from;
  if (!ost) return false;
  to = ost.str();
  return true;
}


// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, std::string const& from)
{
  std::string::const_iterator  b = from.begin()
                            ,  e = from.end();

  enum escape_state { NONE
                    , BACKSLASH
                    , HEX, HEX1
                    , OCT1, OCT2
                    };

  escape_state  state = NONE;
  int code = 0;
  to = "";
  for(; b != e; ++b)  {
    /*std::cerr << "State: " << state << "; char = " << *b << '\n'; //DEBUG*/
    switch(state)  {
      case NONE:  {
        if(*b == '\\')  state = BACKSLASH;
        else              to += to_hex_rep(*b);
        /*std::cerr << "To: |" << to << "|\n"; //DEBUG*/
        break;
      }
      case BACKSLASH:  {
        code = 0;
        switch(*b)  {
          case 'x': case 'X':  {
            state = HEX;
            break;
          }
          case '0': case '1': case '2': case '3':
          case '4': case '5': case '6': case '7':  {
            code = 8 * code + from_hex(*b);
            state = OCT1;
            break;
          }
          case 'n':  {
            to += to_hex_rep(10);
            state = NONE;
            break;
          }
          case 't':  {
            to += to_hex_rep(9);
            state = NONE;
            break;
          }
          default:  {
            to += to_hex_rep(*b);
            state = NONE;
            break;
          }
        }
        break;
      }
      case HEX:  {
        to += *b;
        state = HEX1;
        break;
      }
      case HEX1:  {
        to += *b;
        state = NONE;
        break;
      }
      case OCT1:  {
        switch(*b)  {
          case '0': case '1': case '2': case '3':
          case '4': case '5': case '6': case '7':  {
            code = 8 * code + from_hex(*b);
            state = OCT2;
            break;
          }
          default:  {
            to += to_hex_rep(code);
            state = NONE;
            break;
          }
        }
        break;
      }
      case OCT2:  {
        switch(*b)  {
          case '0': case '1': case '2': case '3':
          case '4': case '5': case '6': case '7':  {
            code = 8 * code + from_hex(*b);
            break;
          }
          default:  {
            to += to_hex_rep(code);
            break;
          }
        }
        state = NONE;
        break;
      }
      default:  {
        throw std::logic_error("can't happen");
        break;
      }
    }
  }  // for

  return true;
}  // encode from String


// ----------------------------------------------------------------------
// vString
// ----------------------------------------------------------------------

bool
  mf::decode(std::vector<std::string> & to, std::string const& from)
{
  std::vector<std::string> temp;
  if(! split(std::back_inserter(temp), from, '{', ',', '}'))
    return false;

  to.clear();
  for(std::vector<std::string>::const_iterator  b = temp.begin()
                                             ,  e = temp.end()
      ; b != e ; ++b)
  {
    std::string  val;
    // treat blank string specially
    if(*b == "XXX") {
       val = "";
    } else if(! decode(val, *b)) {
      return false;
    }
    to.push_back(val);
  }

  return true;
}  // decode to vector<string>

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, std::vector<std::string> const& from)
{
  to = "{";

  std::string  converted;
  for(std::vector<std::string>::const_iterator b = from.begin()
                                              , e = from.end()
     ; b != e ; ++b)
  {
    // treat blank string specially
    if(*b == "") {
       converted = "XXX";
    } else if(! encode(converted, *b)) {
      return false;
    }

    if(b != from.begin())
      to += ",";
    to += converted;
  }

  to += '}';
  return true;
}  // encode from vector<string>


// ----------------------------------------------------------------------
// ParameterSet
// ----------------------------------------------------------------------

bool
  mf::decode(ParameterSet & to, std::string const& from)
{
  to = ParameterSet(from);
  return true;
}  // decode to ParameterSet

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, ParameterSet const& from)
{
  to = from.toString();
  return true;
}  // encode from ParameterSet


// ----------------------------------------------------------------------
// vPSet
// ----------------------------------------------------------------------

bool
  mf::decode(std::vector<ParameterSet> & to, std::string const& from)
{
  std::vector<std::string> temp;
  if(! split(std::back_inserter(temp), from, '{', ',', '}'))
    return false;

  to.clear();
  for(std::vector<std::string>::const_iterator  b = temp.begin()
                                              ,  e = temp.end()
      ; b != e ; ++b)
  {
    ParameterSet val;
    if(! decode(val, *b))
      return false;
    to.push_back(val);
  }

  return true;
}  // decode to vector<ParameterSet>

// ----------------------------------------------------------------------

bool
  mf::encode(std::string & to, std::vector<ParameterSet> const& from)
{
  to = "{";

  std::string  converted;
  for(std::vector<ParameterSet>::const_iterator b = from.begin()
                                       , e = from.end()
     ; b != e ; ++b)
  {
    if(! encode(converted, *b))
      return false;

    if(b != from.begin())
      to += ",";
    to += converted;
  }

  to += '}';
  return true;
}  // encode from vector<ParameterSet>


// ----------------------------------------------------------------------