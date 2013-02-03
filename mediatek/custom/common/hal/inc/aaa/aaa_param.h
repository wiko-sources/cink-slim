#ifndef _AAA_PARAM_H_
#define _AAA_PARAM_H_

#if defined(MT6573)
#include "./mt6573/param/aaa_param_mt6573.h"
#endif

#if defined(MT6575)||defined(MT6577)
#include "./mt6575/param/aaa_param_mt6575.h"
#endif

#endif // _AAA_PARAM_H_

