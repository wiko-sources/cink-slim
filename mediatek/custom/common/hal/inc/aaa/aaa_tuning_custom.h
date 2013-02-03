#ifndef _AAA_TUNING_CUSTOM_H_
#define _AAA_TUNING_CUSTOM_H_

#if defined(MT6573)
#include "./mt6573/tuning_custom/aaa_tuning_custom_mt6573.h"
#endif

#if defined(MT6575)||defined(MT6577)
#include "./mt6575/tuning_custom/aaa_tuning_custom_mt6575.h"
#endif

#endif // _AAA_TUNING_CUSTOM_H_

