// ======================================================================
//
// include - file ->string, interpolating #include'd files along the way
//
// ======================================================================

#include "cetlib/include.h"

#include "cetlib/coded_exception.h"
#include "cetlib/search_path.h"

// ----------------------------------------------------------------------

namespace cet { namespace detail {

  enum error { cant_open
             , cant_read
             , malformed
             };

  std::string
    translate( error code )
  {
    switch( code ) {
      case cant_open  : return "Can't locate or can't open specified file";
      case cant_read  : return "Can't read from supplied input stream";
      case malformed  : return "Malformed #include directive:";
      default         : return "Unknown code";
    }
  }

  typedef  cet::coded_exception<error,translate>
           include_exception;

} }  // cet::detail

using namespace cet::detail;

// ----------------------------------------------------------------------

void
  cet::include( std::istream & in
              , std::string  & result
              )
{
  static std::string const include_lit("#include \"");
  static std::size_t const include_sz = include_lit.size();

  if( ! in )
    throw include_exception(cant_read);

  for( std::string line; std::getline(in, line);  ) {
    if( line.find(include_lit) != 0 )  // ordinary line
      result.append(line)
            .append(1, '\n');
    else if( line.end()[-1] != '\"' )  // #include is missing trailing quote
      throw include_exception(malformed) << line;
    else {
      std::ifstream f( line.substr( include_sz
                                  , line.size() - include_sz - 1
                                  ).c_str()
                     , std::ios_base::in
                     );
      if( ! f )
        throw include_exception(cant_open) << line;
      include(f, result);
    }
  }  // for

}  // include()

// ----------------------------------------------------------------------

void
  cet::include( std::istream & in
              , std::string const & search_path_arg
              , std::string  & result
              )
{
  static std::string const include_lit("#include \"");
  std::size_t include_sz = include_lit.size();

  if( ! in )
    throw include_exception(cant_read);

  for( std::string line; std::getline(in, line);  ) {
    if( line.find(include_lit) != 0 )  // ordinary line
      result.append(line)
            .append(1, '\n');
    else if( line.end()[-1] != '\"' )  // #include is missing trailing quote
      throw include_exception(malformed) << line;
    else {
      cet::search_path paths(search_path_arg);
      std::string fname( line.substr( include_sz
                                    , line.size() - include_sz - 1
                       )            );
      std::ifstream f( paths.find_file(fname).c_str()
                     , std::ios_base::in
                     );
      if( ! f ) {
        std::string printed_paths = paths[0];
        for( int k = 1; k != paths.size(); ++k )
          printed_paths += ':' + paths[k];
        throw include_exception(cant_open) << fname
          << "\nusing path: " << printed_paths;
      }
      include(f, result);
    }
  }  // for

}  // include()

// ======================================================================
