// Yo, Emacs! This is -*- C++ -*-
//
// $Id: MeanAbove.cc,v 1.4 2002/06/26 14:20:16 dwchin Exp $
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

#include "osc/MeanAbove.hh"
#include <iostream>
#include <stdio.h>
#include <string>


using namespace std;

osc::MeanAbove::MeanAbove(Dacc           *access,
                          ostream        *outstream,
                          string          chanName, // name of assoc. chan.
                          osc::IntParMap &intparams,
                          osc::DblParMap &dblparams,
                          int             debug)
    : OperStateCond(access, outstream, chanName, "meanabove",
                    true, intparams, dblparams, debug)
{
    return;
}


bool osc::MeanAbove::satisfied()
{
    if (mDebug > 0) {
        cout << "osc::MeanAbove on channel '" << mChannel 
             << "': entered satisfied()" << endl;
    }

    // Read in data
    TSeries *ptx = dataSeries();

//    if (mDebug > 2) {
//        cout << "osc::MeanAbove on '" << mChannel
//             << "': stride = " << stride() << endl;
//
//        if (!ptx) {
//            cout << "osc::MeanAbove on '" << mChannel
//                 << "': ptx is NULL" << endl;
//        }
//
//        size_t  nsample = ptx->getNSample();
//        cout << "osc::MeanAbove on '" << mChannel
//             << "': nsample = " << nsample << endl;
//        double *ptxdata = new double[nsample];
//        size_t ngot = ptx->getData(nsample, ptxdata);
//
//        cout << "osc::MeanAbove on '" << mChannel
//             << "': got " << ngot << " data points from channel "
//             << mChannel << " out of " << nsample << " points" << endl;
//
//        delete [] ptxdata;
//    }

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
        // Current stuff
        //
        mCurResults.mT0 = t0;

        if (mDebug > 0) {
            printf("osc::MeanAbove on '%s': ptx->getAverage() = %6.4f\n",
                   mChannel.c_str(), ptx->getAverage());

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
                //cout << "osc::MeanAbove on '"
                //<< mChannel << "': error in ptx->setData()"
                //<< endl;
                //}

                // extract data
                size_t nRead = ptx->getData(nSample, ptxdata);

                if (nRead != nSample)
                    cout << "osc::MeanAbove: read too little data" << endl;

                double avg(0);
                for (size_t i = 0; i < nSample; ++i)
                    avg += ptxdata[i];
                avg /= nSample;
                
                delete [] ptxdata;

                printf ("osc::MeanAbove on '%s': manual avg = %6.4f\n",
                          mChannel.c_str(), avg);

            }
        }

        mCurResults.mDoubleVal = ptx->getAverage();
        if (mCurResults.mDoubleVal > mDblParams["threshold"].value())
            mCurResults.mSatisfiedP = true;
        else
            mCurResults.mSatisfiedP = false;

        return mCurResults.mSatisfiedP;
    }
}

