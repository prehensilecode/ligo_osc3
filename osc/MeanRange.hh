// Yo, Emacs! This is -*- C++ -*-
//
// $Id: MeanRange.hh,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#ifndef OSCMEANRANGE_HH
#define OSCMEANRANGE_HH

#include "osc/OperStateCond.hh"

namespace osc
{

    //
    // MEANRANGE
    //
    class MeanRange
        : public OperStateCond
    {
    public:
        MeanRange(Dacc           *access,
                  std::ostream        *outstream,
                  std::string     chanName, // name of assoc. channel
                  osc::IntParMap &intparams,
                  osc::DblParMap &dblparams,
                  int             debug = 0);

        virtual ~MeanRange() {};

        virtual bool satisfied();

    private:
        /** {\tt meanrange} is satisfied if the mean of the data in
         * the current frame is in between the lower and upper limits
         * {\tt lo} and {\tt hi}, respectively.
         * \begin{enumerate}
         *     \item {\bf {\tt lo}} Lower limit
         *     \item {\bf {\tt hi}{ Upper limit
         * \end{enumerate}
         */
    };
} // namespace osc

#endif // !defined OSCMEANRANGE_HH
