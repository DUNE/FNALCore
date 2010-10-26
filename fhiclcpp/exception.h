#ifndef FHICLCPP__EXCEPTION_H
#define FHICLCPP__EXCEPTION_H

// ======================================================================
//
// exception
//
// ======================================================================


#include "cetlib/coded_exception.h"
#include <ostream>
#include <string>


namespace fhicl {
  enum error { cant_find
             , cant_insert
             , cant_happen
             , number_is_too_large
             , type_mismatch
             , parse_error
             };

  namespace detail {
    std::string translate( error );
  }

  typedef  cet::coded_exception<error,detail::translate>
           exception;
}

// ======================================================================

#endif  // FHICLCPP__EXCEPTION_H
