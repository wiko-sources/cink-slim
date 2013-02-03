#ifndef _AF_COEF_H
#define _AF_COEF_H

#include "MediaTypes.h"

    typedef struct
    {
        MINT32  i4AFS_STEP_MIN_ENABLE;
        MINT32  i4AFS_STEP_MIN_NORMAL;
        MINT32  i4AFS_STEP_MIN_MACRO;
        MINT32  i4AFS_NOISE_LV1;
        MINT32  i4AFS_NOISE_POS_OFFSET1;
        MINT32  i4AFS_NOISE_GAIN1;
        MINT32  i4AFS_NOISE_LV2;
        MINT32  i4AFS_NOISE_POS_OFFSET2;
        MINT32  i4AFS_NOISE_GAIN2;
        MINT32  i4AFS_NOISE_POS_OFFSET3;
        MINT32  i4AFS_NOISE_GAIN3;
        
    } AF_COEF_T;

	AF_COEF_T get_AF_Coef();
	
#endif /* _AF_COEF_H */

