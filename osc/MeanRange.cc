// Yo, Emacs! This is -*- C++ -*-
//
// $Id: MeanRange.cc,v 1.4 2002/05/01 18:47:43 dwchin Exp $
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

#include "osc/MeanRange.hh"
#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

osc::MeanRange::MeanRange(Dacc           *access,
                          ostream        *outstream,
                          string          chanName, // name of assoc. chan.
                          osc::IntParMap &intparams,
                          osc::DblParMap &dblparams,
                          int             debug)
    : OperStateCond(access, outstream, chanName, "meanrange", true,
                    intparams, dblparams, debug)
{
    return;
}

//
// satisfied()
//
bool osc::MeanRange::satisfied()
{
    if (mDebug > 0) {
        cout << "osc::MeanRange on channel '" << mChannel 
             << "': entered satisfied()" << endl;
    }

    // Read in data
    TSeries *ptx = dataSeries();

    if (mDebug > 2) {
        cout << "osc::MeanRange on '" << mChannel
             << "': stride = " << stride() << endl;
        size_t  nsample = ptx->getNSample();
        double *ptxdata = new double[nsample];
        size_t ngot = ptx->getData(nsample, ptxdata);

        cout << "osc::MeanRange on '" << mChannel
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
        // Current stuff
        mCurResults.mT0 = t0;

        try {
            readParamsFromChanMaybe();
        }

        catch (DataRead_err &err) {
            err.printErrMsg();
            return false;
        }


        if (mDebug > 0) {
            printf("osc::MeanRange on '%s': ptx->getAverage() = %6.4f\n",
                   mChannel.c_str(), ptx->getAverage());

            if (mDebug > 2) {
                // compute mean by hand
                size_t   nSamples = ptx->getNSample();
                // Interval interval = ptx->getInterval();
                // Time    startTime = ptx->getStartTime();
                double    *ptxdata = new double[nSamples];

                // for (size_t i = 0; i < nSamples; ++i)
                // ptxdata[i] = (double)8.;

                // fill ptx with dummy data
                //int retcode = ptx->setData(startTime, interval, ptxdata,
                //sizeof(double));

                //if (retcode != 0) {
                //cout << "osc::MeanRange on '"
                //<< mChannel << "': error in ptx->setData()"
                //<< endl;
                //}

                // extract data
                size_t nRead = ptx->getData(nSamples, ptxdata);

                if (nRead != nSamples)
                    cout << "osc::MeanRange: read too little data" << endl;

                double avg(0);
                for (size_t i = 0; i < nSamples; ++i)
                    avg += ptxdata[i];
                avg /= nSamples;
                
                delete [] ptxdata;

                printf("osc::MeanRange on '%s': manual avg = %6.4f\n",
                          mChannel.c_str(), avg);

            }
        }


        mCurResults.mDoubleVal = ptx->getAverage();
        if (mDblParams["lo"].value() < mCurResults.mDoubleVal &&
            mCurResults.mDoubleVal < mDblParams["hi"].value())
            mCurResults.mSatisfiedP = true;
        else
            mCurResults.mSatisfiedP = false;

        return mCurResults.mSatisfiedP;
    }
} // END: satisfied()
