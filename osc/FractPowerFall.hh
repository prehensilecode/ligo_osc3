// Yo, Emacs! This is -*- C++ -*-
//
// $Id: FractPowerFall.hh,v 1.4 2002/05/01 18:47:43 dwchin Exp $
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

#ifndef OSCFRACTPOWERFALL_HH
#define OSCFRACTPOWERFALL_HH

#include "osc/OperStateCond.hh"
#include <deque>

#include "FSeries.hh"
#include "FSpectrum.hh"

namespace osc
{
    //
    // FRACTPOWERFALL
    //
    class FractPowerFall
        : public OperStateCond
    {
    public:
        FractPowerFall(Dacc           *access,
                       std::ostream        *outstream,
                       std::string     chanName, // name of assoc. channel
                       osc::IntParMap &intparams,
                       osc::DblParMap &dblparams,
                       int             debug = 0);

        virtual ~FractPowerFall() {};

        virtual bool satisfied();

    private:
        /// Frequency series (computed every frame)
        FSeries   mFreqSeries;

        /// Power spectrum (computed every frame)
        FSpectrum mSpectrum;

        /** Compute band-limited power (mean square) fraction of time
         * series data in current frame.  Power fraction is ratio of
         * band-limited power to full-band power. Computed by
         * transforming to a FSpectrum object first.
         */
        double powerFract(const TSeries *ptx, double freqlo, double freqhi);

        /** Double-ended queue containing history of fractional power
         * values.  The front of the deque is the most recent.
         */
        std::deque<double>  mPowerHist;

        /** {\tt FractPowerFall} has three parameters.  This OSC is
         * satisfied if the fractional power in the frequency range
         * $[{\mathtext freqlo}, {\mathtext freqhi}]$ falls
         * by an amount greater than \texttt{threshold} over
         * \texttt{nstrides} strides. \texttt{nstrides} is optional
         * with default value 1.
         * \begin{enumerate}
         *    \item {\bf {\tt freqlo}} Lower limit of frequency
         *    \item {\bf {\tt freqhi}} Upper limit of frequency
         *    \item {\bf {\tt threshold}} Threshold change in power
         *    \item {\bf {\tt nstrides}} No. of strides over which the
         *                               rate is above \texttt{threshold}
         * \end{enumerate}
         */
    };
    
    
} // namespace osc

#endif // !defined OSCFRACTPOWERFALL_HH
