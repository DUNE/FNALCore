// ======================================================================
//
// test "functional" header
//
// ======================================================================


#include "cpp0x/functional"


int v( int i,int j ) { return j; }
std::function<int(int)>  f( std::bind(v, 1, std::placeholders::_1) );


int
  main( )
{

  return f(0);

}  // main()
