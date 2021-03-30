// Yo, Emacs! This is -*- C++ -*-
//
// $Id: FractPowerAbove.hh,v 1.4 2002/05/01 18:47:43 dwchin Exp $
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

#ifndef OSCFRACTPOWERABOVE_HH
#define OSCFRACTPOWERABOVE_HH

#include "osc/OperStateCond.hh"
#include "FSeries.hh"
#include "FSpectrum.hh"

namespace osc
{
    //
    // FRACTPOWERABOVE
    //
    class FractPowerAbove
        : public OperStateCond
    {
    public:
        FractPowerAbove(Dacc              *access,
                         std::ostream        *outstream,
                         std::string     chanName,  // name of assoc. channel
                         osc::IntParMap &intparams,
                         osc::DblParMap &dblparams,
                         int debug = 0);

        virtual ~FractPowerAbove() {};

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

        /** {\tt FractPowerabove} has three parameters.  This OSC is
         * satisfied if the fractional power in the frequency range
         * $[{\mathtext freqlo}, {\mathtext freqhi}]$ is above {\tt
         * threshold}. The fractional power is defined as the ratio of
         * the power in the frequency band $[{\mathtext lo},
         * {\mathtext hi}]$ to the full-band power.
         * \begin{enumerate}
         *    \item {\bf {\tt freqlo}} Lower limit of frequency
         *    \item {\bf {\tt freqhi}} Upper limit of frequency
         *    \item {\bf {\tt threshold}} Fractional power threshold
         * \end{enumerate}
         */
    };

} // namespace osc

#endif // !defined OSCFRACTPOWERABOVE_HH
