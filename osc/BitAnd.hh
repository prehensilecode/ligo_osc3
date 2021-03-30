// Yo, Emacs! This is -*- C++ -*-
//
// $Id: BitAnd.hh,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#ifndef OSCBITAND_HH
#define OSCBITAND_HH

#include "osc/OperStateCond.hh"

namespace osc
{
    //
    // BITAND
    //
    class BitAnd
        : public OperStateCond
    {
    public:
        BitAnd(Dacc           *access,
               std::ostream        *outstream,
               std::string     chanName,  // name of assoc. channel
               osc::IntParMap &intparams,
               osc::DblParMap &dblparams,
               int                      debug = 0);

        virtual ~BitAnd();

        virtual bool satisfied();

    private:
        /// Array for time series data
        int *pmTxData;

        /// How many data points in time series data pmTxData
        size_t mNSample;
        

        /** {\tt bitand} has one \texttt{int} parameter, and one 
         *  \textt{double} parameter :
         * \begin{enumerate}
         *    \item {\bf {\tt mask}} This OSC is satisfied if the
         *          Boolean AND of the mask  with some {\tt fraction}
         *          of the data in the time series evaluates to True
         *    \item {\bf {\tt fraction}}
         * \end{enumerate}
         */
    };
    
} // namespace osc

#endif // !defined OSCBITAND
