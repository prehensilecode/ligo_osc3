// Yo, Emacs! This is -*- C++ -*-
//
// $Id: ValueRange.hh,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#ifndef OSCVALUERANGE_HH
#define OSCVALUERANGE_HH

#include "osc/OperStateCond.hh"

namespace osc
{
    //
    // VALUERANGE
    //
    class ValueRange
        : public OperStateCond
    {
    public:
        ValueRange(Dacc           *access,
                   std::ostream   *outstream,
                   std::string     chanName,
                   osc::IntParMap &intparams,
                   osc::DblParMap &dblparams,
                   int             debug = 0);

        virtual ~ValueRange();

        virtual bool satisfied();

    private:
        /// Array for time series data
        double *pmTxData;

        /// How many data points in time series data pmTxData
        size_t mNSample;
        
        /** {\tt valuerange} is satisfied if some fraction of the data is
         *    in between the lower and upper limits {\tt lo} and {\tt hi}
         *    respectively. 
         *    \item {\bf {\tt lo}} Lower limit
         *    \item {\bf {\tt hi}} Upper limit
         *    \item {\bf {\tt fraction}} The minimum fraction of data in
         *          the time series which must be between {\tt lo} and
         *          {\tt hi} for this OSC to be satisfied.  If value is
         *          negative, OSC will be satisfied if {\em any} data
         *          points are below the {\tt threshold}.
         *          {\bf Default value} is $-1.$
         */
    };

} // namespace osc

#endif // !defined OSCVALUERANGE_HH
