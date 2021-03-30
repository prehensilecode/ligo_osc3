// Yo, Emacs! This is -*- C++ -*-
//
// $Id: ValueBelow.cc,v 1.5 2002/06/29 01:48:09 dwchin Exp $
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

#include "osc/ValueBelow.hh"

using namespace std;


osc::ValueBelow::ValueBelow(Dacc           *access,
                            ostream        *outstream,
                            std::string     chanName,  // name of assoc. chan.
                            osc::IntParMap &intparams,
                            osc::DblParMap &dblparams,
                            int             debug)
    : OperStateCond(access, outstream, chanName, "valuebelow", true,
                    intparams, dblparams, debug)
{
    // not sensible to return mCurResults.mDoubleVal, so make sure
    // it's zero
    mCurResults.mDoubleVal = 0.;
    
    return;
}


bool osc::ValueBelow::satisfied()
{
    if (mDebug > 0) {
        cout << "osc::ValueBelow on '" << mChannel
             << "': entered satisfied()" << endl;
    }

    // Read in data
    TSeries *ptx = dataSeries();

    // no. of samples
    size_t nSample = ptx->getNSample();

    // If the time series is empty, return false
    if (nSample <= 0 || ptx->isEmpty() == true)
        return false;

    // If this condition has already been checked for in the current
    // Frame, return the buffered result
    Time t0 = ptx->getStartTime();
    if (t0 == mCurResults.mT0) {
        return mCurResults.mSatisfiedP;
    } else {

        updateResultsBuffer();
        
        mCurResults.mT0 = t0;
        mCurResults.mDoubleVal = ptx->getMinimum();

        try {
            readParamsFromChanMaybe();
        }

        catch (DataRead_err &err) {
            err.printErrMsg();
            return false;
        }
        

        if ((mDblParams["fraction"].value() == 0. &&
             ptx->getNLess(mDblParams["threshold"].value()) > 0) ||
            (mDblParams["fraction"].value() > 0. &&
             ptx->getNLess(mDblParams["threshold"].value()) >=
             mDblParams["fraction"].value() * nSample))
            mCurResults.mSatisfiedP = true;
        else
            mCurResults.mSatisfiedP = false;

        // return current results
        return mCurResults.mSatisfiedP;
    }
}

