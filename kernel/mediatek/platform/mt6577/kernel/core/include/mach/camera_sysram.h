#ifndef _CAMERA_SYSRAM_H
#define _CAMERA_SYSRAM_H

#include <linux/ioctl.h>

#define SYSRAM_DEV_NAME         "camera-sysram"
#define CAMERA_SYSRAMMAGICNO    'p'

typedef enum {
    ESysramUser_Begin   = 0,
    ESysramUser_AUDIO   = ESysramUser_Begin, 
    //ESysramUser_DESCRIPTOR, 
    ESysramUser_DESC_VDO_ROT0,
    ESysramUser_DESC_VDO_ROT1,
    ESysramUser_DESC_RGB_ROT0,
    ESysramUser_DESC_RGB_ROT1,
    ESysramUser_DESC_RGB_ROT2,
    ESysramUser_DESC_RDMA0,
    ESysramUser_DESC_RDMA1,
    //ESysramUser_DESC_OVL,
    ESysramUser_FLICKER, 
    ESysramUser_SHADING, 
    ESysramUser_DEFECT, 
    ESysramUser_LCE0,
    ESysramUser_LCE1,
    ESysramUser_FD, 
    ESysramUser_PCA, 
    ESysramUser_EIS, 
    ESysramUser_VIDEO, 
    ESysramUser_TVOUT, 
    ESysramUser_TV_ROT, 
    ESysramUser_JPEG_DMA, 
    ESysramUser_JPEG_CODEC, 
    ESysramUser_BRZ, 
    ESysramUser_VRZ0, 
    ESysramUser_VRZ1, 
    ESysramUser_RGB_ROT0, 
    ESysramUser_RGB_ROT1, 
    ESysramUser_RGB_ROT2, 
    ESysramUser_VDO_ROT0, 
    ESysramUser_VDO_ROT1, 
    ESysramUser_VDO_ROT1_SUB,
    ESysramUser_RDMA0, 
    //ESysramUser_RDMA1, 
    ESysramUser_CNT, 
    ESysramUser_None    = ((unsigned int)-1)
} ESysramUser_T;

//IOCTRL(inode * ,file * ,cmd * ,arg * )
typedef struct {
    unsigned long u4Alignment;  // In : alignment in bytes
    unsigned long u4Size;       // In : Size in bytes
    ESysramUser_T u4Owner;      // In : Owner
    unsigned long u4Addr;       // In/Out : address
    unsigned long u4TimeoutInMS;// In : millisecond
} stSysramParam;

/*******************************************************************************
* IOCTL commnad
********************************************************************************/
//Lock User
#define SYSRAM_X_USRALLOC_TIMEOUT   _IOWR(CAMERA_SYSRAMMAGICNO, 0, stSysramParam)
#define SYSRAM_X_USRALLOC           _IOWR(CAMERA_SYSRAMMAGICNO, 1, stSysramParam)

//Unlock User
#define SYSRAM_S_USRFREE            _IOW(CAMERA_SYSRAMMAGICNO, 2, stSysramParam)

//Dump memory layout
#define SYSRAM_T_DUMPLAYOUT         _IO(CAMERA_SYSRAMMAGICNO, 3)

//
#define SYSRAM_S_SWITCH_BANK        _IOW(CAMERA_SYSRAMMAGICNO, 4, bool)

unsigned long SYSRAM_TVOUT_ALLOC_TIMEOUT(unsigned long const u4Size, unsigned long u4Alignment, unsigned long const u4TimeoutInMS);
void SYSRAM_TVOUT_FREE(void);
unsigned long SYSRAM_TV_ROT_ALLOC_TIMEOUT(unsigned long const u4Size, unsigned long u4Alignment, unsigned long const u4TimeoutInMS);
void SYSRAM_TV_ROT_FREE(void);
unsigned long SYSRAM_AUDIO_ALLOC_TIMEOUT(unsigned long const u4Size, unsigned long u4Alignment, unsigned long const u4TimeoutInMS);
void SYSRAM_AUDIO_FREE(void);
unsigned long SYSRAM_VIDEO_ALLOC_TIMEOUT(unsigned long const u4Size, unsigned long u4Alignment, unsigned long const u4TimeoutInMS);
void SYSRAM_VIDEO_FREE(void);
void SYSRAM_MFG_SWITCH_BANK(bool Enable);


#endif  //_CAMERA_SYSRAM_H

