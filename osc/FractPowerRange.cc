// Yo, Emacs! This is -*- C++ -*-
//
// $Id: FractPowerRange.cc,v 1.2 2002/05/01 18:47:43 dwchin Exp $
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

#include "osc/FractPowerRange.hh"

using namespace std;

osc::FractPowerRange::FractPowerRange(Dacc       *access,
                                      ostream    *outstream,
                                      string      chanName,
                                      osc::IntParMap &intparams,
                                      osc::DblParMap &dblparams,
                                      int debug)
    : OperStateCond(access, outstream, chanName, "fractpowerrange",
                    true, intparams, dblparams, debug)
{
    return;
}


double osc::FractPowerRange::powerFract(const TSeries *ptx,
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


bool osc::FractPowerRange::satisfied()
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
        // Current stuff
        //
        mCurResults.mT0 = t0;

        try {
            readParamsFromChanMaybe();
        }

        catch (DataRead_err &err) {
            err.printErrMsg();
            return false;
        }


        if (mDebug > 1) {
            // construct FSeries from TSeries
            mFreqSeries = FSeries(*ptx);
            
            // construct FSpectrum from FSeries
            mSpectrum   = FSpectrum(mFreqSeries);

            // Check Parseval's
            double rmsPowerT = rms(ptx);
            double rmsPowerF = rms(&mFreqSeries);
            
            cout << "osc::AbsPowerBelow::satisfied(): rmsPowerT = "
                 << rmsPowerT << endl;
            cout << "       full band                 rmsPowerF = "
                 << rmsPowerF << endl;
            cout << "       full band     'corrected' rmsPowerF = "
                 << rmsPowerF - 0.5*mSpectrum.getSum(0.,0.) << endl;
        }

        // fractional power
        mCurResults.mDoubleVal =
            powerFract(ptx, mDblParams["freqlo"].value(),
                       mDblParams["freqhi"].value());

        if (mDebug > 0) {
            cout << "osc::" << mName << "::satisfied(): fract. power = "
                 << mCurResults.mDoubleVal << endl;
        }

        // if fractPower is not a number
        if (isnan(mCurResults.mDoubleVal))
            mCurResults.mSatisfiedP = false;
        else {
            if (mCurResults.mDoubleVal > mDblParams["lo"].value() &&
                mCurResults.mDoubleVal < mDblParams["hi"].value())
                mCurResults.mSatisfiedP = true;
            else
                mCurResults.mSatisfiedP = false;
        }

        return mCurResults.mSatisfiedP;
    }
}

