// Yo, Emacs! This is -*- C++ -*-
//
// $Id: Token.hh,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#ifndef OSCBOOLEANTOKEN_HH
#define OSCBOOLEANTOKEN_HH

#include "osc/boolean/misc.hh"
#include "osc/boolean/InfixStrBuffer.hh"


namespace osc
{
    namespace boolean
    {
        // defined in Scanner.cc
        /// Map of char codes
        extern osc::boolean::CharCode charCodeMap[];

        /// Base token class
        class Token
        {
        public:
            Token();

            virtual     ~Token() {};

            TokenCode    code() const;

            std::string  tokenString() const;

            virtual void get(InfixStrBuffer *pBuffer) = 0;

        protected:
            /// Code of the token
            TokenCode   mCode;

            /// The string containing the literal token
            std::string mTokenStr;
        };


        //
        // Inline Token:: methods
        //
        inline Token::Token()
        {
            mCode = tcDummy;
            mTokenStr = "";
        }

        inline TokenCode Token::code() const
        {
            return mCode;
        }

        inline std::string Token::tokenString() const
        {
            return mTokenStr;
        }



        /// Word token (usually symbols, i.e. OSCs defined prior)
        class WordToken
            : public Token
        {
        public:
            virtual ~WordToken() {};
            virtual void get(InfixStrBuffer *pBuffer);
        };


        /// Unary operator token
        class UnaryToken
            : public Token
        {
        public:
            virtual ~UnaryToken() {};
            virtual void get(InfixStrBuffer *pBuffer);
        };


        /// Binary operator token
        class BinaryToken
            : public Token
        {
        public:
            virtual ~BinaryToken() {};
            virtual void get(InfixStrBuffer *pBuffer);
        };


        /// Special token (will be parentheses only)
        class SpecialToken
            : public Token
        {
        public:
            SpecialToken();
            SpecialToken(std::string tstr);
            virtual ~SpecialToken() {};
            virtual void get(InfixStrBuffer *pBuffer);
        };


        //
        // Inline methods for SpecialToken
        //
        inline SpecialToken::SpecialToken()
        {
            return;
        }

    
        inline SpecialToken::SpecialToken(std::string tstr)
        {
            mCode = tcSpecial;
            mTokenStr = tstr;
        }
        

    
        /// End of string token
        class EOSToken
            : public Token
        {
        public:
            virtual ~EOSToken() {};
            virtual void get(InfixStrBuffer *pBuffer);
        };


        /// Error token
        class ErrorToken
            : public Token
        {
        public:
            ErrorToken();
            virtual ~ErrorToken() {};

            virtual void get(InfixStrBuffer *pBuffer);
        };


        //
        // Inline ErrorToken:: methods
        //
        inline ErrorToken::ErrorToken()
        {
            mCode = tcError;
        }

    } // namespace boolean
} // namespace osc

#endif // !defined OSCBOOLEANTOKEN_HH
