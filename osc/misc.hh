// Yo, Emacs! This is -*- C++ -*-
//
// $Id: misc.hh,v 1.7 2003/04/11 21:24:10 jzweizig Exp $
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

/** Miscellaneous definitions for use among all components of Operating
 * State Condition
 */

#ifndef OSCMISC_HH
#define OSCMISC_HH

#include "PConfig.h"
#include <string>
#ifdef __GNU_STDC_OLD
#include <hash_map>
#else
#include <ext/hash_map>
#ifdef P__GNU
using __gnu_cxx::hash_map;
#endif
#endif
#include <time.h>
#include <strings.h>

#include <cstdlib>
#include <cerrno>
#include <cctype>

// #include "Complex.hh"
#include "osc/Param.hh"

namespace osc
{
    /// Enumerated type for data types
    typedef enum {SHORT    = 0,
                  INT      = 1,
                  FLOAT    = 2,
                  DOUBLE   = 3,
                  FCOMPLEX = 4} tseriesType_t;

    /// A case-insensitive string equality function.  Used in parsing.
    inline bool stringCaseCmp(const std::string s1, const std::string s2)
    {
        return (strcasecmp(s1.c_str(), s2.c_str()) == 0);
    }

    /** We need to extend std::hash since the STL's std::hash<> is not
     * defined for std::string.  Now, we can't extend std::hash, since
     * DMT (in general/util.hh) has specialized std::hash to be
     * case-insensitive.  We'll use our own, then.
     *
     * FIXME-DONE: Actually, I'm thinking case-insensitive names are a
     * little more intuitive.  However, DMT's specialization is
     * broken: get compile-time errors when try to use it as hash
     * function in hash_map<>.
     */

    /** Function to test for equality between strings, in a
     * case-insensitive way. This is used as the comparison function
     * in hash_maps. */
    struct eqstr
    {
        bool operator()(const std::string s1, const std::string s2) const
        {
            return strcasecmp(s1.c_str(), s2.c_str()) == 0;
        }
    };

    
    template <class _Key> struct hash {};

    /// The case-insensitive hash function, in the flesh.  
    inline size_t __osc_hash_string(const char* __s)
    {
        unsigned long __h = 0;
        for ( ; *__s; ++__s)
            __h = 5*__h + tolower(*__s);

        return size_t(__h);
    }

    /// The hash function
    template <> struct hash<std::string>
    {
        size_t operator() (const std::string __s) const
            {
                return __osc_hash_string(__s.c_str());
            }
    };

    /// The hash function
    template <> struct hash<const std::string>
    {
        size_t operator() (const std::string __s) const
            {
                return __osc_hash_string(__s.c_str());
            }
    };

    
    /** Hash map for integer parameters: contains values and
     * information about channel from which to read parameter value
     * (if required)
     */
    typedef hash_map<const std::string,
		     Param<int>,
		     hash<const std::string>,
		     eqstr >                  IntParMap;

    /** Hash map for double parameters: contains values and
     * information about channel from which to read parameter value
     * (if required)
     */
    typedef hash_map<const std::string,
		     Param<double>,
		     hash<const std::string>,
		     eqstr >                  DblParMap;
} // namespace osc
                                    
#endif // !defined OSCMISC_HH
