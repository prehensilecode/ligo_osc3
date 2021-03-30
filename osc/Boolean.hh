// Yo, Emacs! This is -*- C++ -*-
//
// $Id: Boolean.hh,v 1.2 2002/02/13 05:44:21 sigg Exp $
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

#ifndef OSCBOOLEAN_HH
#define OSCBOOLEAN_HH

#include "osc/OperStateCond.hh"
#include "OperStateCondList.hh"
#include "TSeries.hh"

#include "osc/boolean/Parser.hh"

#include <list>

class OperStateCondList;

namespace osc
{
    //
    // BOOLEAN
    //
    class Boolean
        : public OperStateCond
    {
    public:
        Boolean(Dacc                       *access,
                std::ostream                    *outstream,
                std::list<osc::boolean::Token*> *pToklist,
                std::string                 defStr,   // Boolean expression
                OperStateCondList          *pOscList,
                int                         debug = 0);

        virtual ~Boolean() {};

        virtual bool satisfied();

        virtual void printInfo() const;

    private:
        /// Typedef of the token RPN slist
        typedef std::list<osc::boolean::Token*> TokList;
    
        /// Singly-linked list of the RPN form of the definition string
        TokList     mTokRPN;

        /** @memo Definition string: the original infix Boolean expression from
         *        the config file that defines the Boolean */
        std::string mDefStr;

        /** @memo Pointer to OperStateCondList object which contains this
         *  Boolean  */
        OperStateCondList *pmOscList;
    
    };
} // namespace osc

#endif // !defined OSCBOOLEAN_HH
