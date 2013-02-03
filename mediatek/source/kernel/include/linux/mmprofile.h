#ifndef __MMPROFILE_H__
#define __MMPROFILE_H__
#include "mmprofile_static_event.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define MMProfileEventNameMaxLen 31

typedef unsigned int MMP_Event;

typedef enum
{
    MMProfileFlagStart = 1,
    MMProfileFlagEnd = 2,
    MMProfileFlagPulse = 4,
    MMProfileFlagMax = 0xFFFFFFFF
} MMP_LogType;

typedef enum
{
    MMProfileMetaStringMBS = 1,
    MMProfileMetaStringWCS,
    MMProfileMetaStructure,
    MMProfileMetaBitmap,
    MMProfileMetaRaw,
    MMProfileMetaUser = 0x10000000,
    MMProfileMetaMax = 0xFFFFFFFF
} MMP_MetaDataType;

typedef enum
{
    MMProfileBitmapRGB565 = 1,
    MMProfileBitmapRGB888,
    MMProfileBitmapRGBA8888,
    MMProfileBitmapBGR888,
    MMProfileBitmapBGRA8888,
    MMProfileBitmapMax = 0xFFFFFFFF
} MMP_PixelFormat;

typedef struct
{
    unsigned int data1;         // data1 (user defined)
    unsigned int data2;         // data2 (user defined)
    MMP_MetaDataType data_type; // meta data type
    unsigned int size;          // meta data size
    void* pData;                // meta data pointer
} MMP_MetaData_t;

typedef struct  
{
    unsigned int data1;         // data1 (user defined)
    unsigned int data2;         // data2 (user defined)
    unsigned int struct_size;   // structure size (bytes)
    void* pData;                // structure pointer
    char struct_name[32];       // structure name
} MMP_MetaDataStructure_t;

typedef struct  
{
    unsigned int data1;         // data1 (user defined)
    unsigned int data2;         // data2 (user defined)
    unsigned int width;         // image width
    unsigned int height;        // image height
    MMP_PixelFormat format;     // image pixel format
    unsigned int start_pos;     // start offset of image data (base on pData)
    unsigned int bpp;           // bits per pixel
    int pitch;                  // image pitch (bytes per line)
    unsigned int data_size;     // image data size (bytes)
    unsigned int down_sample_x; // horizontal down sample rate (>=1)
    unsigned int down_sample_y; // vertical down sample rate (>=1)
    void* pData;                // image buffer address
} MMP_MetaDataBitmap_t;

#ifdef MTK_MMPROFILE_SUPPORT // MMPROFILE_SUPPORT==yes
MMP_Event MMProfileRegisterEvent(MMP_Event parent, const char* name);
MMP_Event MMProfileFindEvent(MMP_Event parent, const char* name);
void MMProfileEnableEvent(MMP_Event event, int enable);
void MMProfileLog(MMP_Event event, MMP_LogType type);
void MMProfileLogEx(MMP_Event event, MMP_LogType type, unsigned int data1, unsigned int data2);
int MMProfileLogMeta(MMP_Event event, MMP_LogType type, MMP_MetaData_t* pMetaData);
int MMProfileLogMetaString(MMP_Event event, MMP_LogType type, const char* str);
int MMProfileLogMetaStringEx(MMP_Event event, MMP_LogType type, unsigned int data1, unsigned int data2, const char* str);
int MMProfileLogMetaStructure(MMP_Event event, MMP_LogType type, MMP_MetaDataStructure_t* pMetaData);
int MMProfileLogMetaBitmap(MMP_Event event, MMP_LogType type, MMP_MetaDataBitmap_t* pMetaData);

#define MMProfileLogStructure(event, type, pStruct, struct_type) \
{ \
    MMP_MetaDataStructure_t MetaData; \
    MetaData.data1 = 0; \
    MetaData.data2 = 0; \
    strcpy(MetaData.struct_name, #struct_type); \
    MetaData.struct_size = sizeof(struct_type); \
    MetaData.pData = (void*)(pStruct); \
    MMProfileLogMetaStructure(event, type, &MetaData); \
}
#else // MMPROFILE_SUPPORT==no
#define MMProfileRegisterEvent(parent, name) 0
#define MMProfileFindEvent(parent, name) 0
#define MMProfileEnableEvent(event, enable)
#define MMProfileLog(event, type)
#define MMProfileLogEx(event, type, data1, data2)
#define MMProfileLogMeta(event, type, pMetaData) 0
#define MMProfileLogMetaString(event, type, str) 0
#define MMProfileLogMetaStringEx(event, type, data1, data2, str) 0
#define MMProfileLogMetaStructure(event, type, pMetaData) 0
#define MMProfileLogMetaBitmap(event, type, pMetaData) 0
#define MMProfileLogStructure(event, type, pStruct, struct_type) 0
#endif // MMPROFILE_SUPPORT

#ifdef __cplusplus
}
#endif
#endif

