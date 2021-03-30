//
// $Id: RmsBelow.hh,v 1.3 2002/02/13 09:39:41 sigg Exp $
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

#ifndef OSCRMSBELOW_HH
#define OSCRMSBELOW_HH

#include "osc/OperStateCond.hh"
#include "FSeries.hh"
#include "FSpectrum.hh"


namespace osc
{
    //
    // RMSBELOW
    //
    class RmsBelow
        : public OperStateCond
    {
    public:
        RmsBelow(Dacc           *access,
                 std::ostream        *outstream,
                 std::string     chanName,  // name of assoc. channel
                 osc::IntParMap &intparams,
                 osc::DblParMap &dblparams,
                 int debug = 0);

        virtual ~RmsBelow() {};

        virtual bool satisfied();

    private:
        /** {\tt RmsBelow} has one parameter.  This OSC is
         * satisfied if the RMS of the time series data is less
         * than the {\tt threshold}
         * \begin{enumerate}
         *    \item {\bf {\tt threshold}} RMS threshold
         * \end{enumerate}
         */
    };

} // namespace osc

#endif // !defined OSCRMSBELOW_HH
