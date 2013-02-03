#ifndef _MTK_THERMAL_MONITOR_H
#define _MTK_THERMAL_MONITOR_H

#include <linux/thermal.h>


/*
 *  MTK_THERMAL_WRAPPER_BYPASS = 1 (use original Linux Thermal API)
 *  MTK_THERMAL_WRAPPER_BYPASS = 0 (use MTK Thermal API Monitor)
 */
#define MTK_THERMAL_WRAPPER_BYPASS 0

#if MTK_THERMAL_WRAPPER_BYPASS
/* Original LTF API */
#define mtk_thermal_zone_device_register      thermal_zone_device_register
#define mtk_thermal_zone_device_unregister    thermal_zone_device_unregister
#define mtk_thermal_cooling_device_unregister thermal_cooling_device_unregister
#define mtk_thermal_cooling_device_register   thermal_cooling_device_register
#define mtk_thermal_zone_bind_cooling_device  thermal_zone_bind_cooling_device  

#else

extern
struct thermal_zone_device *mtk_thermal_zone_device_register_wrapper
( 
    char *type,
	int  trips, 
	void *devdata,
	const struct thermal_zone_device_ops *ops,
	int tc1, 
	int tc2, 
	int passive_delay, 
	int polling_delay
);

extern 
void mtk_thermal_zone_device_unregister_wrapper
(
    struct thermal_zone_device *tz
);

extern 
struct thermal_cooling_device *mtk_thermal_cooling_device_register_wrapper
(
     char *type, 
     void *devdata, 
     const struct thermal_cooling_device_ops *ops
);

extern 
void mtk_thermal_cooling_device_unregister_wrapper
(
    struct thermal_cooling_device *cdev
);

extern int mtk_thermal_zone_bind_cooling_device_wrapper
(
    struct thermal_zone_device *tz,
    int trip,
    struct thermal_cooling_device *cdev
);

#define mtk_thermal_zone_device_register      mtk_thermal_zone_device_register_wrapper
#define mtk_thermal_zone_device_unregister    mtk_thermal_zone_device_unregister_wrapper
#define mtk_thermal_cooling_device_unregister mtk_thermal_cooling_device_unregister_wrapper
#define mtk_thermal_cooling_device_register   mtk_thermal_cooling_device_register_wrapper
#define mtk_thermal_zone_bind_cooling_device  mtk_thermal_zone_bind_cooling_device_wrapper

#endif

#endif 
