#define LOG_TAG "NSCamCustom/Comm"
//
#include <camera_custom_types.h>
//

namespace NSCamCustom
{
/*******************************************************************************
* During burst shot, we promise each capture takes the specified time at least
* before starting the next capture.
*   Return:
*       the time in microseconds.
*******************************************************************************/
MUINT32
getMinBurstShotToShotTime()
{
    return 350000;
}


/*******************************************************************************
* 
*******************************************************************************/
};  //NSCamCustom

