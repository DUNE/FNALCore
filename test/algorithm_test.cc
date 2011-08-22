// ======================================================================
//
// test "algorithm" header
//
// ======================================================================

#define BOOST_TEST_MODULE ( "algorithm test" )
#include "boost/test/auto_unit_test.hpp"

#include "cpp0x/algorithm"

bool
  is_positive( int x )
{ return x > 0; }

bool
  is_negative( int x )
{ return x < 0; }

bool
  is_zero( int x )
{ return x == 0; }

BOOST_AUTO_TEST_SUITE( algorithm_test )

BOOST_AUTO_TEST_CASE( minmax_element_test )
{
  //           0   1   2   3   4   5   6
  int a[] = {  5, 10, 15, 10,  5,  0, 20 };

  { std::pair<int*, int*> ans = std::minmax_element( a+0, a+0 );
    BOOST_CHECK_EQUAL( ans.first , a+0 );
    BOOST_CHECK_EQUAL( ans.second, a+0 );
  }

  { std::pair<int*, int*> ans = std::minmax_element( a+0, a+1 );
    BOOST_CHECK_EQUAL( ans.first , a+0 );
    BOOST_CHECK_EQUAL( ans.second, a+0 );
  }

  { std::pair<int*, int*> ans = std::minmax_element( a+0, a+2 );
    BOOST_CHECK_EQUAL( ans.first , a+0 );
    BOOST_CHECK_EQUAL( ans.second, a+1 );
  }

  { std::pair<int*, int*> ans = std::minmax_element( a+0, a+3 );
    BOOST_CHECK_EQUAL( ans.first , a+0 );
    BOOST_CHECK_EQUAL( ans.second, a+2 );
  }

  { std::pair<int*, int*> ans = std::minmax_element( a+0, a+4 );
    BOOST_CHECK_EQUAL( ans.first , a+0 );
    BOOST_CHECK_EQUAL( ans.second, a+2 );
  }

  { std::pair<int*, int*> ans = std::minmax_element( a+0, a+5 );
    BOOST_CHECK_EQUAL( ans.first , a+0 );
    BOOST_CHECK_EQUAL( ans.second, a+2 );
  }

  { std::pair<int*, int*> ans = std::minmax_element( a+0, a+6 );
    BOOST_CHECK_EQUAL( ans.first , a+5 );
    BOOST_CHECK_EQUAL( ans.second, a+2 );
  }

  { std::pair<int*, int*> ans = std::minmax_element( a+0, a+7 );
    BOOST_CHECK_EQUAL( ans.first , a+5 );
    BOOST_CHECK_EQUAL( ans.second, a+6 );
  }
}

BOOST_AUTO_TEST_CASE( is_sorted_test )
{
  int  a[] = { 1, 2, 3, 3, 4, 0 };
  BOOST_CHECK( std::is_sorted(a+0, a+0) );
  BOOST_CHECK( std::is_sorted(a+1, a+1) );
  BOOST_CHECK( std::is_sorted(a+0, a+1) );
  BOOST_CHECK( std::is_sorted(a+0, a+2) );
  BOOST_CHECK( std::is_sorted(a+0, a+3) );
  BOOST_CHECK( std::is_sorted(a+0, a+4) );
  BOOST_CHECK( std::is_sorted(a+0, a+5) );

  BOOST_CHECK( ! std::is_sorted(a+0, a+6) );
  BOOST_CHECK_EQUAL( std::is_sorted_until(a+0, a+6), a+5 );
}

BOOST_AUTO_TEST_CASE( all_of_test )
{
  int  a[] = { 1, 2, 3, -3, 4, 0 };
  BOOST_CHECK( std::all_of(a+0, a+0, is_positive) );
  BOOST_CHECK( std::all_of(a+1, a+1, is_positive) );
  BOOST_CHECK( std::all_of(a+0, a+1, is_positive) );
  BOOST_CHECK( std::all_of(a+0, a+2, is_positive) );
  BOOST_CHECK( std::all_of(a+0, a+3, is_positive) );
  BOOST_CHECK( ! std::all_of(a+0, a+4, is_positive) );
  BOOST_CHECK( ! std::all_of(a+0, a+5, is_positive) );
  BOOST_CHECK( ! std::all_of(a+0, a+6, is_positive) );
}

BOOST_AUTO_TEST_CASE( any_of_test )
{
  int  a[] = { 1, 2, 3, -3, 4, 0 };

  BOOST_CHECK( ! std::any_of(a+0, a+0, is_positive) );
  BOOST_CHECK( ! std::any_of(a+1, a+1, is_positive) );
  BOOST_CHECK( std::any_of(a+0, a+1, is_positive) );
  BOOST_CHECK( std::any_of(a+0, a+2, is_positive) );
  BOOST_CHECK( std::any_of(a+0, a+3, is_positive) );
  BOOST_CHECK( std::any_of(a+0, a+4, is_positive) );
  BOOST_CHECK( std::any_of(a+0, a+5, is_positive) );
  BOOST_CHECK( std::any_of(a+0, a+6, is_positive) );

  BOOST_CHECK( ! std::any_of(a+0, a+0, is_negative) );
  BOOST_CHECK( ! std::any_of(a+1, a+1, is_negative) );
  BOOST_CHECK( ! std::any_of(a+0, a+1, is_negative) );
  BOOST_CHECK( ! std::any_of(a+0, a+2, is_negative) );
  BOOST_CHECK( ! std::any_of(a+0, a+3, is_negative) );
  BOOST_CHECK( std::any_of(a+0, a+4, is_negative) );
  BOOST_CHECK( std::any_of(a+0, a+5, is_negative) );
  BOOST_CHECK( std::any_of(a+0, a+6, is_negative) );
}

BOOST_AUTO_TEST_CASE( none_of_test )
{
  int  a[] = { 1, 2, 3, -3, 4, 0 };
  BOOST_CHECK( std::none_of(a+0, a+0, is_negative) );
  BOOST_CHECK( std::none_of(a+1, a+1, is_negative) );
  BOOST_CHECK( std::none_of(a+0, a+1, is_negative) );
  BOOST_CHECK( std::none_of(a+0, a+2, is_negative) );
  BOOST_CHECK( std::none_of(a+0, a+3, is_negative) );
  BOOST_CHECK( ! std::none_of(a+0, a+4, is_negative) );
  BOOST_CHECK( ! std::none_of(a+0, a+5, is_negative) );
  BOOST_CHECK( ! std::none_of(a+0, a+6, is_negative) );
}

BOOST_AUTO_TEST_CASE( find_if_not_test )
{
  int  a[] = { 1, 2, 3, -3, 4, 0 };
  BOOST_CHECK_EQUAL( std::find_if_not(a+0, a+0, is_positive), a+0 );
  BOOST_CHECK_EQUAL( std::find_if_not(a+1, a+1, is_positive), a+1 );
  BOOST_CHECK_EQUAL( std::find_if_not(a+0, a+1, is_positive), a+1 );
  BOOST_CHECK_EQUAL( std::find_if_not(a+0, a+2, is_positive), a+2 );
  BOOST_CHECK_EQUAL( std::find_if_not(a+0, a+3, is_positive), a+3 );
  BOOST_CHECK_EQUAL( std::find_if_not(a+0, a+4, is_positive), a+3 );
  BOOST_CHECK_EQUAL( std::find_if_not(a+0, a+5, is_positive), a+3 );
  BOOST_CHECK_EQUAL( std::find_if_not(a+0, a+6, is_positive), a+3 );
}

BOOST_AUTO_TEST_SUITE_END()