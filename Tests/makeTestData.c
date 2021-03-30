#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "frameint.h"

/*
 * Make numFiles files containing numFrames, 
 * with nData data points per frame.
 *
 * $Id: makeTestData.c,v 1.1 2001/09/29 01:58:59 jzweizig Exp $
 */

short trigChirp(long);
int   heaviside(long);
short linearChirp(long, double);
void squareWave(short *data, float period, double sampleRate,
                long noOfSamples, float lo, float hi);
  
int main(int argc, char *argv[])
{
    FrFile_t     *oFile = NULL;
    FrameH_t     *frame = NULL;
    FrAdcData_t  *adc = NULL;
    FrStatData_t *statdata = NULL;
    time_t        initialLocalTime, UTC, frLocTime, startTime, residualTime;
    const time_t  leapS = 31;        /* leap secs. for Sep 1997 */
    double        sampleRate, dt, phase, realCurTimeGPS;
    long          nData, i,  nADC, nData5;
    const int     numChannel = 8;
    long          j;
    long          last_j[12];         /* number of data for each of 12 chans. */
    char          name[100], ofilename[100];
    unsigned int  dtS, dtN, sec;
    FrVect_t     *locklo_hi = NULL;
    const int     numFrames = 64;    /* total number of frames */
    int           framesPerFile;     /* no. of frames per file */
    const int     numFiles = 8;      /* total number of files */
    int           iFrame, iFile;     /* counters for no. of frames & files */

    /* these 2 are for the SINEWAVE channel */
    double freq1, freq2;
    double ampl, ampl2;
    
    /* INITIALISATION */
    xFrLibIni("full.lis", NULL, 1);
    iFile = 0;


    framesPerFile = numFrames / numFiles;
    
#   ifdef DEBUG
    fprintf(stderr, "numFrames     = %d\n", numFrames);
    fprintf(stderr, "numFrils      = %d\n", numFiles);
    fprintf(stderr, "framesPerFile = %d\n", framesPerFile);
#   endif
    
    /* sampleRate       = 9868.421053; /* primary sampling rate */
    sampleRate       = 16384.;      /* primary sampling rate */
    nData            = 16384;       /* no. of data points */
    dt               = (double)nData / sampleRate; /* time interval of frame */
    initialLocalTime = time(NULL);          /* get current local time */
    UTC              = mktime(gmtime(&initialLocalTime)); /* convert to Zulu */
    frLocTime        = initialLocalTime - UTC; /* local seasonal time - UTC */
    startTime        = UTC - UTCTOGPS + leapS; /* Zulu time in GPS */
    realCurTimeGPS   = (double)startTime;    /* this will track the 
                                              * "actual" current GPS time */
    
    /* URK! pretend like it's a few years earlier */
    realCurTimeGPS -= 0.2 * (double)UTCTOGPS; /* */
    
#   ifdef DEBUG
    fprintf(stderr, "exampleFull: dt = %f\n", dt);
#   endif
    
    for (i = 0; i < numChannel; ++i)
        last_j[i] = 0;
    
    for (iFrame = 0; iFrame < numFrames; ++iFrame)
    {
        /*
         * create one Frame header and set its time
         */
        frame = xFrameNew("Test data");
        
        /*
         * set run and frame number
         */
        frame->run       = -1;
        frame->frame     = iFrame + 1;
        frame->ULeapS    = leapS;
        frame->dt        = dt;    /* all frames last the same length of time */
        
        realCurTimeGPS  += dt;
        
        frLocTime     += (unsigned int)(frame->dt);
        startTime      = (unsigned int)(realCurTimeGPS);
        residualTime   = (unsigned int)((realCurTimeGPS - 
                                        (double)((unsigned
                                                  int)realCurTimeGPS))
                                       * 1000000000);
        
        frame->localTime = frLocTime;
        frame->GTimeS    = startTime;
        frame->GTimeN    = residualTime;
        
        /* frame->localTime = frLocTime + 
            (unsigned int)iFrame * (unsigned int)(frame->dt);
            
        frame->GTimeS    = startTime + 
            (unsigned int)iFrame * (unsigned int)(frame->dt);
            
        frame->GTimeN    = (unsigned int)iFrame * 
            (unsigned int)((frame->dt - 
                            (double)((unsigned int)frame->dt)) *
                            * 1000000000); */
                            
        
#       ifdef DEBUG
        fprintf(stderr, "frame->dt = %f\n", frame->dt);
        fprintf(stderr, "frame->GTimeS = %d\n", frame->GTimeS);
        fprintf(stderr, "frame->GTimeN = %d\n", frame->GTimeN);
#       endif
        
        /* history */
        FrHistoryAdd(frame, "Created by makeTestData.c");
    
        /* add detector infomation */
        frame->detectProc = FrDetectorNew("Test Interferometer");
        frame->detectProc->armLength = 40.;

        
        /*****************************************************************
         * OK, here's the plan for the sample channels, to facilitate
         * testing of the OSCs.
         *
         * 1. Constant "high" value of +8.0
         * 2. Constant "low" value of 0.0
         * 3. Slowly switching (square wave) between +8 and 0. Period to
         *    be longer than one Frame (1 s)
         * 4. Slowly switching (sawtooth wave) between +8 and 0. Period
         *    to be longer than one Frame (1 s)
         * 5. Sine Wave, zero DC offset, period of 0.25 s, amplitude 5.
         * 6. Sine Wave, +1. DC offset, period of 0.25 s, amplitude 5.
         * 7. Sum of two Sine Waves, zero DC offset, periods 0.25 s and
         *    0.005 s, amplitudes 2, and 5, respectively, with some
         *    arbitrary phase offset
         *****************************************************************/

        
        /****************************
         * add H0:SAMPLE1 ADC channel 
         ****************************/
        sprintf(name, "H0:SAMPLE1");
        adc = xFrAdcDataNew(frame, name, sampleRate, nData, -32);
        
#       ifdef DEBUG
        fprintf(stderr, "Line #%d:\n", __LINE__);        
        fprintf(stderr, "\tadc->data->nData = %d\n", adc->data->nData);
#       endif

        adc->data->type = FR_VECT_4R;
        
        /* fill this ADC with data; make some sequences cross frame
         * and file boundaries        */
        printf("H0:SAMPLE1:\n");
        for (j = 0; j < nData; ++j) {
            adc->data->dataF[j] = (float)8.;

            if (j % 4096 == 0)
                printf("\tadc->data->dataF[%d] = %f\n", j, adc->data->dataF[j]);
        }

        last_j[0] += j;


        /****************************
         * add H0:SAMPLE2 ADC channel 
         ****************************/
        sprintf(name, "H0:SAMPLE2");
        // -1 => float
        adc = xFrAdcDataNew(frame, name, sampleRate, nData, -32);
        
#       ifdef DEBUG
        fprintf(stderr, "Line #%d:\n", __LINE__);
        fprintf(stderr, "\tadc->data->nData = %d\n", adc->data->nData);
#       endif

        adc->data->type = FR_VECT_4R;
        
        /* fill this ADC with data; make some sequences cross frame
         * and file boundaries        */
        for (j = 0; j < nData; ++j)
            adc->data->dataF[j] = (float)0.;

        last_j[1] += j;
    
        
        /***************************
         * add H0:SAMPLE3 channel
         ***************************/
        sprintf(name, "H0:SAMPLE3");
        adc = xFrAdcDataNew(frame, name, sampleRate, nData, -32);

#       ifdef DEBUG
        fprintf(stderr, "Line #%d:\n", __LINE__);
        fprintf(stderr, "\tadc->data->nData = %d\n", adc->data->nData);
        fprintf(stderr, "\tiFrame \% 2 = %d\n", iFrame % 2);
#       endif

        adc->data->type = FR_VECT_4R;

        if ((iFrame % 2) == 0)
            for (j = 0; j < nData; ++j)
                adc->data->dataF[j] = (float)0.;
        else
            for (j = 0; j < nData; ++j)
                adc->data->dataF[j] = (float)8.;


#       ifdef DEBUG
        for (j = 0; j < nData; ++j)
            printf("SAMPLE3: %f\n", adc->data->dataF[j]);
#       endif

        last_j[2] += j;

        /***************************
         * add H0:SAMPLE4 channel
         ***************************/
        sprintf(name, "H0:SAMPLE4");
        adc = xFrAdcDataNew(frame, name, sampleRate, nData, -32);

        adc->data->type = FR_VECT_4R;
        /*
         * FIXME: put in a sawtooth.
         */
        for (j = 0; j < nData; ++j)
            adc->data->dataF[j] = (float)0.;

        last_j[3] += j;
        
        /***************************
         * add H0:SAMPLE5 channel
         ***************************/
        sprintf(name, "H0:SAMPLE5");
        adc = xFrAdcDataNew(frame, name, sampleRate, nData, -32);
        
        adc->data->type = FR_VECT_4R;
        freq1           = 4.;
        ampl            = 5.;              /* 8-bit ADC */
        for (j = 0; j < nData; ++j)
            adc->data->dataF[j] = (float) (ampl * 
                                          sin(M_2PI * freq1 * 
                                              (double)j / sampleRate));

        last_j[4] += j;
        
        /**************************
         * add H0:SAMPLE6 channel
         **************************/
        sprintf(name, "H0:SAMPLE6");
        adc = xFrAdcDataNew(frame, name, sampleRate, nData, -32);

#       ifdef DEBUG
        fprintf(stderr, "Line #%d:\n", __LINE__);
        fprintf(stderr, "\tadc->data->nData = %d\n", adc->data->nData);
#       endif
        
        adc->data->type = FR_VECT_4R;
        freq1           = 4.;
        ampl            = 5.;
        for (j = 0; j < nData; ++j)
            adc->data->dataF[j] = (float) (ampl * 
                                          sin(M_2PI * freq1 * 
                                              (double)j / sampleRate)
                                          + 1.);

        last_j[5] += j;
        
        /***************************
         * add H0:SAMPLE7 channel
         ***************************/
        sprintf(name, "H0:SAMPLE7");
        adc = xFrAdcDataNew(frame, name, sampleRate, nData, -32);
        
        adc->data->type = FR_VECT_4R;
        freq1           = 4.;
        ampl            = 2.;
        freq2           = 200.;
        ampl2           = 5.;
        for (j = 0; j < nData; ++j)
            adc->data->dataF[j] = (float) (ampl * 
                                          sin(M_2PI * freq1 * 
                                              (double)j / sampleRate)
                                          + ampl2 *
                                          sin(M_2PI * freq2 *
                                              (double)(j + 719)/ sampleRate));

        last_j[4] += j;
        
        
        /*----- dump on stdout the current frame  ----------*/
        FrameDump(frame, stdout, 3);

#       ifdef DEBUG
        fprintf(stderr, "Line #%d:\n", __LINE__);
        fprintf(stderr, "\tchecking if a new file is needed...\n");
        fprintf(stderr, "\tiFrame = %d; framesPerFile = %d\n",
                iFrame, framesPerFile); 
#       endif
        
        /* if we have framesPerFile number of frames, open a new file */
        if (iFrame % framesPerFile == 0)
        {
#           ifdef DEBUG
            fprintf(stderr, "Line #%d:\n", __LINE__);
            fprintf(stderr, "\tfilled up file.\n");
#           endif

            if (oFile != NULL)
                FrFileOEnd(oFile);
            
            /*----- open the output file in binary format 
              with differentiation and compression ------*/
            sprintf(ofilename, "Datadir/H0/C1-%d.F", frame->GTimeS);
            fprintf(stderr, "ofilename = %s\n", ofilename);
            oFile = xFrFileONew(ofilename, 6); 
            
            iFile++;
        }

#       ifdef DEBUG
        fprintf(stderr, "Line #%d:\n", __LINE__);
        fprintf(stderr, "\tabout to write frame\n");
#       endif
        

        /* write the frame to the file */
        printf("output frame %d\n",frame->frame);
        xFrameWrite(frame, oFile);
        
        dtS = (unsigned int)(frame->dt);
        dtN = ((unsigned int)(1.e+9 * frame->dt)) % 1000000000;
        
        /* frame->GTimeS += (unsigned int)(frame->dt);
           frame->GTimeN += ((unsigned int)(1.e+9 * frame->dt)) 
           % 1000000000; */
        /* frame->GTimeS += dtS;
           frame->GTimeN += dtN;
           sec = frame->GTimeN / 1000000000;
           frame->GTimeS += sec;
           frame->GTimeN -= sec * 1000000000; */
        
        /*----- close everything -------------------------*/
        FrameFree(frame);
    }
    
    
    printf("DONE!\n");

    return(0);
}


/*
 * Generate trigonometric chirp: formula from Carmona et al (Gabor
 * Transforms and Wavelets), p. 153
 */

short trigChirp(long x)
{
    const long x0       = 92000;
    const double lambda = -0.25;
    const double mu     = -0.375;
    const double amp    = 200.;
    /* const double freq   = 1.32e3;   /* 1.32 kHz */
    const double freq   = 0.65e3;
    const double phi    = 0.4212;
    /* const double offset = 125.; */
    const double offset = 0.;
    double delta_x;
    double tmp1, tmp2, tmp3;
    double retval;
    
    delta_x = (double)(x0 - x);
    tmp1    = pow(delta_x, lambda);
    tmp2    = phi - (M_2PI)/(mu + 1.) * pow(delta_x, mu + 1.);
    tmp3    = cos(tmp2);
    
    retval  = amp * (double)heaviside(delta_x) * tmp1 * tmp3 + offset;

    return (short)retval;
}

/* 
 * Heaviside function
 */
int heaviside(long x)
{
    if (x < 0)
        return (int)0;
    
    if (x >= 0)
        return (int)1;
}
    

/*
 * Linear chirp
 */
short linearChirp(long x, double sampleRate)
{
    const double f0   =  57.21523;
    const double dfdt =   5.31097;
    const double ampl =  10.;
    const double dc   =   2.;
    double       retval;
    
    retval = ampl * sin(M_2PI * (f0 + dfdt * (double)x / sampleRate) *
                        (double)x / sampleRate) + ampl + dc;
    
    return (short)retval;
} /* END linearChirp() */


/*
 * Square wave
 */
void squareWave(short *data, float period, double sampleRate,
                long noOfSamples, float lo, float hi)
{
    float samplesPerPeriod = (float)sampleRate * period;
    int   i;

    static done = 0;

#   ifdef DEBUG
    if (done == 0) {
        printf("squareWave(): period           = %f\n", period);
        printf("squareWave(): sampleRate       = %f\n", sampleRate);
        printf("squareWave(): noOfSamples      = %d\n", noOfSamples);
        printf("squareWave(): samplesPerPeriod = %f\n", samplesPerPeriod);
        done = 1;
    }
#   endif

    for (i = 0; i < noOfSamples; ++i) {
#       ifdef DEBUG
        printf("i = %d\n", i);
#       endif
        if ((double)(i % (int)samplesPerPeriod)  <=
            (double)samplesPerPeriod / (double)2.) {
#           ifdef DEBUG
            printf("  HI\n");
#           endif
            data[i] = (short)hi;
        } else {
#           ifdef DEBUG
            printf("  LO\n");
#           endif
            data[i] = (short)lo;
        }
    }
}
