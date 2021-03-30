// Yo, Emacs! This is -*- C++ -*-
//
// $Id: Scanner.cc,v 1.4 2002/02/13 09:39:41 sigg Exp $
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

#include <time.h>
#include "osc/boolean/Scanner.hh"

namespace osc
{
    namespace boolean
    {
        /// Maps a character to its code
        CharCode charCodeMap[128];
    }
}

using namespace std;

osc::boolean::TextScanner::TextScanner(const string &buffer)
{
    pmBuffer = new InfixStrBuffer(buffer);

    //
    // Initialize the character code map
    //
    int i;
    for (i = 0; i <= 127; ++i)
        charCodeMap[i] = ccError;

    for (i = 'a'; i <= 'z'; ++i)
        charCodeMap[i] = ccLetter;

    for (i = 'A'; i <= 'Z'; ++i)
        charCodeMap[i] = ccLetter;

    for (i = '0'; i <= '9'; ++i)
        charCodeMap[i] = ccDigit;

    charCodeMap['_'] = ccUnderScore;
    charCodeMap[':'] = ccColon;
    charCodeMap['!'] = ccUnary;
    charCodeMap['&'] = charCodeMap['|'] = ccBinary;
    charCodeMap['('] = charCodeMap[')'] = ccSpecial;
    charCodeMap[' '] = charCodeMap['\t'] = ccWhiteSpace;
    charCodeMap[nullChar] = ccEndOfString;
}


//
// skipWhiteSpace()
// Repeatedly fetch characters from the text input as long as they're
// white space.
//
void osc::boolean::TextScanner::skipWhiteSpace()
{
    char ch = pmBuffer->curChar();

    while (charCodeMap[ch] == ccWhiteSpace)
        ch = pmBuffer->nextChar();
}

//
// get()
// Extract the next token from the infix string, based on the current
// character
//
osc::boolean::Token *osc::boolean::TextScanner::get()
{
    Token *pToken;  // ptr to token to return

    skipWhiteSpace();

    // Determine the token class, based on the current character
    switch (charCodeMap[pmBuffer->curChar()]) {
    case ccLetter:
        pToken = new WordToken;
        break;

    case ccUnary:
        pToken = new UnaryToken;
        break;

    case ccBinary:
        pToken = new BinaryToken;
        break;

    case ccSpecial:
        pToken = new SpecialToken;
        break;

    case ccEndOfString:
        pToken = new EOSToken;
        break;

    default:
        pToken = new ErrorToken;
        break;
    }

    // Extract a token of that class, and return a pointer to it
    pToken->get(pmBuffer);
    return pToken;
}

