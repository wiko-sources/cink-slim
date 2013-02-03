#ifndef __MMPROFILE_STATIC_EVENT_H__
#define __MMPROFILE_STATIC_EVENT_H__


typedef enum
{
    MMP_InvalidEvent = 0,
    MMP_RootEvent = 1,
    // User defined static events begin

    // User defined static events end.
    MMP_MaxStaticEvent
} MMP_StaticEvents;

#ifdef MMPROFILE_INTERNAL
typedef struct
{
    MMP_StaticEvents event;
    char*            name;
    MMP_StaticEvents parent;
} MMP_StaticEvent_t;

static MMP_StaticEvent_t MMProfileStaticEvents[] = 
{
    {MMP_RootEvent,         "Root_Event",           MMP_InvalidEvent},
};

#endif

#endif