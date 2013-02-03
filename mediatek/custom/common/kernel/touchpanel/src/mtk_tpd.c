/******************************************************************************
 * mtk_tpd.c - MTK Android Linux Touch Panel Device Driver               *
 *                                                                            *
 * Copyright 2008-2009 MediaTek Co.,Ltd.                                      *
 *                                                                            *
 * DESCRIPTION:                                                               *
 *     this file provide basic touch panel event to input sub system          *
 *                                                                            *
 * AUTHOR:                                                                    *
 *     Kirby.Wu (mtk02247)                                                    *
 *                                                                            *
 * NOTE:                                                                      *
 * 1. Sensitivity for touch screen should be set to edge-sensitive.           *
 *    But in this driver it is assumed to be done by interrupt core,          *
 *    though not done yet. Interrupt core may provide interface to            *
 *    let drivers set the sensitivity in the future. In this case,            *
 *    this driver should set the sensitivity of the corresponding IRQ         *
 *    line itself.                                                            *
 ******************************************************************************/

#include "tpd.h"

/* function definitions */
static int __init  tpd_device_init(void);
static void __exit tpd_device_exit(void);
static int         tpd_probe(struct platform_device *pdev);
static int tpd_remove(struct platform_device *pdev);

extern void        tpd_suspend(struct early_suspend *h);
extern void        tpd_resume(struct early_suspend *h);
extern void tpd_button_init(void);

//int tpd_load_status = 0; //0: failed, 1: sucess
int tpd_register_flag=0;
/* global variable definitions */
struct tpd_device  *tpd = 0;
static struct tpd_driver_t tpd_driver_list[TP_DRV_MAX_COUNT] ;//= {0};

//Ivan added gobel flag to all TP module
//Ivan added
int tp_boot_mode = 0;
int key_event_flag=0;

static struct platform_driver tpd_driver = {
    .remove     = tpd_remove,
    .shutdown   = NULL,
    .probe      = tpd_probe,
    #ifndef CONFIG_HAS_EARLYSUSPEND
    .suspend    = NULL,
    .resume     = NULL,
    #endif
    .driver     = {
        .name = TPD_DEVICE,
    },
};

/*20091105, Kelvin, re-locate touch screen driver to earlysuspend*/
#ifdef CONFIG_HAS_EARLYSUSPEND
static struct early_suspend MTK_TS_early_suspend_handler = 
{
    .level = EARLY_SUSPEND_LEVEL_DISABLE_FB+1,    
    .suspend = NULL,
    .resume  = NULL,
};
#endif

static struct tpd_driver_t *g_tpd_drv = NULL;
/* Add driver: if find TPD_TYPE_CAPACITIVE driver sucessfully, loading it */
int tpd_driver_add(struct tpd_driver_t *tpd_drv)
{
	int i;
	
	if(g_tpd_drv != NULL)
	{
		TPD_DMESG("touch driver exist \n");
		return -1;
	}
	/* check parameter */
	if(tpd_drv == NULL)
	{
		return -1;
	}
	/* R-touch */
	if(strcmp(tpd_drv->tpd_device_name, "generic") == 0)
	{
			tpd_driver_list[0].tpd_device_name = tpd_drv->tpd_device_name;
			tpd_driver_list[0].tpd_local_init = tpd_drv->tpd_local_init;
			tpd_driver_list[0].suspend = tpd_drv->suspend;
			tpd_driver_list[0].resume = tpd_drv->resume;
			tpd_driver_list[0].tpd_have_button = tpd_drv->tpd_have_button;
			tpd_driver_list[0].tpd_x_res = tpd_drv->tpd_x_res;
			tpd_driver_list[0].tpd_y_res = tpd_drv->tpd_y_res;
            tpd_driver_list[0].tpd_get_fw_version = NULL;
			return 0;
	}
	for(i = 1; i < TP_DRV_MAX_COUNT; i++)
	{
		/* add tpd driver into list */
		if(tpd_driver_list[i].tpd_device_name == NULL)
		{
			tpd_driver_list[i].tpd_device_name = tpd_drv->tpd_device_name;
			tpd_driver_list[i].tpd_local_init = tpd_drv->tpd_local_init;
			tpd_driver_list[i].suspend = tpd_drv->suspend;
			tpd_driver_list[i].resume = tpd_drv->resume;
			tpd_driver_list[i].tpd_have_button = tpd_drv->tpd_have_button;
			tpd_driver_list[i].tpd_x_res = tpd_drv->tpd_x_res;
			tpd_driver_list[i].tpd_y_res = tpd_drv->tpd_y_res;
            tpd_driver_list[i].tpd_get_fw_version = tpd_drv->tpd_get_fw_version;
			//edit by Magnum 2012-7-10
			 tpd_driver_list[i].tpd_get_vendor_version= tpd_drv->tpd_get_vendor_version;
			#if 0
			if(tpd_drv->tpd_local_init()==0)
			{
				TPD_DMESG("load %s sucessfully\n", tpd_driver_list[i].tpd_device_name);
				g_tpd_drv = &tpd_driver_list[i];
			}
			#endif
			break;
		}
		if(strcmp(tpd_driver_list[i].tpd_device_name, tpd_drv->tpd_device_name) == 0)
		{
			return 1; // driver exist
		}
	}
	
	return 0;
}

int tpd_driver_remove(struct tpd_driver_t *tpd_drv)
{
	int i = 0;
	/* check parameter */
	if(tpd_drv == NULL)
	{
		return -1;
	}
	for(i = 0; i < TP_DRV_MAX_COUNT; i++)
	{
		/* find it */
		if (strcmp(tpd_driver_list[i].tpd_device_name, tpd_drv->tpd_device_name) == 0)
		{
			memset(&tpd_driver_list[i], 0, sizeof(struct tpd_driver_t));
			break;
		}
	}
	return 0;
}

//edit by Magnum 2012-7-10 add ctp vendor-id attr
static ssize_t tpd_vendor_version_show(struct device *dev, 
		struct device_attribute *attr, char *buf)
{
    int vendor = 0;
    if(g_tpd_drv && g_tpd_drv->tpd_get_vendor_version ){
        vendor = g_tpd_drv->tpd_get_vendor_version();
    }
    return snprintf(buf, PAGE_SIZE, "%x\n", vendor);
}

static DEVICE_ATTR(vendor_version, S_IRUGO, tpd_vendor_version_show, NULL);

static ssize_t tpd_fw_version_show(struct device *dev, 
		struct device_attribute *attr, char *buf)
{
    int version = 0;
    if(g_tpd_drv && g_tpd_drv->tpd_get_fw_version ){
        version = g_tpd_drv->tpd_get_fw_version();
    }
    return snprintf(buf, PAGE_SIZE, "%x\n", version);
}

static DEVICE_ATTR(panel_version, S_IRUGO, tpd_fw_version_show, NULL);

static ssize_t tpd_device_name_show(struct device *dev, 
		struct device_attribute *attr, char *buf)
{
    if (g_tpd_drv && g_tpd_drv->tpd_device_name) {
        return snprintf(buf, PAGE_SIZE, "%s\n", g_tpd_drv->tpd_device_name);
    }

    return 0;
}
static DEVICE_ATTR(device_name, S_IRUGO, tpd_device_name_show, NULL);
char phonestate=0;
/******************************************************************************/
static ssize_t ft5x06_show_phone_state(struct device* dev, 
                                 struct device_attribute *attr, char *buf)
{
    printk("ft5x06_show_phone_state phonestate=%d\n",phonestate);
    return snprintf(buf, PAGE_SIZE, "%d\n", phonestate);
}
/******************************************************************************/
static ssize_t ft5x06_store_phone_state(struct device* dev, struct device_attribute *attr,
                                  const char *buf, size_t count)
{
    phonestate=buf[0];
    printk("ft5x06_store_phone_state phonestate=%d,buf=%d\n",phonestate,buf[0]);
    return 1;
}
/******************************************************************************/
DEVICE_ATTR(phone_state,     S_IWUSR | S_IWGRP | S_IRUGO, ft5x06_show_phone_state,  ft5x06_store_phone_state);
void tpd_create_attr_file(void)
{
    /* register the attributes */
     printk("tpd_create_attr_file \n");
     if (device_create_file(&tpd->dev->dev, &dev_attr_phone_state)){
             printk("Register the attributes panel version is failed.");
     }
} 
/* touch panel probe */
static int tpd_probe(struct platform_device *pdev) {
		int  touch_type = 1; // 0:R-touch, 1: Cap-touch
		int i=0;
	  TPD_DMESG("enter %s, %d\n", __FUNCTION__, __LINE__); 
	  /* Select R-Touch */
	 // if(g_tpd_drv == NULL||tpd_load_status == 0) 
#if 0
	 if(g_tpd_drv == NULL) 
	  {
	  	g_tpd_drv = &tpd_driver_list[0];
	  	/* touch_type:0: r-touch, 1: C-touch */
	  	touch_type = 0;
	  	TPD_DMESG("Generic touch panel driver\n");
	  }
	  	
    #ifdef CONFIG_HAS_EARLYSUSPEND
    MTK_TS_early_suspend_handler.suspend = g_tpd_drv->suspend;
    MTK_TS_early_suspend_handler.resume = g_tpd_drv->resume;
    register_early_suspend(&MTK_TS_early_suspend_handler);
    #endif
    #endif
    if((tpd=(struct tpd_device*)kmalloc(sizeof(struct tpd_device), GFP_KERNEL))==NULL) return -ENOMEM;
    memset(tpd, 0, sizeof(struct tpd_device));

    /* allocate input device */
    if((tpd->dev=input_allocate_device())==NULL) { kfree(tpd); return -ENOMEM; }
  
//Ivan move to lower part of this function
/*
    TPD_RES_X = simple_strtoul(LCM_WIDTH, NULL, 0);
    TPD_RES_Y = simple_strtoul(LCM_HEIGHT, NULL, 0);
  
    printk("mtk_tpd: TPD_RES_X = %d, TPD_RES_Y = %d\n", TPD_RES_X, TPD_RES_Y);
  
    tpd_mode = TPD_MODE_NORMAL;
    tpd_mode_axis = 0;
    tpd_mode_min = TPD_RES_Y/2;
    tpd_mode_max = TPD_RES_Y;
    tpd_mode_keypad_tolerance = TPD_RES_X*TPD_RES_X/1600;
*/

    /* struct input_dev dev initialization and registration */
    tpd->dev->name = TPD_DEVICE;
    set_bit(EV_ABS, tpd->dev->evbit);
    set_bit(EV_KEY, tpd->dev->evbit);
    set_bit(ABS_X, tpd->dev->absbit);
    set_bit(ABS_Y, tpd->dev->absbit);
    set_bit(ABS_PRESSURE, tpd->dev->absbit);
    set_bit(BTN_TOUCH, tpd->dev->keybit);
    set_bit(INPUT_PROP_DIRECT, tpd->dev->propbit);
#if 1    
  for(i = 1; i < TP_DRV_MAX_COUNT; i++)
	{
    		/* add tpd driver into list */
		if(tpd_driver_list[i].tpd_device_name != NULL)
		{
			tpd_driver_list[i].tpd_local_init();
			//msleep(1);
			if(tpd_load_status ==1) {
				TPD_DMESG("[mtk-tpd]tpd_probe, tpd_driver_name=%s\n", tpd_driver_list[i].tpd_device_name);
				g_tpd_drv = &tpd_driver_list[i];
				break;
			}
		}    
  }
	 if(g_tpd_drv == NULL) {
	 	if(tpd_driver_list[0].tpd_device_name != NULL) {
	  	g_tpd_drv = &tpd_driver_list[0];
	  	/* touch_type:0: r-touch, 1: C-touch */
	  	touch_type = 0;
	  	g_tpd_drv->tpd_local_init();
	  	TPD_DMESG("[mtk-tpd]Generic touch panel driver\n");
	  } else {
	  	TPD_DMESG("[mtk-tpd]cap touch and Generic touch both are not loaded!!\n");
	  	return 0;
	  	} 
	  }	
    #ifdef CONFIG_HAS_EARLYSUSPEND
    MTK_TS_early_suspend_handler.suspend = g_tpd_drv->suspend;
    MTK_TS_early_suspend_handler.resume = g_tpd_drv->resume;
    register_early_suspend(&MTK_TS_early_suspend_handler);
    #endif		  
#endif	  
//Ivan <<
    TPD_RES_X = g_tpd_drv->tpd_x_res;
    TPD_RES_Y = g_tpd_drv->tpd_y_res;

    printk("mtk_tpd: TPD_RES_X = %d, TPD_RES_Y = %d\n", TPD_RES_X, TPD_RES_Y);

    tpd_mode = TPD_MODE_NORMAL;
    tpd_mode_axis = 0;
    tpd_mode_min = TPD_RES_Y/2;
    tpd_mode_max = TPD_RES_Y;
    tpd_mode_keypad_tolerance = TPD_RES_X*TPD_RES_X/1600;
//Ivan >>

//#ifdef TPD_TYPE_CAPACITIVE
		/* TPD_TYPE_CAPACITIVE handle */
		if(touch_type == 1){
			
//Ivan		set_bit(ABS_MT_TRACKING_ID, tpd->dev->absbit);
    	set_bit(ABS_MT_TOUCH_MAJOR, tpd->dev->absbit);
    	set_bit(ABS_MT_TOUCH_MINOR, tpd->dev->absbit);
    	set_bit(ABS_MT_POSITION_X, tpd->dev->absbit);
    	set_bit(ABS_MT_POSITION_Y, tpd->dev->absbit);
        #if 0 // linux kernel update from 2.6.35 --> 3.0
    	tpd->dev->absmax[ABS_MT_POSITION_X] = TPD_RES_X;
    	tpd->dev->absmin[ABS_MT_POSITION_X] = 0;
    	tpd->dev->absmax[ABS_MT_POSITION_Y] = TPD_RES_Y;
    	tpd->dev->absmin[ABS_MT_POSITION_Y] = 0;
    	tpd->dev->absmax[ABS_MT_TOUCH_MAJOR] = 100;
    	tpd->dev->absmin[ABS_MT_TOUCH_MINOR] = 0;
#else
		input_set_abs_params(tpd->dev, ABS_MT_POSITION_X, 0, TPD_RES_X, 0, 0);
		input_set_abs_params(tpd->dev, ABS_MT_POSITION_Y, 0, TPD_RES_Y, 0, 0);
		input_set_abs_params(tpd->dev, ABS_MT_TOUCH_MAJOR, 0, 100, 0, 0);
		input_set_abs_params(tpd->dev, ABS_MT_TOUCH_MINOR, 0, 100, 0, 0); 	
#endif
    	TPD_DMESG("Cap touch panel driver\n");
  	}
//#endif
    #if 0 //linux kernel update from 2.6.35 --> 3.0
    tpd->dev->absmax[ABS_X] = TPD_RES_X;
    tpd->dev->absmin[ABS_X] = 0;
    tpd->dev->absmax[ABS_Y] = TPD_RES_Y;
    tpd->dev->absmin[ABS_Y] = 0;
	
    tpd->dev->absmax[ABS_PRESSURE] = 255;
    tpd->dev->absmin[ABS_PRESSURE] = 0;
    #else
		input_set_abs_params(tpd->dev, ABS_X, 0, TPD_RES_X, 0, 0);
		input_set_abs_params(tpd->dev, ABS_Y, 0, TPD_RES_Y, 0, 0);
		input_abs_set_res(tpd->dev, ABS_X, TPD_RES_X);
		input_abs_set_res(tpd->dev, ABS_Y, TPD_RES_Y);
		input_set_abs_params(tpd->dev, ABS_PRESSURE, 0, 255, 0, 0);

    #endif
    if(input_register_device(tpd->dev))
        TPD_DMESG("input_register_device failed.(tpd)\n");
    else {
			tpd_register_flag = 1;
			TPD_DMESG("input_register_device succeed!!\n");
		}
    /* init R-Touch */
    #if 0
	  if(touch_type == 0) 
	  {
	  	g_tpd_drv->tpd_local_init();
	  }    
		#endif
    if(g_tpd_drv->tpd_have_button)
    {
    	tpd_button_init();
    }
	
    device_create_file(&tpd->dev->dev, &dev_attr_panel_version);
    device_create_file(&tpd->dev->dev, &dev_attr_device_name);
	//edit by Magnum 2012-7-10
	device_create_file(&tpd->dev->dev, &dev_attr_vendor_version);
    tpd_create_attr_file();
    return 0;
}

static int tpd_remove(struct platform_device *pdev)
{
    device_remove_file(&tpd->dev->dev, &dev_attr_device_name);
    device_remove_file(&tpd->dev->dev, &dev_attr_panel_version);
	   input_unregister_device(tpd->dev);
    #ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&MTK_TS_early_suspend_handler);
    #endif
    return 0;
}

/* called when loaded into kernel */
static int __init tpd_device_init(void) {
    printk("MediaTek touch panel driver init\n");
    if(platform_driver_register(&tpd_driver)!=0) {
        TPD_DMESG("unable to register touch panel driver.\n");
        return -1;
    }   
    return 0;
}

/* should never be called */
static void __exit tpd_device_exit(void) {
    TPD_DMESG("MediaTek touch panel driver exit\n");
    //input_unregister_device(tpd->dev);
    platform_driver_unregister(&tpd_driver);
    #ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&MTK_TS_early_suspend_handler);
    #endif
}

module_init(tpd_device_init);
module_exit(tpd_device_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MediaTek touch panel driver");
MODULE_AUTHOR("Kirby Wu<kirby.wu@mediatek.com>");

