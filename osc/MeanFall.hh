// Yo, Emacs! This is -*- C++ -*-
//
// $Id: MeanFall.hh,v 1.3 2002/02/13 09:39:41 sigg Exp $
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

#ifndef OSCMEANFALL_HH
#define OSCMEANFALL_HH

#include "osc/OperStateCond.hh"
#include <deque>


namespace osc
{

    //
    // MeanFall
    //
    class MeanFall
        : public OperStateCond
    {
    public:
        MeanFall(Dacc           *access,
                 std::ostream        *outstream,
                 std::string     chanName, // name of assoc. channel
                 osc::IntParMap &intparams,
                 osc::DblParMap &dblparams,
                 int             debug = 0);

        virtual ~MeanFall() {};

        virtual bool satisfied();

    private:
        /** Double-ended queue containing history of mean
         *  values.  The front of the deque is the most recent.
         */
        std::deque<double>  mMeanHist;
        
        /** \texttt{meanfall} has two parameters:
         * \begin{enumerate}
         *     \item \textbf{\texttt{threshold}} This OSC is satisfied if the
         *           mean of the data decreases by an amount $>$
         *           \texttt{threshold} over \texttt{nstrides}
         *     \item \textbf{\texttt{nstrides}} The number of strides
         *           over which to measure the mean. Default value 1.
         * \end{enumerate}
         */
    };
} // namespace osc

#endif // !defined OSCMEANFALL_HH
