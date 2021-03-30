// Yo, Emacs! This is -*- C++ -*-
//
// $Id: osctest.cc,v 1.3 2002/06/26 14:20:16 dwchin Exp $
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

#include "OperStateCondList.hh"
#include "DatEnv.hh"

using namespace std;

class osctest
    : public DatEnv
{
public:
    int maxFrame;

    osctest(int argc, const char *argv[]);
    ~osctest();
    void ProcessData();

private:
    int               mFrameCount;
    OperStateCondList mOsclist;
    int               mDebug;
};


#ifndef __CINT__
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "TSeries.hh"
#include "FSeries.hh"
#include "Dacc.hh"


// Generate main routine
EXECDAT(osctest);

#endif // !def __CINT__


// Constructor
osctest::osctest(int argc, const char *argv[])
    : DatEnv(argc, argv),
      maxFrame(999999),
      mOsclist(getDacc(),3)
{
    std::string configfile;

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-config") == 0) {
            if (++i == argc)
                break;
            configfile = argv[i];
        }
    }
    
    cout << "Starting osctest from config file '"
         << configfile << "'...." << endl;

    if (configfile.length() == 0)
        configfile = "osctest.config";

    // Set debug level
    mDebug = 2;

    // Set time stride
    float tStep = 1.0;
    mIn.setStride(tStep);

    // Initialize frame counter
    mFrameCount = 0;

    // Read in config file
    mOsclist.readConfig(configfile);

    // Set debug level of X_arm_lock_acquired
    //mOsclist.setDebug("X_arm_lock_acquired", 3);

    if (mDebug > 2)
        cout << "WHERE ARE WE? Line #" << __LINE__ << endl;

    OperStateCondList::const_iterator iter = mOsclist.begin();
    for ( ; iter != mOsclist.end(); ++iter) {
        cout << (*iter).first << ": ";
        (*iter).second->printInfo();
    }

    if (mDebug > 0) {
        cout << mIn.getTime() << endl;
        mIn.list(cout); // list added channels

        if (mDebug > 2)
            cout << "DONE WITH OSCTEST CONSTRUCTOR!" << endl;
    }
}

// Destructor
osctest::~osctest()
{
    cout << "osctest is done" << endl;
}


// Frame processing function
void osctest::ProcessData()
{
    if (mDebug > 2)
        cout << "HERE WE ARE! Line#" << __LINE__ << endl;
    
    ++mFrameCount;

    cout << "Entered ProcessData() with frame count = "
         << mFrameCount << endl;

    OperStateCondList::iterator iter = mOsclist.begin(); 
    for (; iter != mOsclist.end(); ++iter) {
        std::cout << "'" << (*iter).first << "'\t" << std::endl;
        if ((*iter).second->satisfied() == true)
            std::cout << "\tsatisfied" << std::endl;
        else
            std::cout << "\t\tNOT satisfied" << std::endl;
    }
    std::cout << std::endl;

    /*
    std::cout << "CUBAAN!" << std::endl;
    std::string foo = "Foo";
    if (mOsclist.satisfied(foo + "Bar")) {
        std::cout << foo + "Bar" << " = satisfied" << std::endl;
    } else {
        std::cout << foo + "Bar" << " = NOT satisfied" << std::endl;
    }
    */


//    cout << "Foo watchedQuantity = " 
//         << mOsclist.watchedQuantity("Foo") << endl;

//    cout << "TestAbsPowAbove watchedQuantity = "
//         << mOsclist.watchedQuantity("TestAbsPowAbove") << endl;

//    cout << "SHORT watchedQuantity = "
//         << mOsclist.watchedQuantity("SHORT") << endl;

}

