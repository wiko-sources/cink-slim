#ifndef __TV_DEV_H__
#define __TV_DEV_H__


#if 0
#define TV_PRINTF             printk
#define TV_INFO(fmt, arg...)       TV_PRINTF("[TV INFO]: %s(): "fmt,__FUNCTION__, ##arg)
#define TV_WARNING(fmt, arg...)    TV_PRINTF("[TV WARNING]: %s(): "fmt,__FUNCTION__, ##arg)
#define TV_ERROR(fmt, arg...)      TV_PRINTF("[TV ERROR]: %s(): %s@%d: "fmt,__FUNCTION__, __FILE__,__LINE__, ##arg)
#endif


#define TVOUT_USER_ENABLE_BUTTON //auto-detect or need user enable in setting menu

#define TV_BUFFER_PIPE //use TV Buffer pipe mechanism

#define TV_BUFFER_FMT_UYVY

//#define TVOUT_PORTING_WAIT_OTHER_READY



#endif

