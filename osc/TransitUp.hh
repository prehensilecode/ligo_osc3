// Yo, Emacs! This is -*- C++ -*-
//
// $Id: TransitUp.hh,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#ifndef OSCTRANSITUP_HH
#define OSCTRANSITUP_HH


#include "osc/OperStateCond.hh"
#include "OperStateCondList.hh"
#include "TSeries.hh"

class OperStateCondList;

namespace osc
{
    //
    // TRANSITUP
    //
    class TransitUp
        : public OperStateCond
    {
    public:
        TransitUp(Dacc              *access,
                  std::ostream           *outstream,
                  std::string        oscName,      // name of assoc. OSC
                  osc::IntParMap    &intparams,
                  osc::DblParMap    &dblparams,
                  OperStateCondList *pOscList,
                  int                debug = 0);

        virtual ~TransitUp() {};

        /** @memo Transitup returns True when the OSC referred to goes from
         * False to True, and keeps returning True for mHoldDuration Frames,
         * i.e. like a zero-order hold
         */
        virtual bool satisfied();

        virtual void printInfo() const;

    private:

        /// Name of OSC to which this condition refers
        std::string mOSCname;

        /// Was OSC satisfied during last Frame?
        bool mWasSatisfiedP;

        /// Is OSC currently satisfied?
        bool mIsSatisfiedP;

        /// Predicate: we are in hold period
        bool mInHoldP;

        /// Predicate: we are in dead period
        bool mInDeadP;

        /// Index of current Frame
        //  This is initialized to -1 when first constructed
        int mCurrentFrame;

        /// Index of last Frame when last transitup occured
        int mLastTransitupFrame;

        /** @memo Pointer to OperStateCondList object which contains this
         * TransitUp */
        OperStateCondList *pmOscList;

        /** {\tt transitup} has one optional parameter:
         * \begin{enumerate}
         *    \list {\bf {\tt hold}} Number of Frames to hold the
         *    satisfied value at true. {\bf Default value} is 0.  If
         *    value is 0 or 1, the the satisfied value is true only for
         *    the Frame where the transition occurs.
         * \end{enumerate}
         */
    };
}

#endif // !defined OSCTRANSITUP_HH
