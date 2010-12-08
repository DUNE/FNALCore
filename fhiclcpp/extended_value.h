#ifndef FHICLCPP__EXTENDED_VALUE_H
#define FHICLCPP__EXTENDED_VALUE_H

// ======================================================================
//
// extended_value
//
// ======================================================================

#include "boost/any.hpp"
#include <map>
#include <string>
#include <vector>

// ----------------------------------------------------------------------

namespace fhicl {

  class intermediate_table;
  class extended_value;

  enum value_tag { UNKNOWN
                 , NIL, BOOL, NUMBER, COMPLEX, STRING, SEQUENCE, TABLE
                 };

}  // fhicl

// ----------------------------------------------------------------------

struct fhicl::extended_value
{
  friend  class fhicl::intermediate_table;

public:
  typedef  std::string                            atom_t;
  typedef  std::pair<std::string, std::string>    complex_t;
  typedef  std::vector<extended_value>            sequence_t;
  typedef  std::map<std::string, extended_value>  table_t;

  extended_value( )
  : in_prolog( false )
  , tag      ( UNKNOWN )
  , value    ( )
  { }

  extended_value( bool       in_prolog
                , value_tag  tag
                , boost::any value
                )
  : in_prolog( in_prolog )
  , tag      ( tag )
  , value    ( value )
  { }

  bool
    is_a( value_tag t ) const
  { return t == tag; }

  std::string
    to_string( ) const;

  operator atom_t( ) const
  { return boost::any_cast<atom_t>(value); }

  operator complex_t( ) const
  { return boost::any_cast<complex_t>(value); }

  operator sequence_t( ) const
  { return boost::any_cast<sequence_t>(value); }

  operator table_t( ) const
  { return boost::any_cast<table_t>(value); }

  bool       in_prolog;
  value_tag  tag;
  boost::any value;

};  // extended_value

// ======================================================================

#endif  // FHICLCPP__EXTENDED_VALUE_H
