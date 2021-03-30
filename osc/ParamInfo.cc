// Yo, Emacs! This is -*- C++ -*-
//
// $Id: ParamInfo.cc,v 1.2 2002/02/13 09:39:41 sigg Exp $
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
#include <cstdio>

#include "osc/ParamInfo.hh"

using namespace std;

void osc::ParamInfo::printInfo() const
{
    cout << "Data type: ";

    switch(type()) {
    case osc::SHORT:
        cout << "short" << endl;
        break;

    case osc::INT:
        cout << "int" << endl;
        break;

    case osc::FLOAT:
        cout << "float" << endl;
        break;

    case osc::DOUBLE:
        cout << "double" << endl;
        break;

    case osc::FCOMPLEX:
        cout << "fComplex" << endl;
        break;

    default:
        cout << "unknown" << endl;
        break;
    }

    cout << "Optional: ";
    if (optP() == true)
        cout << "Yes" << endl;
    else
        cout << "No" << endl;

    cout << "Default value: ";
    switch(type()) {
    case osc::INT:
        printf("%5d\n", intDefault());
        break;

    case osc::DOUBLE:
        printf("%12.8e\n", dblDefault());
        break;

    default:
        cout << "unknown" << endl;
        break;
    }
}


void osc::ParamInfo::printInfo(ostream &os) const
{
    os << "Data type: ";

    switch(type()) {
    case osc::SHORT:
        os << "short" << endl;
        break;

    case osc::INT:
        os << "int" << endl;
        break;

    case osc::FLOAT:
        os << "float" << endl;
        break;

    case osc::DOUBLE:
        os << "double" << endl;
        break;

    case osc::FCOMPLEX:
        os << "fComplex" << endl;
        break;

    default:
        os << "unknown" << endl;
        break;
    }

    os << "Optional: ";
    if (optP() == true)
        os << "Yes" << endl;
    else
        os << "No" << endl;

    os << "Default value: ";
    switch(type()) {
    case osc::INT:
        printf("%5d\n", intDefault());
        break;

    case osc::DOUBLE:
        printf("%12.8e\n", dblDefault());
        break;

    default:
        os << "unknown" << endl;
        break;
    }
}
