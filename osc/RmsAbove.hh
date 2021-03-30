//
// $Id: RmsAbove.hh,v 1.3 2002/02/13 09:39:41 sigg Exp $
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

#ifndef OSCRMSABOVE_HH
#define OSCRMSABOVE_HH

#include "osc/OperStateCond.hh"
#include "FSeries.hh"
#include "FSpectrum.hh"


namespace osc
{
    //
    // RMSABOVE
    //
    class RmsAbove
        : public OperStateCond
    {
    public:
        RmsAbove(Dacc           *access,
                 std::ostream        *outstream,
                 std::string     chanName,  // name of assoc. channel
                 osc::IntParMap &intparams,
                 osc::DblParMap &dblparams,
                 int debug = 0);

        virtual ~RmsAbove() {};

        virtual bool satisfied();

    private:
        /** {\tt RmsAbove} has one parameter.  This OSC is
         * satisfied if the RMS of the time series data is greater
         * than the {\tt threshold}
         * \begin{enumerate}
         *    \item {\bf {\tt threshold}} RMS threshold
         * \end{enumerate}
         */
    };

} // namespace osc

#endif // !defined OSCRMSABOVE_HH
