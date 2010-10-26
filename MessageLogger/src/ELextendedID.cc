// ----------------------------------------------------------------------
//
// ELextendedID.cc      is used as a key in maps for both counting toward
//                      limits on how many times a destination will react
//                      to a type of message, and for statistics.
//
// 07-Jul-1998 mf       Created file.
// 26-Aug-1998 WEB      Updated with ELseverityLevel in place of int.
//
// ----------------------------------------------------------------------


#include "MessageLogger/interface/ELextendedID.h"


namespace mf
{


// ----------------------------------------------------------------------
// Comparator:
// ----------------------------------------------------------------------

bool ELextendedID::operator<( const ELextendedID & xid ) const  {

  return
          ( process    < xid.process    )  ?  true
        : ( process    > xid.process    )  ?  false

        : ( severity   < xid.severity   )  ?  true
        : ( severity   > xid.severity   )  ?  false

        : ( id         < xid.id         )  ?  true
        : ( id         > xid.id         )  ?  false

        : ( module     < xid.module     )  ?  true
        : ( module     > xid.module     )  ?  false

        : ( subroutine < xid.subroutine )  ?  true
        : ( subroutine > xid.subroutine )  ?  false

        :                                     false
        ;

}  // ELextendedID::operator<()


// ----------------------------------------------------------------------
// (Re)initializer:
// ----------------------------------------------------------------------

void ELextendedID::clear()  {

  process    = "";
  id         = "";
  severity   = ELunspecified;
  module     = "";
  subroutine = "";
  hostname   = "";
  hostaddr   = "";
  application= "";
  pid        = 0;

}  // clear()


// ----------------------------------------------------------------------

} // end of namespace mf  */