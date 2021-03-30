// Yo, Emacs! This is -*- C++ -*-
//
// $Id: ValueBelow.hh,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#ifndef OSCVALUEBELOW_HH
#define OSCVALUEBELOW_HH

#include "osc/OperStateCond.hh"

namespace osc
{

    //
    // VALUEBELOW
    //
    class ValueBelow
        : public OperStateCond
    {
    public:
        ValueBelow(Dacc           *access,
                   std::ostream   *outstream,
                   std::string     chanName,   // name of associated channel
                   osc::IntParMap &intparams,
                   osc::DblParMap &dblparams,
                   int debug = 0);

        virtual ~ValueBelow() {};

        virtual bool satisfied();

    private:
        /** {\tt valuebelow} has two parameters:
         * \begin{enumerate}
         *    \item {\bf {\tt threshold}} This OSC is satisfied if some
         *          {\tt fraction} of data is below the {\tt threshold}
         *    \item {\bf {\tt fraction}} The minimum fraction of data in
         *          the time series which must be below {\tt threshold}
         *          for this OSC to be satisfied.  If value is negative,
         *          OSC will be satisfied if {\em any} data points are
         *          below the {\tt threshold}. {\bf Default value} is $-1.$
         * \end{enumerate}
         */
    };
} // namespace osc

#endif // !defined OSCVALUEBELOW_HH
