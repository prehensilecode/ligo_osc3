// Yo, Emacs! This is -*- C++ -*-
//
// $Id: FractPowerRise.cc,v 1.2 2002/05/01 18:47:43 dwchin Exp $
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

#include "osc/FractPowerRise.hh"

using namespace std;

osc::FractPowerRise::FractPowerRise(Dacc           *access,
                                ostream        *outstream,
                                std::string     chanName,
                                osc::IntParMap &intparams,
                                osc::DblParMap &dblparams,
                                int             debug)
    : OperStateCond(access, outstream, chanName, "FractPowerRise",
                    true, intparams, dblparams, debug)
{
    return;
}


double osc::FractPowerRise::powerFract(const TSeries *ptx,
                                       double         freqlo,
                                       double         freqhi)
{
    // construct FSeries from TSeries
    mFreqSeries = FSeries(*ptx);
    
    // construct FSpectrum from FSeries
    mSpectrum = FSpectrum(mFreqSeries);
    
    return ((mSpectrum.getSum(freqlo, freqhi-freqlo) * mSpectrum.getFStep())
            / mSpectrum.getSum(mSpectrum.getLowFreq(),
                               mSpectrum.getHighFreq() -
                               mSpectrum.getLowFreq()));
}


bool osc::FractPowerRise::satisfied()
{
    if (mDebug > 0) {
        cout << "osc::" << mName << ": entered satisfied()" << endl;
    }

    // Read in time-series data
    TSeries *ptx = dataSeries();
    
    // If the time series is empty, return False
    if (ptx->getNSample() <= 0 || ptx->isEmpty() == true)
        return false;

    // If this condition has already been checked for in the current Frame,
    // return the buffered result
    Time t0 = ptx->getStartTime();
    if (t0 == mCurResults.mT0) {
        return mCurResults.mSatisfiedP;
    } else {

        updateResultsBuffer();

        if (mDebug > 0)
            cout << "And the time at the beep is : "
                 << mPrevResults.mT0 << endl;

        //
        // Current results
        //
        mCurResults.mT0 = t0;

        try {
            readParamsFromChanMaybe();
        }

        catch (DataRead_err &err) {
            err.printErrMsg();
            return false;
        }
        
        // If the deque is full, make space for current results
        if ((int)(mPowerHist.size()) == mIntParams["nstrides"].value())
            mPowerHist.pop_back();

        // push current band-limited rms (power) to front of deque
        mPowerHist.push_front(powerFract(ptx, mDblParams["freqlo"].value(),
                                         mDblParams["freqhi"].value()));

        if (mDebug > 0) {
            cout << "osc::fractpowerrise::satisfied(): "
                 << "current power fraction = "
                 << *(mPowerHist.begin()) << endl;
        }

        //
        // Various reasons for returning false without actually
        // looking at the results
        //
        // No data last time around; OR Not enough data to make a judgement
        if (mPrevResults.mT0 == Time(0,0) ||
            (int)(mPowerHist.size()) < mIntParams["nstrides"].value()) {
            mCurResults.mSatisfiedP = false;
            return mCurResults.mSatisfiedP;
        }

        // Look at power nstrides ago, and compute the rise from then.
        mCurResults.mDoubleVal = *(mPowerHist.begin()) -
            *(mPowerHist.end() - 1);
        
        if (isnan(mCurResults.mDoubleVal)) {
            nanErrMesg();
            mCurResults.mSatisfiedP = false;
        } else {
            if (mCurResults.mDoubleVal > mDblParams["threshold"].value())
                mCurResults.mSatisfiedP = true;
            else
                mCurResults.mSatisfiedP = false;
        }

        return mCurResults.mSatisfiedP;
    }
}
