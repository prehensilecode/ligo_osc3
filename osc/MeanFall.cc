// Yo, Emacs! This is -*- C++ -*-
//
// $Id: MeanFall.cc,v 1.4 2002/05/01 18:47:43 dwchin Exp $
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

#include "osc/MeanFall.hh"
#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

osc::MeanFall::MeanFall(Dacc           *access,
                        ostream        *outstream,
                        string          chanName, // name of assoc. chan.
                        osc::IntParMap &intparams,
                        osc::DblParMap &dblparams,
                        int             debug)
    : OperStateCond(access, outstream, chanName, "MeanFall",
                    true, intparams, dblparams, debug)
{
    return;
}


bool osc::MeanFall::satisfied()
{
    if (mDebug > 0) {
        cout << "osc::" << mName << " on channel '" << mChannel 
             << "': entered satisfied()" << endl;
    }

    // Read in data
    TSeries *ptx = dataSeries();

    if (mDebug > 2) {
        cout << "osc::MeanFall on '" << mChannel
             << "': stride = " << stride() << endl;
        size_t  nsample = ptx->getNSample();
        double *ptxdata = new double[nsample];
        size_t ngot = ptx->getData(nsample, ptxdata);

        cout << "osc::MeanFall on '" << mChannel
             << "': got " << ngot << " data points from channel "
             << mChannel << " out of " << nsample << " points" << endl;

        delete [] ptxdata;
    }

    // If the time seires is empty, return false
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

        if (mDebug > 0) {
            printf("osc::%s on '%s': ptx->getAverage() = %6.4f\n",
                   mName.c_str(), mChannel.c_str(), ptx->getAverage());

            if (mDebug > 2) {
                // compute mean by hand
                size_t   nSample = ptx->getNSample();
                // Interval interval = ptx->getInterval();
                // Time    startTime = ptx->getStartTime();
                double    *ptxdata = new double[nSample];

                // for (size_t i = 0; i < nSample; ++i)
                // ptxdata[i] = (double)8.;

                // fill ptx with dummy data
                //int retcode = ptx->setData(startTime, interval, ptxdata,
                //sizeof(double));

                //if (retcode != 0) {
                //cout << "osc::MeanFall on '"
                //<< mChannel << "': error in ptx->setData()"
                //<< endl;
                //}

                // extract data
                size_t nRead = ptx->getData(nSample, ptxdata);

                if (nRead != nSample)
                    cout << "osc::" << mName << ": read too little data"
                         << endl;

                double avg(0);
                for (size_t i = 0; i < nSample; ++i)
                    avg += ptxdata[i];
                avg /= nSample;
                
                delete [] ptxdata;

                printf("osc::%s on '%s': manual avg = %6.4f\n",
                          mName.c_str(), mChannel.c_str(), avg);

            }
        }

        // if the deque is full, make space for current results
        if ((int)(mMeanHist.size()) == mIntParams["nstrides"].value())
            mMeanHist.pop_back();

        // push current mean to front of queue
        mMeanHist.push_front(ptx->getAverage());

        //
        // Various reasons for returning false without actually
        // looking at the results
        //
        // No data last time around; OR Not enough data to make a
        // judgement
        if (mPrevResults.mT0 == Time(0,0) ||
            (int)(mMeanHist.size()) < mIntParams["nstrides"].value()) {
            mCurResults.mSatisfiedP = false;
            return mCurResults.mSatisfiedP;
        }

        // Look at mean nstrides ago, and compute the fall
        mCurResults.mDoubleVal =  *(mMeanHist.end() - 1) -
            *(mMeanHist.begin());

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

