// Yo, Emacs! This is -*- C++ -*-
//
// $Id: MeanAbove.hh,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#ifndef OSCMEANABOVE_HH
#define OSCMEANABOVE_HH

#include "osc/OperStateCond.hh"

namespace osc
{

    //
    // MEANABOVE
    //
    class MeanAbove
        : public OperStateCond
    {
    public:
        MeanAbove(Dacc           *access,
                  std::ostream        *outstream,
                  std::string     chanName, // name of assoc. channel
                  osc::IntParMap &intparams,
                  osc::DblParMap &dblparams,
                  int             debug = 0);

        virtual ~MeanAbove() {};

        virtual bool satisfied();

    private:
        /** {\tt meanabove} has one parameter:
         * \begin{enumerate}
         *     \item {\bf {\tt threshold}} This OSC is satisfied if the
         *           mean of the data in the current Frame is above {\tt
         *           threshold}
         * \end{enumerate}
         */
    };
} // namespace osc

#endif // !defined OSCMEANABOVE_HH
