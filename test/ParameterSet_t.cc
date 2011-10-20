#define BOOST_TEST_MODULE ( ParameterSet test)

#include "boost/test/auto_unit_test.hpp"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>

using namespace fhicl;

struct SampleConfigFixture {
   SampleConfigFixture();

   ParameterSet pset;
};

SampleConfigFixture::SampleConfigFixture() {
   putenv(const_cast<char*>("FHICL_FILE_PATH=./test:."));
   cet::filepath_lookup policy("FHICL_FILE_PATH");
   intermediate_table tbl;
   std::string cfg_in("Sample.cfg");
   parse_document(cfg_in, policy, tbl);
   make_ParameterSet(tbl, pset);
}

BOOST_FIXTURE_TEST_SUITE ( sampleConfig, SampleConfigFixture )

BOOST_AUTO_TEST_CASE ( Local ) {
   fhicl::ParameterSet j;
   j.put<int>("y", -1);
   fhicl::ParameterSet orig(pset.get<fhicl::ParameterSet>("j"));
   BOOST_CHECK_EQUAL( j, orig );
   BOOST_CHECK_EQUAL( orig.get<int>("y"), -1 );
   BOOST_CHECK_EQUAL( pset.get<std::vector<int> >("m")[0], -1 );

   std::vector<std::string> const names = pset.get_keys();
   for( std::size_t k = 0; k != names.size(); ++k )
      BOOST_CHECK ( "x" != names[k] );
}

BOOST_AUTO_TEST_CASE ( DeepInjection ) {
   fhicl::ParameterSet l; l.put<int>("zz", -2);
   fhicl::ParameterSet k; k.put<fhicl::ParameterSet>("l", l);
   fhicl::ParameterSet orig(pset.get<fhicl::ParameterSet>("k"));
   BOOST_CHECK_EQUAL( k, orig );
   BOOST_CHECK_EQUAL( orig.get<fhicl::ParameterSet>("l")
                          .get<int>("zz")
                     , -2 );
   BOOST_CHECK_EQUAL( orig.get<int>("l.zz"), -2 );
}

BOOST_AUTO_TEST_CASE ( DoubleStringMismatchDefaulted ) {
   std::string s;
   BOOST_CHECK_MESSAGE( pset.get_if_present("e", s), "Failed to get string" );
   BOOST_CHECK_EQUAL( s, "rain" );

   try {
      pset.get<double>("e", 2.0);
      BOOST_FAIL("Failed to throw an exception as expected");
   }
   catch (fhicl::exception &e) {
      BOOST_CHECK_EQUAL( e.categoryCode(), type_mismatch );
   }
   catch (...) {
      BOOST_FAIL("Wrong exception type thrown");
   }
}

BOOST_AUTO_TEST_CASE ( DoubleStringMismatchNoDefault ) {
   try {
      pset.get<double>("e");
      BOOST_FAIL("Failed to throw an exception as expected");
   }
   catch (fhicl::exception &e) {
      BOOST_CHECK_EQUAL( e.categoryCode(), type_mismatch );
   }
   catch (...) {
      BOOST_FAIL("Wrong exception type thrown");
   }
}

BOOST_AUTO_TEST_CASE ( StringSuccess ) {
   BOOST_CHECK_EQUAL( "rain", pset.get<std::string>("e") );
}

BOOST_AUTO_TEST_CASE ( NotFound ) {
   try {
      pset.get<double>("zz");
      BOOST_FAIL("Failed to throw an exception as expected");
   }
   catch (fhicl::exception &e) {
      BOOST_CHECK_EQUAL( e.categoryCode(), cant_find );
   }
   catch (...) {
      BOOST_FAIL("Wrong exception type thrown");
   }
}

BOOST_AUTO_TEST_CASE (DeepVector) {
   typedef std::vector<std::vector<uint32_t> > vv_t;
   vv_t vv;
   BOOST_CHECK_NO_THROW( vv = pset.get<vv_t>("vv") );
   BOOST_CHECK_EQUAL( vv.front().back(), 3u );
   BOOST_CHECK_EQUAL( vv.back().front(), 2u );
   BOOST_CHECK_EQUAL( vv.back().back() , 4u );
}

unsigned
  ctox( char c )
{
  switch( c ) {
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
    return c - '0';
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    return 10 + c - 'a';
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    return 10 + c - 'A';
  default:
    throw std::string("ctox(): invalid hex character");
  }
}

unsigned
  hex( std::string const & from )
{
  if( from.size() < 3 || from[0] != '0' || (from[1] != 'x' && from[1] != 'X') )
    throw std::string("hex(): invalid argument");

  unsigned result = 0u;
  for( std::string::const_iterator it = from.begin() + 2
                                 , e  = from.end(); it != e; ++it )
    result = 16u * result + ctox(*it);
  return result;
}

BOOST_AUTO_TEST_CASE( Custom ) {
   BOOST_CHECK_EQUAL( pset.get<std::string>("n"), "0x123" );
   unsigned u;
   BOOST_CHECK( pset.get_if_present("n", u, hex) );
   BOOST_CHECK_EQUAL( pset.get<unsigned>("n", hex)
                    , u );
   BOOST_CHECK_EQUAL( pset.get<unsigned>("n", hex)
                    , (((1u) * 16u + 2u) * 16u + 3u)
                    );
   BOOST_CHECK_THROW( pset.get_if_present("e", u, hex), std::string );
}

BOOST_AUTO_TEST_SUITE_END()
