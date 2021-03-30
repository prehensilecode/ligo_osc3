// Yo, Emacs! This is -*- C++ -*-
//
// $Id: TypeInfo.hh,v 1.4 2003/04/11 21:24:10 jzweizig Exp $
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

#ifndef OSCTYPEINFO_HH
#define OSCTYPEINFO_HH

#include "PConfig.h"
#ifdef __GNU_STDC_OLD
#include <hash_map>
#else
#include <ext/hash_map>
#ifdef P__GNU
using __gnu_cxx::hash_map;
#endif
#endif
#include <vector>
#include <string>

#include "osc/misc.hh"
#include "osc/ParamInfo.hh"

namespace osc
{
    //
    // TypeInfo
    //
    // I see no way to get around needing to have this info separate from
    // the OperStateCond objects.


    /** The {\tt TypeInfo} class encapsulates information about each
     * OSC type.
     */
    class TypeInfo
    {
    public:
        //
        // Constructors
        //
        // We need to deal with optional parameters, and if there are
        // optional parameters, there must be default values for those
        // parameters. I.e. optional only in the sense that they may
        // be excluded from the config file, but they must still have
        // a well-defined value.  Hence the different types of
        // constructors. 
        //
        TypeInfo(int dbg = 0);

        TypeInfo(const ParamInfoMap& paramInfoMap,
                 bool atomic_p,
                 int dbg = 0);

        /** @return A vector  of parameter names, sorted in
         *          alphabetical order
         */
        std::vector<std::string> paramNames() const;

        /** @memo Datatype of the parameter named {\tt paramName}
         *  @param   paramName Name of the parameter
         *  @return  The datatype of the parameter
         */
        tseriesType_t paramType(std::string paramName);

        /// @return Total number of parameters
        unsigned int nParams() const;

        /// @return Number of optional parameters
        unsigned int nOptParams() const;

        /// @return Number of compulsory parameters
        unsigned int nCompulsoryParams() const;

        /// Print out a list of parameter names and types
        void printParamInfo() const;

        /// Predicate: this OSC type is atomic
        bool atomicP() const;

        /** @memo Predicate: parameter named {\tt paramName} is optional
         *  @param paramName Name of parameter to check
         *  @return True or False
         */
        bool optP(std::string paramName);

        /// Default value of integer param named {\tt paramName}
        int intDefault(std::string paramName);

        /// Default value of double param named {\tt paramName}
        double dblDefault(std::string paramName);

    private:
        /// Map of parameter information
        ParamInfoMap mParamInfoMap;
        
        /// Predicate: this OSC type is atomic
        bool mAtomicP;

        /// Debug level
        int  mDebug;
    };


    //
    // INLINE methods for TypeInfo
    //
    
    inline tseriesType_t TypeInfo::paramType(std::string paramName)
    {
        tseriesType_t retval = mParamInfoMap[paramName].type();
        return retval;
    }

    inline unsigned int TypeInfo::nParams() const
    {
        return mParamInfoMap.size();
    }

    inline bool TypeInfo::atomicP() const
    {
        return mAtomicP;
    }
    
    inline bool TypeInfo::optP(std::string paramName)
    {
        return mParamInfoMap[paramName].optP();
    }

    inline int TypeInfo::intDefault(std::string paramName)
    {
        return mParamInfoMap[paramName].intDefault();
    }

    inline double TypeInfo::dblDefault(std::string paramName)
    {
        return mParamInfoMap[paramName].dblDefault();
    }

    /// Store information about each OSC type in a hash_map.
    typedef hash_map<const std::string, TypeInfo,
		     osc::hash<const std::string>,
		     osc::eqstr >  TypeInfoMap;
    
}  // namepsace osc

#endif // !defined OSCTYPEINFO_HH
