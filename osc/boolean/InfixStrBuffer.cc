// Yo, Emacs! This is -*- C++ -*-
//
// $Id: InfixStrBuffer.cc,v 1.3 2002/02/13 09:39:41 sigg Exp $
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
#include <fstream>
#include <string>

#include "osc/boolean/InfixStrBuffer.hh"

using namespace std;

osc::boolean::InfixStrBuffer::InfixStrBuffer(const string &infixString)
{
    mText = infixString;
    mCurCharLoc = 0;
}


osc::boolean::InfixStrBuffer::InfixStrBuffer(const char *infixString)
{
    mText = infixString;
    mCurCharLoc = 0;
}

char osc::boolean::InfixStrBuffer::curChar() const
{
    return mText[mCurCharLoc];
};


char osc::boolean::InfixStrBuffer::nextChar()
{
    if (mCurCharLoc >= (int)mText.length()) {
        return nullChar;
    } else {
        ++mCurCharLoc;  // next char

        if (mCurCharLoc < (int)mText.length())
            return mText[mCurCharLoc];
        else
            return nullChar;
    }
}
    

char osc::boolean::InfixStrBuffer::putBackChar()
{
    --mCurCharLoc;

    return mText[mCurCharLoc];
}

    
void osc::boolean::InfixStrBuffer::print() const
{
    cout << mText << "; mCurCharLoc = " << mCurCharLoc << endl;
}
