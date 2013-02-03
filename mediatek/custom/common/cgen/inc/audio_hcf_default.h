/*******************************************************************************
 *
 * Filename:
 * ---------
 * audio_hcf_default.h
 *
 * Project:
 * --------
 *   ALPS
 *
 * Description:
 * ------------
 * This file is the header of audio customization related parameters or definition.
 *
 * Author:
 * -------
 * Tina Tsai
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 *
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef AUDIO_HCF_DEFAULT_H
#define AUDIO_HCF_DEFAULT_H

/* Compensation Filter HSF coeffs: default all pass filter       */
/* BesLoudness also uses this coeffs    */
#define BES_LOUDNESS_HCF_HSF_COEFF \
    0, 0, 0, 0, \
    0, 0, 0, 0, \
    0, 0, 0, 0, \
    0, 0, 0, 0, \
    0, 0, 0, 0, \
    0, 0, 0, 0, \
    0, 0, 0, 0, \
    0, 0, 0, 0, \
    0, 0, 0, 0

/* Compensation Filter BPF coeffs: default all pass filter      */
#define BES_LOUDNESS_HCF_BPF_COEFF \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    \     
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    \     
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    \     
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0, \
    0, 0, 0

#define BES_LOUDNESS_HCF_DRC_FORGET_TABLE \
    0, 0, \
    0, 0, \
    0, 0, \
    0, 0, \
    0, 0, \
    0, 0, \
    0, 0, \
    0, 0, \
    0, 0

#define BES_LOUDNESS_HCF_WS_GAIN_MAX  0

#define BES_LOUDNESS_HCF_WS_GAIN_MIN  0

#define BES_LOUDNESS_HCF_FILTER_FIRST  0

#define BES_LOUDNESS_HCF_GAIN_MAP_IN \
    0, 0, 0, 0, 0

#define BES_LOUDNESS_HCF_GAIN_MAP_OUT \
    0, 0, 0, 0, 0

#endif
