/*
 * $Id: frameint.h,v 1.1 2001/09/29 01:58:59 jzweizig Exp $
 */

/*
 * NOTE: We are assuming Frame version 3.72
 */

#include "sys-defines.h"

#include <string.h>
#include <errno.h>
#include <values.h>
#include <stddef.h>

/* (real) FFTW library header */
#ifndef RFFTW_INC
#   include "rfftw.h"
#   define RFFTW_INC
#endif

/* Frame library header */
#include "FrameL.h"

/* GRASP library header */
/* FIXME: should be removed.  We only need 2 functions:
   utctime(), and gpstime(); so, cannibalize */
/* #include "grasp.h" */

/* LLAL software spec headers */
/* #include "llal/LLALstdlib.h" */
/* #include "llal/llalDatatypes.h" */

#ifndef _FRAMEINT_H
#define _FRAMEINT_H

/* UTC time to GPS time conversion const (from the GRASP library) */
/* 
 * difference between UTC and GPS time 315964811 = 
 *     3600 sec/hour x 24 hours/day x (365 days/year x 8 years + 
 *     366 days/year x 2 years + 5 days) + 11 leap seconds 
 */
#define UTCTOGPS  315964811              

/* maximum allowable time difference between frames */
#define RESOLUTION 3.2e-3

/* work with UTC time rather than GPS time */
#define WORKONBADFRAMES 1

/* 2. * M_PI */
#define M_2PI 6.2831853071795864769253

/* square of argument (a) */
#define SQUARE(a) ((a) * (a))


/* 
 * Some useful typedefs 
 */
typedef struct FrFile     FrFile_t;
typedef struct FrameH     FrameH_t;
typedef struct FrAdcData  FrAdcData_t;
typedef struct FrVect     FrVect_t;
typedef struct FrStatData FrStatData_t;
typedef struct FrDetector FrDetector_t;


/* Define these purely for Emacs color-coded editing (font-lock-mode) */
typedef fftw_real  fftw_real_t;
typedef rfftw_plan rfftw_plan_t;
typedef bool       bool_t;

/* Define an enumerated type (rather than pre-processor macros) for 
 * FFT window types */
#ifdef __cplusplus
typedef enum { rect = 0, parzen = 1, 
               hanning = 2, hamming = 3, welch = 4,
               gauss = 5 } window_t;
#else /* not __cplusplus */
#ifdef __STDC__
typedef enum { rect = 0, parzen = 1, 
               hanning = 2, hamming = 3, welch = 4,
               gauss = 5 } window_t;
#else /* not __STDC__, do things the old-fashioned way */
typedef int window_t;
#define rect    0
#define parzen  1
#define hanning 2
#define hamming 3
#define welch   4
#define gauss   5
#endif
#endif /* not __cplusplus */


/* This data structure is for the user. It will contain the ADC data
 * of the channel named in channelWanted. */
typedef struct
{
    double   sampleRate; /* sampling rate of the ADC */
    short   *data;       /* array to contain actual data points */
    size_t   nData;      /* number of data points contained */
    size_t   totSpace;   /* total space allocated, in number of 
                          * shorts */
} AdcData_t;


/* A simple vector for containing the power spectrum */
typedef struct
{
    fftw_real_t *data;
    int          nData;
} PowerSpec_t;


/* This data structure is for the user.  It will contain the running
 * mean power spectrum with deviations for each bin. */
typedef struct
{
    fftw_real_t  *data;
    int           nData;
    fftw_real_t  *sigma;
} MeanPowerSpec_t;

/*****************************************************
 * Structures for dealing with locked sections of data
 *****************************************************/

/* A LockedSubSection is the largest part of OneLockedSection 
 * that is contained completely in one Frame. */
typedef struct
{
    size_t       frameNo;       /* frame number */
    /* size_t                  nDataPerFrame;      /* # data per frame */
    size_t       nFound;        /* # of data in subsection */
    size_t       lockStart;     /* index of start of subsection */          
} LockedSubSection_t;

/* OneLockedSection one or more LockedSubSection */
typedef struct
{
    size_t                nFound;         /* tot. # data in section */
    size_t                nSSreserved;    /* # subsections reserved */
    size_t                nSubSections;   /* # subsections */
    double                startTime;
    double                endTime;
    LockedSubSection_t   *subSectionArr;  /* array of subsections */
} LockedSection_t;

/* This data structure is used to store information about the location
 * of the sections of data that are in lock. */
typedef struct
{
    size_t           nReserved;      /* number of OneLockedSections for which 
                                      * space is reserved */
    size_t           nSections;      /* number of OneLockedSections 
                                      * stored/found */
    size_t           nDataPerFrame;  /* # of data per frame for this frame */
    double           lockSampleRate; /* sampling rate of lock channel */
    double           dataSampleRate; /* sampling rate of data channel */
    size_t           runNo;          /* run number */
    LockedSection_t *lockedSections; /* array of OneLockedSections */
    
    /*char      **filename;    /* UNUSED:names of files with locked sections */
    /* int          *nDataPerFrame; /* no. data pts per frame; spec
                                  * does not require this to be the same
                                  * for all frames */
    /*int          *frameNo;     /* frame number */
    /*size_t       *lockStart;   /* indices of heads of locked sections */ 
    /*size_t       *nFound;      /* number of points in locked sections */
    /*double       *startTime;   /* starting and ending times of each */
    /*double       *endTime;     /* locked section */
} LockInfo_t;


/*****************************
 * Utility function prototypes
 *****************************/

/*
 * Error-handled wrappers to Frame library
 */
extern FrAdcData_t *xFrAdcDataNew(FrameH_t*, char*, double, long, long);
extern FILE        *xFrLibIni(char *, FILE*, long);
extern FrameH_t    *xFrameNew(char*);
extern int          xFrameWrite(FrameH_t*, FrFile_t*);
extern FrameH_t    *xFrameRead(FrFile_t*);
extern FrAdcData_t *xFrAdcDataFind(FrameH_t*, char*);
extern FrFile_t    *xFrFileINew(char*);
extern FrFile_t    *xFrFileONew(char*, int);

/*
 * Misc. functions
 */
extern char            *nextFrameFile(char*, bool_t);

extern int              getAdcData(AdcData_t*, char*, int, int, bool_t, 
                                   int, char*);

extern PowerSpec_t     *powerSpectrum(AdcData_t*, PowerSpec_t*, window_t);
extern MeanPowerSpec_t *meanPowerSpectrum(PowerSpec_t*, MeanPowerSpec_t*);
extern fftw_real_t     *make_rfftw_one(int, fftw_real_t*, fftw_real_t*);
extern void             printParseval(AdcData_t*, PowerSpec_t*);
extern fftw_real_t      mean(fftw_real_t*, int, int, int);
extern fftw_real_t      variance(fftw_real_t*, int, int, int);
extern fftw_real_t      adcDataMean(AdcData_t*);
extern fftw_real_t      adcDataVariance(AdcData_t*);
extern fftw_real_t      powerSpecMean(PowerSpec_t*);
extern fftw_real_t      pwerSpecVariance(PowerSpec_t*);


/* extern LockedSection_t *callocOneLockedSection(LockedSection_t*, size_t);
extern LockedSection_t *addCallocOneLockedSection(LockedSection_t*, size_t);
extern void             freeOneLockedSection(LockedSection_t*);
extern LockInfo_t      *callocLockedSections (LockInfo_t*, size_t);
extern LockInfo_t      *addCallocLockedSections (LockInfo_t*, size_t);
extern LockInfo_t      *shrinkLockedSections (LockInfo_t*);
extern void             freeLockedSections(LockInfo_t*);
/* extern void              printLockedSectInfo(FILE*, LockInfo_t*); */

/*
 * LockedSubSection_t utility functions 
 */
extern size_t getSubSectFrameNo(LockedSubSection_t*);
extern void   setSubSectFrameNo(LockedSubSection_t*, size_t);
extern size_t getSubSectNFound(LockedSubSection_t*);
extern void   setSubSectNFound(LockedSubSection_t*, size_t);
extern void   incrSubSectNFound(LockedSubSection_t*);
extern size_t getSubSectLockStart(LockedSubSection_t*);
extern void   setSubSectLockStart(LockedSubSection_t*, size_t);
extern void   setSubSect(LockedSubSection_t*, size_t, size_t, size_t);
extern void   initSubSect(LockedSubSection_t*);

/*
 * LockedSection_t utility functions
 */
extern size_t getLockedSectNFound(LockedSection_t*);
extern void   setLockedSectNFound(LockedSection_t*, size_t);
extern size_t getLockedSectNSSreserved(LockedSection_t*);
extern void   setLockedSectNSSreserved(LockedSection_t*, size_t);
extern void   incrLockedSectNSSreservedBy(LockedSection_t*, size_t);
extern size_t getLockedSectNSubSections(LockedSection_t*);
extern void   setLockedSectNSubSections(LockedSection_t*, size_t);
extern void   incrLockedSectNSubSections(LockedSection_t*);
extern double getLockedSectStartTime(LockedSection_t*);
extern void   setLockedSectStartTime(LockedSection_t*, double);
extern double getLockedSectEndTime(LockedSection_t*);
extern void   setLockedSectEndTime(LockedSection_t *, double);
extern void   reserveSubSects(LockedSection_t*, size_t);
extern void   reserveMoreSubSects(LockedSection_t*, size_t);
extern LockedSubSection_t *getSubSect(LockedSection_t*, size_t);
extern void   addSubSect(LockedSection_t*, size_t, size_t, size_t);
extern void   freeLockedSection(LockedSection_t*);

/*
 * LockInfo_t utility functions
 */
extern size_t getLockInfoNReserved(LockInfo_t*);
extern void   setLockInfoNReserved(LockInfo_t*, size_t);
extern size_t getLockInfoNSections(LockInfo_t*);
extern void   setLockInfoNSections(LockInfo_t*, size_t);
extern void   incrLockInfoNSections(LockInfo_t*);
extern size_t getLockInfoNDataPerFrame(LockInfo_t*);
extern void   setLockInfoNDataPerFrame(LockInfo_t*, size_t);
extern double getLockInfoLockSampleRate(LockInfo_t*);
extern void   setLockInfoLockSampleRate(LockInfo_t*, double);
extern double getLockInfoDataSampleRate(LockInfo_t*);
extern void   setLockInfoDataSampleRate(LockInfo_t*, double);
extern size_t getLockInfoRunNo(LockInfo_t*);
extern void   setLockInfoRunNo(LockInfo_t*, size_t);
extern void   reserveSects(LockInfo_t*, size_t);
extern void   reserveMoreSects(LockInfo_t*, size_t);
extern LockedSection_t *getSection(LockInfo_t*, size_t);
extern LockedSection_t *getLatestSection(LockInfo_t*);
extern void   freeLockInfo(LockInfo_t*);

/* 
 * Other functions 
 */
extern int              findlock(LockInfo_t*, char*, int, char*, FILE*);

/* 
 * Time functions 
 */
extern struct tm *utctime(const time_t*); /* replacement for 
                                           * system's gmtime() */
extern struct tm *gpstime(const time_t*);

#endif /* ifndef _FRAMEINT_H */
