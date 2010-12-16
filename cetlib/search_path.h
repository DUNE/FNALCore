#ifndef CETLIB__SEARCH_PATH_H
#define CETLIB__SEARCH_PATH_H

// ======================================================================
//
// search_path: seek filename or pattern in a given list of pathnames
//
// ======================================================================

#include "cetlib/split.h"
#include <algorithm>  // copy
#include <cstdlib>    // size_t
#include <string>
#include <vector>

namespace cet {
  class search_path;
}

// ----------------------------------------------------------------------

class cet::search_path
{
public:
  //c'tor:
  search_path( std::string const & name_or_path );

  // observers:
  bool
    empty( ) const  { return dirs.empty(); }
  std::size_t
    size( ) const  { return dirs.size(); }
  std::string const &
    operator [] ( int k ) const  { return dirs.at(k); }

  // workers:
  std::string
    find_file( std::string const & filename ) const;
  bool
    find_file( std::string filename, std::string & result ) const;
  std::size_t
    find_files( std::string const        & filename_pattern
              , std::vector<std::string> & result
              ) const;
  template< class OutIter >
  std::size_t
    find_files( std::string const & filename_pattern
              , OutIter             dest
              ) const;

private:
  std::vector<std::string>                 dirs;
  std::vector<std::string>::const_iterator end;

};  // search_path

// ======================================================================

template< class OutIter >
std::size_t
  cet::search_path::find_files( std::string const & pattern
                              , OutIter             dest
                              ) const
{
  std::vector<std::string> results;
  size_t nfound = find_files(pattern, results);
  std::copy( results.begin(), results.end()
           , dest
           );
  return nfound;
}

// ======================================================================

#endif  // CETLIB__SEARCH_PATH_H
