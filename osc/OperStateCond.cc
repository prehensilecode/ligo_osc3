// Yo, Emacs! This is -*- C++ -*-
//
// $Id: OperStateCond.cc,v 1.7 2002/06/26 14:20:16 dwchin Exp $
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

#include <time.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>

#include "FSeries.hh"
#include "FSpectrum.hh"

#include "Sine.hh"
#include "osc/OperStateCond.hh"

using namespace std;

//
// Constructor
//
osc::OperStateCond::OperStateCond(Dacc       *access,
                                  ostream    *outstream,
                                  std::string chanName, // name of assoc. chan
                                  std::string oscName,
                                  bool        isAtomic,
                                  int         debug)
    : pmDataAccess(access),
      pmOut(outstream),
      mChannel(chanName),
      mName(oscName),
      mAtomicP(isAtomic),
      mDebug(debug)
{
    init();
}

osc::OperStateCond::OperStateCond(Dacc       *access,
                                  ostream    *outstream,
                                  std::string chanName, // name of assoc. chan
                                  std::string oscName,
                                  bool        isAtomic,
                                  osc::IntParMap &intparams,
                                  osc::DblParMap &dblparams,
                                  int         debug)
    : pmDataAccess(access),
      pmOut(outstream),
      mChannel(chanName),
      mName(oscName),
      mAtomicP(isAtomic),
      mDebug(debug),
      mIntParams(intparams),
      mDblParams(dblparams)
{
    init();
}

//
// Destructor
//
osc::OperStateCond::~OperStateCond()
{
    // Remove channel on deletion so that Dacc doesn't keep
    // looking for the channel
    //   Can't do this because we can't assume that other OSCs
    //   will not want to use this same channel.
    //pmDataAccess->rmChannel(mChannel.c_str());

    // Ideally, this should be done here with the help of some static
    // channel utilization hash_map.  But, I'm doing it in OperStateCondList.
}

//
// Print information
//
void osc::OperStateCond::printInfo() const
{
    cout << "osc::" << mName << " on '" << mChannel << "'";

    printParamInfo();
    
} // END: printInfo()


//
// get TSeries pointer
//
TSeries* osc::OperStateCond::dataSeries() const
{
    return pmDataAccess->refData(mChannel.c_str());
}

//
// get data stride
//
Interval osc::OperStateCond::stride() const
{
    return pmDataAccess->getStride();
}



//
// printParamInfo()
//
void osc::OperStateCond::printParamInfo() const
{
    if (mDebug > 2) {
        cout << "OperStateCond::printParamInfo() entered" << endl;
    }
    
    osc::IntParMap::const_iterator iiter = mIntParams.begin();
    for(; iiter != mIntParams.end(); ++iiter) {
        cout << "; " << (*iiter).first;
        
        if ((*iiter).second.readFromChanP() == false) {
            if ((*iiter).first == "mask")  // print out mask as Hex
                printf(" = 0x%x = %d", (*iiter).second.value(),
                          (*iiter).second.value());
            else
                cout << " = " << (*iiter).second.value();
        } else 
            cout << " read from '" << (*iiter).second.channel() << "'";
    }

    osc::DblParMap::const_iterator diter = mDblParams.begin();
    for(; diter != mDblParams.end(); ++diter) {
        cout << "; " << (*diter).first;
        
        if ((*diter).second.readFromChanP() == false)
            cout << " = " << (*diter).second.value();
        else 
            cout << " read from '" << (*diter).second.channel() << "'";
    }
    
    cout << endl;
    
} // END: printParamInfo()


//
// readParamsFromChanMaybe()
//
void osc::OperStateCond::readParamsFromChanMaybe(void)
{
    //
    // Integer params
    //
    
    // If have to read param value from channel,
    // take the first value in the time series
    osc::IntParMap::iterator iiter = mIntParams.begin();
    for (; iiter != mIntParams.end(); ++iiter) {
        if ((*iiter).second.readFromChanP() == true) {
            TSeries *ptxpar =
                pmDataAccess->refData((*iiter).second.channel().c_str());

            // If time series is empty, throw exception
            if (ptxpar->getNSample() <= 0 || ptxpar->isEmpty() == true) {
                throw DataRead_err(mName, (*iiter).second.channel());
            }

            // Read the parameter value
            int parval;

            size_t nParRead = ptxpar->getData(1, &parval);

            // if error, throw exception
            if (nParRead != 1) {
                throw DataRead_err(mName, (*iiter).second.channel());
            }

            // take the first data point in Frame for param value
            (*iiter).second.setValue(parval);

            if (mDebug > 0) {
                cout << "osc::" << mName << ": read "
                     << (*iiter).first << " value from '"
                     << (*iiter).second.channel()
                     << "': " << (*iiter).second.value() << endl;
            }
        }
    }

    //
    // Double params
    //

    // If have to read param value from channel,
    // take the first value in the time series
    osc::DblParMap::iterator diter = mDblParams.begin();
    for(; diter != mDblParams.end(); ++diter) {
        if ((*diter).second.readFromChanP() == true) {
            TSeries *ptxpar =
                pmDataAccess->refData((*diter).second.channel().c_str());
                
            // If time series is empty, throw exception
            if (ptxpar->getNSample() <= 0 || ptxpar->isEmpty() == true) {
                throw DataRead_err(mName, (*diter).second.channel());
            }

            double parval;
            size_t nParRead = ptxpar->getData(1, &parval);

            if (nParRead != 1) {
                throw DataRead_err(mName, (*diter).second.channel());
            }

            // take the first data point in Frame for param value
            (*diter).second.setValue(parval);

            if (mDebug > 0) {
                cout << "osc::" << mName << ": read "
                     << (*diter).first << " value from '"
                     << (*diter).second.channel()
                     << "': " << (*diter).second.value() << endl;
            }
        }
    }
}

// NOTE: Relationship between FSeries and FSpectrum is:
//   FSeries   fseries;
//   FSpectrum fspectrum(fseries);
//   fspectrum[i] == |fseries[i]|^2  (i.e. mag. squared)
//
// Now, FSeries.hh says,
//    for Y = fft(X)
//    Sum(|Yi|^2) = Sum(Xi^2)/N
//
// So, define rms in time domain to be sqrt(sum(xi^2)/N)
//
// Note: this quantity depends on the number of samples



//
// rms()
//
double osc::OperStateCond::rms(const TSeries *ptx) const
{
    //return sqrt((*ptx)*(*ptx) * (double)(ptx->getTStep()));
    return sqrt((*ptx)*(*ptx) / (double)(ptx->getNSample()));
}

//
// rms()
//
double osc::OperStateCond::rms(const FSeries *pfx) const
{
    FSpectrum fs(*pfx);

    return sqrt(fs.getSum(fs.getLowFreq(),
                          fs.getHighFreq() - fs.getLowFreq())
                * fs.getFStep());
}


//
// rms()
//
double osc::OperStateCond::rms(const FSpectrum *pfs) const
{
    return sqrt(pfs->getSum(pfs->getLowFreq(),
                            pfs->getHighFreq() - pfs->getLowFreq())
                * pfs->getFStep());
}

//
// Initializer
//
void osc::OperStateCond::init()
{
    if (mDebug > 0) {

        if (mAtomicP == true) {
            cout << "osc::" << mName << " on '" << mChannel
                 << "' constructed" << endl;
            printParamInfo();
        }

        pmDataAccess->setDebug(mDebug);

        if (mDebug > 1)
            pmDataAccess->list(cout);

        if (mDebug > 2)
            pmDataAccess->listFiles(cout);
    }

    // If channel name is non-null
    if (mChannel.length() > 0) {
        // Add it if it hasn't been added already
        if (pmDataAccess->isChannelRead(mChannel.c_str()) == false) {
            if (mDebug > 1) {
                cout << "OperStateCond: channel " << mChannel
                     << " not read yet: adding." << endl;
            }
            pmDataAccess->addChannel(mChannel.c_str());
        } 
    }
    
    // Initialize results buffers
    mPrevResults.mT0 = mCurResults.mT0 = Time(0, 0);
    mPrevResults.mSatisfiedP = mCurResults.mSatisfiedP = false;
    mPrevResults.mDoubleVal = mCurResults.mDoubleVal = 0.;
    mPrevResults.mFloatVal  = mCurResults.mFloatVal = 0.;
    mPrevResults.mIntVal = mCurResults.mIntVal = 0;
    mPrevResults.mShortVal = mCurResults.mShortVal = 0;
    mPrevResults.mFcomplexVal = mCurResults.mFcomplexVal = fComplex();
} // END: init()

