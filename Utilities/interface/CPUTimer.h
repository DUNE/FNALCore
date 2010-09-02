#ifndef FWCore_Utilities_CPUTimer_h
#define FWCore_Utilities_CPUTimer_h
// -*- C++ -*-
//
// Package:     Utilities
// Class  :     CPUTimer
// 
/**\class CPUTimer CPUTimer.h FWCore/Utilities/interface/CPUTimer.h

 Description: Timer which measures the CPU and wallclock time

 Usage:
    <usage>

*/
//
// Original Author:  Chris Jones
//         Created:  Sun Apr 16 20:32:13 EDT 2006
// $Id: CPUTimer.h,v 1.3 2007/06/14 02:00:58 wmtan Exp $
//

// system include files
#include <sys/time.h>

// user include files

// forward declarations
namespace edm {
class CPUTimer
{

   public:
      CPUTimer();
      virtual ~CPUTimer();

      // ---------- const member functions ---------------------
      double realTime() const ;
      double cpuTime() const ;
      
      // ---------- static member functions --------------------

      // ---------- member functions ---------------------------
      void start();
      void stop();
      void reset();
      
   private:
      CPUTimer(const CPUTimer&); // stop default

      const CPUTimer& operator=(const CPUTimer&); // stop default

      struct Times {
        double real_;
        double cpu_;
      };
      
      Times calculateDeltaTime() const;
      
      // ---------- member data --------------------------------
      enum State {kRunning, kStopped} state_;
      struct timeval startRealTime_;
      struct timeval startCPUTime_;
      
      double accumulatedRealTime_;
      double accumulatedCPUTime_;
      
};
}

#endif