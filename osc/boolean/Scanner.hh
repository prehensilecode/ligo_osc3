// Yo, Emacs! This is -*- C++ -*-
//
// $Id: Scanner.hh,v 1.2 2002/02/13 09:39:41 sigg Exp $
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

#ifndef OSCBOOLEANSCANNER_HH
#define OSCBOOLEANSCANNER_HH

#include "osc/boolean/misc.hh"
#include "osc/boolean/InfixStrBuffer.hh"
#include "osc/boolean/Token.hh"

namespace osc
{
    namespace boolean
    {
        class Scanner
        {
        public:
            virtual ~Scanner() {};
            virtual Token *get() = 0;

        protected:
            //WordToken    mWordToken;
            //SpecialToken mSpecialToken;
            //UnaryToken   mUnaryToken;
            //BinaryToken  mBinaryToken;
            //ErrorToken   mErrorToken;
        };


        class TextScanner
            : public Scanner
        {
        public:
            TextScanner(const std::string &buffer);

            virtual ~TextScanner();

            virtual Token *get();
        
        private:
            InfixStrBuffer *pmBuffer;
        
            void skipWhiteSpace();
        };


        inline TextScanner::~TextScanner()
        {
            delete pmBuffer;
        }

    } // namespace boolean
} // namespace osc

#endif // !defined OSCBOOLEANSCANNER_HH
