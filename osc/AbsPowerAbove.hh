// Yo, Emacs! This is -*- C++ -*-
//
// $Id: AbsPowerAbove.hh,v 1.4 2002/05/01 18:47:43 dwchin Exp $
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

#ifndef OSCABSPOWERABOVE_HH
#define OSCABSPOWERABOVE_HH

#include "osc/OperStateCond.hh"
#include "FSeries.hh"
#include "FSpectrum.hh"

namespace osc
{
    //
    // ABSPOWERABOVE
    //
    class AbsPowerAbove
        : public OperStateCond
    {
    public:
        AbsPowerAbove(Dacc           *access,
                      std::ostream   *outstream,
                      std::string     chanName,  // name of assoc. channel
                      osc::IntParMap &intparams,
                      osc::DblParMap &dblparams,
                      int             debug = 0);

        virtual ~AbsPowerAbove() {};

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

        /** {\tt abspowerabove} has three parameters.  This OSC is
         * satisfied if the absolute power in the frequency range
         * $[{\mathtext freqlo}, {\mathtext freqhi}]$ is above {\tt threshold}
         * \begin{enumerate}
         *    \item {\bf {\tt freqlo}} Lower limit of frequency
         *    \item {\bf {\tt freqhi}} Upper limit of frequency
         *    \item {\bf {\tt threshold}} Absolute power threshold
         * \end{enumerate}
         * The power here is the square of the RMS that is plotted in
         * the DTT Fourier Tools.
         */
    };

} // namespace osc

#endif // !defined OSCABSPOWERABOVE_HH
