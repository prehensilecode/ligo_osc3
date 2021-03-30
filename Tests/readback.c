#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "frameint.h"

/* $Id: readback.c,v 1.1 2001/09/29 01:58:59 jzweizig Exp $ */


/*
 * Read back a datafile to make sure it's OK
 */

int main(int argc, char *argv[])
{
    FrFile_t *iFile = NULL;
    FrameH_t *frame = NULL;
    FrAdcData_t *adc = NULL;
    FrVect      *vect = NULL;
    short       *txdata = NULL;
    int          i;

    xFrLibIni("full.lis", NULL, 1);

    iFile = xFrFileINew(argv[1]);

    while ((frame = FrameRead(iFile)) != NULL) {
        FrameDump(frame, stdout, 2);

        adc = FrAdcDataFind(frame, "H0:SAMPLE1");

        if (adc == NULL) {
            printf("Could not find H0:SAMPLE1\n");
            continue;
        } else {
            printf("readback: ADC has %6d samples\n", adc->data->nData);
            for (i = 0; i < adc->data->nData; ++i)
                printf("\tdataF[%d] = %6.4f\n", i, adc->data->dataF[i]);
        }

        FrAdcDataFree(adc);
        FrameFree(frame);
    }

    FrFileIEnd(iFile);

    return 0;
}
    
    
