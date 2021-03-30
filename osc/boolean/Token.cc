// Yo, Emacs! This is -*- C++ -*-
//
// $Id: Token.cc,v 1.3 2002/02/13 09:39:41 sigg Exp $
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
#include <iostream>
#include <string>

#include "osc/boolean/Token.hh"


////////////////
// WordToken
////////////////
//
// Extract a word token from the infix string
// A word begins with a letter, and comprises letters, digits, 
// underscores, and colons. (Begins with a letter: see OSCBooleanScanner)
//
void osc::boolean::WordToken::get(InfixStrBuffer *pBuffer)
{
    char   ch  = pBuffer->curChar();
    int    pos = 0;     // position in the token string

    // Extract word
    do {
        mTokenStr += ch;
        ++pos;
        ch = pBuffer->nextChar();
    } while (charCodeMap[ch] == ccLetter     ||
             charCodeMap[ch] == ccDigit      ||
             charCodeMap[ch] == ccUnderScore ||
             charCodeMap[ch] == ccColon);

    mCode = tcWord;
}


///////////////
// UnaryToken
///////////////
//
// Extract a unary operator token from the infix string
// Unary operators consist of a single character
//
void osc::boolean::UnaryToken::get(InfixStrBuffer *pBuffer)
{
    mTokenStr += pBuffer->curChar();
    pBuffer->nextChar();
    mCode = tcUnary;
}


////////////////
// BinaryToken
////////////////
//
// Extract a binary operator token from the infix string.
// Binary operators consist of a single character
//
void osc::boolean::BinaryToken::get(InfixStrBuffer *pBuffer)
{
    mTokenStr += pBuffer->curChar();
    pBuffer->nextChar();
    mCode = tcBinary;
}

    
//////////////////
// SpecialToken
//////////////////
//
// Extract a special token from the infix string
// Special tokens, i.e. only parentheses, consist of a single
// character
//
void osc::boolean::SpecialToken::get(InfixStrBuffer *pBuffer)
{
    mTokenStr += pBuffer->curChar();
    pBuffer->nextChar();
    mCode = tcSpecial;
}


/////////////
// EOSToken
/////////////
//
// Extract an end of string token
//
void osc::boolean::EOSToken::get(InfixStrBuffer *pBuffer)
{
    mTokenStr += pBuffer->curChar();
    mCode = tcEndOfString;
}

    
/////////////////
// ErrorToken
/////////////////
//
// Extract an invalid character from the infix string
void osc::boolean::ErrorToken::get(InfixStrBuffer *pBuffer)
{
    mTokenStr += pBuffer->curChar();
    pBuffer->nextChar();
}

