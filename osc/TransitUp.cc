// Yo, Emacs! This is -*- C++ -*-
//
// $Id: TransitUp.cc,v 1.2 2002/05/01 18:47:43 dwchin Exp $
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

#include "osc/TransitUp.hh"

using namespace std;


//
// Constructor
//
osc::TransitUp::TransitUp(Dacc              *access,
                          ostream           *outstream,
                          std::string        oscName,  // name of assoc. OSC
                          osc::IntParMap    &intparams,
                          osc::DblParMap    &dblparams,
                          OperStateCondList *pOscList,
                          int                debug)
    : OperStateCond(access, outstream, std::string(), "transitup",
                    false, intparams, dblparams, debug),
      mOSCname(oscName),
      mWasSatisfiedP(true),
      mInHoldP(false),
      mInDeadP(false),
      mCurrentFrame(-1),
      mLastTransitupFrame(-1),
      pmOscList(pOscList)
{
    if (mDebug > 0) {
        cout << "osc::TransitUp on OSC '"
             << mOSCname << "' constructed" << endl;

        printParamInfo();
    }

    return;
}

//
// satisfied()
//
bool osc::TransitUp::satisfied()
{
    if (mCurrentFrame < 0)
        mCurrentFrame = 1;   // start the Frame counter at 1
    else
        ++mCurrentFrame;

    if (mDebug > 2)
        cout << "osc::transitup on '" << mOSCname
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
            if (mCurrentFrame == 19 || mCurrentFrame == 39)
                mIsSatisfiedP = false;

            if (mCurrentFrame == 20 || mCurrentFrame == 40)
                mIsSatisfiedP = true;

            cout << "osc::transitup on '" << mOSCname
                 << "': mWasSatisfiedP = " << mWasSatisfiedP << endl;
            cout << "osc::transitup on '" << mOSCname
                 << "': mIsSatisfiedP = " << mIsSatisfiedP << endl;
            cout << "osc::transitup on '" << mOSCname
                 << "': mLastTransitupFrame = "
                 << mLastTransitupFrame << endl;
            cout << "osc::transitup on '" << mOSCname
                 << "': deadtime duration = "
                 << mIntParams["dead"].value() << endl;
        }

        // mLastTransitupFrame is < 0 if no transitup has occured
        // before
        if (mLastTransitupFrame > 0) {
            // in dead period?
            int timeFromEndOfHold = mCurrentFrame -
                (mLastTransitupFrame + mIntParams["hold"].value());
            mInDeadP = (timeFromEndOfHold > 0 &&
                        timeFromEndOfHold <= mIntParams["dead"].value() );

            // in hold period?
            int timeFromStartOfHold = mCurrentFrame - mLastTransitupFrame;
            mInHoldP = ((timeFromStartOfHold >= 0) &&
                        (timeFromStartOfHold <= mIntParams["hold"].value()));
        } else {
            // no transitups have occured yet, so we cannot be in
            // deadtime or hold period
            mInDeadP = false;
            mInHoldP = false;
        }

        // if in dead period
        if (mInDeadP == true) {
            if (mDebug > 1) 
                *pmOut << "osc::transitup on '" << mOSCname
                       << "': in deadtime" << endl;
            return false;
        }

        // if in hold period
        if (mInHoldP == true) {
            if (mDebug > 1)
                *pmOut << "osc::transitup on '" << mOSCname
                       << "': in hold" << endl;
            return true;
        }

        //
        // neither in hold nor dead
        //
        
        if (mDebug > 1) {
            *pmOut << "osc::transitup on '" << mOSCname
                   << "': not in hold or deadtime" << endl;
        }

        // if OSC has transitioned from False to True
        if (mIsSatisfiedP == true && mWasSatisfiedP == false) {

            if (mDebug > 1)
                *pmOut << "osc::transitup on '" << mOSCname
                       << "': just transitioned" << endl;

            mLastTransitupFrame = mCurrentFrame;
            mWasSatisfiedP      = mIsSatisfiedP;
            return true;
            
        } else {  // no transition happened

            if (mDebug > 1)
                *pmOut << "osc::transitup on '" << mOSCname
                       << "': no transition" << endl;

            mWasSatisfiedP = mIsSatisfiedP;
            return false;
        }
    }

    cerr << "osc::transitup: WHOA, Nelly! How'd we get here!?" << endl;
    throw;
    
} // END: TransitUp::satisfied()


//
// printInfo()
//
void osc::TransitUp::printInfo() const
{
    cout << "osc::" << mName << " on OSC '" << mOSCname << "'";
    printParamInfo();
}
