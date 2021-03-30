// Yo, Emacs! This is -*- C++ -*-
//
// $Id: ParamInfo.hh,v 1.4 2003/04/11 21:24:10 jzweizig Exp $
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

#ifndef OSCPARAMINFO_HH
#define OSCPARAMINFO_HH

#include "PConfig.h"
#include <iostream>
#include <fstream>
#ifdef __GNU_STDC_OLD
#include <hash_map>
#else
#include <ext/hash_map>
#ifdef P__GNU
using __gnu_cxx::hash_map;
#endif
#endif
#include <string>

#include "osc/misc.hh"

namespace osc
{
    //
    // ParamInfo
    //

    /** The {\tt ParamInfo} class encapsulates information about
     * parameters for OperStateCond objects:
     * \begin{enumerate}
     * \item {\bf Data type} Parameters may be of type {\tt integer}
     *       or {\tt double}, for the moment.
     * \item {\bf Optional} A parameter may or may not be optional.
     * \item {\bf Default Value} If a parameter is optional, it will
     *       have a default value.
     * \end{enumerate}
     */
    class ParamInfo
    {
    public:
        /// Default constructor
        ParamInfo();
        
        /** @memo Constructor
         *  @param typ Datatype of parameter
         *  @param int_default Default value of integer parameter
         *  @param dbl_default Default value of double parameter
         */
        ParamInfo(tseriesType_t typ,
                  bool          opt_p,
                  int           int_default = 0,
                  double        dbl_default = 0.);

        /// Destructor
        ~ParamInfo();

        //
        // Accessors
        //

        /// Type of parameter
        tseriesType_t type() const;

        /// Predicate: this parameter is optional
        bool optP() const;

        /// Default value of int param.
        int intDefault() const;

        /// Default value of double param.
        double dblDefault() const;

        /// Print out information
        void printInfo() const;
        void printInfo(std::ostream &os) const;

    private:
        /// Data type of parameter
        tseriesType_t mType;

        /// Predicate: this parameter is optional
        bool          mOptP;

        /// Default value of integer parameter
        int           mIntDefault;

        /// Default value of double parameter
        double        mDblDefault;
    };


    //
    // Inline methods for ParamInfo
    //

    inline ParamInfo::ParamInfo()
    {
        return;
    }
    
    inline ParamInfo::ParamInfo(tseriesType_t typ,
                                bool          opt_p,
                                int           int_default,
                                double        dbl_default)
        : mType(typ),
          mOptP(opt_p),
          mIntDefault(int_default),
          mDblDefault(dbl_default)
    {
        return;
    }

    
    inline ParamInfo::~ParamInfo()
    {
        return;
    }

    inline tseriesType_t ParamInfo::type() const
    {
        return mType;
    }

    inline bool ParamInfo::optP() const
    {
        return mOptP;
    }

    inline int ParamInfo::intDefault() const
    {
        return mIntDefault;
    }

    inline double ParamInfo::dblDefault() const
    {
        return mDblDefault;
    }


    /// Hash map of ParamInfo, indexed by parameter name
    typedef hash_map<const std::string, ParamInfo,
                          osc::hash<const std::string>,
                          osc::eqstr >                   ParamInfoMap;


}  // namespace osc


#endif // !defined OSCPARAMINFO_HH
