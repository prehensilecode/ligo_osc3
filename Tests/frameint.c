#include "frameint.h"

/*
 * These are wrapper functions for often-used Frame library functions 
 * that return even when an error occurs.  These wrapper functions
 * exit with the EXIT_FAILURE errno value (see errno.h) when errors occur.
 */

/*
 * xFrAdcDataNew
 */
FrAdcData_t *
#ifdef _HAVE_PROTOS
xFrAdcDataNew(FrameH_t *frame, char *name, 
              double sampleRate, long nData, long nBits)
#else
xFrAdcDataNew(frame, name, sampleRate, nData, nBits)
    FrameH_t *frame;
    char     *char;
    double    sampleRate;
    long      nData;
    long      nBits;
#endif
{
    FrAdcData_t *adcp;
    
    adcp = FrAdcDataNew(frame, name, sampleRate, nData, nBits);
    
    if (adcp == (FrAdcData_t *)NULL)
    {
        fprintf(stderr, "Error during ADC creation\n");
        fprintf(stderr, "  Last errors were:\n%s", FrErrorGetHistory());
        exit(EXIT_FAILURE);
    }
    
    return adcp;
} /* end xFrAdcData() */



/*
 * xFrameNew
 */
FrameH_t *
#ifdef _HAVE_PROTOS
xFrameNew(char *name)
#else
xFrameNew(name)
    char *name;
#endif
{
    FrameH_t *p;
    
    p = FrameNew(name);
    
    if (p == (FrameH_t *)NULL)
    {
        fprintf(stderr, "xFrameNew: Error during frame creation\n");
        fprintf(stderr, "  Last errors are: \n%s", FrErrorGetHistory());
        exit(EXIT_FAILURE);
    }
    
    return p;
} /* END of xFrameHNew() */



/*
 * xFrLibIni
 */
FILE *
#ifdef _HAVE_PROTOS
xFrLibIni(char *outFile, FILE *fOut, long dbglvl)
#else
xFrLibIni(outFile, fOut, dbglvl)
    char *outFile;
    FILE *fOut;
    long  dbglvl;
#endif    
{
    FILE *fp;
    
    fp = (FILE *)FrLibIni(outFile, fOut, dbglvl);
    
    if (fp == (FILE *)NULL)
    {
        fprintf(stderr, "Error during initialization\n");
        fprintf(stderr, "  Last errors were:\n%s", FrErrorGetHistory());
        exit(EXIT_FAILURE);
    }
    
    return fp;
} /* END of xFrLibIni() */
    


/*
 * xFrameRead
 */
FrameH_t *
#ifdef _HAVE_PROTOS
xFrameRead(FrFile_t *frfile)
#else
xFrameRead(frfile)
    FrFile_t *frfile;
#endif
{
    FrameH_t *p;
    
    p = (FrameH_t *)FrameRead(frfile);
    
    if (p == (FrameH_t *)NULL)
    {
        fprintf(stderr, "xFrameRead: no more frames\n");
        exit(EXIT_FAILURE);
    }
    
    return (FrameH_t *)p;
} /* END xFrameRead() */



/*
 * xFrameWrite
 */
int
#ifdef _HAVE_PROTOS
xFrameWrite(FrameH_t *frameH, FrFile_t *oFile)
#else
xFrameWrite(frameH, oFile)
    FrameH_t *frameH;
    FrFile_t *oFile;
#endif
{
    int retval;
    
    retval = FrameWrite(frameH, oFile);
    
    if (retval != FR_OK)
    {
        fprintf(stderr, "xFrameWrite: failed\n");
        fprintf(stderr, "Last errors are:\n%s\n", FrErrorGetHistory());
        exit(EXIT_FAILURE);
    }
    
    return retval;
} /* END xFrameWrite() */



/*
 * xFrAdcDataFind
 */
FrAdcData_t *
#ifdef _HAVE_PROTOS
xFrAdcDataFind(FrameH_t *frame, char *chan_name)
#else
xFrAdcDataFind(frame, chan_name)
    FrameH_t *frame;
    char     *chan_name;
#endif
{
    FrAdcData_t *p;
    
    p = (FrAdcData_t *)FrAdcDataFind(frame, chan_name);
    
    if (p == (FrAdcData_t *)NULL)
    {
        fprintf(stderr, "xFrAdcDataFind: no such channel: %s\n",
                chan_name);
        exit(EXIT_FAILURE);
    }

    return (FrAdcData_t *)p;
} /* END xFrAdcDataFind() */



/*
 * xFrStatDataFind
 */
FrStatData_t *
#ifdef _HAVE_PROTOS
xFrStatDataFind(FrDetector_t *detector, char *name, unsigned int timeNow)
#else
xFrStatDataFind(detector, name, timeNow)
    FrDetector_t *detector;
    char         *name;
    unsigned int  timeNow;
#endif
{
    FrStatData_t *p;
    
    p = (FrStatData_t *)FrStatDataFind(detector, name, timeNow);
    
    if (p == (FrStatData_t *)NULL)
    {
        fprintf(stderr, "xFrStatDataFind: no static data named '%s'\n",
                name);
        exit(EXIT_FAILURE);
    }
    
    return (FrStatData_t *)p;
} /* END xFrStatDataFind() */



/*
 * xFrFileINew
 */
FrFile_t *
#ifdef _HAVE_PROTOS
xFrFileINew(char *fn)
#else
xFrFileINew(fn)
    char *fn;
#endif
{
    FrFile_t *p;
    
    p = (FrFile_t *)FrFileINew(fn);
    
    if (p == (FrFile_t *)NULL) 
    {
        fprintf(stderr, "xFrFileINew: Error opening input file: %s\n",
                fn);
        fprintf(stderr, "  Last errors are:\n%s", FrErrorGetHistory());
        exit(EXIT_FAILURE);
    }
    
    return (FrFile_t *)p;
} /* END xFrFileINew() */



/*
 * xFrFileONew
 */
FrFile_t *
#ifdef _HAVE_PROTOS
xFrFileONew(char *fileName, int compress)
#else
xFrFileONew(fileName, compress)
    char   *fileName;
    int     compress;
#endif
{
    FrFile_t *p;
    
    p = (FrFile_t *)FrFileONew(fileName, compress);
    
    if (p == (FrFile_t *)NULL) 
    {
        fprintf(stderr, "xFrFileONew: Error during output file creation\n");
        fprintf(stderr, "  Last errors are:\n%s", FrErrorGetHistory());
        exit(EXIT_FAILURE);
    }
    
    return (FrFile_t *)p;
} /* END xFrFileONew() */

/* END of wrapper functions */


