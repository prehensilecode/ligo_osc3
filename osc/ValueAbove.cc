// Yo, Emacs! This is -*- C++ -*-
//
// $Id: ValueAbove.cc,v 1.5 2002/06/27 18:27:00 dwchin Exp $
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

#include "osc/ValueAbove.hh"

using namespace std;


osc::ValueAbove::ValueAbove(Dacc           *access,
                            ostream        *outstream,
                            std::string     chanName,  // name of assoc. chan.
                            osc::IntParMap &intparams,
                            osc::DblParMap &dblparams,
                            int             debug)
    : OperStateCond(access, outstream, chanName, "valueabove", true,
                    intparams, dblparams, debug)
{
    // not sensible to return mCurResults.mDoubleVal, so make sure
    // it's zero
    mCurResults.mDoubleVal = 0.;
    
    return;
}


bool osc::ValueAbove::satisfied()
{
    if (mDebug > 0) {
        cout << "osc::ValueAbove on '" << mChannel
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
    // Frame, return the  buffered result.
    Time t0 = ptx->getStartTime();
    if (t0 == mCurResults.mT0) {
        return mCurResults.mSatisfiedP;
    } else {

        updateResultsBuffer();
        
        //
        // Current stuff
        //
        mCurResults.mT0 = t0;
        mCurResults.mDoubleVal = ptx->getMaximum();

        try {
            readParamsFromChanMaybe();
        }

        catch (DataRead_err &err) {
            err.printErrMsg();
            return false;
        }

        if (mDebug > 1) {
            cout << "osc::ValueAbove::satisfied: "
                 << "mDblParams[\"threshold\"].value() = "
                 << mDblParams["threshold"].value() << endl;
            cout << "osc::ValueAbove::satisfied: "
                 << "ptx->getNGreater(mDblParams[\"threshold\"].value()) = "
                 << ptx->getNGreater(mDblParams["threshold"].value())
                 << endl;
            cout << "osc::ValueAbove::satisfied: "
                 << "nSample = " << nSample << endl;
            cout << "osc::ValueAbove::satisfied: "
                 << "product = " << mDblParams["fraction"].value() * nSample
                 << endl;
        }

        if ((mDblParams["fraction"].value() == 0. &&
             ptx->getNGreater(mDblParams["threshold"].value()) > 0) ||
            (mDblParams["fraction"].value() > 0. &&
             ptx->getNGreater(mDblParams["threshold"].value()) >=
             mDblParams["fraction"].value() * nSample)) {
            mCurResults.mSatisfiedP = true;
            if (mDebug > 1)
                cout << "osc::ValueAbove::satisfied: TRUE" << endl;
        } else {
            mCurResults.mSatisfiedP = false;
            if (mDebug > 1)
                cout << "osc::ValueAbove::satisfied: FALSE" << endl;
        }

        // return current results
        return mCurResults.mSatisfiedP;
    }
}

