// Yo, Emacs! This is -*- C++ -*-
//
// $Id: InfixStrBuffer.hh,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#ifndef OSCBOOLEANINFIXSTRBUFFER_HH
#define OSCBOOLEANINFIXSTRBUFFER_HH

#include <string>

#include "osc/boolean/misc.hh"

namespace osc
{
    namespace boolean
    {
        /** @memo Buffer for infix boolean string, used for parsing definition
         * string of a Boolean OSC into RPN form */
        class InfixStrBuffer
        {
        public:
            InfixStrBuffer(const std::string&);
            InfixStrBuffer(const char*);
            ~InfixStrBuffer() {};

            char curChar() const;
            char nextChar();
            char putBackChar();

            void print() const;

        protected:
            /// Infix string buffer
            std::string mText;

            /// Location of current char in text buffer
            int  mCurCharLoc;
        };

    } // namespace boolean
} // namespace osc

#endif // !defined OSCBOOLEANINFIXSTRBUFFER_HH
