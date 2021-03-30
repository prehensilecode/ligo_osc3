// Yo, Emacs! This is -*- C++ -*-
//
// $Id: AbsPowerBelow.cc,v 1.2 2002/05/01 18:47:43 dwchin Exp $
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

#include "osc/AbsPowerBelow.hh"

using namespace std;

osc::AbsPowerBelow::AbsPowerBelow(Dacc       *access,
                                  ostream    *outstream,
                                  string      chanName,
                                  osc::IntParMap &intparams,
                                  osc::DblParMap &dblparams,
                                  int debug)
    : OperStateCond(access, outstream, chanName, "abspowerbelow",
                    true, intparams, dblparams, debug)
{
    return;
}


double osc::AbsPowerBelow::power(const TSeries *ptx,
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



bool osc::AbsPowerBelow::satisfied()
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
        // current stuff
        //
        mCurResults.mT0 = t0;

        
        try {
            readParamsFromChanMaybe();
        }

        catch (DataRead_err& err) {
            err.printErrMsg();
            return false;
        }
        

        if (mDebug > 1) {
            // construct FSeries from TSeries
            mFreqSeries = FSeries(*ptx);
            
            // construct FSpectrum from FSeries
            mSpectrum = FSpectrum(mFreqSeries);
            
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
        
        // aka band-limited power
        mCurResults.mDoubleVal = power(ptx, mDblParams["freqlo"].value(),
                                       mDblParams["freqhi"].value());

        if (mDebug > 0) {
            cout << "osc::AbsPowerBelow::satisfied(): current power = "
                 << mCurResults.mDoubleVal << endl;
        }

        if (isnan(mCurResults.mDoubleVal)) {
            nanErrMesg();
            mCurResults.mSatisfiedP = false;
        } else {
            if (mCurResults.mDoubleVal < mDblParams["threshold"].value())
                mCurResults.mSatisfiedP = true;
            else
                mCurResults.mSatisfiedP = false;
        }

        return mCurResults.mSatisfiedP;
    }
}
      
