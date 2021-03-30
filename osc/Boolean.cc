// Yo, Emacs! This is -*- C++ -*-
//
// $Id: Boolean.cc,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#include <errno.h>
#include <assert.h>
#include <stack>

#include "osc/Boolean.hh"

using namespace std;

osc::Boolean::Boolean(Dacc              *access,
                      ostream           *outstream,
                      TokList           *pTokRPN,
                      string             defStr, // Boolean expr.
                      OperStateCondList *pOscList,
                      int                debug)
    : OperStateCond(access, outstream, std::string(), "boolean", false, debug),
      mTokRPN(*pTokRPN),
      mDefStr(defStr),
      pmOscList(pOscList)
{
    if (mDebug > 0) {
        cout << "osc::Boolean: constructed with definition: "
             << defStr << endl;
    }
    
    return;
}

bool osc::Boolean::satisfied()
{
    //
    // Loop through our RPN slist, and pick out the word tokens.  Each
    // is the name of an OSC, so do the test.
    //

    // Evaluation stack
    stack<bool> evalStack;
    bool        a, b;
    
    TokList::iterator iter = mTokRPN.begin();
    for (; iter != mTokRPN.end(); ++iter) {
        // word
        if ((*iter)->code() == osc::boolean::tcWord) {
            a = (*pmOscList)[(*iter)->tokenString()]->satisfied();
            evalStack.push(a);
        }

        // unary op, i.e. NEGATION; take top, negate, pop, push result
        else if ((*iter)->code() == osc::boolean::tcUnary) {
            a = !evalStack.top();
            evalStack.pop();
            evalStack.push(a);
        }

        // binary op, i.e. AND or OR;
        else if ((*iter)->code() == osc::boolean::tcBinary) {
            a = evalStack.top();
            evalStack.pop();
            b = evalStack.top();
            evalStack.pop();

            if ((*iter)->tokenString() == "&")
                evalStack.push((a && b));
            else if ((*iter)->tokenString() == "|")
                evalStack.push((a || b));
            else {
                // HUH? Can never get here
                cerr << "osc::Boolean::satisfied(): unknown error" << endl;
                exit(EXIT_FAILURE);
            }
        }

        else {
            // HUH? Can never get here
            cerr << "osc::Boolean::satisfied(): unknown error" << endl;
            exit(EXIT_FAILURE);
        }
    }  // for (; iter != mTokRPN.end(); ++iter)

    // We should now have only one value on our evalStack
    assert(evalStack.size() == 1);

    return evalStack.top();
}


//
// Print definition string to stdout.
//
void osc::Boolean::printInfo() const
{
    cout << "osc::boolean: definition string: " << mDefStr << endl;
}

