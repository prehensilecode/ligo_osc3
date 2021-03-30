// Yo, Emacs! This is -*- C++ -*-
//
// $Id: AbsPowerRange.hh,v 1.6 2003/04/11 21:24:10 jzweizig Exp $
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

#ifndef OSCABSPOWERRANGE_HH
#define OSCABSPOWERRANGE_HH

#include "FSeries.hh"
#include "FSpectrum.hh"

#include "osc/OperStateCond.hh"

namespace osc
{
    //
    // ABSPOWERRANGE
    //
    class AbsPowerRange
        : public OperStateCond
    {
    public:
        AbsPowerRange(Dacc           *access,
                      std::ostream   *outstream,
                      std::string     chanName,  // name of assoc. channel
                      osc::IntParMap &intparams,
                      osc::DblParMap &dblparams,
                      int             debug = 0);

        virtual ~AbsPowerRange() {};

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

        /** {\tt AbsPowerRange} has three parameters.  This OSC is
         * satisfied if the absolute power in the frequency range
         * $[{\mathtext freqlo}, {\mathtext freqhi}]$ is between
         * {\tt lo} and {\tt hi}
         * \begin{enumerate}
         *    \item {\bf {\tt freqlo}} Lower limit of frequency
         *    \item {\bf {\tt freqhi}} Upper limit of frequency
         *    \item {\bf {\tt lo}} Lower limit of abs. power
         *    \item {\bf {\tt hi}} Upper limit of abs. power
         * \end{enumerate}
         * The power here is the square of the RMS that is plotted in
         * the DTT Fourier Tools.
         */
    };

}; // namespace osc

#endif // !defined OSCABSPOWERRANGE_HH
