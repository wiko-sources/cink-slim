#ifndef FTM_CUST_FONT_H
#define FTM_CUST_FONT_H

#include "cust.h"

#if defined(FEATURE_FTM_FONT_10x18)
#define CHAR_WIDTH      10
#define CHAR_HEIGHT     18
#elif defined(FEATURE_FTM_FONT_8x14)
#define CHAR_WIDTH      8
#define CHAR_HEIGHT     14
#elif defined(FEATURE_FTM_FONT_6x10)
#define CHAR_WIDTH      6
#define CHAR_HEIGHT     10
#elif defined(FEATURE_FTM_FONT_16x28)
#define CHAR_WIDTH      16
#define CHAR_HEIGHT     28
#else
#error "font size is not definied"
#endif

#endif /* FTM_CUST_H */
