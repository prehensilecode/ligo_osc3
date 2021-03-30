// Yo, Emacs! This is -*- C++ -*-
//
// $Id: AbsPowerGain.hh,v 1.4 2002/05/01 18:47:43 dwchin Exp $
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

#ifndef OSCABSPOWERGAIN_HH
#define OSCABSPOWERGAIN_HH

#include "osc/OperStateCond.hh"
#include <deque>

#include "FSeries.hh"
#include "FSpectrum.hh"

namespace osc
{
    //
    // ABSPOWERGAIN
    //
    class AbsPowerGain
        : public OperStateCond
    {
    public:
        AbsPowerGain(Dacc           *access,
                     std::ostream        *outstream,
                     std::string     chanName, // name of assoc. channel
                     osc::IntParMap &intparams,
                     osc::DblParMap &dblparams,
                     int             debug = 0);

        virtual ~AbsPowerGain() {};

        virtual bool satisfied();

    private:
        /// Frequency series (computed every frame)
        FSeries   mFreqSeries;

        /// Power spectrum (computed every frame)
        FSpectrum mSpectrum;

        /** Compute band-limited power (mean square) of time series
         * data in current frame.  Computed by transforming to a
         * FSpectrum object first.
         */
        double power(const TSeries *ptx, double freqlo, double freqhi);

        /** Double-ended queue containing history of power 
         * values.  The front of the deque is the most recent.
         */
        std::deque<double>  mPowerHist;

        /** {\tt Abspowergain} has three parameters.  This OSC is
         * satisfied if the absolute power in the frequency range
         * $[{\mathtext freqlo}, {\mathtext freqhi}]$ rises or falls
         * by an average factor greater than or equal to
         * \texttt{threshold} over \texttt{nstrides}
         * strides: average gain is computed by $(\mathrm{power}_{N} /
         * \mathrm{power}_{0} / N$, where $N$ is
         * \texttt{nstrides} . \texttt{nstrides} is optional with
         * default value 1.
         * \begin{enumerate}
         * \item {\bf {\tt freqlo}} Lower limit of frequency
         * \item {\bf {\tt freqhi}} Upper limit of frequency
         * \item {\bf {\tt threshold}} Threshold gain of
         * abs. power. Must be positive-definite $>0$.
         * \item {\bf {\tt nstrides}} No. of strides over
         *   which the gain is computed
         * \end{enumerate}
         * The power here is the square of the RMS that is plotted in
         * the DTT Fourier Tools.
         */
    };
    
} // namespace osc

#endif  // !defined OSCABSPOWERGAIN_HH 
