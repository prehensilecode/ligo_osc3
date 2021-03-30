// Yo, Emacs! This is -*- C++ -*-
//
// $Id: TypeInfo.cc,v 1.1 2001/09/29 01:59:00 jzweizig Exp $
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

#include "osc/TypeInfo.hh"

using namespace std;

namespace osc
{
    TypeInfo::TypeInfo(int dbg)
        : mDebug(dbg)
    {
        if (mDebug > 0) {
            cout << "TypeInfo: constructed without paramInfoMap; "
                 << "no. of params = " << mParamInfoMap.size() << endl;
        }
    }


    TypeInfo::TypeInfo(const ParamInfoMap &paramInfoMap,
                       bool atomic_p, int dbg)
        : mParamInfoMap(paramInfoMap),
          mAtomicP(atomic_p),
          mDebug(dbg)
    {
        if (mDebug > 0) {
            cout << "TypeInfo: constructed with paramInfoMap" << endl;
            ParamInfoMap::const_iterator iter = mParamInfoMap.begin();
            for (; iter != mParamInfoMap.end(); ++iter) {
                cout << (*iter).first << ":" << endl;
                (*iter).second.printInfo();
            }
        }
    }

    
    vector<string> TypeInfo::paramNames() const
    {
        vector<string> retval;

        ParamInfoMap::const_iterator iter = mParamInfoMap.begin();
        for (; iter != mParamInfoMap.end(); ++iter) {
            retval.insert(retval.end(), (*iter).first);
        }

        return retval;
    }


    unsigned int TypeInfo::nOptParams() const
    {
        unsigned int retval = 0;

        ParamInfoMap::const_iterator iter = mParamInfoMap.begin();
        for (; iter != mParamInfoMap.end(); ++iter)
            if ((*iter).second.optP() == true)
                ++retval;

        return retval;
    }

    
    unsigned int TypeInfo::nCompulsoryParams() const
    {
        return (nParams() - nOptParams());
    }

    
    void TypeInfo::printParamInfo() const
    {
        cout << "(Parameter name, Type)" << endl;
        ParamInfoMap::const_iterator iter = mParamInfoMap.begin();
        for (; iter != mParamInfoMap.end(); ++iter) {
            cout << "("  << (*iter).first;
            (*iter).second.printInfo();
        }
    }

}; // namespace osc
