// Yo, Emacs! This is -*- C++ -*-
//
// $Id: misc.hh,v 1.3 2002/02/13 05:44:21 sigg Exp $
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

#ifndef OSCBOOLEANMISC_HH
#define OSCBOOLEANMISC_HH

namespace osc
{
    namespace boolean
    {
        const char nullChar = '\0';
    
        /// Character codes
        enum CharCode {
            ccLetter, ccDigit, ccUnary, ccBinary, ccSpecial, ccWhiteSpace,
            ccColon, ccUnderScore, ccEndOfString, ccError
        };

    
        /// Token codes
        enum TokenCode {
            tcDummy, tcWord, tcSpecial, tcUnary, tcBinary,
            tcEndOfString, tcError
        };
        
    } // namespace boolean
} // namespace osc

#endif // !defined OSCBOOLEANMISC_HH
