// Yo, Emacs! This is -*- C++ -*-
//
// $Id: OperStateCondList.hh,v 1.5 2003/04/11 21:24:10 jzweizig Exp $
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

#ifndef OPERSTATECONDLIST_HH
#define OPERSTATECONDLIST_HH

#include "PConfig.h"
#include <map>
#ifdef __GNU_STDC_OLD
#include <hash_map>
#else
#include <ext/hash_map>
#ifdef P__GNU
using __gnu_cxx::hash_map;
#endif
#endif
#include <list>
#include <vector>

#include "osc/misc.hh"
#include "osc/ParamInfo.hh"
#include "osc/TypeInfo.hh"
#include "osc/OperStateCond.hh"


/** @name OSC Package
 *
 *  @memo Package for defining and testing data channel conditions
 *        in the Data Monitor Tool
 *
 *  @version 2.1
 *  @author  David Chin <dwchin@umich.edu>
 *
 */

//@Include: osc/users_guide.hh


/// OperStateCondList: Set of Operational State Conditions

class OperStateCondList
    : public hash_map<const std::string, osc::OperStateCond*,
                      osc::hash<const std::string>, osc::eqstr >
{
public:
    //
    // CONSTRUCTORS
    //
    /** @memo Default OperStateCondList constructor.
     * @param dataAccess Data Accessor object
     * @param debug Debug level
     */
    OperStateCondList(Dacc& dataAccess, 
                      int debug = 0,
                      int stride = 0,
                      std::ostream *pOs = &std::clog);

    //
    // DESTRUCTOR
    //
    ~OperStateCondList();

    //
    // Other methods
    //
    
    /** @memo Read configuration file
     *  @param filename Name of config file to read from
     */
    void readConfig(std::string filename);
    void readConfig(const char *filename);

    /** @memo Is condition named {\tt oscname} satisfied?
     *  @param oscname Name of OSC
     *  @return True or False
     */
    bool satisfied(std::string oscname);

    /** @memo Gets debugging level
     */
    int debug(void) const;

    /** @memo Gets stride
     */
    int  stride(void) const;

    /** @memo Sets time stride
     *  @param stride Stride in seconds
     */
    void setStride(int stride);

    /** @memo Sets debugging level
     *  @param debuglvl Debug level
     */
    void setDebug(int debuglvl);

    //
    // These next three are just wrappers to handle the case when the
    // OSC asked for does not exist.
    //
    /** @memo Sets debugging level for named OSC. (Also, can use
     * \texttt{hash\_map} indexing to set debug level of particular OSCs.) 
     * @param oscname Name of OSC
     * @param debuglvl Debug level
     */
    void setDebug(std::string oscname, int debuglvl);

    /** @memo Removes OSC from OperStateCondList. (Aliases the
     *        \texttt{hash\_map<>::erase()} method.)
     * @param oscname Name of OSC to be removed
     */
    void ignore(std::string oscname);

    /** @memo Removes OSCs from OperStateCondList except those named
     *  "prefix*" (the asterisk is a wildcard).  Boolean conditions,
     *  which do not depend on data channels directly, are not
     *  removed.  NOTE: This can be dangerous, especially if you have
     *  hidden dependencies on OSCs that read other channels.
     *  @param prefix Prefix of data channel name for which to
     *  retain OSCs
     */
    void ignoreAllExcept(std::string prefix);

    /** @memo Get value of quantity being monitored (\textit{i.e.}
     * amplitude, power (RMS), etc.
     * @param oscname Name of OSC from which quantity is to be taken
     */
    double watchedQuantity(std::string oscname);

    /** @memo Print channel utilization info (mainly for devel. debugging)
     */
    void chanUtilInfo(void) const;

    /** @memo Print all defined OSCs
     */
    void listOSCs(void) const;

private:
    /// Data accessor from which to read data.
    Dacc&    mDataAccess;

    /// Output logging stream
    std::ostream *pmOutstream;

    /// Debug level
    int      mDebug;

    /// Time stride, i.e. time length of TSeries objects
    double   mStride;

    /// Maximum length of char[] strings
    static const int mMaxStrLen = 512;

    /** List of channels used, tied to the number of OSCs that
     *  read them.  This is so that when we destruct an OSC, we
     *  know if there are any other OSCs that read that channel,
     *  and so know if it is safe to remove said channel from
     *  Dacc's channel list.
     *  Dunno how to initialize a hash_map, so do this instead of
     *  making it a static class member of OperStateCond.
     */
    typedef hash_map<const std::string, size_t,
                     osc::hash<const std::string>,
                     osc::eqstr>                    ChannelUtilMap;
    ChannelUtilMap mChannelUtilization;

    /** @memo Increment channel utilization count or add to it.
     *  @param chanName Name of channel
     */
    void incrChanUtilMaybe(std::string chanName);

    //
    // Utility functions
    //

    /// Re-entrant tokenizer
    static char *strtok_r(char *s, const char *delim, char **lasts);

    /** @memo Tokenizer
     *  @param line A line consisting of tokens separated by
     *             whitespace (space and tab)
     *  @return A vector containing the tokens
     */
    //std::vector<std::string> tokenize(std::string line);

    /** @memo Parameter-parsing function for all "atomic" OSC types
     *  @return 0 if no errors; >0 if error
     */
    int parseAtomicParams(char token[][mMaxStrLen], int lineNum,
                          std::string filename,
                          osc::IntParMap *intParams,
                          osc::DblParMap *doubleParams);

    
    /// Remove double-quotes from a string
    void removeDblQuotes(std::string *str);

    /// Remove double-quotes from a string
    void removeDblQuotes(char   *str);

    /** @memo Predicate: name has been defined already
     * @param oscname Name of OSC to check
     * @return True or False
     */
    bool oscNameDefinedP(std::string oscName);

    //  I would really like the TypeInfoMap to be a static member, but
    // according to "experts" who post on comp.lang.c++ there's no
    // way to initialize such a complex structure at compile time.
    // All this means is that we'll be memory-inefficient if there
    // are more than one OperStateCondList objects in the user's
    // program..
    
    /// Map of OSC type information
    osc::TypeInfoMap mTypeInfoMap;

    /** @memo The copy constructor and assignment operator are to
     * satisfy the "Rule of 3".  They are declared private to avoid
     * people using them, and the compiler generating them. (See also
     * Stroustrup, \textit{The C++ Programming Language}, Sec.11.2.2)
     */
    //OperStateCondList(const OperStateCondList&);

    /// Assignment
    void operator =(const OperStateCondList&);

    /// Reference
    //void operator &();

    /// Sequence
    void operator ,(const OperStateCondList&);
};



#endif // !defined OPERSTATECONDLIST_HH
