// Yo, Emacs! This is -*- C++ -*-
//
// $Id: error.hh,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#ifndef OSCBOOLEANERROR_H
#define OSCBOOLEANERROR_H

namespace osc
{
    namespace boolean
    {
        extern int errorCount;

        enum AbortCode {
            abortNestingTooDeep       = -8,
            abortRuntimeError         = -9,
            abortUnimplementedFeature = -10
        };

        enum ErrorCode {
            errorUndefinedSymbol = -1,
            errorMissingRightParen = -2,
            errorMissingDblQuote = -3,
            errorInvalidExpression = -4,
            errorNestingTooDeep = -5
        };

        void abortTranslation(AbortCode ac);

        void error(ErrorCode ec);

    } // namespace boolean
} // namespace osc


#endif // !defined OSCBOOLEANERROR_H
