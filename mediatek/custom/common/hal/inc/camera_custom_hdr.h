#ifndef _CAMERA_CUSTOM_HDR_H_
#define _CAMERA_CUSTOM_HDR_H_

#include "camera_custom_types.h"	// For MUINT*/MINT*/MVOID/MBOOL type definitions.

/**************************************************************************
 *                      D E F I N E S / M A C R O S                       *
 **************************************************************************/
// For HDR Customer Parameters

// [Blurring Ratio]
//     - Higher value:
//         - Decrease the degree of artifact (halo-like around object boundary).
//         - Avoid non-continued edge in fusion result.
//         - Decrease little dynamic range.
//         - Decrease sharpness.
//     - Value Range: 1 (non-blur) ~ 160. (Default: 40).
#define CUST_HDR_BLUR_RATIO		40

// [Gain]
//     - Higher value increase sharpness, but also increase noise.
//     - Value Range: 256 ~ 512. (Default: 256 (1x gain)) (384/256 = 1.5x gain)
#define CUST_HDR_GAIN_00	384
#define CUST_HDR_GAIN_01	384
#define CUST_HDR_GAIN_02	384
#define CUST_HDR_GAIN_03	256
#define CUST_HDR_GAIN_04	256
#define CUST_HDR_GAIN_05	256
#define CUST_HDR_GAIN_06	256
#define CUST_HDR_GAIN_07	256
#define CUST_HDR_GAIN_08	256
#define CUST_HDR_GAIN_09	256
#define CUST_HDR_GAIN_10	256

// [Flare Control]
//     -Higher value decreases the degree of flare, but also decrease parts of the image details.
//     - Value Range: 0 ~ 50. (Default: 10)
#define CUST_HDR_BOTTOM_FLARE_RATIO		10
//     - Value Range: 0 ~ 50. (Default: 10)
#define CUST_HDR_TOP_FLARE_RATIO		10
//     - Value Range: 0 ~ 24. (Default: 16)
#define CUST_HDR_BOTTOM_FLARE_BOUND		16
//     - Value Range: 0 ~ 24. (Default: 16)
#define CUST_HDR_TOP_FLARE_BOUND		16

// [De-halo Control]
//     - Higher value reduce more halo for sky, but also reduce more dynamic range in some parts of the image.
//     - Value Range: 0 (off) ~ 255. (Default: 245)
#define CUST_HDR_TH_HIGH				245

// [Noise Control]
//     - Higher value reduce more noise, but also reduce dynamic range in low light region.
//     - Value Range: 0 (off) ~ 255. (Default: 25)
#define CUST_HDR_TH_LOW					0

// [Level Subtract]
//     - Value Range: 0 (more low-frequency halo, more dynamic range) or 1 (less low-frequency halo, less dynamic range). (Default: 0)
#define CUST_HDR_TARGET_LEVEL_SUB		0


/**************************************************************************
 *     E N U M / S T R U C T / T Y P E D E F    D E C L A R A T I O N     *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *        P U B L I C    F U N C T I O N    D E C L A R A T I O N         *
 **************************************************************************/
MUINT32 CustomHdrBlurRatioGet(void);
MUINT32 CustomHdrGainArrayGet(MUINT32 u4ArrayIndex);
double CustomHdrBottomFlareRatioGet(void);
double CustomHdrTopFlareRatioGet(void);
MUINT32 CustomHdrBottomFlareBoundGet(void);
MUINT32 CustomHdrTopFlareBoundGet(void);
MINT32 CustomHdrThHighGet(void);
MINT32 CustomHdrThLowGet(void);
MUINT32 CustomHdrTargetLevelSubGet(void);

/**************************************************************************
 *                   C L A S S    D E C L A R A T I O N                   *
 **************************************************************************/




#endif	// _CAMERA_CUSTOM_HDR_H_

