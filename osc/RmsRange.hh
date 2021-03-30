//
// $Id: RmsRange.hh,v 1.3 2002/02/13 09:39:41 sigg Exp $
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

#ifndef OSCRMSRANGE_HH
#define OSCRMSRANGE_HH

#include "osc/OperStateCond.hh"
#include "FSeries.hh"
#include "FSpectrum.hh"


namespace osc
{
    //
    // RMSRANGE
    //
    class RmsRange
        : public OperStateCond
    {
    public:
        RmsRange(Dacc           *access,
                 std::ostream        *outstream,
                 std::string     chanName,  // name of assoc. channel
                 osc::IntParMap &intparams,
                 osc::DblParMap &dblparams,
                 int debug = 0);

        virtual ~RmsRange() {};

        virtual bool satisfied();

    private:
        /** {\tt rmsrange} has two parameters.  This OSC is
         * satisfied if the RMS of the time series data is between
         * {\tt lo} and {\tt hi}
         * \begin{enumerate}
         *    \item {\bf {\tt lo}} RMS lower limit
         *    \item {\bf {\tt hi}} RMS upper limit
         * \end{enumerate}
         */
    };

} // namespace osc

#endif // !defined OSCRMSRANGE_HH
