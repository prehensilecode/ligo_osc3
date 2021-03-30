// Yo, Emacs! This is -*- C++ -*-
//
// $Id: Parser.hh,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

// NOTE: The object structure of Parser, Scanner, and Token are from
// Roland Mak's book.

#ifndef OSCBOOLEANPARSER_HH
#define OSCBOOLEANPARSER_HH

#include <list>
#include <stack>

#include "osc/boolean/error.hh"
#include "osc/boolean/misc.hh"
#include "osc/boolean/InfixStrBuffer.hh"
#include "osc/boolean/Token.hh"
#include "osc/boolean/Scanner.hh"


namespace osc
{
    namespace boolean
    {
        /// Parser class for the OSC type Boolean.
        class Parser
        {
        public:
            Parser(std::string bufStr);

            ~Parser();

            /** @memo Parse infix definition string into Reverse Polish,
             *  and store it in pToklist
             */
            void parse(std::list<Token*> *pToklist);

        private:
            /// Ptr to the scanner
            TextScanner *const pmScanner;

            /// Ptr to the current token
            Token *pmToken;

            /// Code of current token
            TokenCode mTokenCode;

            /// Get a token
            void getToken();

            /// Precedence of token in infix string
            int inPrec(Token *tok);

            /// Precedence of token in stack
            int stPrec(Token *tok);

            /// Rank of token
            int rank(Token *tok);

            /// Debug level
            int mDebug;
        };


        //
        // Inline Parser methods
        //
    
        inline Parser::Parser(std::string bufStr)
            : pmScanner(new TextScanner(bufStr))
        {
            return;
        }

        
        inline Parser::~Parser()
        {
            delete pmScanner;
        }


        inline void Parser::getToken()
        {
            pmToken    = pmScanner->get();
            mTokenCode = pmToken->code();
        }

    } // namespace boolean
} // namespace osc


#endif // !defined OSCBOOLEANPARSE.HH
