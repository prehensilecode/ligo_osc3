// Yo, Emacs! This is -*- C++ -*-
//
// $Id: RmsBelow.cc,v 1.2 2002/05/01 18:47:43 dwchin Exp $
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

#include "osc/RmsBelow.hh"

using namespace std;

osc::RmsBelow::RmsBelow(Dacc       *access,
                        ostream    *outstream,
                        string      chanName,
                        osc::IntParMap &intparams,
                        osc::DblParMap &dblparams,
                        int debug)
    : OperStateCond(access, outstream, chanName, "rmsbelow",
                    true, intparams, dblparams, debug)
{
    return;
}


bool osc::RmsBelow::satisfied()
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

                
        // compute RMS of time-series
        mCurResults.mDoubleVal = sqrt((*ptx)*(*ptx) / ptx->getNSample());

        if (mDebug > 0) {
            printf("osc::RmsBelow on '%s': RMS = %6.4f\n",
                   mChannel.c_str(), mCurResults.mDoubleVal);
        }

        if (mCurResults.mDoubleVal < mDblParams["threshold"].value())
            mCurResults.mSatisfiedP = true;
        else
            mCurResults.mSatisfiedP = false;

        return mCurResults.mSatisfiedP;
    }
}
