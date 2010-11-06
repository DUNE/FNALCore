// ======================================================================
//
// ParameterSet
//
// ======================================================================


#include "fhiclcpp/ParameterSet.h"

#include "fhiclcpp/ParameterSetRegistry.h"
#include <limits>
#include <utility>  // pair

using namespace boost;
using namespace fhicl;
using namespace std;

typedef  long double  ldbl;

// ======================================================================

static inline  bool
  is_table( any const & val )
{ return val.type() == typeid(ParameterSetID); }

static  string
  escape( string const & str )
{
  string result;
  for( string::const_iterator it = str.begin()
                            , e  = str.end(); it != e; ++it ) {
    switch( *it )  {
    case '\"':  result.append("\\\"");  continue;
    case '\'':  result.append("\\\'");  continue;
    case '\\':  result.append("\\\\");  continue;
    case '\n':  result.append("\\n" );  continue;
    case '\t':  result.append("\\t" );  continue;
    default  :  result.append(1, *it);  continue;
    }
  }
  return result;
}  // escape()

static  string
  unescape( string const & str )
{
  string result;
  for( string::const_iterator it = str.begin()
                            , e  = str.end(); it != e; ++it ) {
    char ch = *it;
    if( ch == '\\' && it != e-1 ) {
      switch( *++it ) {
      case '"' :  ch = '"' ;  continue;
      case '\'':  ch = '\'';  continue;
      case '\\':  ch = '\\';  continue;
      case 'n' :  ch = '\n';  continue;
      case 't' :  ch = '\t';  continue;
      default  :  throw fhicl::exception(type_mismatch, "unknown escape: ")
                    << str;
      }
    }
    result.append(1, ch);
  }
  return result;
}  // unescape()

// ----------------------------------------------------------------------

static inline  string
  fhicl_nil( )
{
  static  string  fhicl_nil("nil");
  return fhicl_nil;
}

static inline  string
  fhicl_true( )
{
  static  string  fhicl_true("true");
  return fhicl_true;
}

static inline  string
  fhicl_false( )
{
  static  string  fhicl_false("false");
  return fhicl_false;
}

static inline  string
  fhicl_infinity( )
{
  static  string  fhicl_infinity("infinity");
  return fhicl_infinity;
}

// ----------------------------------------------------------------------

string
  ParameterSet::stringify( any const & a ) const
{
  if( is_table(a) ) {
    ParameterSetID psid = any_cast<ParameterSetID>(a);
    return '{' + ParameterSetRegistry::get(psid).to_string() + '}';
  }
  else {
    return any_cast<string>(a);
    string str;
    decode( a, str );
    return str;
  }
}

// ----------------------------------------------------------------------

bool
  ParameterSet::is_empty( ) const
{ return mapping_.empty(); }

ParameterSetID
  ParameterSet::id( ) const
{
  if( ! id_.is_valid() )
    id_.reset(*this);
  return id_;
}

string
  ParameterSet::to_string( ) const
{
  string result;
  if( mapping_.empty() )
    return result;

  map_iter_t it = mapping_.begin();
  result.append( it->first )
        .append( 1, ':' )
        .append( stringify(it->second) )
        ;
  for( map_iter_t const e = mapping_.end(); ++it != e; )
    result.append( 1, ' ' )
          .append( it->first )
          .append( 1, ':' )
          .append( stringify(it->second) )
          ;

  return result;
}

vector<string>
  ParameterSet::get_keys( ) const
{
  vector<string> keys;  keys.reserve( mapping_.size() );
  for( map_iter_t it = mapping_.begin()
                , e  = mapping_.end(); it != e; ++it )
    keys.push_back( it->first );
  return keys;
}

vector<string>
  ParameterSet::get_pset_keys( ) const
{
  vector<string> keys;
  for( map_iter_t it = mapping_.begin()
                , e  = mapping_.end(); it != e; ++it )
    if( is_table(it->second) )
      keys.push_back( it->first );
  return keys;
}

// ----------------------------------------------------------------------

void
  ParameterSet::insert( string const & key, any const & value)
{
  mapping_.insert( pair<string const,any>(key, value) );
  id_.invalidate();
}

// ----------------------------------------------------------------------

string  // string with quotes iff needed
  ParameterSet::encode( string const & value ) const
{
  bool needquotes = false;
  string result;

  if( value.empty() )
    needquotes = true;

  else if( value[0] == '"' && value.end()[-1] == '"' ) {
    result = unescape( value.substr(1, value.size()-2) );
    needquotes = result.find_first_of("\"\'\\\n\t ") != string::npos;
    result = escape(result);
  }

  else if( value[0] == '\'' && value.end()[-1] == '\'' ) {
    result = escape( value.substr(1, value.size()-2) );
    needquotes = value.find_first_of("\"\'\\\n\t ") != string::npos;
  }

  else {
    result = escape( value );
    needquotes = value.find_first_of("\"\'\\\n\t ") != string::npos;
  }

  return needquotes  ?  '"' + result + '"'
                     :        result;
}

string  // nil
  ParameterSet::encode( void * ) const
{
  return fhicl_nil();
}

string  // bool
  ParameterSet::encode( bool value ) const
{
  return value ? fhicl_true() : fhicl_false();
}

ParameterSetID  // table
  ParameterSet::encode( ParameterSet const & value ) const
{
  return ParameterSetRegistry::put(value);
}

string  // unsigned
  ParameterSet::encode( uintmax_t value ) const
{
  string result = lexical_cast<string>(value);
  if( result.size() > 6 ) {
    size_t sz = result.size() - 1;
    result.insert(1, ".");
    result += "e+" + lexical_cast<string>(sz);
  }
  return result;
}

string  // signed
  ParameterSet::encode( intmax_t value ) const
{
  string result = encode( uintmax_t(abs(value)) );
  if( value < 0 )
    result.insert(0, "-");
  return result;
}

string  // floating-point
  ParameterSet::encode( ldbl value ) const
{
  if( value == numeric_limits<ldbl>::infinity() )
    return '+' + fhicl_infinity();
  if( value == - numeric_limits<ldbl>::infinity() )
    return '-' + fhicl_infinity();

  intmax_t chopped = static_cast<intmax_t>( value );
  if( static_cast<ldbl>(chopped) == value )
    return encode(chopped);

  return lexical_cast<string>(value);
}

// ----------------------------------------------------------------------

void  // string without quotes
  ParameterSet::decode( boost::any const & a, string & result ) const
{
  result = any_cast<string>(a);
  if( result[0] == '"' || result.end()[-1] == '"' )
    result = unescape( result.substr(1, result.size()-2) );
}

void  // nil
  ParameterSet::decode( boost::any const & a, void * & result ) const
{
  string str = any_cast<string>(a);
  if( str == fhicl_nil() )
    result = 0;
  else
    throw fhicl::exception(type_mismatch, "invalid nil string: ")
      << str;
}

void  // bool
  ParameterSet::decode( boost::any const & a, bool & result ) const
{
  string str = any_cast<string>(a);
  if( str == fhicl_true() )
    result = true;
  else if( str == fhicl_false() )
    result = false;
  else
    throw fhicl::exception(type_mismatch, "invalid bool string: ")
      << str;
}

void  // table
  ParameterSet::decode( boost::any const & a, ParameterSet & result ) const
{
  ParameterSetID id = any_cast<ParameterSetID>(a);
  result = ParameterSetRegistry::get(id);
}

void  // unsigned
  ParameterSet::decode( boost::any const & a, uintmax_t & result ) const
{
  string str = any_cast<string>(a);
  ldbl via = boost::lexical_cast<ldbl>(str);
  result = boost::numeric_cast<uintmax_t>(via);
}

void  // signed
  ParameterSet::decode( boost::any const & a, intmax_t & result ) const
{
  string str = any_cast<string>(a);
  ldbl via = boost::lexical_cast<ldbl>(str);
  result = boost::numeric_cast<intmax_t>(via);
}

void  // floating-point
  ParameterSet::decode( boost::any const & a, ldbl & result ) const
{
  string str = any_cast<string>(a);
  if( str.substr(1) == fhicl_infinity() )  {
    switch( str[0] ) {
    case '+': result = + numeric_limits<ldbl>::infinity(); return;
    case '-': result = - numeric_limits<ldbl>::infinity(); return;
    default : throw fhicl::exception(type_mismatch, "invalid float string: ")
                << str;
    }
  }
  else
    result = boost::lexical_cast<ldbl>(str);
}

void  // complex
  ParameterSet::decode( boost::any const & a, complex<ldbl> & result ) const
{
  string str = any_cast<string>(a);
  size_t comma = str.find(',');
  if(  str.empty()
    || str[0] != '('
    || str.end()[-1] != ')'
    || comma == string::npos
    )
    throw fhicl::exception(type_mismatch, "invalid complex string: ")
      << str;

  size_t end = str.size() - 1 - comma - 1;
  ldbl real, imag;
  decode( str.substr(1,comma-1)  , real );
  decode( str.substr(comma+1,end), imag );
  result = complex<ldbl>(real,imag);
}

// ======================================================================
