// Yo, Emacs! This is -*- C++ -*-
//
// $Id: OperStateCondList.cc,v 1.10 2003/04/11 21:24:10 jzweizig Exp $
//
// Author: David Chin <dwchin@umich.edu>
//
//         2477 Randall Lab, Univ. of Michigan
//         500 E. University Ave.
//         Ann Arbor, MI 48109
//         +1-734-763-7208
//         +1-734-730-1274
//
// Written based on idea by K. Riles <kriles@umich.edu>

#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <list>

#include <cstdlib>  // aka stdlib.h

#include "osc/boolean/error.hh"
#include "osc/boolean/Token.hh"
#include "osc/boolean/Parser.hh"
#include "OperStateCondList.hh"

#include "osc/ValueAbove.hh"
#include "osc/ValueBelow.hh"
#include "osc/ValueRange.hh"

#include "osc/MeanAbove.hh"
#include "osc/MeanBelow.hh"
#include "osc/MeanRange.hh"

#include "osc/MeanRise.hh"
#include "osc/MeanFall.hh"

#include "osc/RmsAbove.hh"
#include "osc/RmsBelow.hh"
#include "osc/RmsRange.hh"

#include "osc/BitAnd.hh"
#include "osc/BitNand.hh"
#include "osc/BitOr.hh"
#include "osc/BitNor.hh"

#include "osc/AbsPowerAbove.hh"
#include "osc/AbsPowerBelow.hh"
#include "osc/AbsPowerRange.hh"

#include "osc/FractPowerAbove.hh"
#include "osc/FractPowerBelow.hh"
#include "osc/FractPowerRange.hh"

#include "osc/AbsPowerRise.hh"
#include "osc/AbsPowerFall.hh"
#include "osc/AbsPowerGain.hh"

#include "osc/FractPowerRise.hh"
#include "osc/FractPowerFall.hh"

#include "osc/Boolean.hh"

#include "osc/TransitUp.hh"
#include "osc/TransitDown.hh"


using namespace std;
using namespace osc;


//
// Constructor
//
OperStateCondList::OperStateCondList(Dacc& dataAccess,
                                     int debug,
                                     int stride,
                                     std::ostream *pOs)
    : mDataAccess(dataAccess),
      pmOutstream(pOs),
      mDebug(debug),
      mStride(stride)
{
    //
    // Initialize the TypeInfoMap.  
    //

    // Aauugghh!! I can't think of a better way to initialize each TypeInfo
    // object with its ParamInfoMap. In fact, the
    // comp.lang.c++.moderated posters say it's not possible to do a
    // compile-time initialization for a complex structure as this.

    // NOTE: the "fraction" parameter is only applicable for
    // "point-by-point" OSCs.

    // Reminder: ParamInfo(dataType, OptionalP);
    //           ParamInfo(dataType, OptionalP, int_default, dbl_default)

    // At some point, will allow for "debug" parameter
  
    // BOOLEAN
    osc::ParamInfoMap booleanPIM;

    // TRANSITUP
    osc::ParamInfoMap transitupPIM;
    transitupPIM["hold"] = osc::ParamInfo(osc::INT, true, 0, 0.);
    transitupPIM["dead"] = osc::ParamInfo(osc::INT, true, 0, 0.);

    // TRANSITDOWN
    osc::ParamInfoMap transitdownPIM;
    transitdownPIM["hold"] = osc::ParamInfo(osc::INT, true, 0, 0.);
    transitdownPIM["dead"] = osc::ParamInfo(osc::INT, true, 0, 0.);

    // VALUEABOVE
    osc::ParamInfoMap valueabovePIM;
    valueabovePIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);
    valueabovePIM["fraction"] = osc::ParamInfo(osc::DOUBLE, true, 0, 0.);

    // VALUEBELOW
    osc::ParamInfoMap valuebelowPIM;
    valuebelowPIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);
    valuebelowPIM["fraction"] = osc::ParamInfo(osc::DOUBLE, true, 0, 0.);

    // VALUERANGE
    osc::ParamInfoMap valuerangePIM;
    valuerangePIM["lo"] = osc::ParamInfo(osc::DOUBLE, false);    
    valuerangePIM["hi"] = osc::ParamInfo(osc::DOUBLE, false);
    valuerangePIM["fraction"] = osc::ParamInfo(osc::DOUBLE, true, 0, 0.);

    // MEANABOVE
    osc::ParamInfoMap meanabovePIM;
    meanabovePIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);

    // MEANBELOW
    osc::ParamInfoMap meanbelowPIM;
    meanbelowPIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);

    // MEANRANGE
    osc::ParamInfoMap meanrangePIM;
    meanrangePIM["lo"] = osc::ParamInfo(osc::DOUBLE, false);
    meanrangePIM["hi"] = osc::ParamInfo(osc::DOUBLE, false);

    // MEANRISE
    osc::ParamInfoMap meanrisePIM;
    meanrisePIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);
    meanrisePIM["nstrides"]  = osc::ParamInfo(osc::INT, true, 1, 0.);
    
    // MEANFALL
    osc::ParamInfoMap meanfallPIM;
    meanfallPIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);
    meanfallPIM["nstrides"]  = osc::ParamInfo(osc::INT, true, 1, 0.);
    
    // RMSABOVE
    osc::ParamInfoMap rmsabovePIM;
    rmsabovePIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);

    // RMSBELOW
    osc::ParamInfoMap rmsbelowPIM;
    rmsbelowPIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);

    // RMSRANGE
    osc::ParamInfoMap rmsrangePIM;
    rmsrangePIM["lo"] = osc::ParamInfo(osc::DOUBLE, false);
    rmsrangePIM["hi"] = osc::ParamInfo(osc::DOUBLE, false);

    // BITAND
    osc::ParamInfoMap bitandPIM;
    bitandPIM["fraction"] = osc::ParamInfo(osc::DOUBLE, true, 0, 0.);
    bitandPIM["mask"] = osc::ParamInfo(osc::INT, false);

    // BITNAND
    osc::ParamInfoMap bitnandPIM;
    bitnandPIM["fraction"] = osc::ParamInfo(osc::DOUBLE, true, 0, 0.);
    bitnandPIM["mask"] = osc::ParamInfo(osc::INT, false);

    // BITOR
    osc::ParamInfoMap bitorPIM;
    bitorPIM["fraction"] = osc::ParamInfo(osc::DOUBLE, true, 0, 0.);
    bitorPIM["mask"] = osc::ParamInfo(osc::INT, false);

    // BITNOR
    osc::ParamInfoMap bitnorPIM;
    bitnorPIM["fraction"] = osc::ParamInfo(osc::DOUBLE, true, 0, 0.);
    bitnorPIM["mask"] = osc::ParamInfo(osc::INT, false);

    // ABSPOWERABOVE
    osc::ParamInfoMap abspowerabovePIM;
    abspowerabovePIM["freqhi"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerabovePIM["freqlo"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerabovePIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);

    // ABSPOWERBELOW
    osc::ParamInfoMap abspowerbelowPIM;
    abspowerbelowPIM["freqhi"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerbelowPIM["freqlo"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerbelowPIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);

    // ABSPOWERRANGE
    osc::ParamInfoMap abspowerrangePIM;
    abspowerrangePIM["freqhi"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerrangePIM["freqlo"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerrangePIM["lo"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerrangePIM["hi"] = osc::ParamInfo(osc::DOUBLE, false);

    // FRACTPOWERABOVE
    osc::ParamInfoMap fractpowerabovePIM;
    fractpowerabovePIM["freqhi"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerabovePIM["freqlo"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerabovePIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);

    // FRACTPOWERBELOW
    osc::ParamInfoMap fractpowerbelowPIM;
    fractpowerbelowPIM["freqhi"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerbelowPIM["freqlo"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerbelowPIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);

    // FRACTPOWERRANGE
    osc::ParamInfoMap fractpowerrangePIM;
    fractpowerrangePIM["freqhi"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerrangePIM["freqlo"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerrangePIM["lo"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerrangePIM["hi"] = osc::ParamInfo(osc::DOUBLE, false);

    // ABSPOWERRISE
    osc::ParamInfoMap abspowerrisePIM;
    abspowerrisePIM["freqhi"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerrisePIM["freqlo"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerrisePIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerrisePIM["nstrides"]  = osc::ParamInfo(osc::INT, true, 1, 0.);

    // ABSPOWERFALL
    osc::ParamInfoMap abspowerfallPIM;
    abspowerfallPIM["freqhi"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerfallPIM["freqlo"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerfallPIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowerfallPIM["nstrides"]  = osc::ParamInfo(osc::INT, true, 1, 0.);

    // ABSPOWERGAIN
    osc::ParamInfoMap abspowergainPIM;
    abspowergainPIM["freqhi"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowergainPIM["freqlo"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowergainPIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);
    abspowergainPIM["nstrides"] = osc::ParamInfo(osc::INT, true, 1, 0.);

    // FRACTPOWERRISE
    osc::ParamInfoMap fractpowerrisePIM;
    fractpowerrisePIM["freqhi"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerrisePIM["freqlo"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerrisePIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerrisePIM["nstrides"] = osc::ParamInfo(osc::INT, true, 1, 0.);

    // FRACTPOWERFALL
    osc::ParamInfoMap fractpowerfallPIM;
    fractpowerfallPIM["freqhi"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerfallPIM["freqlo"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerfallPIM["threshold"] = osc::ParamInfo(osc::DOUBLE, false);
    fractpowerfallPIM["nstrides"] = osc::ParamInfo(osc::INT, true, 1, 0.);

    // DEBUG
    if (mDebug > 2) {
        cout << "OperStateCondList: constructing (debug level "
             << mDebug << ")..." << endl;
        
        cout << "\tboolean has " << booleanPIM.size()
             << " parameters" << endl;
        
        cout << "\ttransitup has " << transitupPIM.size()
             << " parameters" << endl;
        
        cout << "\ttransitdown has " << transitdownPIM.size()
             << " parameters" << endl;
        
        cout << "\tvalueabove has " << valueabovePIM.size()
             << " parameters" << endl;

        cout << "\tvaluebelow has " << valuebelowPIM.size()
             << " parameters" << endl;

        cout << "\tvaluerange has " << valuerangePIM.size()
             << " parameters" << endl;

        cout << "\tmeanabove has " << meanabovePIM.size()
             << " parameters" << endl;
        
        cout << "\tmeanbelow has " << meanbelowPIM.size()
             << " parameters" << endl;

        cout << "\tmeanrange has " << meanrangePIM.size()
             << " parameters" << endl;

        cout << "\tmeanrise has " << meanrisePIM.size()
             << " parameters" << endl;

        cout << "\tmeanfall has " << meanfallPIM.size()
             << " parameters" << endl;
        
        cout << "\trmsabove has " << rmsabovePIM.size()
             << " parameters" << endl;

        cout << "\trmsbelow has " << rmsbelowPIM.size()
             << " parameters" << endl;

        cout << "\trmsrange has " << rmsrangePIM.size()
             << " parameters" << endl;

        cout << "\tbitand has " << bitandPIM.size()
             << " parameters" << endl;

        cout << "\tbitnand has " << bitnandPIM.size()
             << " parameters" << endl;

        cout << "\tbitor has " << bitorPIM.size()
             << " parameters" << endl;

        cout << "\tbitnor has " << bitnorPIM.size()
             << " parameters" << endl;
        
        cout << "\tabspowerabove has " << abspowerabovePIM.size()
             << " parameters" << endl;

        cout << "\tabspowerbelow has " << abspowerbelowPIM.size()
             << " parameters" << endl;

        cout << "\tabspowerrange has " << abspowerrangePIM.size()
             << " parameters" << endl;

        cout << "\tfractpowerabove has " << fractpowerabovePIM.size()
             << " parameters" << endl;

        cout << "\tfractpowerbelow has " << fractpowerbelowPIM.size()
             << " parameters" << endl;

        cout << "\tfractpowerrange has " << fractpowerrangePIM.size()
             << " parameters" << endl;

        cout << "\tabspowerrise has " << abspowerrisePIM.size()
             << " parameters" << endl;

        cout << "\tabspowerfall has " << abspowerfallPIM.size()
             << " parameters" << endl;

        cout << "\tabspowergain has " << abspowergainPIM.size()
             << " parameters" << endl;

        cout << "\tfractpowerrise has " << fractpowerrisePIM.size()
             << " parameters" << endl;

        cout << "\tfractpowerfall has " << fractpowerfallPIM.size()
             << " parameters" << endl;
        
        cout << endl;

        cout << "OperStateCondList::OperStateCondList(): about to put PIMs "
             << "into TypeInfoMap" << endl;
    }


    // Put all the PIMs from above into the TypeInfoMap
    mTypeInfoMap["boolean"] = osc::TypeInfo(booleanPIM, false);
    mTypeInfoMap["transitup"] = osc::TypeInfo(transitupPIM, false);
    mTypeInfoMap["transitdown"] = osc::TypeInfo(transitdownPIM, false);
    mTypeInfoMap["valueabove"] = osc::TypeInfo(valueabovePIM, true);
    mTypeInfoMap["valuebelow"] = osc::TypeInfo(valuebelowPIM, true);
    mTypeInfoMap["valuerange"] = osc::TypeInfo(valuerangePIM, true);
    mTypeInfoMap["meanabove"] = osc::TypeInfo(meanabovePIM, true);
    mTypeInfoMap["meanbelow"] = osc::TypeInfo(meanbelowPIM, true);
    mTypeInfoMap["meanrange"] = osc::TypeInfo(meanrangePIM, true);
    mTypeInfoMap["meanrise"] = osc::TypeInfo(meanrisePIM, true);
    mTypeInfoMap["meanfall"] = osc::TypeInfo(meanfallPIM, true);
    mTypeInfoMap["rmsabove"] = osc::TypeInfo(rmsabovePIM, true);
    mTypeInfoMap["rmsbelow"] = osc::TypeInfo(rmsbelowPIM, true);
    mTypeInfoMap["rmsrange"] = osc::TypeInfo(rmsrangePIM, true);
    mTypeInfoMap["bitand"] = osc::TypeInfo(bitandPIM, true);
    mTypeInfoMap["bitnand"] = osc::TypeInfo(bitnandPIM, true);
    mTypeInfoMap["bitor"] = osc::TypeInfo(bitorPIM, true);
    mTypeInfoMap["bitnor"] = osc::TypeInfo(bitnorPIM, true);
    mTypeInfoMap["abspowerabove"] = osc::TypeInfo(abspowerabovePIM, true);
    mTypeInfoMap["abspowerbelow"] = osc::TypeInfo(abspowerbelowPIM, true);
    mTypeInfoMap["abspowerrange"] = osc::TypeInfo(abspowerrangePIM, true);
    mTypeInfoMap["fractpowerabove"] = osc::TypeInfo(fractpowerabovePIM, true);
    mTypeInfoMap["fractpowerbelow"] = osc::TypeInfo(fractpowerbelowPIM, true);
    mTypeInfoMap["fractpowerrange"] = osc::TypeInfo(fractpowerrangePIM, true);
    mTypeInfoMap["abspowerrise"] = osc::TypeInfo(abspowerrisePIM, true);
    mTypeInfoMap["abspowerfall"] = osc::TypeInfo(abspowerfallPIM, true);
    mTypeInfoMap["abspowergain"] = osc::TypeInfo(abspowergainPIM, true);
    mTypeInfoMap["fractpowerrise"] = osc::TypeInfo(fractpowerrisePIM, true);
    mTypeInfoMap["fractpowerfall"] = osc::TypeInfo(fractpowerfallPIM, true);

    if (mDebug > 1) {
        cout << "OperStateCondList(): mTypeInfoMap.size() = "
             << mTypeInfoMap.size() << endl;
        osc::TypeInfoMap::iterator iter = mTypeInfoMap.begin();
        for (; iter != mTypeInfoMap.end(); ++iter) {
            cout << "\tNo. of params in " << (*iter).first
                 << ": " << (*iter).second.nParams() << endl;
            cout << "\tNo. of optional params in " << (*iter).first
                 << ": " << (*iter).second.nOptParams() << endl;
            cout << "\tNo. of compulsory params in " << (*iter).first
                 << ": " << (*iter).second.nCompulsoryParams()
                 << endl << endl;
        }
    }
}


//
// Destructor
//
OperStateCondList::~OperStateCondList()
{
    return;
}

//
// debug(void)
//
int OperStateCondList::debug(void) const
{
    return mDebug;
}


//
// setDebug(int debug)
//
void OperStateCondList::setDebug(int debuglvl)
{
    mDebug = debuglvl;
}

//
// stride(void)
//
int OperStateCondList::stride(void) const
{
    return (int)mStride;
}

//
// setStride(int stride)
//
void OperStateCondList::setStride(int stride)
{
    if (stride > 0) {
        mStride = (double)stride;
    } else {
        cerr << "OperStateCondList::setStride(): "
             << "stride must be positive definite. Using default: ";
        mStride = mDataAccess.getStride().GetSecs();
        if (mStride == 0.) {
            mStride = 1.;
        }

        cerr << mStride << " secs" << endl;
    }

    mDataAccess.setStride(mStride);
}

//
// setDebug(std::string, int)
//
void OperStateCondList::setDebug(std::string oscname, int debuglvl)
{
    if (find(oscname) != end()) {
        if (mDebug > 0)
            cerr << "OperStateCondList: setting debug level of '"
                 << oscname << "' to " << debuglvl << endl;
        ((*this)[oscname])->setDebug(debuglvl);
    } else {
        if (mDebug > 0) 
            cerr << "OperStateCondList: ERROR: setDebug(): no OSC named '"
                 << oscname << "'; ignoring" << endl;
    }
}

//
// ignore
//
void OperStateCondList::ignore(std::string oscname)
{
    // if OSC exists
    if (find(oscname) != end()) {
        
        if (mDebug > 0) {
            cerr << "OperStateCondList: NOTICE: ignoring condition '"
                 << oscname << "'" << endl;
        }

        // if there is a channel associated with the OSC, fix up the
        // channel utilisation number
        if ((*this)[oscname]->channel().size() != 0) {

            if (mDebug > 1) {
                cerr << "OperStateCondList::ignore() INFO: channel name: "
                     << (*this)[oscname]->channel() << endl;
                cerr << "OperStateCondList::ignore() INFO: utilization: "
                     << mChannelUtilization[(*this)[oscname]->channel()]
                     << endl;
            }
            
            // If there is only one OSC utilizing the channel associated
            // with this OSC, then this OSC is the only one utilizing the
            // channel. Thus, we may remove it from the DataAccessor.
            if (mChannelUtilization[(*this)[oscname]->channel()] == 1) {
                if (mDebug > 1) {
                    cerr << "OperStateCondList: INFO: removing channel '"
                         << (*this)[oscname]->channel() << "'" << endl;
                }
                mDataAccess.rmChannel((*this)[oscname]->channel().c_str());
                mChannelUtilization.erase((*this)[oscname]->channel());
            } else { // more than one OSC uses this channel
                mChannelUtilization[(*this)[oscname]->channel()] -= 1;
            }
        }
        
        delete (*this)[oscname]; 
        erase(oscname);
        
    } else { // OSC does not exist; fuggedaboudit
        
        if (mDebug > 0)
            cerr << "OperStateCondList: ERROR: ignore(): no OSC named '"
                 << oscname << "'; skipping" << endl;
            
        return;
    }
}

//
// ignoreAllExcept()
//
void OperStateCondList::ignoreAllExcept(std::string prefix)
{
    if (mDebug > 0) {
        cerr << "OperStateCondList::ignoreAllExcept(): prefix: "
             << prefix << endl;
    }

    if (prefix.size() == 0) {
        cerr << "OperStateCondList::ignoreAllExcept(): zero length prefix; "
             << "skipping" << endl;
        return;
    }

    OperStateCondList::iterator iter;
top:
    iter = begin();
    for (; iter != end(); ++iter) {
        if ((*iter).first.size() > 0 &&
            (*iter).first.substr(0,prefix.size()) != prefix) {
            ignore((*iter).first);
            goto top;
        }
    }
}

//
// watchedQuantity()
//
double OperStateCondList::watchedQuantity(std::string oscname)
{
    if (find(oscname) != end()) {
        return (*this)[oscname]->watchedQuantity();
    } else {
        if (mDebug > 0) {
            cerr << "OperStateCondList: ERROR: watchedQuantity(): no OSC named '"
                 << oscname << "'; ignoring" << endl;
        }
        return (double)0.;
    }
}


//
// chanUtilInfo() -- only for debugging
//
void OperStateCondList::chanUtilInfo(void) const
{
    if (mDebug > 0) {
        cerr << "OperStateCondList: INFO: Channel Utilization Info: " << endl;
        cerr << "\tsize = " << mChannelUtilization.size() << endl;
        ChannelUtilMap::const_iterator iter = mChannelUtilization.begin();
        for (; iter != mChannelUtilization.end(); ++iter)
            cerr << "\t" << (*iter).first << ": " << (*iter).second << endl;
    }
}

//
// listOSCs()
//
void OperStateCondList::listOSCs(void) const
{
    std::cerr << "OperStateCondList: INFO: OSCs defined:" << std::endl;
    const_iterator iter = begin();
    for (; iter != end(); ++iter)
        std::cerr << "\t" << (*iter).first << std::endl;
}


//
// readConfig()
//
// File format:
//    # This is a comment
//    include <foo.conf>  # this is in some std. dir
//    include "bar.conf"  # this is in same dir as this file
//    OSC_name   OSC_type   Channel_name  param_name=param_value 
//    OSC_name   OSC_type   Channel_name  param_name="channel_name"
//
void OperStateCondList::readConfig(std::string filename)
{
    if (mDebug > 0)
        cout << "OperStateCondList: Reading config file "
             << filename << endl;

    // Open config file for reading
    ifstream configfile(filename.c_str());

    if (!configfile) {
        cout << "OperStateCondList.readConfig(): error opening file "
             << filename.c_str() << endl;
        std::exit(EXIT_FAILURE);
    }

    char  line[mMaxStrLen]; // char array to store line currently being parsed
    char *lasts = 0;  // pointer to last position in line being parsed;
                      // for use in strtok_r()  
    std::string oscName;     // Name of OSC
    std::string typeName;    // Name of OSC type
    std::string chanName;    // Name of Channel
    static const int maxTokens = 64;
    char  token[maxTokens][mMaxStrLen]; // Array of tokens pulled from line
    int   nTokens = 0;                  // Number of tokens pulled from line
    int   lineNum = 0;                  // Line number
    static int   numErrors = 0;         // tot. number of errors in
                                        //    reading config file

    if (mDebug > 1) {
        cout << "OperStateCondList.readconfig(): mTypeInfoMap.size() = "
             << mTypeInfoMap.size() << endl;
    }


    // Read file in line by line
    while (!configfile.eof()) {

        // read in next line
        configfile.getline(line, mMaxStrLen);

        ++lineNum;
        if (mDebug > 0) {
            cout << "OperStateCondList.readConfig(): Read line #"
                 << lineNum << endl;
            
            if (mDebug > 1)
                cout << "\tLine is: " << line << endl;
        }

        // if line is not zero-length
        if (strlen(line) > 0) {
            
            // Skip comments: lines beginning with "#" or "*"
            if (strchr("#", line[0]) != NULL || strchr("*", line[0]) != NULL) 
                continue; // go to next line

            //
            // Parse line
            //

            // Count number of tokens in line, and put tokens into an array
            // of char[]'s
            nTokens = 0;
            static const char *sepChars = " \t";
            char *p = strtok_r(line, sepChars, &lasts);
            while (p != NULL) {
                strcpy(token[nTokens], p);

                if (mDebug > 1)
                    cout << "token[" << nTokens << "] = "
                         << token[nTokens] << endl; 
                
                // skip inline comments
                if (p[0] == '#' || p[0] == '*')
                    break;

                ++nTokens;
                p = strtok_r(NULL, sepChars, &lasts);
            }

            //
            // Form a parse tree...
            //

            //
            // INCLUDE
            //
            if (strcmp(token[0], "include") == 0) {
                if (nTokens == 1) {
                    cerr << "Include file not specified in line #"
                         << lineNum << "; skipping" << endl;
                } else {
                    if (token[1][0] == '"') {  // include "file.conf"
                        removeDblQuotes(token[1]);
                        readConfig(token[1]);
                    } else if (token[1][0] == '<') {  // include <file.conf>
                        int len = strlen(token[1]);

                        // Remove angle brackets
                        if (token[1][len-1] != '>') {
                            ++numErrors;
                            cerr << filename << ": line #" << lineNum
                                 << ": Error: Missing '>'; skipping" 
                                 << endl;
                        } else {
                            char *tmpstr = new char[len];
                            for (int cnt = 0; cnt < len; ++cnt)
                                tmpstr[cnt] = '\0';
                            strncpy(tmpstr, token[1]+1, len-2);
                            strcpy(token[1], tmpstr);
                            delete [] tmpstr;
                        }

                        // Extract DMTPARS env. variable
                        char *tmpdmtpars = getenv("DMTPARS");
                        if (tmpdmtpars == NULL) {
                            ++numErrors;
                            cerr << filename << ":" << "line #" << lineNum
                                 << ": Error"
                                 << ": Environment variable DMTPARS undefined;"
                                 << " skipping" << endl;
                        } else {
                            std::string dmtpars = tmpdmtpars;
                            readConfig(dmtpars + "/" + std::string(token[1]));
                        }
                    } else {
                        ++numErrors;
                        cerr << filename << ":" << "line #" << lineNum 
                             << ": Error: syntax; skipping" << endl;
                    }
                }

                continue; // go to next line
            } // End: INCLUDE

            //
            // IGNORE
            // Note: User is responsible for dependencies.
            //
            if (strcmp(token[0], "ignore") == 0) {
                if (nTokens == 1) {
                    ++numErrors;
                    cerr << filename << ":" << "line #" << lineNum
                         << ": Error: must specify OSC name; skipping" << endl;
                } else if (token[1][0] == '"') {
                    removeDblQuotes(token[1]);
                    
                    if (find(token[1]) == end()) {
                        ++numErrors;
                        cerr << filename << ":" << "line #" << lineNum
                             << ": Error: OSC named '" << token[1]
                             << "' not found; skipping" << endl;
                    } else {
                        if (mDebug > 0) {
                            cout << "OperStateCondList.readConfig(): "
                                 << "ignore " << token[1] << endl;
                        }
                        
                        ignore(token[1]);
                    }
                } else {
                    ++numErrors;
                    cerr << filename << ":" << "line #" << lineNum
                         << ": Error: need double-quotes around OSC name"
                         << endl;
                }

                continue;
            } // End: IGNORE

            //
            // IGNOREALLEXCEPT
            // Format: ignoreallexcept "H1" "H0"
            //
            if (strcmp(token[0], "ignoreallexcept") == 0) {
                if (nTokens == 1) {
                    ++numErrors;
                    cerr << filename << ":" << "line #" << lineNum
                         << ": Error: must specify site name(s); skipping"
                         << endl;
                } else if (token[1][0] == '"') {
                    removeDblQuotes(token[1]);
                    ignoreAllExcept(token[1]);
                } else {
                    ++numErrors;
                    cerr << filename << ":" << "line #" << lineNum
                         << ": Error: need double-quotes around "
                         << "string '" << token[1]
                         << "'; skipping" << endl;
                }
                
                continue;
            } // End: IGNOREALLEXCEPT

            //
            // DEBUGLEVEL
            // Note: User is responsible for a sensible (i.e. integer)
            // debuglevel 
            //
            if (strcmp(token[0], "debuglevel") == 0) {
                if (nTokens == 1) {
                    ++numErrors;
                    cerr << filename << ":" << "line #" << lineNum 
                         << ": Error: Debug level not specified; skipping"
                         << endl;
                } else {
                    setDebug(atoi(token[1]));
                }

                continue;
            } // End: DEBUGLEVEL

            //
            // STRIDE
            // Sets the time stride that is used for TSeries objects.
            // Must be an integer, although internally stored as a double.
            //
            if (strcmp(token[0], "stride") == 0) {
                if (nTokens == 1) {
                    ++numErrors;
                    cerr << filename << ":" << "line #" << lineNum
                         << ":Error: Time stride not specified; skipping"
                         << endl;
                } else {
                    setStride(atoi(token[1]));
                }

                continue;
            } // END: STRIDE

            //
            // Define OSCs
            //

            if (nTokens < 3) {
                ++numErrors;
                cerr << filename << ":" << "line #" << lineNum 
                     << ": Error: Incomplete line; skipping" << endl;
                continue;  // go to next line
            }
            
            // New OSCs are defined by a line that looks like:
            // name OSCtype "channel_name" paramname=param
            //    [paramname=param ...]
            // name boolean "definition_string"
            
            oscName = token[0];
            typeName = token[1];

            // Skip if this name has already been defined
            if (oscNameDefinedP(oscName) == true) {
                ++numErrors;
                cerr << filename << ":" << "line #" << lineNum
                     << ": Error: Redefinition of OSC named \""
                     << oscName << "\": skipping" << endl;
                continue; // go to next line
            }

            // Skip if no such OSC
            if (mTypeInfoMap.find(typeName) == mTypeInfoMap.end()) {
                ++numErrors;
                cerr << filename << ":" << "line #" << lineNum << ": Error"
                     << ": no such OSC type \"" << typeName
                     << "\"; skipping" << endl;
                continue; // go to next line
            }
            
            //
            // Parse the parameter definitions
            //
            // We need to allow for a channel name in the value
            // position so that parameter values may be read from a
            // named Epics channel.  Do this by making two maps: one
            // which stores a bool saying if param is to be read from
            // channel; other one contains channel name

            // Pick out the parameters
            unsigned int      nParams;      // number of params
            osc::IntParMap    intParams;    // map of param values
            osc::DblParMap    dblParams;    // map of param values
            
            lasts = 0;                // place holder for strtok_r

            // DEBUG
            if (mDebug > 2) {
                cout << "OperStateCondList.readConfig(): "
                     << "mTypeInfoMap.size() = "
                     << mTypeInfoMap.size() << endl;
            }

            // if atomic OSC type
            if (mTypeInfoMap[typeName].atomicP() == true) {

                // number of compulsory parameters
                nParams = mTypeInfoMap[typeName].nCompulsoryParams();

                // do we have the wrong number of parameters?
                if ((int)nParams > nTokens - 3) {
                    ++numErrors;
                    cerr << filename << ": line #" << lineNum
                         << ": Error: Too few parameters #" << lineNum
                         << "; skipping" << endl;
                    
                    if (mDebug > 0) {
                        cerr << "   got " << nTokens - 3 << "; expect "
                             << nParams << endl;
                    }
                    continue;
                } else {  // we have the correct number of params
                    chanName = token[2];
                    removeDblQuotes(&chanName);

                    if (mDebug > 2)
                        cout << "OperStateCondList::readConfig(): chanName = "
                             << chanName << endl;

                    // Stuff parameter values into maps
                    int error = parseAtomicParams(token, lineNum,
                                                  filename,
                                                  &intParams, &dblParams);

                    if (error > 0) { // FIXME: ignore errors and skip
                                     // to next line for now
                        ++numErrors;
                        continue;    // go to next line
                    }

                    // Parameters are now in maps. Construct OSC type
                    // with these parameters.
                    if (osc::stringCaseCmp(typeName,"valueabove")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new ValueAbove(&mDataAccess, pmOutstream,
                                           chanName, intParams,
                                           dblParams, mDebug);
                        
                    } else if (osc::stringCaseCmp(typeName,"valuebelow")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new ValueBelow(&mDataAccess, pmOutstream,
                                           chanName, intParams,
                                           dblParams, mDebug);

                    } else if (stringCaseCmp(typeName,"valuerange")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new ValueRange(&mDataAccess, pmOutstream,
                                           chanName, intParams,
                                           dblParams, mDebug);
                        
                    } else if (osc::stringCaseCmp(typeName,"meanabove")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new MeanAbove(&mDataAccess, pmOutstream,
                                          chanName, intParams,
                                          dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"meanbelow")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new MeanBelow(&mDataAccess, pmOutstream,
                                          chanName, intParams,
                                          dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"meanrange")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new MeanRange(&mDataAccess, pmOutstream,
                                          chanName, intParams,
                                          dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"meanrise")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new MeanRise(&mDataAccess, pmOutstream,
                                         chanName, intParams,
                                         dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"meanfall")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new MeanFall(&mDataAccess, pmOutstream,
                                         chanName, intParams,
                                         dblParams, mDebug);

                    
                    } else if (osc::stringCaseCmp(typeName,"rmsabove")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new RmsAbove(&mDataAccess, pmOutstream,
                                         chanName, intParams,
                                         dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"rmsbelow")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new RmsBelow(&mDataAccess, pmOutstream,
                                         chanName, intParams,
                                         dblParams, mDebug);
                        
                    } else if (osc::stringCaseCmp(typeName,"rmsrange")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new RmsRange(&mDataAccess, pmOutstream,
                                         chanName, intParams,
                                         dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"bitand")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new BitAnd(&mDataAccess, pmOutstream,
                                       chanName, intParams,
                                       dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"bitnand")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new BitNand(&mDataAccess, pmOutstream,
                                        chanName, intParams,
                                        dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"bitor")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new BitOr(&mDataAccess, pmOutstream,
                                      chanName, intParams,
                                      dblParams, mDebug);
                        
                    } else if (osc::stringCaseCmp(typeName,"bitnor")) {

                        incrChanUtilMaybe(chanName);
    
                        (*this)[oscName] =
                            new BitNor(&mDataAccess, pmOutstream,
                                       chanName, intParams,
                                       dblParams, mDebug);
                    
                    } else if (osc::stringCaseCmp(typeName,"abspowerabove")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new AbsPowerAbove(&mDataAccess, pmOutstream,
                                              chanName, intParams,
                                              dblParams, mDebug);
                        
                    } else if (osc::stringCaseCmp(typeName,"abspowerbelow")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new AbsPowerBelow(&mDataAccess, pmOutstream,
                                              chanName, intParams,
                                              dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"abspowerrange")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new AbsPowerRange(&mDataAccess, pmOutstream,
                                              chanName, intParams,
                                              dblParams, mDebug);
                        
                    } else if (osc::stringCaseCmp(typeName,"fractpowerabove")) {

                        (*this)[oscName] =
                            new FractPowerAbove(&mDataAccess, pmOutstream,
                                                chanName, intParams,
                                                dblParams, mDebug);
                        
                    } else if (osc::stringCaseCmp(typeName,"fractpowerbelow")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new FractPowerBelow(&mDataAccess, pmOutstream,
                                                chanName, intParams,
                                                dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"fractpowerrange")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new FractPowerRange(&mDataAccess, pmOutstream,
                                                chanName, intParams,
                                                dblParams, mDebug);
                        
                    } else if (osc::stringCaseCmp(typeName,"abspowerrise")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new AbsPowerRise(&mDataAccess, pmOutstream,
                                             chanName, intParams,
                                             dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"abspowerfall")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new AbsPowerFall(&mDataAccess, pmOutstream,
                                             chanName, intParams,
                                             dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"abspowergain")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new AbsPowerGain(&mDataAccess, pmOutstream,
                                             chanName, intParams,
                                             dblParams, mDebug);

                    } else if (osc::stringCaseCmp(typeName,"fractpowerrise")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new FractPowerRise(&mDataAccess, pmOutstream,
                                               chanName, intParams,
                                               dblParams, mDebug);
                        
                    } else if (osc::stringCaseCmp(typeName,"fractpowerfall")) {

                        incrChanUtilMaybe(chanName);

                        (*this)[oscName] =
                            new FractPowerFall(&mDataAccess, pmOutstream,
                                               chanName, intParams,
                                               dblParams, mDebug);
                        
                    }
                } // if ((int)nParams...) ... else ...
                
            } else { // NOT atomic OSC type
                
                if (osc::stringCaseCmp(typeName,"boolean")) { // BOOLEAN

                    // Since we tokenized on whitespace above, we now
                    // have put the boolean definition string back
                    // together by concatenating
                    std::string defstr;
                    int         cnt;
                    for (cnt = 2; cnt < nTokens-1; ++cnt)
                        defstr += std::string(token[cnt]) + std::string(" ");
                    defstr += std::string(token[cnt]);

                    if (mDebug > 2)
                        cout << "OperStateCondList::readConfig() line "
                             << __LINE__ << ": defstr = '"
                             << defstr << "'" << endl;
                    
                    removeDblQuotes(&defstr);

                    // Parse the infix string, and put the tokens into a
                    // singly-linked list.
                    // For the infix->RPN algorithm, we need to append ')'
                    // to the end of the definition string
                    defstr += ")";

                    if (mDebug > 2)
                        cout << "OperStateCondList::readConfig() line "
                             << __LINE__ << ": defstr = '"
                             << defstr << "'" << endl;

                    list<osc::boolean::Token*> tokRPN;
                    osc::boolean::Parser parser(defstr);
                    parser.parse(&tokRPN);

                    // Check that each word token in tokRPN corresponds to
                    // an OSC already defined in the config file.
                    // This also ensures that the osc::Boolean definition
                    // string does not refer to itself. 
                    list<osc::boolean::Token*>::const_iterator iter =
                        tokRPN.begin();
                    for (; iter != tokRPN.end(); ++iter) {
                        
                        if ((*iter)->code() == osc::boolean::tcWord)
                            if (find((*iter)->tokenString()) == end()) {
                                ++numErrors;
                                cerr << filename << ":" << "line #"
                                     << lineNum << ": Error"
                                     << ": Boolean: " << oscName
                                     << ": OSC named '"
                                     << (*iter)->tokenString()
                                     << "' not found; skipping" << endl;
                                goto skipBoolean;
                            }

                    } // for (; iter != tokRPN.end(); ++iter)

                    // Now, construct BOOLEAN type OSC, and put it
                    // in our map
                    (*this)[std::string(oscName)] =
                        new Boolean(&mDataAccess, pmOutstream, &tokRPN,
                                    defstr.substr(0,defstr.length()-1),
                                    this, mDebug);
                    
                skipBoolean:
                    continue;
                } // end BOOLEAN

                //
                // The transitup and transitdown OSCs are similar to atomic
                // OSCs
                //

                if (typeName.substr(0, 7) == "transit") {  // TRANSIT
                
                    std::string referredOsc = std::string(token[2]);

                    // Remove double-quotes if we have to
                    if (referredOsc[0] == '"' &&
                        referredOsc[referredOsc.length()-1] == '"')
                        removeDblQuotes(&referredOsc);
                    
                    OperStateCondList::const_iterator iter = find(referredOsc);

                    // have to declare and initialize here because the
                    // goto in the next block would jump over the declaration
                    int error = 0;

                    if (iter == end()) {
                        ++numErrors;
                        cerr << filename << ":" << "line #" << lineNum
                             << ": Error"
                             << ": " << typeName
                             << ": " << oscName
                             << ": OSC named '"
                             << referredOsc << "' not found; skipping" << endl;
                        goto skipTransit;  // gloria mundi
                    }

                    // Stuff parameter values into vectors
                    error = parseAtomicParams(token, lineNum,
                                              filename,
                                              &intParams, &dblParams);

                    if (error > 0) {
                        ++numErrors;
                        continue;
                    }

                    // Call constructor
                    if (typeName.substr(7, 2) == "up") { // TRANSITUP
                        (*this)[std::string(oscName)] =
                            new TransitUp(&mDataAccess, pmOutstream,
                                          referredOsc, intParams,
                                          dblParams, this, mDebug);
                    }


                    else if (typeName.substr(7, 4) == "down") { // TRANSITDOWN
                        (*this)[std::string(oscName)] =
                            new TransitDown(&mDataAccess, pmOutstream,
                                            referredOsc, intParams,
                                            dblParams, this, mDebug);
                    }

                    else {
                        ++numErrors;
                        cerr << filename << ":" << "line #" << lineNum
                             << ": Error: unknown error; skipping" << endl;
                        goto skipTransit;
                    }

                skipTransit:  // gloria mundi
                    continue;
                }  // end TRANSIT
                
            } // if (mTypeInfoMap.atomicP() == true) ... else ...
            
        } // !(strlen(line) > 0)
        
    } // while()

    // Close config file stream
    configfile.close();

    if (mDebug > 0) {
        cout << "OperStateCondList::readConfig(): Done reading config file "
             << filename << ". " << numErrors << " errors encountered" << endl;
    }


    if (numErrors > 0) {
        cerr << "OperStateCondList::readConfig(): Done reading config file "
             << filename << ". " << numErrors << " errors encountered" << endl;
    }

    // If stride has not been set, do it now: passing setStride() a
    // non-positive value just sets mStride to the stride 
    // mDataAccess.getStride().getSecs()
    if (mStride == 0)
        setStride(0);
    
    return;
} // END: readConfig(std::string)

//
// Overload of readConfig()
//
void OperStateCondList::readConfig(const char *filename)
{
    if (mDebug > 1) {
        cout << "OperStateCondList::readConfig(const char*) entered" << endl;
        cout << "                   filename = " << filename << endl;
    }
    
    readConfig(std::string(filename));
} // END: readConfig(const char *filename)


//
// Is condition named 'name' satisfied?
//
bool OperStateCondList::satisfied(std::string oscname)
{
    if (find(oscname) != end())
        return (*this)[oscname]->satisfied();
    else {
        cerr << "OperStateCondList: ERROR: no OSC named '"
             << oscname << "'" << endl;
        exit(1);
    }
}

//
// Increment channel utilization counter, or add a new entry
//
void OperStateCondList::incrChanUtilMaybe(std::string chanName)
{
    if (mChannelUtilization.find(chanName) != mChannelUtilization.end()) {
        if (mDebug > 1) {
            cerr << "OperStateCondList: INFO: incr. channel util. for '"
                 << chanName << "'" << endl;
        }
        mChannelUtilization[chanName] += 1;
    } else {
        if (mDebug > 1) {
            cerr << "OperStateCondList: INFO: creating channel util. for '"
                 << chanName << "'" << endl;
        }
        mChannelUtilization[chanName] = 1;
    }
}

//
// strtok_r(): re-entrant version of strtok()
//
/* This code for strtok_r() is from:
 * http://www.ajk.tele.fi/libc/string/strtok_r.c.html
 */
/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

char *OperStateCondList::strtok_r(register char *s,
                                  register const char *delim,
                                  register char **lasts)
{
    register char *spanp;
    register int c, sc;
    char *tok;

    if (s == NULL && (s = *lasts) == NULL)
        return (NULL);

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
cont:
    c = *s++;
    for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
        if (c == sc)
            goto cont;
    }

    if (c == 0) {           /* no non-delimiter characters */
        *lasts = NULL;
        return (NULL);
    }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;) {
        c = *s++;
        spanp = (char *)delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *lasts = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
} // END: strtok_r()



//
// This is not used, unfortunately... One day, we'll get rid of the
// C-ish stuff.
// tokenize()
// Uses " " and "\t" as separator characters; ignores inline comments
//     marked by '#'
//
/*
vector<std::string> tokenize(std::string line)
{
    static const std::string whitespace = " \t";
    vector<std::string> tokenVec;
    size_t i = 0;
    size_t head = 0;
    size_t tail = 0;

    // while not end of line
    while (i < line.length()) {
        
        // while whitespace, skip ahead
        while (line[i] == whitespace[0] || line[i] == whitespace[1])
            ++i;

        if (line[i] != '#') {
            head = i;
            ++i;

            // while not white space and not end, move ahead
            while (line[i] != whitespace[0] && line[i] != whitespace[1] && 
                   i < line.length())
                ++i;

            tail = i;

            tokenVec.insert(tokenVec.end(), line.substr(head, tail-head));
        }
    }

    return tokenVec;
}
*/


//
// parseAtomicParams()
//
// Parse parameters from config file
// The parameter parsing function takes in an array of strings
// defining parameter values (from the config file), which look
// like:
//
//     param1_name=value1
//
// or
//
//     param1_name="channel_name"
//
// and strips out the appropriate parameters, stuffing them into
// paramter maps (intParams and dblParams; see osc/Param.hh) which
// will be taken by the constructors of each OSC type.
//
// Returns 0 if OK, 1 if error.
//
// NOTE:
// parseAtomicParams() is not used for Boolean OSCs.
//
int
OperStateCondList::parseAtomicParams(char            token[][mMaxStrLen],
                                     int             lineNum,
                                     std::string     filename,
                                     osc::IntParMap *intParams,
                                     osc::DblParMap *dblParams)
{
    if (mDebug > 1)
        cout << "OperStateCondList::parseAtomicParams() entered." << endl;

    osc::tseriesType_t typeOfParam;
    int i;                             // counter over tokens
    std::string osctype = token[1];
    
    // Loop over parameter names
    const vector<std::string> &paramNames =
        mTypeInfoMap[token[1]].paramNames();

    // total no. of parameters for this OSCtype
    unsigned int nParams = mTypeInfoMap[token[1]].nParams();

    // iterator to loop over parameter names
    vector<std::string>::const_iterator iter;

    // Map of predicates: parameter is found in line
    hash_map<const std::string, bool, 
             osc::hash<const std::string> > paramFoundP;
    
    for (iter = paramNames.begin(); iter != paramNames.end(); ++iter)
        paramFoundP[*iter] = false;  // initialize

    // Check that each token corresponds to a parameter name
    // This implicitly ignores tokens which don't correspond to a known
    // parameter.
    int nParamsFound = 0;
    for (i = 0; i < (int)nParams; ++i) {
        std::string curToken = token[i+3];

        // Loop over parameter names
        for (iter = paramNames.begin(); iter != paramNames.end(); ++iter) {
            // DEBUG
            std::string tmpstr = curToken.substr(0,curToken.find("="));
            
            if (curToken.substr(0,curToken.find("=")) == *iter) { // found
                paramFoundP[*iter] = true;
                ++nParamsFound;
            } else {
                ; // do nothing
            }
        } // for (iter = paramNames.begin(); ...)
    } // for (i = 0; ...)


    // Now that we've checked all tokens, return an error if any of the
    // compulsory parameters were not found among the tokens
    for (iter = paramNames.begin(); iter != paramNames.end(); ++iter) {

        // if parameter was not defined in config file,
        // check that it's an optional param
        if (paramFoundP[*iter] == false) {

            // if parameter is not optional
            if (mTypeInfoMap[osctype].optP(*iter) == false) {
                cerr << filename << ":" << "line #" << lineNum << ": Error" 
                     << ": " << osctype << ": Compulsory param. named '"
                     << *iter << "' not set; skipping" << endl;
                return 1;
            } else {  // optional param; set to default value
                
                typeOfParam = mTypeInfoMap[osctype].paramType(*iter);
                switch (typeOfParam) {
                case osc::INT:
                    (*intParams)[*iter] =
                        Param<int>(mTypeInfoMap[osctype].intDefault(*iter));
                    break;

                case osc::DOUBLE:
                    (*dblParams)[*iter] =
                        Param<double>(mTypeInfoMap[osctype].dblDefault(*iter));
                    break;

                default:
                    // FIXME: no other types of params defined, yet
                    cerr << "Only int and double params available" << endl;
                    return 1;
                    break;
                }
            }
        }
    }

    // DEBUG
    if (mDebug > 0) {
        cout << "parseAtomicParams(): nParamsFound = " << nParamsFound << endl;
    }

    // Now, we can parse the tokens to extract param. values
    for (i = 0; i < (int)nParamsFound; ++i) {
        std::string tokenTmp  = token[i+3];
        size_t      sepLoc    = tokenTmp.find("=");
        std::string paramName = tokenTmp.substr(0, sepLoc);
        std::string valTmp    = tokenTmp.substr(sepLoc+1, std::string::npos);

        typeOfParam = mTypeInfoMap[osctype].paramType(paramName);

        // DEBUG
        if (mDebug > 0) {
            cout << "Token[" << i << "] = " << tokenTmp << endl;
            cout << "paramName = " << paramName << endl;
        }


        // the value field contains either a string naming a channel
        // or a number (the param value itself)
        if (valTmp[0] == '"') {   // value field is a "-enclosed string

            // DEBUG
            if (mDebug > 0) {
                cout << "Setting param " << paramName
                     << " to read from channel '"
                     << valTmp << "'" << endl;
            }
            
            removeDblQuotes(&valTmp);

            switch (typeOfParam) {
            case osc::INT:
                (*intParams)[paramName] = osc::Param<int>(valTmp,
                                                          &mDataAccess);
                break;
        
            case osc::DOUBLE:
                (*dblParams)[paramName] = osc::Param<double>(valTmp,
                                                             &mDataAccess);
                break;
            
            default:
                cerr << "Unknown error" << endl;
                return 1;
                break; // should never reach here
            }

        } else {   // value field is a number
            
            switch (typeOfParam) {
            case osc::INT:
                    
                // hold and dead for transit conditions must be >= 0
                if (paramName == "hold" || paramName == "dead") {
                    if (atoi(valTmp.c_str()) <= 0) {
                        cerr << filename << ":line #" << lineNum
                             << ": Warning: " << paramName << " = "
                             << atoi(valTmp.c_str())
                             << " -- must be >= 0; setting "
                             << paramName << " = 0"
                             << endl;
                        (*intParams)[paramName] = osc::Param<int>(0);
                    }
                }

                (*intParams)[paramName] = osc::Param<int>(strtol(valTmp.c_str(), 0, 0));
                break;
        
            case osc::DOUBLE:
                // thresholds for fractpower* must be <= 1.
                if (osctype.find("fractpower") != std::string::npos) {

                    // fractpowerrange
                    if (osctype == "fractpowerrange"               &&
                        (paramName == "lo" || paramName == "hi")   &&
                        atof(valTmp.c_str()) > 1.) { 
                        cerr << filename << ":line #" << lineNum
                             << ": Warning: " << paramName << " = "
                             << atof(valTmp.c_str())
                             << " -- must be <= 1; setting "
                             << paramName << " = 1."
                             << endl;
                        (*dblParams)[paramName] = osc::Param<double>(1.);
                        
                    } else if (paramName == "threshold" &&
                               atof(valTmp.c_str()) > 1.) {

                        // fractpowerabove, fractpowerbelow,
                        // fractpowerrise, fractpowerfall
                        cerr << filename << ":line #" << lineNum
                             << ": Warning: " << paramName << " = "
                             << atof(valTmp.c_str())
                             << " -- must be <= 1; setting "
                             << paramName << " = 1." << endl;
                        (*dblParams)[paramName] = osc::Param<double>(1.);
                        
                    } else
                        (*dblParams)[paramName] =
                            osc::Param<double>(atof(valTmp.c_str()));

                } else if (osctype.find("gain") != std::string::npos) {
                    // abspowergain thresholds must be > 0

                    // threshold
                    if (paramName == "threshold" &&
                        atof(valTmp.c_str()) <= 0.) {

                        cerr << filename << ":line #" << lineNum
                             << ": Error: " << paramName << " = "
                             << atof(valTmp.c_str())
                             << " -- must be >= 0; skipping... " << endl;
                        return 1;
                    }
                                            
                } else { // not fractpower* or abspowergain
                    // ensure fraction is <= 1 and >= 0
                    if (paramName == "fraction" &&
                        (atof(valTmp.c_str()) < 0. ||
                         atof(valTmp.c_str()) > 1.)) {
                        cerr << filename << ":line #" << lineNum
                             << ": Warning: fraction = "
                             << atof(valTmp.c_str())
                             << " -- must be <=1 and >=0; "
                             << "setting fraction = 0." << endl;
                        (*dblParams)[paramName] = osc::Param<double>(0.);
                    } else
                        (*dblParams)[paramName] =
                            osc::Param<double>(atof(valTmp.c_str()));
                }
                break;
            
            default:
                cerr << "Unknown error" << endl;
                return 1;
                break; // should never reach here
            } // switch
        }
        
    } // for (i = 0; ...)
    
    return 0;
} // END: parseAtomicParams()


//
// Remove enclosing double quotes from a string
//
void OperStateCondList::removeDblQuotes(std::string *str)
{
    int len = str->length();

    if ((*str)[0] == '"' && (*str)[len-1] == '"')
        *str = str->substr(1, (str->length()-2));
    else
        osc::boolean::error(osc::boolean::errorMissingDblQuote);
}

//
// Remove enclosing double quotes from a char array
//
void OperStateCondList::removeDblQuotes(char *str)
{
    int len = strlen(str);

    if (str[0] != '"' || str[len-1] != '"')
        osc::boolean::error(osc::boolean::errorMissingDblQuote);
    else {
        char *newstr = new char[len];
        for (int i = 0; i < len; ++i)
            newstr[i] = '\0';
        strncpy(newstr, str+1, len-2);
        strcpy(str, newstr);
        delete [] newstr;
    }
}


//
// Predicate: oscName is already defined
//
bool OperStateCondList::oscNameDefinedP(std::string oscName)
{
    OperStateCondList::const_iterator iter = find(oscName);

    if (iter != end())
        return true;
    else
        return false;
}
