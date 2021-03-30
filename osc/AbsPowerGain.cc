// Yo, Emacs! This is -*- C++ -*-
//
// $Id: AbsPowerGain.cc,v 1.2 2002/05/01 18:47:43 dwchin Exp $
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

#include "osc/AbsPowerGain.hh"

using namespace std;

osc::AbsPowerGain::AbsPowerGain(Dacc           *access,
                                ostream        *outstream,
                                string          chanName,
                                osc::IntParMap &intparams,
                                osc::DblParMap &dblparams,
                                int             debug)
    : OperStateCond(access, outstream, chanName, "abspowergain",
                    true, intparams, dblparams, debug)
{
    return;
}

double osc::AbsPowerGain::power(const TSeries *ptx,
                                double         freqlo,
                                double         freqhi)
{
    // construct FSeries from TSeries
    mFreqSeries = FSeries(*ptx);
    
    // construct FSpectrum from FSeries
    mSpectrum = FSpectrum(mFreqSeries);
            
    return (mSpectrum.getSum(freqlo, freqhi -freqlo)
            * mSpectrum.getFStep());
}


bool osc::AbsPowerGain::satisfied()
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
        
        // We want mean square value of PSD multiplied by frequency
        // interval (a quantity that is independent of the time
        // stride):  <a2>*Df, Df is the frequency interval (passband)
        // Now, FSpectrum::getSum() returns <a2>*Nf, where Nf is the
        // number of points in the FSpectrum band selected.  But,
        // Nf = Df * Nt / Fs, where Nt is no. of samples in TSeries,
        // and Fs is sampling frequency.
        // So, getSum() returns <a2>*Df*Nt/Fs
        // We want <a2>*Df, so we have to divide getSum() by
        // (Nt/Fs) = 1. / FSpectrum.getFStep()

        // push current band-limited rms (power) to front of deque
        mPowerHist.push_front(power(ptx, mDblParams["freqlo"].value(),
                                    mDblParams["freqhi"].value()));
        
        if (mDebug > 0) {
            cout << "osc::AbsPowerGain::satisfied(): current power = "
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

        // Look at power nstrides ago, and compute the gain from then
        mCurResults.mDoubleVal = *(mPowerHist.begin()) /
            *(mPowerHist.end() - 1);

        // This is a funny OSC. What it does depends on the value of
        // the threshold
        if (isnan(mCurResults.mDoubleVal)) {
            nanErrMesg();
            mCurResults.mSatisfiedP = false;
        } else {


            if (mDblParams["threshold"].value() > 1.) {

                // watch for amplification
                if (mCurResults.mDoubleVal > mDblParams["threshold"].value())
                    mCurResults.mSatisfiedP = true;
                else
                    mCurResults.mSatisfiedP = false;
                
            } else if (mDblParams["threshold"].value() < 1.) {
                
                // watch for de-amplification
                if (mCurResults.mDoubleVal < mDblParams["threshold"].value())
                    mCurResults.mSatisfiedP = true;
                else
                    mCurResults.mSatisfiedP = false;
                
            } else if (mDblParams["threshold"].value() == 1.) {

                // watch for constancy
                if (mCurResults.mDoubleVal == mDblParams["threshold"].value())
                    mCurResults.mSatisfiedP = true;
                else
                    mCurResults.mSatisfiedP = false;
                
            }
        }

        return mCurResults.mSatisfiedP;
    }
}
