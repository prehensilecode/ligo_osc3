// Yo, Emacs! This is -*- C++ -*-
//
// $Id: error.cc,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#include <stdlib.h>
#include <iostream>

#include "osc/boolean/error.hh"

using namespace std;

namespace osc
{
    namespace boolean
    {
        /// Count of syntax errors
        int errorCount = 0;

        /// Abort messages. Keyed to AbortCode enum type
        static const char* const abortMsg[] = {
            NULL,
            "Nesting too deep",
            "Runtime error",
            "Unimplemented feature",
        };

        /// Error messages. Keyed to ErrorCode enum type
        static const char* const errorMsg[] = {
            NULL,
            "Undefined symbol",
            "Missing )",
            "Missing \"",
            "Invalid expression",
            "Nesting too deep",
        };
    }
}

    
/// A fatal error occured during translation.
void osc::boolean::abortTranslation(AbortCode ac)
{
    cerr << "*** Fatal translation error ***"
         << abortMsg[-ac] << endl;
    exit(ac);
}

/// A non-fatal error occured during translation
void osc::boolean::error(ErrorCode ec)
{
    cerr << "*** Translation error: skipping ***"
         << errorMsg[-ec] << endl;
}

