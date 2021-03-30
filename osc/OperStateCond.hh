// Yo, Emacs! This is -*- C++ -*-
//
// $Id: OperStateCond.hh,v 1.9 2003/04/11 21:24:10 jzweizig Exp $
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

#ifndef OPERSTATECOND_HH
#define OPERSTATECOND_HH


#include "PConfig.h"
#include <time.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstdio>

#include "DVector.hh"
//#include "Time.hh"
//#include "TSeries.hh"
//#include "FSeries.hh"
//#include "FSpectrum.hh"
#include "Dacc.hh"
#include "Complex.hh"

#include "misc.hh"

//class Dacc;
class Time;
class TSeries;
class FSeries;
class FSpectrum;

namespace osc
{
    
    /** OperStateCond will be a base class for the various types (classes)
     * of Operating State Conditions.
     */
    class OperStateCond
    {
    public:
        /// Constructors
        OperStateCond(Dacc        *access,
                      std::ostream     *outstream, // pointer to output stream
                      std::string  chanName,  // name of associated channel
                      std::string  oscname,   // type of OSC
                      bool         isAtomic,
                      int          debug = 0);
        
        OperStateCond(Dacc           *access,
                      std::ostream        *outstream, // pointer to output stream
                      std::string     chanName,  // name of associated channel
                      std::string     oscname,   // type of OSC
                      bool            isAtomic,
                      osc::IntParMap &intparams,
                      osc::DblParMap &dblparams,
                      int             debug = 0);
    
        /// Destructor
        virtual ~OperStateCond();
    
        
        //
        // ACCESSORS
        //
    
        /// Get channel name
        std::string channel() const;
    
        /// Get data series pointer
        TSeries* dataSeries() const;
    
        /// Get data stride
        Interval stride() const;
        
        /// Get debug level
        int debugLevel() const;
    
        /// Name of this OSC (i.e. its type)
        std::string name() const;

        /// Predicate: this OSC is atomic
        bool atomicP() const;
    
        //
        // MISC
        //

        /// Sets debug level
        void setDebug(int debuglvl);
    
        /// Predicate: this OperStateCond is satisfied
        virtual bool satisfied() = 0;
    
        /// Prints info about self
        virtual void printInfo() const;

        
        /// Return value of "watched" quantity/variable
        virtual double watchedQuantity() const
            {
                if (mDebug > 0) {
                    printf("osc::OperStateCond::watchedQuantity() = %12.7e\n",
                           mCurResults.mDoubleVal);
                }
                return mCurResults.mDoubleVal;
            }

        /// Updates values of parameters from channels, maybe
        void updateParamsMaybe();

    
    protected:
        /// Data accessor
        Dacc         *pmDataAccess;
    
        /// Output stream
        std::ostream      *pmOut;
    
        /// Name of channel from which data is to be read
        std::string   mChannel;

        /// Name of this OSC (i.e. its type)
        std::string   mName;

        /// Predicate: this OSC is atomic
        bool          mAtomicP;
    
        /// Debug level
        int           mDebug;

        /// Type of data to be read in.
        tseriesType_t mDataType;
    
        
        // For now, we only have integer and double parameters
        
        /// Integer parameters
        IntParMap mIntParams;
    
        /// Double parameters
        DblParMap mDblParams;

        /// Store results from previous Frame and current Frame
        typedef struct OscResults_tag {
            Time     mT0;
            bool     mSatisfiedP;
            double   mDoubleVal;
            float    mFloatVal;
            int      mIntVal;
            short    mShortVal;
            fComplex mFcomplexVal;
        } OscResults;
    
        /// Stored results from previous Frame
        OscResults mPrevResults;
    
        /// Stored results from current Frame
        OscResults mCurResults;

        /// Print param info
        void printParamInfo() const;

        
        //
        // Exception classes
        //

        // Base error class 
        class Error
        {
        public:
            virtual void debugPrint() const
                {
                    std::cerr << "OperStateCond::Error";
                }
        };

        // Data read errors
        class DataRead_err
            : public Error
        {
        public:
            DataRead_err(std::string osc, std::string chan)
                : mOSC(osc),
                  mChannel(chan)
                {};
            
            virtual ~DataRead_err() {};
            
            std::string channel() const { return mChannel; };

            virtual void printErrMsg() const
                {
                    std::cerr << "osc::" << mOSC << ": error reading parameter "
                         << "value from channel '" << mChannel
                         << "'" << std::endl;
                }
            
            virtual void debugPrint() const
                {
                    std::cerr << "OperStateCond::DataRead";
                }

        private:
            /// Name of OSC that called this
            std::string mOSC;
            
            /// Name of channel for which error occured
            std::string mChannel;
        };
        

        //
        // utility functions
        //

        /// Transfer current results to previous results
        void updateResultsBuffer();

        /// Not-A-Number error
        void nanErrMesg() const;

        /** @memo Read param values from channel, if needed
         *  @return None. Fills param values from channels as needed.
         */
        void readParamsFromChanMaybe(void);

        /// Compute RMS (mean power) of time series data in current frame
        double rms(const TSeries *ptx) const;

        /// Compute mean power of freq. series data in current frame
        double rms(const FSeries *pfx) const;

        /// Compute mean power of freq. spectrum data in current frame
        double rms(const FSpectrum *pfs) const;

    private:
        /// Initializer
        void init();
    };  // END class OperStateCond
    
    
    
    
    //
    // Returns debug level
    //
    inline int OperStateCond::debugLevel() const
    {
        return mDebug;
    }

    
    //
    // Returns input channel name
    //
    inline std::string OperStateCond::channel() const
    {
        return mChannel;
    }


    //
    // Returns name of this OSC (i.e. its type)
    //
    inline std::string OperStateCond::name() const
    {
        return mName;
    }
    

    //
    // Is OSC atomic? 
    //
    inline bool OperStateCond::atomicP() const
    {
        return mAtomicP;
    }

    
    //
    // Sets debug level
    //
    inline void OperStateCond::setDebug(int debuglvl)
    {
        std::cerr << "OperStateCond: " << mName << " setting debug level to "
             << debuglvl << std::endl;
        mDebug = debuglvl;
    }

    //
    // Transfers current results to previous results
    //
    inline void OperStateCond::updateResultsBuffer()
    {
        mPrevResults.mT0 = mCurResults.mT0;
        mPrevResults.mSatisfiedP = mCurResults.mSatisfiedP;
        mPrevResults.mDoubleVal = mCurResults.mDoubleVal;
        mPrevResults.mFloatVal = mCurResults.mFloatVal;
        mPrevResults.mIntVal = mCurResults.mIntVal;
        mPrevResults.mShortVal = mCurResults.mShortVal;
        mPrevResults.mFcomplexVal = mCurResults.mFcomplexVal;
    }

    //
    // Prints out NaN error message
    //
    inline void OperStateCond::nanErrMesg() const
    {
        *pmOut << "Warning: osc::" << mName << ": not-a-number error"
               << std::endl;
    }


    // A list of all the types of OSCs
    
    //      * boolean
    //      * valuerange
    //      * valueabove
    //      * valuebelow
    //      * meanrange
    //      * meanabove
    //      * meanbelow
    //      * rmsrange
    //      * rmsabove
    //      * rmsbelow
    //      * bitandany
    //      * bitnandany
    //      * bitorany
    //      * bitnorany
    //      * bitandall
    //      * bitnandall
    //      * bitorall
    //      * bitnorall
    //      * abspowerabove
    //      * abspowerbelow
    //      * fractpowerabove
    //      * fractpowerbelow
    //      * abspoweranyrise
    //      * abspoweranyfall
    //      * fractpoweranyrise
    //      * fractpoweranyfall
    //      * poweranymag
    //      * poweranydemag
    //      * abspoweraverise
    //      * abspoweravefall
    //      * fractpowerrise
    //      * fractpoweravefall
    //      * poweravemag
    //      * poweravedemag
    
} // namespace osc

#endif // !defined OPERSTATECOND_HH
    
