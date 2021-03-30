// Yo, Emacs! This is -*- C++ -*-
//
// $Id: TransitDown.cc,v 1.2 2002/05/01 18:47:43 dwchin Exp $
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

#include "osc/TransitDown.hh"

using namespace std;


//
// Constructor
//
osc::TransitDown::TransitDown(Dacc              *access,
                              ostream           *outstream,
                              std::string        oscName, // name of assoc. OSC
                              osc::IntParMap    &intparams,
                              osc::DblParMap    &dblparams,
                              OperStateCondList *pOscList,
                              int                debug)
    : OperStateCond(access, outstream, std::string(), "transitdown",
                    false, intparams, dblparams, debug),
      mOSCname(oscName),
      mWasSatisfiedP(true),
      mInHoldP(false),
      mInDeadP(false),
      mCurrentFrame(-1),
      mLastTransitdownFrame(-1),
      pmOscList(pOscList)
{
    if (mDebug > 0) {
        cout << "osc::TransitDown on OSC '"
             << mOSCname << "' constructed" << endl;

        printParamInfo();
    }

    return;
}


//
// satisfied()
//

// algorithm:

// if in deadtime
//     return false
//
// if in hold
//     return true
//
// if transition down occured
//     set in hold
//     return true
// else
//     return false
//
// in deadtime if condition is false and 0 < (current frame # - (last
//   satisfied frame # + hold dur.)) < deadtime duration
//
// in hold if (current frame # - last satisfied frame #) < hold duration
//     
//

bool osc::TransitDown::satisfied()
{
    if (mCurrentFrame < 0)
        mCurrentFrame = 1;   // start the Frame counter at 1
    else
        ++mCurrentFrame;

    if (mDebug > 2)
        cout << "osc::transitdown on '" << mOSCname
             << "': mCurrentFrame = " << mCurrentFrame << endl;

    if (mCurrentFrame == 1) {
        // Initialize on first frame
        mWasSatisfiedP = (*pmOscList)[mOSCname]->satisfied();
        return false;
    } else {

        // is OSC currently satisfied?
        mIsSatisfiedP = (*pmOscList)[mOSCname]->satisfied();

        try {
            readParamsFromChanMaybe();
        }

        catch (DataRead_err& err) {
            err.printErrMsg();
            return false;
        }

        if (mDebug > 2) {
            if (mCurrentFrame == 10 || mCurrentFrame == 33)
                mIsSatisfiedP = true;

            if (mCurrentFrame == 11 || mCurrentFrame == 34)
                mIsSatisfiedP = false;

            cout << "osc::transitdown on '" << mOSCname
                 << "': mWasSatisfiedP = " << mWasSatisfiedP << endl;
            cout << "osc::transitdown on '" << mOSCname
                 << "': mIsSatisfiedP = " << mIsSatisfiedP << endl;
            cout << "osc::transitdown on '" << mOSCname
                 << "': mLastTransitdownFrame = "
                 << mLastTransitdownFrame << endl;
            cout << "osc::transitdown on '" << mOSCname
                 << "': deadtime duration = "
                 << mIntParams["dead"].value() << endl;
        }

        // mLastTransitdownFrame is < 0 if no transitdown has occured before
        if (mLastTransitdownFrame > 0) {
            // in dead period?
            int timeFromEndOfHold = mCurrentFrame -
                (mLastTransitdownFrame + mIntParams["hold"].value());
            mInDeadP = (timeFromEndOfHold > 0 &&
                        timeFromEndOfHold <= mIntParams["dead"].value() );

            // in hold period?
            int timeFromStartOfHold = mCurrentFrame - mLastTransitdownFrame;
            mInHoldP = ((timeFromStartOfHold >= 0) &&
                        (timeFromStartOfHold <= mIntParams["hold"].value()));
        } else {
            // no transitdowns have occured yet, so we cannot be in
            // deadtime or hold period
            mInDeadP = false;
            mInHoldP = false;
        }

        // if in dead period
        if (mInDeadP == true) {
            if (mDebug > 1) 
                *pmOut << "osc::transitdown on '" << mOSCname
                       << "': in deadtime" << endl;
            return false;
        }

        // if in hold period
        if (mInHoldP == true) {
            if (mDebug > 1)
                *pmOut << "osc::transitdown on '" << mOSCname
                       << "': in hold" << endl;
            return true;
        }

        //
        // neither in hold nor dead
        //
            
        if (mDebug > 1) {
            *pmOut << "osc::transitdown on '" << mOSCname
                   << "': not in hold or deadtime" << endl;
        }
        
        // if OSC has transitioned from True to False
        if (mIsSatisfiedP == false && mWasSatisfiedP == true) {

            if (mDebug > 1)
                *pmOut << "osc::transitdown on '" << mOSCname
                       << "': just transitioned" << endl;

            mLastTransitdownFrame = mCurrentFrame;
            mWasSatisfiedP        = mIsSatisfiedP;
            return true;
            
        } else {    // no transition happened

            if (mDebug > 1)
                *pmOut << "osc::transitdown on '" << mOSCname
                       << "': no transition" << endl;

            mWasSatisfiedP = mIsSatisfiedP;
            return false;
        }
    }

    
    cerr << "osc::transitdown: WHOA, NELLY! How'd we get here!?" << endl;
    throw;
}


//
// printInfo()
//
void osc::TransitDown::printInfo() const
{
    cout << "osc::" << mName << " on OSC' " << mOSCname << "'";
    printParamInfo();
}
