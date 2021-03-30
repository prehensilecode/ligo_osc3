// Yo, Emacs! This is -*- C++ -*-
//
// $Id: Param.hh,v 1.1 2001/09/29 01:59:00 jzweizig Exp $
//
// Author: David Chin <dwchin@umich.edu>
//
//         2477 Randall Lab, Univ. of Michigan
//         500 E. University Ave.
//         Ann Arbor, MI 48109
//         +1-734-764-5146
//         +1-734-730-1274
//
// Written based on idea by K. Riles <kriles@umich.edu>

#ifndef OSCPARAM_HH
#define OSCPARAM_HH

#include <iostream>
#include <string>

#include "TSeries.hh"

class Dacc;

namespace osc
{
    /// Template class for parameter values.
    template <class C> class Param
    {
    public:
        /// Constructors
        Param() : mReadFromChanP(false) {};
        
        Param(const C &c) : mValue(c), mReadFromChanP(false) {};
        
        Param(const C *c) : mValue(*c), mReadFromChanP(false) {};

        /*
        Param(const Param &p)
            : mValue(p.value()),
              mReadFromChanP(p.readFromChanP()),
              mChannel(p.channel())
            {};
        */

        // If constructor is called with a string, assume it's a
        // channel name
        Param(std::string chan, Dacc *pDacc)
            : mReadFromChanP(true),
              mChannel(chan),
              pmDataAccess(pDacc)
            {
                // Make sure string is not empty
                if (mChannel.empty() == true) {
                    cerr << "Param: Error: channel name not set! Parameter "
                         << "value undefined. Aborting." << endl;
                    throw;
                }
            }
        

        /// Value of parameter
        C value() const
            {
                return mValue;
            }

        
        /// Predicate: value is to be read from channel
        bool readFromChanP() const { return mReadFromChanP; };

        /// Name of channel from which value is to be read
        std::string channel() const { return mChannel; };

        /// Set value
        void setValue(C val) { mValue = val; };

    private:
        /// Value of parameter
        C mValue;

        /// Predicate: value is to be read from channel
        bool mReadFromChanP;

        /// Name of channel from which value is to be read
        std::string mChannel;

        /// Pointer to TSeries into which to read data
        TSeries *pmTimeSeries;

        // *sigh* STILL can't get rid of pointers
        /// Pointer to a data accessor
        Dacc *pmDataAccess;
    };

};  // namespace osc

#endif // !defined OSCPARAM_HH
