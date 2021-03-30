// Yo, Emacs! This is -*- C++ -*-
//
// $Id: AbsPowerAbove.cc,v 1.5 2002/07/24 05:56:21 jzweizig Exp $
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

#include "osc/AbsPowerAbove.hh"
#include "Sine.hh"

using namespace std;

osc::AbsPowerAbove::AbsPowerAbove(Dacc       *access,
                                  ostream    *outstream,
                                  string      chanName,
                                  osc::IntParMap &intparams,
                                  osc::DblParMap &dblparams,
                                  int debug)
    : OperStateCond(access, outstream, chanName, "abspowerabove",
                    true, intparams, dblparams, debug)
{
    return;
}


double osc::AbsPowerAbove::power(const TSeries *ptx,
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



bool osc::AbsPowerAbove::satisfied()
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

        catch (DataRead_err &err) {
            err.printErrMsg();
            return false;
        }

        if (mDebug > 1) {
            //
            // Check Parseval's
            //

            if (mDebug > 3) {
                // First, make a time series with two
                // cosine waves of unit amplitude + DC offset
                size_t   ndata = ptx->getNSample();
                double   sampleInterval = (double)(ptx->getTStep());
                double   sineFreq1  = 60.31049;
                double   sineFreq2  = 30.84102;
                Interval timeOffset1 = .25/sineFreq1;
                Interval timeOffset2 = .25/sineFreq2;
                Time     startTime1  = t0 - timeOffset1;
                Time     startTime2  = t0 - timeOffset2;
                Sine     sinewave1(sineFreq1, 1, 0.,sampleInterval,startTime1);
                Sine     sinewave2(sineFreq2, 1, 0.,sampleInterval,startTime2);

                double  *fakedata;
                fakedata = new double[ndata];
                for (unsigned int i = 0; i < ndata; ++i)
                    fakedata[i] = cos(2. * M_PI * sineFreq1 * (double)i /
                                      (double)ndata) + 0.75;

                /*
                TSeries fakeTseries1(t0, ptx->getTStep(),
                                     ndata, sinewave1);
                TSeries fakeTseries2(t0, ptx->getTStep(),
                                     ndata, sinewave2);
                fakeTseries1 += fakeTseries2;
                */

                TSeries fakeTseries1(t0, ptx->getTStep(), ndata, fakedata);

                FSeries fakeFseries(fakeTseries1);
                FSpectrum fakeSpectrum(fakeFseries);

                double rmsPowerT = rms(&fakeTseries1);
                double rmsPowerF = rms(&fakeFseries);

                printf("osc::AbsPowerAbove::satisfied():rmsPowerT = %20.14e\n",
                       rmsPowerT);
                printf("       full band                rmsPowerF = %20.14e\n",
                       rmsPowerF);
                printf("       full band    'corrected' rmsPowerF = %20.14e\n",
                       rmsPowerF - 0.5*fakeSpectrum.getSum(0.,0.));

                /* 
                if (rmsPowerT != rmsPowerF) {
                    cout << "osc::AbsPowerAbove::satisfied(): Parseval's "
                         << "thm NOT satisfied" << endl;
                    throw;
                }
                */

                delete [] fakedata;

            } else {  // mDebug <= 2

                // construct FSeries from TSeries
                mFreqSeries = FSeries(*ptx);
                
                // construct FSpectrum from FSeries
                mSpectrum = FSpectrum(mFreqSeries);
            
                double rmsPowerT = rms(ptx);
                double rmsPowerF = rms(&mFreqSeries);
            
                cout << "osc::AbsPowerAbove::satisfied(): rmsPowerT = "
                     << rmsPowerT << endl;
                cout << "       full band                 rmsPowerF = "
                     << rmsPowerF << endl;
                cout << "       full band     'corrected' rmsPowerF = "
                     << rmsPowerF - 0.5*mSpectrum.getSum(0.,0.) << endl;
            }
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

        // band-limited mean-square
        /* mCurResults.mDoubleVal =
            mSpectrum.getSum(mDblParams["freqlo"].value(),
                             mDblParams["freqhi"].value() -
                             mDblParams["freqlo"].value())
            * mSpectrum.getFStep();
            */
        mCurResults.mDoubleVal = power(ptx, mDblParams["freqlo"].value(),
                                       mDblParams["freqhi"].value());


        if (mDebug > 0) {
            cout << "osc::AbsPowerAbove::satisfied(): current power = ";
            printf("%12.7e", mCurResults.mDoubleVal);
            cout << " (channel = " << mChannel << ")" << endl;

            if (mDebug > 1) {
                cout << "osc::AbsPowerAbove::satisfied(): current power ii = ";
                printf("%12.7e", power(ptx, mDblParams["freqlo"].value(),
                                       mDblParams["freqhi"].value()));
                cout << " (channel = " << mChannel << ")" << endl;
            }
        }

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
