// Yo, Emacs! This is -*- C++ -*-
//
// $Id: Parser.cc,v 1.4 2002/02/13 09:39:41 sigg Exp $
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
#include <list>

#include "osc/boolean/InfixStrBuffer.hh"
#include "osc/boolean/Parser.hh"

using namespace std; 

// The algorithm for parsing an infix string into an RPN string is from
// Discrete Mathematical Structures with Applications to Computer
// Science by Tremblay and Manohar, McGraw Hill, 1975.
//
// In converting an infix string to an RPN string, we use ranks
// and precedence. These will also allow us to evaluate validity
// of infix expression.
//
// Symbol       infix prec.     stack prec.     rank
//  &,|             1               2            -1
//  !               3               4             0
// variables        7               8             1
//  (               9               0             n.a.
//  )               0               n.a.          n.a.
//
// Algorithm:
// ----------
// 1. stack.push('(')
// 2. rank := 0
// 3. next := nextchar(infix_string)
// 4. if next = ENDofSTRING then    // end of infix string
//        goto 7.
// 5. if infix_prec(next) > stack_prec(stack.top()) then // stack next?
//        stack.push(next)
//        goto 3.
// 6. if infix_prec(next)= stack_prec(stack.top()) then // move to RPN?
//        stack.pop()
//        goto 3.
//    else
//        rpn.append(stack.top())
//        cur_rank := cur_rank + rank(stack.top())
//        if cur_rank <= 0 then
//            // invalid infix string
//            exit  // or throw exception
//        else
//            stack.pop()
//            goto 5.
// 7. if stack.empty() = False OR cur_rank != 1 then
//        // invalid infix string
//    else
//        // valid infix string
//    exit

// parse()
// Parse the infix string, and stuff it into a singly-linked list of
// tokens in RPN order.
// NOTE: this depends on the OperStateCondList::readConfig() method
// having appended a ')' to the end of the "definition string"
void osc::boolean::Parser::parse(list<Token*> *pTokRPN)
{
    // Back of the RPN string
    //slist<Token*>::iterator back = pTokRPN->previous(pTokRPN->end());
        
    // Stack for parsing
    stack<Token*> parseStack;
    Token        *pToken;

    // Current rank of infix string
    int curRank = 0;

    pToken = new SpecialToken("(");
    parseStack.push(pToken);

getNextToken:
    getToken();

    if (mTokenCode == tcEndOfString)
        goto end;

stackNextToken:
    // push token onto stack?
    if (inPrec(pmToken) > stPrec(parseStack.top())) {
        parseStack.push(pmToken);
        goto getNextToken;
    }

    // move token to RPN string?
    if (inPrec(pmToken) == stPrec(parseStack.top())) {
        parseStack.pop();
        goto getNextToken;
    } else {
        //back = pTokRPN->insert_after(back, parseStack.top());
        pTokRPN->push_back(parseStack.top());

        curRank += rank(parseStack.top());

        if (curRank <= 0) {
            // invalid infix Boolean expression
            // FIXME: skip this and clear out RPN string
            cout << "ERROR: invalid infix string" << endl;
            exit (1);
        } else {
            parseStack.pop();
            goto stackNextToken;
        }
    }

end:
    if (parseStack.empty() == false || curRank != 1) {
        // invalid infix Boolean expression
        // FIXME: exception handling: skip this and move on
        cout << "Parser::parse: invalid Boolean expression" << endl;
        exit(1);
    } else {
        // valid Boolean expression; do nothing
        ;
    }
}

//
// Precendence value of a token in the infix string
//
int osc::boolean::Parser::inPrec(Token *tok)
{
    switch (tok->code()) {
    case tcUnary:
        return 3;
        break;

    case tcBinary:
        return 1;
        break;

    case tcWord:
        return 7;
        break;

    case tcSpecial:
        if (tok->tokenString() == "(")
            return 9;
        else if (tok->tokenString() == ")")
            return 0;
        else {
            cerr << "Parser::inPrec(): unknown error" << endl;
            exit(EXIT_FAILURE);
        }
        break;

    default:
        cerr << "Parser::inPrec(): unknown error: token is '" 
             << tok->tokenString() << "'" << endl;
        exit(EXIT_FAILURE);
    }
}


//
// Precedence value of a token in the stack
//
int osc::boolean::Parser::stPrec(Token *tok)
{
    switch (tok->code()) {
    case tcUnary:
        return 4;
        break;

    case tcBinary:
        return 2;
        break;

    case tcWord:
        return 8;
        break;

    case tcSpecial:
        if (tok->tokenString() == "(")
            return 0;
        else {
            cerr << "Parser::inPrec(): unknown error" << endl;
            exit(EXIT_FAILURE);
        }
        break;

    default:
        cerr << "Parser::inPrec(): unknown error" << endl;
        exit(EXIT_FAILURE);
    }
}


//
// Rank of a token
//
int osc::boolean::Parser::rank(Token *tok)
{
    switch (tok->code()) {
    case tcBinary:
        return -1;
        break;

    case tcUnary:
        return 0;
        break;

    case tcWord:
        return 1;
        break;

    default:
        return 0;
    }
}
    


