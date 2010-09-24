#ifndef CPP0X_MEMORY
#define CPP0X_MEMORY

// ======================================================================
//
// memory - accommodations for as-yet unsupported C++0X features
//
// ----------------------------------------------------------------------
//
// This source file is intended for only short-term use.  Once compilers
// support the necessary C++0X features, this file should be no longer
// used and should go away.
//
// ======================================================================


#include "_cpp0x_config_"

#include <memory>

// ----------------------------------------------------------------------
// std::shared_ptr, etc.

#if defined(CPP0X_HAS_MEMORY_SHARED_PTR)

#else
  #include <boost/smart_ptr/shared_ptr.hpp>
  #include <boost/smart_ptr/make_shared.hpp>
  namespace std {
    using boost::enable_shared_from_this;
    using boost::make_shared;
    using boost::shared_ptr;
    using boost::weak_ptr;
  }

#endif  // CPP0X_HAS_MEMORY_SHARED_PTR


// ----------------------------------------------------------------------
// std::unique_ptr, etc.

#if defined(CPP0X_HAS_MEMORY_UNIQUE_PTR)

#else
  #include "_unique_ptr_"
  namespace std {
    using boost::default_delete;
    using boost::unique_ptr;
  }

#endif  // CPP0X_HAS_MEMORY_UNIQUE_PTR


// ======================================================================


#endif  // CPP0X_MEMORY
