// Yo, Emacs! This is -*- C++ -*-
//
// $Id: ValueRange.cc,v 1.6 2002/06/29 01:48:09 dwchin Exp $
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

#include "osc/ValueRange.hh"


using namespace std;

    
osc::ValueRange::ValueRange(Dacc           *access,
                            ostream        *outstream,
                            std::string     chanName,
                            osc::IntParMap &intparams,
                            osc::DblParMap &dblparams,
                            int             debug)
    : OperStateCond(access, outstream, chanName, "valuerange", true,
                    intparams, dblparams, debug),
      mNSample(16384)
{
    // We'll guess a size; 16k is a good size since Frames are
    // generally 1 second long, and the fast channels are 16kHz.
    pmTxData = new double[mNSample];

    // not sensible to return mCurResults.mDoubleVal, so make sure
    // it's zero
    mCurResults.mDoubleVal = 0.; 
    
    return;
}


osc::ValueRange::~ValueRange()
{
    delete [] pmTxData;
    
    return;
}


bool osc::ValueRange::satisfied()
{
    if (mDebug > 0) {
        cout << "osc::ValueRange on channel '" << mChannel
             << "': entered satisfied()" << endl;
    }

    // Read in data
    TSeries *ptx = dataSeries();

    // no. of samples
    size_t nSample = ptx->getNSample();

    // This would be avoided if TSeries::getData() would take a
    // vector<> object.  Until then, we'll have to do things the old
    // fashioned way.  We shall assume that the number of samples
    // doesn't change much from frame to frame, so we will use a
    // less efficient algorithm.
    if (mNSample != nSample) {
        mNSample = nSample;
        delete [] pmTxData;
        pmTxData = new double[mNSample];
    }

    // If time series is empty, return false
    if (nSample <= 0 || ptx->isEmpty() == true)
        return false;

    // If this condition has already been checked for in the current
    // Frame, return the buffered result
    Time t0 = ptx->getStartTime();
    if (t0 == mCurResults.mT0) {
        return mCurResults.mSatisfiedP;
    } else {

        updateResultsBuffer();

        //
        // Current stuff
        //
        mCurResults.mT0 = t0;
        mCurResults.mDoubleVal = ptx->getDouble(4000);

        try {
            readParamsFromChanMaybe();
        }

        catch (DataRead_err &err) {
            err.printErrMsg();
            return false;
        }

        // since TSeries doesn't have a getNBetween() method, we'll
        // have to do this "by hand"
        size_t nRead = ptx->getData(nSample, pmTxData);

        // give up if error
        if (nRead != nSample) {
            cerr << "osc::ValueRange: error reading data from channel '"
                 << mChannel << "'; exiting" << endl;
            exit(EXIT_FAILURE);
        }

        size_t nBetween = 0;
        double lo = mDblParams["lo"].value();
        double hi = mDblParams["hi"].value();
        for (size_t i = 0; i < nSample; ++i) {
            if (lo < pmTxData[i]  &&  pmTxData[i] < hi)
                ++nBetween;
        }

        if ((mDblParams["fraction"].value() == 0. &&
             ptx->getNBetween(mDblParams["lo"].value(), mDblParams["hi"].value()) > 0) ||
            (mDblParams["fraction"].value() > 0. &&
             ptx->getNBetween(mDblParams["lo"].value(), mDblParams["hi"].value()) >=
             mDblParams["fraction"].value() * nSample)) 
            mCurResults.mSatisfiedP = true;
        else
            mCurResults.mSatisfiedP = false;

        return mCurResults.mSatisfiedP;
    }
}


