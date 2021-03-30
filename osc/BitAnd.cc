// Yo, Emacs! This is -*- C++ -*-
//
// $Id: BitAnd.cc,v 1.4 2002/06/27 18:27:00 dwchin Exp $
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

#include "osc/BitAnd.hh"

using namespace std;

osc::BitAnd::BitAnd(Dacc           *access,
                    ostream        *outstream,
                    std::string     chanName,  // name of assoc. chan.
                    osc::IntParMap &intparams,
                    osc::DblParMap &dblparams,
                    int             debug)
    : OperStateCond(access, outstream, chanName, "bitand", true,
                    intparams, dblparams, debug),
      mNSample(16384)
{
    // We'll guess a size; 16k is a good size since Frames are
    // generally 1 second long, and the fast channels are 16kHz.
    pmTxData = new int[mNSample];

    return;
}

osc::BitAnd::~BitAnd()
{
    delete [] pmTxData;
}


bool osc::BitAnd::satisfied()
{
    if (mDebug > 0) {
        cout << "osc::BitAnd on '" << mChannel
             << "': entered satisfied()" << endl;
    }

    // Read in data
    TSeries *ptx = dataSeries();

    // no. of samples
    size_t nSample = ptx->getNSample();

    // If the time series is empty, return false
    if (nSample <= 0 || ptx->isEmpty() == true)
        return false;


    // This would be avoided if TSeries::getData() would take a
    // vector<> object.  Until then, we'll have to do things the old
    // fashioned way.  We shall assume that the number of samples
    // doesn't change from frame to frame, so we will use a less
    // efficient algorithm.
    if (mNSample != nSample) {
        mNSample = nSample;
        delete [] pmTxData;
        pmTxData = new int[mNSample];
    }

    // If this condition has already been checked for in the current
    // Frame, return the buffered result
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
        
        size_t nRead = ptx->getData(nSample, pmTxData);

        // if error, give up
        if (nRead != nSample) {
            cerr << "osc::bitand: error reading data from channel '"
                 << mChannel << "'; exiting" << endl;
            exit(EXIT_FAILURE);
        }

        size_t nSatisfies = 0;

        for (size_t i = 0; i < nSample; ++i) {
            if ((pmTxData[i] & mIntParams["mask"].value()) ==
                mIntParams["mask"].value())
                ++nSatisfies;
        }

        if (mDebug > 2) {
            for (size_t i = 0; i < nSample; ++i) {
                std::cout << pmTxData[i] << "  " 
                          << mIntParams["mask"].value() << "  "
                          << (pmTxData[i] & mIntParams["mask"].value())
                          << std::endl;
            }
        }

        if ((mDblParams["fraction"].value() == 0. && nSatisfies > 0) ||
            (mDblParams["fraction"].value() > 0. &&
             nSatisfies >= mDblParams["fraction"].value() * nSample))
            mCurResults.mSatisfiedP = true;
        else
            mCurResults.mSatisfiedP = false;

        return mCurResults.mSatisfiedP;
    }
}

