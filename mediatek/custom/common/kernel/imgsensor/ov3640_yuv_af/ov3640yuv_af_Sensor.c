/*****************************************************************************
 *
 * Filename:
 * ---------
 *   sensor.c
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 *   Source code of Sensor driver
 *
 *
 * Author:
 * -------
 *   PC Huang (MTK02204)
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 * 10 27 2010 sean.cheng
 * [ALPS00130222] [MPEG4 recording] Frame rate is 30fps by nigh mode
 * .check in for YUV night mode fps = 15
 *
 * 10 12 2010 sean.cheng
 * [ALPS00021722] [Need Patch] [Volunteer Patch][Camera]MT6573 Camera related function
 * .rollback the lib3a for mt6573 camera related files
 *
 * 09 10 2010 jackie.su
 * [ALPS00002279] [Need Patch] [Volunteer Patch] ALPS.Wxx.xx Volunteer patch for
 * .alps dual sensor
 *
 * 09 02 2010 jackie.su
 * [ALPS00002279] [Need Patch] [Volunteer Patch] ALPS.Wxx.xx Volunteer patch for
 * .roll back dual sensor
 *
 * 07 27 2010 sean.cheng
 * [ALPS00003112] [Need Patch] [Volunteer Patch] ISP/Sensor driver modification for Customer support
 * .1. add master clock switcher 
 *  2. add master enable/disable 
 *  3. add dummy line/pixel for sensor 
 *  4. add sensor driving current setting
 *
 * 07 19 2010 sean.cheng
 * [ALPS00002994][Need Patch] [Volunteer Patch] E1K YUV sensor update customer parameters 
 * .Optimize the sensor paramter & flicker caputre shutter setting
 *
 * 07 06 2010 sean.cheng
 * [ALPS00121501][Need Resolved][E1K][camera]The preview display abnormal when switch scen mode between auto  and night 
 * .Remove the gamma setting in night mode
 *
 * 07 06 2010 sean.cheng
 * [ALPS00121385][Camera] set EV as one non-zero value, after capturing one iamge , the value seems to be set to zero 
 * .change effect_off setting to reserve the EV setting
 *
 * 07 02 2010 sean.cheng
 * [ALPS00121364][Camera] when set AE value, the effect is disabled 
 * .Modify exposure setting to let effect remain
 *
 * 07 01 2010 sean.cheng
 * [ALPS00121215][Camera] Change color when switch low and high 
 * .Add video delay frame.
 *
 * 06 18 2010 sean.cheng
 * [ALPS00008131][E1K][Camera]Screen will flash some times in this case 
 * .Add 2 frame delay for capture back to preview
 *
 * 06 13 2010 sean.cheng
 * [ALPS00002514][Need Patch] [Volunteer Patch] ALPS.10X.W10.11 Volunteer patch for E1k Camera 
 * .Modify e1k sensor setting
 *
 * 06 13 2010 sean.cheng
 * [ALPS00002514][Need Patch] [Volunteer Patch] ALPS.10X.W10.11 Volunteer patch for E1k Camera 
 * .
 * 1. Add set zoom factor and capdelay frame for YUV sensor 
 * 2. Modify e1k sensor setting
 *
 * 06 09 2010 sean.cheng
 * [ALPS00007960][E1K][Camera]There will be a yellow block show on screen left side when preview 
 * .Change the VGA setting
 *
 * 05 27 2010 sean.cheng
 * [ALPS00002309][Need Patch] [Volunteer Patch] ALPS.10X.W10.24 Volunteer patch for E1k YUV Sensor support 
 * .
 * Update OV3640 yuv sensor init setting
 *
 * 05 26 2010 sean.cheng
 * [ALPS00001357][Meta]CameraTool 
 * .
 * Update OV3640 yuv sensor init setting
 *
 * 05 25 2010 sean.cheng
 * [ALPS00001357][Meta]CameraTool 
 * .
 * Add OV3640 YUV sensor driver support
 *
 * 05 03 2010 sean.cheng
 * [ALPS00001357][Meta]CameraTool 
 * .
 * Fix OV3640 YUV sensor frame rate to 30fps in vidoe mode
 *
 * Mar 4 2010 mtk70508
 * [DUMA00154792] Sensor driver
 * 
 *
 * Mar 4 2010 mtk70508
 * [DUMA00154792] Sensor driver
 * 
 *
 * Mar 1 2010 mtk01118
 * [DUMA00025869] [Camera][YUV I/F & Query feature] check in camera code
 * 
 *
 * Feb 24 2010 mtk01118
 * [DUMA00025869] [Camera][YUV I/F & Query feature] check in camera code
 * 
 *
 * Nov 24 2009 mtk02204
 * [DUMA00015869] [Camera Driver] Modifiy camera related drivers for dual/backup sensor/lens drivers.
 * 
 *
 * Oct 29 2009 mtk02204
 * [DUMA00015869] [Camera Driver] Modifiy camera related drivers for dual/backup sensor/lens drivers.
 * 
 *
 * Oct 27 2009 mtk02204
 * [DUMA00015869] [Camera Driver] Modifiy camera related drivers for dual/backup sensor/lens drivers.
 * 
 *
 * Aug 13 2009 mtk01051
 * [DUMA00009217] [Camera Driver] CCAP First Check In
 * 
 *
 * Aug 5 2009 mtk01051
 * [DUMA00009217] [Camera Driver] CCAP First Check In
 * 
 *
 * Jul 17 2009 mtk01051
 * [DUMA00009217] [Camera Driver] CCAP First Check In
 * 
 *
 * Jul 7 2009 mtk01051
 * [DUMA00008051] [Camera Driver] Add drivers for camera high ISO binning mode.
 * Add ISO query info for Sensor
 *
 * May 18 2009 mtk01051
 * [DUMA00005921] [Camera] LED Flashlight first check in
 * 
 *
 * May 16 2009 mtk01051
 * [DUMA00005921] [Camera] LED Flashlight first check in
 * 
 *
 * May 16 2009 mtk01051
 * [DUMA00005921] [Camera] LED Flashlight first check in
 * 
 *
 * Apr 7 2009 mtk02204
 * [DUMA00004012] [Camera] Restructure and rename camera related custom folders and folder name of came
 * 
 *
 * Mar 27 2009 mtk02204
 * [DUMA00002977] [CCT] First check in of MT6516 CCT drivers
 *
 *
 * Mar 25 2009 mtk02204
 * [DUMA00111570] [Camera] The system crash after some operations
 *
 *
 * Mar 20 2009 mtk02204
 * [DUMA00002977] [CCT] First check in of MT6516 CCT drivers
 *
 *
 * Mar 2 2009 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 *
 *
 * Feb 24 2009 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 *
 *
 * Dec 27 2008 MTK01813
 * DUMA_MBJ CheckIn Files
 * created by clearfsimport
 *
 * Dec 10 2008 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 *
 *
 * Oct 27 2008 mtk01051
 * [DUMA00000851] Camera related drivers check in
 * Modify Copyright Header
 *
 * Oct 24 2008 mtk02204
 * [DUMA00000851] Camera related drivers check in
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
//#include <windows.h>
//#include <memory.h>
//#include <nkintr.h>
//#include <ceddk.h>
//#include <ceddk_exp.h>

//#include "kal_release.h"
//#include "i2c_exp.h"
//#include "gpio_exp.h"
//#include "msdk_exp.h"
//#include "msdk_sensor_exp.h"
//#include "msdk_isp_exp.h"
//#include "base_regs.h"
//#include "Sensor.h"
//#include "camera_sensor_para.h"
//#include "CameraCustomized.h"

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
//#include <mach/mt6516_pll.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"

#include "ov3640yuv_af_Sensor.h"
#include "ov3640yuv_af_Camera_Sensor_para.h"
#include "ov3640yuv_af_CameraCustomized.h"

#define OV3640YUV_DEBUG
#ifdef OV3640YUV_DEBUG
#define SENSORDB(fmt, arg...) printk( "[OV3640YUVAF] "  fmt, ##arg)
#else
#define SENSORDB(x,...)
#endif

extern int iReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId);
extern int iWriteReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId);
extern int iMultiWriteReg(u8 *pData, u16 lens);

static int sensor_id_fail = 0; 
#define OV3640_write_cmos_sensor(addr, para) iWriteReg((u16) addr , (u32) para ,1,OV3640_WRITE_ID)
#define OV3640_write_cmos_sensor_2(addr, para, bytes) iWriteReg((u16) addr , (u32) para ,bytes,OV3640_WRITE_ID)
#define OV3640_multi_write_cmos_sensor(pData, bytes)  iMultiWriteReg(pData, bytes)

kal_uint16 OV3640_read_cmos_sensor(kal_uint32 addr)
{
kal_uint16 get_byte=0;
    iReadReg((u16) addr ,(u8*)&get_byte,OV3640_WRITE_ID);
    return get_byte;
}


/*******************************************************************************
* // Adapter for Winmo typedef 
********************************************************************************/
#define WINMO_USE 0

#define Sleep(ms) mdelay(ms)
#define RETAILMSG(x,...)
#define TEXT


/*******************************************************************************
* // End Adapter for Winmo typedef 
********************************************************************************/


#define	OV3640_LIMIT_EXPOSURE_LINES				(1253)
#define	OV3640_VIDEO_NORMALMODE_30FRAME_RATE       (30)
#define	OV3640_VIDEO_NORMALMODE_FRAME_RATE         (15)
#define	OV3640_VIDEO_NIGHTMODE_FRAME_RATE          (7.5)
#define BANDING50_30HZ
/* Global Valuable */

static kal_uint32 zoom_factor = 0; 

//static kal_uint8 OV3640_exposure_line_h = 0, OV3640_exposure_line_l = 0,OV3640_extra_exposure_line_h = 0, OV3640_extra_exposure_line_l = 0;

static kal_bool OV3640_gPVmode = KAL_TRUE; //PV size or Full size
static kal_bool OV3640_VEDIO_encode_mode = KAL_FALSE; //Picture(Jpeg) or Video(Mpeg4)
static kal_bool OV3640_sensor_cap_state = KAL_FALSE; //Preview or Capture

//static kal_uint16 OV3640_dummy_pixels=0, OV3640_dummy_lines=0;
kal_uint32 OV3640_FULL_dummy_pixels = 0;
kal_uint32 OV3640_FULL_dummy_lines = 0;


//static kal_uint16 OV3640_exposure_lines=0, 
static kal_uint16 OV3640_extra_exposure_lines = 0;


//static kal_int8 OV3640_DELAY_AFTER_PREVIEW = -1;

static kal_uint8 OV3640_Banding_setting = AE_FLICKER_MODE_50HZ;  //Wonder add

/****** OVT 6-18******/
static kal_uint16 OV3640_Capture_Max_Gain16= 6*16;
static kal_uint16 OV3640_Capture_Gain16=0 ;    
static kal_uint16 OV3640_Capture_Shutter=0;
static kal_uint16 OV3640_Capture_Extra_Lines=0;

static kal_uint16  OV3640_PV_Dummy_Pixels =0, OV3640_Capture_Dummy_Pixels =0, OV3640_Capture_Dummy_Lines =0;
static kal_uint16  OV3640_PV_Gain16 = 0;
static kal_uint16  OV3640_PV_Shutter = 0;
static kal_uint16  OV3640_PV_Extra_Lines = 0;

kal_uint16 OV3640_sensor_gain_base=0,OV3640_FAC_SENSOR_REG=0,OV3640_iOV3640_Mode=0,OV3640_max_exposure_lines=0;
kal_uint32 OV3640_capture_pclk_in_M=520,OV3640_preview_pclk_in_M=390,OV3640_PV_dummy_pixels=0,OV3640_PV_dummy_lines=0,OV3640_isp_master_clock=0;
static kal_uint32  OV3640_preview_pclk = 0, OV3640_capture_pclk = 0;
kal_bool OV3640_Night_mode = KAL_FALSE;
kal_bool OV3640_Y_Target_L = 64; 
kal_bool OV3640_Y_Target_H = 72; 

OV3640_OP_TYPE OV3640_g_iOV3640_Mode = OV3640_MODE_NONE;

static kal_uint32  OV3640_sensor_pclk=390;
kal_bool OV3640_VEDIO_MPEG4 = KAL_FALSE; //Picture(Jpeg) or Video(Mpeg4);

kal_bool first_enter_preview = KAL_FALSE;

#if WINMO_USE
kal_uint8 OV3640_sensor_write_I2C_address = OV3640_WRITE_ID;
kal_uint8 OV3640_sensor_read_I2C_address = OV3640_READ_ID;

UINT32 OV3640GPIOBaseAddr;
HANDLE OV3640hGPIO;
HANDLE OV3640hDrvI2C;
I2C_TRANSACTION OV3640I2CConfig;
#endif 
UINT8 OV3640_PixelClockDivider=0;

static kal_bool OV3640_AWB_ENABLE = KAL_TRUE; 
static kal_bool OV3640_AE_ENABLE = KAL_TRUE; 

static kal_uint32 Capture_Shutter = 0; 
static kal_uint32 Capture_Gain = 0; 


//add by lingnan for af status
static UINT8 STA_FOCUS = 0x8F; 
//static kal_uint32 MAC = 255;
//static kal_uint32 INF = 0;
static kal_uint32 AF_XS = 40;//version2.01, aug.2009
//static kal_uint32 AF_YS = 30;//version2.01, aug.2009
static kal_bool AF_INIT = FALSE;
static UINT8 ZONE[4] = {16, 11, 24, 19};////version0.21, aug.2009,center 4:3 window



//SENSOR_REG_STRUCT OV3640SensorCCT[FACTORY_END_ADDR]=CAMERA_SENSOR_CCT_DEFAULT_VALUE;
//SENSOR_REG_STRUCT OV3640SensorReg[ENGINEER_END]=CAMERA_SENSOR_REG_DEFAULT_VALUE;
//	camera_para.SENSOR.cct	SensorCCT	=> SensorCCT
//	camera_para.SENSOR.reg	SensorReg
MSDK_SENSOR_CONFIG_STRUCT OV3640SensorConfigData;


//=====================touch AE begin==========================//
typedef enum
{
    AE_SECTION_INDEX_BEGIN=0, 
    AE_SECTION_INDEX_1=AE_SECTION_INDEX_BEGIN, 
    AE_SECTION_INDEX_2, 
    AE_SECTION_INDEX_3, 
    AE_SECTION_INDEX_4, 
    AE_SECTION_INDEX_5, 
    AE_SECTION_INDEX_6, 
    AE_SECTION_INDEX_7, 
    AE_SECTION_INDEX_8, 
    AE_SECTION_INDEX_9, 
    AE_SECTION_INDEX_10, 
    AE_SECTION_INDEX_11, 
    AE_SECTION_INDEX_12, 
    AE_SECTION_INDEX_13, 
    AE_SECTION_INDEX_14, 
    AE_SECTION_INDEX_15, 
    AE_SECTION_INDEX_16,  
    AE_SECTION_INDEX_MAX
}AE_SECTION_INDEX;

typedef enum
{
    AE_VERTICAL_BLOCKS=4,
    AE_VERTICAL_BLOCKS_MAX,
    AE_HORIZONTAL_BLOCKS=4,
    AE_HORIZONTAL_BLOCKS_MAX
}AE_VERTICAL_HORIZONTAL_BLOCKS;

typedef enum
{
    PRV_W=640,
    PRV_H=480
}PREVIEW_VIEW_SIZE;

static UINT32 line_coordinate[AE_VERTICAL_BLOCKS_MAX] = {0};//line[0]=0      line[1]=160     line[2]=320     line[3]=480     line[4]=640
static UINT32 row_coordinate[AE_HORIZONTAL_BLOCKS_MAX] = {0};//line[0]=0       line[1]=120     line[2]=240     line[3]=360     line[4]=480
static BOOL AE_1_ARRAY[AE_SECTION_INDEX_MAX] = {FALSE};
static BOOL AE_2_ARRAY[AE_HORIZONTAL_BLOCKS][AE_VERTICAL_BLOCKS] = {{FALSE},{FALSE},{FALSE},{FALSE}};//how to ....

void readAEReg(void)
{
    SENSORDB("0x303c=0x%x\n",OV3640_read_cmos_sensor(0x303c));
    SENSORDB("0x303d=0x%x\n",OV3640_read_cmos_sensor(0x303d));
    SENSORDB("0x303e=0x%x\n",OV3640_read_cmos_sensor(0x303e));
    SENSORDB("0x303f=0x%x\n",OV3640_read_cmos_sensor(0x303f));

    SENSORDB("0x3030=0x%x\n",OV3640_read_cmos_sensor(0x3030));
    SENSORDB("0x3031=0x%x\n",OV3640_read_cmos_sensor(0x3031));
    SENSORDB("0x3032=0x%x\n",OV3640_read_cmos_sensor(0x3032));
    SENSORDB("0x3033=0x%x\n",OV3640_read_cmos_sensor(0x3033));
    SENSORDB("0x3034=0x%x\n",OV3640_read_cmos_sensor(0x3034));
    SENSORDB("0x3035=0x%x\n",OV3640_read_cmos_sensor(0x3035));
    SENSORDB("0x3036=0x%x\n",OV3640_read_cmos_sensor(0x3036));
    SENSORDB("0x3037=0x%x\n",OV3640_read_cmos_sensor(0x3037));

}
void writeAEReg(void)
{	
    UINT8 temp;
    //write 640X480
    OV3640_write_cmos_sensor(0x3038,0x01);
    OV3640_write_cmos_sensor(0x3039,0x1d);
    OV3640_write_cmos_sensor(0x303a,0x00);
    OV3640_write_cmos_sensor(0x303b,0x02);

    OV3640_write_cmos_sensor(0x303c,0x02);
    OV3640_write_cmos_sensor(0x303d,0x19);//0x0206,  2148/4
    OV3640_write_cmos_sensor(0x303e,0x00);
    OV3640_write_cmos_sensor(0x303f,0xC0);//0x0030,   192
    
    temp=0x11;
    if(AE_1_ARRAY[AE_SECTION_INDEX_1]==TRUE)    { temp=temp|0x0F;}
    if(AE_1_ARRAY[AE_SECTION_INDEX_2]==TRUE)    { temp=temp|0xF0;}
    //write 0x3030
    OV3640_write_cmos_sensor(0x3030,temp);
    
    temp=0x11;
    if(AE_1_ARRAY[AE_SECTION_INDEX_3]==TRUE)    { temp=temp|0x0F;}
    if(AE_1_ARRAY[AE_SECTION_INDEX_4]==TRUE)    { temp=temp|0xF0;}
    //write 0x3031
    OV3640_write_cmos_sensor(0x3031,temp);

    temp=0x11;
    if(AE_1_ARRAY[AE_SECTION_INDEX_5]==TRUE)    { temp=temp|0x0F;}
    if(AE_1_ARRAY[AE_SECTION_INDEX_6]==TRUE)    { temp=temp|0xF0;}
    //write 0x3032
    OV3640_write_cmos_sensor(0x3032,temp);
    
    temp=0x11;
    if(AE_1_ARRAY[AE_SECTION_INDEX_7]==TRUE)    { temp=temp|0x0F;}
    if(AE_1_ARRAY[AE_SECTION_INDEX_8]==TRUE)    { temp=temp|0xF0;}
    //write 0x3033
    OV3640_write_cmos_sensor(0x3033,temp);

    temp=0x11;
    if(AE_1_ARRAY[AE_SECTION_INDEX_9]==TRUE)    { temp=temp|0x0F;}
    if(AE_1_ARRAY[AE_SECTION_INDEX_10]==TRUE)  { temp=temp|0xF0;}
	//write 0x3034
    OV3640_write_cmos_sensor(0x3034,temp);

    temp=0x11;
    if(AE_1_ARRAY[AE_SECTION_INDEX_11]==TRUE)    { temp=temp|0x0F;}
    if(AE_1_ARRAY[AE_SECTION_INDEX_12]==TRUE)    { temp=temp|0xF0;}
    //write 0x3035
    OV3640_write_cmos_sensor(0x3035,temp);    
    
    temp=0x11;
    if(AE_1_ARRAY[AE_SECTION_INDEX_13]==TRUE)    { temp=temp|0x0F;}
    if(AE_1_ARRAY[AE_SECTION_INDEX_14]==TRUE)    { temp=temp|0xF0;}
	//write 0x3036
    OV3640_write_cmos_sensor(0x3036,temp);

    temp=0x11;
    if(AE_1_ARRAY[AE_SECTION_INDEX_15]==TRUE)    { temp=temp|0x0F;}
    if(AE_1_ARRAY[AE_SECTION_INDEX_16]==TRUE)    { temp=temp|0xF0;}
	//write 0x3037
    OV3640_write_cmos_sensor(0x3037,temp);

    readAEReg();

}


void printAE_1_ARRAY(void)
{
    UINT32 i;
    for(i=0; i<AE_SECTION_INDEX_MAX; i++)
    {
        SENSORDB("AE_1_ARRAY[%2d]=%d\n", i, AE_1_ARRAY[i]);
    }
}

void printAE_2_ARRAY(void)
{
    UINT32 i, j;
    SENSORDB("\t\t");
    for(i=0; i<AE_VERTICAL_BLOCKS; i++)
    {
        printk("      line[%2d]", i);
    }
    printk("\n");
    for(j=0; j<AE_HORIZONTAL_BLOCKS; j++)
    {
        SENSORDB("\trow[%2d]", j);
        for(i=0; i<AE_VERTICAL_BLOCKS; i++)
        {
            //SENSORDB("AE_2_ARRAY[%2d][%2d]=%d\n", j,i,AE_2_ARRAY[j][i]);
            printk("  %7d", AE_2_ARRAY[j][i]);
        }
        printk("\n");
    }
}

void clearAE_2_ARRAY(void)
{
    UINT32 i, j;
    for(j=0; j<AE_HORIZONTAL_BLOCKS; j++)
    {
        for(i=0; i<AE_VERTICAL_BLOCKS; i++)
        {AE_2_ARRAY[j][i]=FALSE;}
    }
}

void mapAE_2_ARRAY_To_AE_1_ARRAY(void)
{
    UINT32 i, j;
    for(j=0; j<AE_HORIZONTAL_BLOCKS; j++)
    {
        for(i=0; i<AE_VERTICAL_BLOCKS; i++)
        { AE_1_ARRAY[j*AE_VERTICAL_BLOCKS+i] = AE_2_ARRAY[j][i];}
    }
}

void mapMiddlewaresizePointToPreviewsizePoint(
    UINT32 mx,
    UINT32 my,
    UINT32 mw,
    UINT32 mh,
    UINT32 * pvx,
    UINT32 * pvy,
    UINT32 pvw,
    UINT32 pvh
)
{
    *pvx = pvw * mx / mw;
    *pvy = pvh * my / mh;
    SENSORDB("mapping middlware x[%d],y[%d], [%d X %d]\n\t\tto x[%d],y[%d],[%d X %d]\n ",
        mx, my, mw, mh, *pvx, *pvy, pvw, pvh);
}


void calcLine(void)
{//line[5]
    UINT32 i;
    UINT32 step = PRV_W / AE_VERTICAL_BLOCKS;
    for(i=0; i<=AE_VERTICAL_BLOCKS; i++)
    {
        *(&line_coordinate[0]+i) = step*i;
        SENSORDB("line[%d]=%d\t",i, *(&line_coordinate[0]+i));
    }
    SENSORDB("\n");
}

void calcRow(void)
{//row[5]
    UINT32 i;
    UINT32 step = PRV_H / AE_HORIZONTAL_BLOCKS;
    for(i=0; i<=AE_HORIZONTAL_BLOCKS; i++)
    {
        *(&row_coordinate[0]+i) = step*i;
        SENSORDB("row[%d]=%d\t",i,*(&row_coordinate[0]+i));
    }
    SENSORDB("\n");
}

void calcPointsAELineRowCoordinate(UINT32 x, UINT32 y, UINT32 * linenum, UINT32 * rownum)
{
    UINT32 i;
    i = 1;
    while(i<=AE_VERTICAL_BLOCKS)
    {
        if(x<line_coordinate[i])
        {
            *linenum = i;
            break;
        }
        *linenum = i++;
    }
    
    i = 1;
    while(i<=AE_HORIZONTAL_BLOCKS)
    {
        if(y<row_coordinate[i])
        {
            *rownum = i;
            break;
        }
        *rownum = i++;
    }
    SENSORDB("PV point [%d, %d] to section line coordinate[%d] row[%d]\n",x,y,*linenum,*rownum);
}



MINT32 clampSection(UINT32 x, UINT32 min, UINT32 max)
{
    if (x > max) return max;
    if (x < min) return min;
    return x;
}

void mapCoordinate(UINT32 linenum, UINT32 rownum, UINT32 * sectionlinenum, UINT32 * sectionrownum)
{
    *sectionlinenum = clampSection(linenum-1,0,AE_VERTICAL_BLOCKS-1);
    *sectionrownum = clampSection(rownum-1,0,AE_HORIZONTAL_BLOCKS-1);	
    SENSORDB("mapCoordinate from[%d][%d] to[%d][%d]\n",
		linenum, rownum,*sectionlinenum,*sectionrownum);
}

void mapRectToAE_2_ARRAY(UINT32 x0, UINT32 y0, UINT32 x1, UINT32 y1)
{
    UINT32 i, j;
    SENSORDB("([%d][%d]),([%d][%d])\n", x0,y0,x1,y1);
    clearAE_2_ARRAY();
    x0=clampSection(x0,0,AE_VERTICAL_BLOCKS-1);
    y0=clampSection(y0,0,AE_HORIZONTAL_BLOCKS-1);
    x1=clampSection(x1,0,AE_VERTICAL_BLOCKS-1);
    y1=clampSection(y1,0,AE_HORIZONTAL_BLOCKS-1);

    for(j=y0; j<=y1; j++)
    {
        for(i=x0; i<=x1; i++)
        {
            AE_2_ARRAY[j][i]=TRUE;
        }
    }
}

void resetPVAE_2_ARRAY(void)
{
    mapRectToAE_2_ARRAY(1,1,2,2);
}

//update ae window
//@input zone[] addr
void OV3640_FOCUS_AD5820_Set_AE_Window(UINT32 zone_addr)
{//update global zone
    
    //input:
    UINT32 FD_XS;
    UINT32 FD_YS;	
    UINT32 x0, y0, x1, y1;
    UINT32 pvx0, pvy0, pvx1, pvy1;
    UINT32 linenum, rownum;
    UINT32 rightbottomlinenum,rightbottomrownum;
    UINT32 leftuplinenum,leftuprownum;
    UINT32* zone = (UINT32*)zone_addr;
    x0 = *zone;
    y0 = *(zone + 1);
    x1 = *(zone + 2);
    y1 = *(zone + 3);	
    FD_XS = *(zone + 4);
    FD_YS = *(zone + 5);

    SENSORDB("AE x0=%d,y0=%d,x1=%d,y1=%d,FD_XS=%d,FD_YS=%d\n",
    x0, y0, x1, y1, FD_XS, FD_YS);	
    
    //print_sensor_ae_section();
    //print_AE_section();	

    //1.transfer points to preview size
    //UINT32 pvx0, pvy0, pvx1, pvy1;
    mapMiddlewaresizePointToPreviewsizePoint(x0,y0,FD_XS,FD_YS,&pvx0, &pvy0, PRV_W, PRV_H);
    mapMiddlewaresizePointToPreviewsizePoint(x1,y1,FD_XS,FD_YS,&pvx1, &pvy1, PRV_W, PRV_H);
    
    //2.sensor AE line and row coordinate
    calcLine();
    calcRow();

    //3.calc left up point to section
    //UINT32 linenum, rownum;
    calcPointsAELineRowCoordinate(pvx0,pvy0,&linenum,&rownum);    
    //UINT32 leftuplinenum,leftuprownum;
    mapCoordinate(linenum, rownum, &leftuplinenum, &leftuprownum);
    //SENSORDB("leftuplinenum=%d,leftuprownum=%d\n",leftuplinenum,leftuprownum);

    //4.calc right bottom point to section
    calcPointsAELineRowCoordinate(pvx1,pvy1,&linenum,&rownum);    
    //UINT32 rightbottomlinenum,rightbottomrownum;
    mapCoordinate(linenum, rownum, &rightbottomlinenum, &rightbottomrownum);
    //SENSORDB("rightbottomlinenum=%d,rightbottomrownum=%d\n",rightbottomlinenum,rightbottomrownum);

    //5.update global section array
    mapRectToAE_2_ARRAY(leftuplinenum, leftuprownum, rightbottomlinenum, rightbottomrownum);
    //print_AE_section();

    //6.write to reg
    mapAE_2_ARRAY_To_AE_1_ARRAY();
    //printAE_1_ARRAY();
    printAE_2_ARRAY();
    writeAEReg();

}
//=====================touch AE end==========================//


#if WINMO_USE
void OV3640_write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
	OV3640I2CConfig.operation=I2C_OP_WRITE;
	OV3640I2CConfig.slaveAddr=OV3640_sensor_write_I2C_address>>1;
	OV3640I2CConfig.transfer_num=1;	/* TRANSAC_LEN */
	OV3640I2CConfig.transfer_len=3;
	OV3640I2CConfig.buffer[0]=(UINT8)(addr>>8);
	OV3640I2CConfig.buffer[1]=(UINT8)(addr&0xFF);
	OV3640I2CConfig.buffer[2]=(UINT8)para;
	DRV_I2CTransaction(OV3640hDrvI2C, &OV3640I2CConfig);

}	/* OV3640_write_cmos_sensor() */

kal_uint32 OV3640_read_cmos_sensor(kal_uint32 addr)
{
	kal_uint8 get_byte=0xFF;

	OV3640I2CConfig.operation=I2C_OP_WRITE;
	OV3640I2CConfig.slaveAddr=OV3640_sensor_write_I2C_address>>1;
	OV3640I2CConfig.transfer_num=1;	/* TRANSAC_LEN */
	OV3640I2CConfig.transfer_len=2;
	OV3640I2CConfig.buffer[0]=(UINT8)(addr>>8);
	OV3640I2CConfig.buffer[1]=(UINT8)(addr&0xFF);
	DRV_I2CTransaction(OV3640hDrvI2C, &OV3640I2CConfig);

	OV3640I2CConfig.operation=I2C_OP_READ;
	OV3640I2CConfig.slaveAddr=OV3640_sensor_read_I2C_address>>1;
	OV3640I2CConfig.transfer_num=1;	/* TRANSAC_LEN */
	OV3640I2CConfig.transfer_len=1;
	DRV_I2CTransaction(OV3640hDrvI2C, &OV3640I2CConfig);
	get_byte=OV3640I2CConfig.buffer[0];

	return get_byte;
}	/* OV3640_read_cmos_sensor() */
#endif 
void OV3640_set_dummy(kal_uint16 pixels, kal_uint16 lines)
{
    kal_uint8 temp_reg1, temp_reg2;
    kal_uint16 temp_reg;

    /*Very Important: The line_length must < 0x1000, it is to say 0x3028 must < 0x10, or else the sensor will crash*/
    /*The dummy_pixel must < 2156*/
    if (pixels >= 2156) 
        pixels = 2155;
    if (pixels < 0x100)
    {
        OV3640_write_cmos_sensor(0x302c,(pixels&0xFF)); //EXHTS[7:0]
        temp_reg = OV3640_FULL_PERIOD_PIXEL_NUMS;
        OV3640_write_cmos_sensor(0x3029,(temp_reg&0xFF));         //H_length[7:0]
        OV3640_write_cmos_sensor(0x3028,((temp_reg&0xFF00)>>8));  //H_length[15:8]
    }
    else
    {
        OV3640_write_cmos_sensor(0x302c,0);
        temp_reg = pixels + OV3640_FULL_PERIOD_PIXEL_NUMS;
        OV3640_write_cmos_sensor(0x3029,(temp_reg&0xFF));         //H_length[7:0]
        OV3640_write_cmos_sensor(0x3028,((temp_reg&0xFF00)>>8));  //H_length[15:8]
    }

    // read out and + line
    temp_reg1 = OV3640_read_cmos_sensor(0x302B);    // VTS[b7~b0]
    temp_reg2 = OV3640_read_cmos_sensor(0x302A);    // VTS[b15~b8]
    temp_reg = (temp_reg1 & 0xFF) | (temp_reg2 << 8);

    temp_reg += lines;

    OV3640_write_cmos_sensor(0x302B,(temp_reg&0xFF));         //VTS[7:0]
    OV3640_write_cmos_sensor(0x302A,((temp_reg&0xFF00)>>8));  //VTS[15:8]
}    /* OV3640_set_dummy */

kal_uint16 OV3640_read_OV3640_gain(void)
{
    kal_uint8  temp_reg;
    kal_uint16 sensor_gain;

    temp_reg=OV3640_read_cmos_sensor(0x3000);  


    sensor_gain=(16+(temp_reg&0x0F));
    if(temp_reg&0x10)
        sensor_gain<<=1;
    if(temp_reg&0x20)
        sensor_gain<<=1;
      
    if(temp_reg&0x40)
        sensor_gain<<=1;
      
    if(temp_reg&0x80)
        sensor_gain<<=1;
      
    return sensor_gain;
}  /* OV3640_read_OV3640_gain */


kal_uint16 OV3640_read_shutter(void)
{
    kal_uint8 temp_reg1, temp_reg2;
    kal_uint16 temp_reg, extra_exp_lines;

    temp_reg1 = OV3640_read_cmos_sensor(0x3003);    // AEC[b7~b0]
    temp_reg2 = OV3640_read_cmos_sensor(0x3002);    // AEC[b15~b8]
    temp_reg = (temp_reg1 & 0xFF) | (temp_reg2 << 8);

    temp_reg1 = OV3640_read_cmos_sensor(0x302E);    // EXVTS[b7~b0]
    temp_reg2 = OV3640_read_cmos_sensor(0x302D);    // EXVTS[b15~b8]
    extra_exp_lines = (temp_reg1 & 0xFF) | (temp_reg2 << 8);

    OV3640_PV_Shutter = temp_reg ;
    OV3640_PV_Extra_Lines = extra_exp_lines;
    return temp_reg + extra_exp_lines;
}    /* OV3640_read_shutter */

void OV3640_write_OV3640_gain(kal_uint16 gain)
{    
    kal_uint16 temp_reg;
   
	RETAILMSG(1, (TEXT("OV3640 write gain: %d\r\n"), gain));
   
   if(gain > 248)  return ;//ASSERT(0);
   
    temp_reg = 0;
    if (gain > 31)
    {
        temp_reg |= 0x10;
        gain = gain >> 1;
    }
    if (gain > 31)
    {
        temp_reg |= 0x20;
        gain = gain >> 1;
    }

    if (gain > 31)
    {
        temp_reg |= 0x40;
        gain = gain >> 1;
    }
    if (gain > 31)
    {
        temp_reg |= 0x80;
        gain = gain >> 1;
    }
    
    if (gain > 16)
    {
        temp_reg |= ((gain -16) & 0x0f);
    }   
  
   OV3640_write_cmos_sensor(0x3000,temp_reg);
}  /* OV3640_write_OV3640_gain */

static void OV3640_write_shutter(kal_uint16 shutter)
{
    if (OV3640_gPVmode) 
    {
        if (shutter <= OV3640_PV_EXPOSURE_LIMITATION) 
        {
            OV3640_extra_exposure_lines = 0;
        }
        else 
        {
            OV3640_extra_exposure_lines=shutter - OV3640_PV_EXPOSURE_LIMITATION;
        }

        if (shutter > OV3640_PV_EXPOSURE_LIMITATION) 
        {
            shutter = OV3640_PV_EXPOSURE_LIMITATION;
        }
    }
    else 
    {
        if (shutter <= OV3640_FULL_EXPOSURE_LIMITATION) 
        {
            OV3640_extra_exposure_lines = 0;
    }
        else 
        {
            OV3640_extra_exposure_lines = shutter - OV3640_FULL_EXPOSURE_LIMITATION;
        }

        if (shutter > OV3640_FULL_EXPOSURE_LIMITATION) {
            shutter = OV3640_FULL_EXPOSURE_LIMITATION;
        }
    }

    // set extra exposure line
    OV3640_write_cmos_sensor(0x302E, OV3640_extra_exposure_lines & 0xFF);          // EXVTS[b7~b0]
    OV3640_write_cmos_sensor(0x302D, (OV3640_extra_exposure_lines & 0xFF00) >> 8); // EXVTS[b15~b8]

    /* Max exporsure time is 1 frmae period event if Tex is set longer than 1 frame period */
    OV3640_write_cmos_sensor(0x3003, shutter & 0xFF);           //AEC[7:0]
    OV3640_write_cmos_sensor(0x3002, (shutter & 0xFF00) >> 8);  //AEC[8:15]

}    /* OV3640_write_shutter */
/*
void OV3640_Computer_AEC(kal_uint16 preview_clk_in_M, kal_uint16 capture_clk_in_M)
{
    kal_uint16 PV_Line_Width;
    kal_uint16 Capture_Line_Width;
    kal_uint16 Capture_Maximum_Shutter;
    kal_uint16 Capture_Exposure;
    kal_uint16 Capture_Gain16;
    kal_uint16 Capture_Banding_Filter;
    kal_uint32 Gain_Exposure=0;

    PV_Line_Width = OV3640_PV_PERIOD_PIXEL_NUMS + OV3640_PV_Dummy_Pixels;   

    Capture_Line_Width = OV3640_FULL_PERIOD_PIXEL_NUMS + OV3640_Capture_Dummy_Pixels;
    Capture_Maximum_Shutter = OV3640_FULL_EXPOSURE_LIMITATION + OV3640_Capture_Dummy_Lines;
    Gain_Exposure = 1;
    ///////////////////////
    Gain_Exposure *=(OV3640_PV_Shutter+OV3640_PV_Extra_Lines);
    Gain_Exposure *=PV_Line_Width;  //970
    //   Gain_Exposure /=g_Preview_PCLK_Frequency;
    Gain_Exposure /=Capture_Line_Width;//1940
    Gain_Exposure = Gain_Exposure*capture_clk_in_M/preview_clk_in_M;// for clock   

    //OV3640_Capture_Gain16 = Capture_Gain16;
    OV3640_Capture_Extra_Lines = (Gain_Exposure > Capture_Maximum_Shutter)?
            (Gain_Exposure - Capture_Maximum_Shutter):0;     
    
    OV3640_Capture_Shutter = Gain_Exposure - OV3640_Capture_Extra_Lines;
}
*/

void OV3640_Computer_AECAGC(kal_uint16 preview_clk_in_M, kal_uint16 capture_clk_in_M)
{
    kal_uint16 PV_Line_Width;
    kal_uint16 Capture_Line_Width;
    kal_uint16 Capture_Maximum_Shutter;
    kal_uint16 Capture_Exposure;
    kal_uint16 Capture_Gain16;
    kal_uint32 Capture_Banding_Filter;
    kal_uint32 Gain_Exposure=0;

    PV_Line_Width = OV3640_PV_PERIOD_PIXEL_NUMS + OV3640_PV_Dummy_Pixels;   

    Capture_Line_Width = OV3640_FULL_PERIOD_PIXEL_NUMS + OV3640_Capture_Dummy_Pixels;
    Capture_Maximum_Shutter = OV3640_FULL_EXPOSURE_LIMITATION + OV3640_Capture_Dummy_Lines;

    if (OV3640_Banding_setting == AE_FLICKER_MODE_50HZ)
#if WINMO_USE        
        Capture_Banding_Filter = (kal_uint32)(capture_clk_in_M*1000000/100/(2*Capture_Line_Width)+0.5);
#else 
        Capture_Banding_Filter = (kal_uint32)(capture_clk_in_M*100000/100/(2*Capture_Line_Width));
#endif 
    else
#if WINMO_USE
        Capture_Banding_Filter = (kal_uint16)(capture_clk_in_M*1000000/120/(2*Capture_Line_Width)+0.5);
#else 
        Capture_Banding_Filter = (kal_uint32)(capture_clk_in_M*100000/120/(2*Capture_Line_Width) );
#endif 

    /*   Gain_Exposure = OV3640_PV_Gain16*(OV3640_PV_Shutter+OV3640_PV_Extra_Lines)*PV_Line_Width/g_Preview_PCLK_Frequency/Capture_Line_Width*g_Capture_PCLK_Frequency
    ;*/
    OV3640_PV_Gain16 = OV3640_read_OV3640_gain();
    Gain_Exposure = 1 * OV3640_PV_Gain16;  //For OV3640
    ///////////////////////
    Gain_Exposure *=(OV3640_PV_Shutter+OV3640_PV_Extra_Lines);
    Gain_Exposure *=PV_Line_Width;  //970
    //   Gain_Exposure /=g_Preview_PCLK_Frequency;
    Gain_Exposure /=Capture_Line_Width;//1940
    Gain_Exposure = Gain_Exposure*capture_clk_in_M/preview_clk_in_M;// for clock   

    //redistribute gain and exposure
    if (Gain_Exposure < (kal_uint32)(Capture_Banding_Filter * 16))     // Exposure < 1/100/120
    {
       if(Gain_Exposure<16){//exposure line smaller than 2 lines and gain smaller than 0x08 
            Gain_Exposure = Gain_Exposure*4;     
            Capture_Exposure = 1;
            Capture_Gain16 = (Gain_Exposure*2 + 1)/Capture_Exposure/2/4;
        }
        else
        {
            Capture_Exposure = Gain_Exposure /16;
            Capture_Gain16 = (Gain_Exposure*2 + 1)/Capture_Exposure/2;
        }
    }
    else 
    {
        if (Gain_Exposure >(kal_uint32)( Capture_Maximum_Shutter * 16)) // Exposure > Capture_Maximum_Shutter
        {
           
            Capture_Exposure = Capture_Maximum_Shutter/Capture_Banding_Filter*Capture_Banding_Filter;
            Capture_Gain16 = (Gain_Exposure*2 + 1)/Capture_Exposure/2;
            if (Capture_Gain16 > OV3640_Capture_Max_Gain16) 
            {
                // gain reach maximum, insert extra line
                Capture_Exposure = (kal_uint16)(Gain_Exposure*11 /10 /OV3640_Capture_Max_Gain16);
                
                // Exposure = n/100/120
                Capture_Exposure = Capture_Exposure/Capture_Banding_Filter * Capture_Banding_Filter;
                Capture_Gain16 = ((Gain_Exposure *4)/ Capture_Exposure+3)/4;
            }
        }
        else  // 1/100 < Exposure < Capture_Maximum_Shutter, Exposure = n/100/120
        {
            Capture_Exposure = Gain_Exposure/16/Capture_Banding_Filter;
            Capture_Exposure = Capture_Exposure * Capture_Banding_Filter;
            Capture_Gain16 = (Gain_Exposure*2 +1) / Capture_Exposure/2;
        }
    }
    
    OV3640_Capture_Gain16 = Capture_Gain16;
    OV3640_Capture_Extra_Lines = (Capture_Exposure > Capture_Maximum_Shutter)?
            (Capture_Exposure - Capture_Maximum_Shutter/Capture_Banding_Filter*Capture_Banding_Filter):0;     
    
    OV3640_Capture_Shutter = Capture_Exposure - OV3640_Capture_Extra_Lines;
}

#if WINMO_USE
void OV3640_set_isp_driving_current(kal_uint8 current)
{
}
#endif 



/*************************************************************************
* FUNCTION
*	OV3640_NightMode
*
* DESCRIPTION
*	This function night mode of OV3640.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void OV3640_night_mode(kal_bool enable)
{
	//kal_uint16 night = 0;
	//kal_uint16 temp=OV3640_read_cmos_sensor(0x3302);
	OV3640_Night_mode = enable;

	if (OV3640_sensor_cap_state == KAL_TRUE) {
		return ;	//If capture mode, return directely.
	}
   
	if (enable) {
		if (OV3640_VEDIO_encode_mode == KAL_TRUE)	//Fix video frame rate to 15fps
		   /* MJPEG or MPEG4  Apps */
		   //set max gain to 8x
		{		
		    //printk("[OV3640YUV]: Night Mode Video Mode setting\n"); 
		//	OV3640_write_cmos_sensor(0x3014,0x84);
			OV3640_write_cmos_sensor(0x3015,0x32);
			OV3640_write_cmos_sensor(0x3011,0x01);
		//	OV3640_write_cmos_sensor(0x3071,0x76);
		//	OV3640_write_cmos_sensor(0x3073,0x62);
		//	OV3640_write_cmos_sensor(0x301c,0x05);
		//	OV3640_write_cmos_sensor(0x301d,0x07);
			OV3640_write_cmos_sensor(0x302d,0x00);
			OV3640_write_cmos_sensor(0x302e,0x00);

		//	OV3640_write_cmos_sensor(0x3014, night | 0x08); //Enable night mode		
		}
		else  /* camera mode only */
		{     //set max gain to 16x
		//	OV3640_write_cmos_sensor(0x3015, 0x12);			
		//	OV3640_write_cmos_sensor(0x3014, night | 0x08); //Enable night mode
		    
	//	OV3640_write_cmos_sensor(0x3014,0xac);
		    //printk("[OV3640YUV]: Night Mode Normal Mode setting\n"); 	
		OV3640_write_cmos_sensor(0x3015,0x32);         //0x32
		OV3640_write_cmos_sensor(0x3011,0x00);
	//	OV3640_write_cmos_sensor(0x3071,0xED);
	//	OV3640_write_cmos_sensor(0x3073,0xC5);
	//	OV3640_write_cmos_sensor(0x301c,0x02);
	//	OV3640_write_cmos_sensor(0x301d,0x03);

	    //; Gamma adjust  for 	low noise 	
	  //  OV3640_write_cmos_sensor(0x3302,temp&0xfb);//turn off gamma
	   // OV3640_write_cmos_sensor(0x3302,0xeb);
#if 0
	OV3640_write_cmos_sensor(0x331b,0x04);								   
	OV3640_write_cmos_sensor(0x331c,0x08);								   
	OV3640_write_cmos_sensor(0x331d,0x12);								   
	OV3640_write_cmos_sensor(0x331e,0x28);								   
	OV3640_write_cmos_sensor(0x331f,0x32);								   
	OV3640_write_cmos_sensor(0x3320,0x3e);								   
	OV3640_write_cmos_sensor(0x3321,0x48);								   
	OV3640_write_cmos_sensor(0x3322,0x52);								   
	OV3640_write_cmos_sensor(0x3323,0x60);								   
	OV3640_write_cmos_sensor(0x3324,0x68);								   
	OV3640_write_cmos_sensor(0x3325,0x78);								   
	OV3640_write_cmos_sensor(0x3326,0x88);								   
	OV3640_write_cmos_sensor(0x3327,0xa8);								   
	OV3640_write_cmos_sensor(0x3328,0xc2);								   
	OV3640_write_cmos_sensor(0x3329,0xdc);								   
	OV3640_write_cmos_sensor(0x332a,0x30);	
#endif 	
	
	//@@ UV Auto Mode gth1=2 gth2=3 offset=16 ;K=(31-offset)/(G2-G1) By OV-Allen 100705_01
	//OV3640_write_cmos_sensor(0x3302,0xef);    //;enable UV adj
	OV3640_write_cmos_sensor(0x30b8,0xf0);		//;auto mode slop
	OV3640_write_cmos_sensor(0x30b9,0x16);    //;offset
	OV3640_write_cmos_sensor(0x30ba,0x02);    //;gth1
	OV3640_write_cmos_sensor(0x30bb,0x03);    //;gth2																	   	    
		}
	}
	else {
	     /* when enter normal mode (disable night mode) without light,the AE vibrate */
		
		if (OV3640_VEDIO_encode_mode == KAL_TRUE)	//Fix video frame rate to 30fps
		{    /* MJPEG or MPEG4 Apps */
		     // set max gain to 4x
		//	 OV3640_write_cmos_sensor(0x3014,0x84);
			 OV3640_write_cmos_sensor(0x3015,0x21);
			 OV3640_write_cmos_sensor(0x3011,0x00);
		//	 OV3640_write_cmos_sensor(0x3071,0xED);
		//	 OV3640_write_cmos_sensor(0x3073,0xC5);
		//	 OV3640_write_cmos_sensor(0x301c,0x02);
		//	 OV3640_write_cmos_sensor(0x301d,0x03);
			 OV3640_write_cmos_sensor(0x302d,0x00);
			 OV3640_write_cmos_sensor(0x302e,0x00);

		}
		else
		{
			/*  camera mode only */           
		//	OV3640_write_cmos_sensor(0x3014,0xac);
		//OV3640_write_cmos_sensor(0x3300,0x08); // sensor pattern gen Danny
			OV3640_write_cmos_sensor(0x3015,0x21);
			OV3640_write_cmos_sensor(0x3011,0x00);
		//	OV3640_write_cmos_sensor(0x3071,0xED);
		//	OV3640_write_cmos_sensor(0x3073,0xC5);
		//	OV3640_write_cmos_sensor(0x301c,0x02);
		//	OV3640_write_cmos_sensor(0x301d,0x03);
			 OV3640_write_cmos_sensor(0x302d,0x00);
			 OV3640_write_cmos_sensor(0x302e,0x00);
		 //   OV3640_write_cmos_sensor(0x3302,temp|0x04);// turn on gamma
		 	//OV3640_write_cmos_sensor(0x3302,0xef);// turn on gamma
	//@@ UV Auto Mode gth1=2 gth2=3 offset=16 ;K=(31-offset)/(G2-G1) By OV-Allen 100705_01
	//OV3640_write_cmos_sensor(0x3302,0xef);    //;enable UV adj
	OV3640_write_cmos_sensor(0x30b8,0xf0);		//;auto mode slop
	OV3640_write_cmos_sensor(0x30b9,0x16);    //;offset
	OV3640_write_cmos_sensor(0x30ba,0x02);    //;gth1
	OV3640_write_cmos_sensor(0x30bb,0x03);    //;gth2
	    }
#if 0		
        	OV3640_write_cmos_sensor(0x331b,0x0a);								   
        	OV3640_write_cmos_sensor(0x331c,0x18);								   
        	OV3640_write_cmos_sensor(0x331d,0x30);								   
        	OV3640_write_cmos_sensor(0x331e,0x5a);								   
        	OV3640_write_cmos_sensor(0x331f,0x67);								   
        	OV3640_write_cmos_sensor(0x3320,0x72);								   
        	OV3640_write_cmos_sensor(0x3321,0x7e);								   
        	OV3640_write_cmos_sensor(0x3322,0x87);								   
        	OV3640_write_cmos_sensor(0x3323,0x8f);								   
        	OV3640_write_cmos_sensor(0x3324,0x96);								   
        	OV3640_write_cmos_sensor(0x3325,0xa3);								   
        	OV3640_write_cmos_sensor(0x3326,0xaf);								   
        	OV3640_write_cmos_sensor(0x3327,0xc4);								   
        	OV3640_write_cmos_sensor(0x3328,0xd7);								   
        	OV3640_write_cmos_sensor(0x3329,0xe8);								   
        	OV3640_write_cmos_sensor(0x332a,0x20);								   
#endif 

		//clear extra exposure line
		 //   OV3640_write_cmos_sensor(0x302d, 0x00);		
		//	OV3640_write_cmos_sensor(0x302e, 0x00);
		
	}
#if WINMO_USE	
	kal_prompt_trace(MOD_ENG,"call nightmode function ");
	kal_prompt_trace(MOD_ENG,"night_mode=%d,video_mode=%d",enable,OV3640_VEDIO_encode_mode);
	kal_prompt_trace(MOD_ENG,"Register 0x3001=0x%x",OV3640_read_cmos_sensor(0x3001));
	kal_prompt_trace(MOD_ENG,"Register 0x3002=0x%x",OV3640_read_cmos_sensor(0x3002));
	kal_prompt_trace(MOD_ENG,"Register 0x3003=0x%x",OV3640_read_cmos_sensor(0x3003));
#endif	

}	/* OV3640_NightMode */


/* Register setting from capture to preview. */
static void OV3640_set_VGA_mode(void)
{
	//-----------------------------------
	//From capture to preview
	//-----------------------------------

	//-------------------------------------------------------------------------------
    // PLL MY_OUTPUT clock(fclk)
    // fclk = (0x40 - 0x300E[5:0]) x N x Bit8Div x MCLK / M, where
    //      N = 1, 1.5, 2, 3 for 0x300F[7:6] = 0~3, respectively
    //      M = 1, 1.5, 2, 3 for 0x300F[1:0] = 0~3, respectively
    //      Bit8Div = 1, 1, 4, 5 for 0x300F[5:4] = 0~3, respectively
    // Sys Clk = fclk / Bit8Div / SenDiv
    // Sensor MY_OUTPUT clock(DVP PCLK)
    // DVP PCLK = ISP CLK / DVPDiv, where
    //      ISP CLK =  fclk / Bit8Div / SenDiv / CLKDiv / 2, where
    //          Bit8Div = 1, 1, 4, 5 for 0x300F[5:4] = 0~3, respectively
    //          SenDiv = 1, 2 for 0x3010[4] = 0 or 1 repectively
    //          CLKDiv = (0x3011[5:0] + 1)
    //      DVPDiv = 0x304C[3:0] * (2 ^ 0x304C[4]), if 0x304C[3:0] = 0, use 16 instead
    //
    // Base shutter calculation
    //      60Hz: (1/120) * ISP Clk / QXGA_MODE_WITHOUT_DUMMY_PIXELS
    //      50Hz: (1/100) * ISP Clk / QXGA_MODE_WITHOUT_DUMMY_PIXELS
    //-------------------------------------------------------------------------------

	    //26MHz Mclk,22.5Mhz Pclk 30fps	   

	    OV3640_write_cmos_sensor(0x300e,0x33); 
	    OV3640_write_cmos_sensor(0x300f,0x21); 
	    OV3640_write_cmos_sensor(0x3010,0x20); 
	    OV3640_write_cmos_sensor(0x3011,0x00); 
	    OV3640_write_cmos_sensor(0x304c,0x85);      //reserve

	    //size:640x480	

#if 0
	    OV3640_write_cmos_sensor(0x3012,0x10);					//soft reset,  sensor array resolution:XGA(1024X768) 	
	    
		//OV3640_write_cmos_sensor(0x3023,0x07);					// ;05 VS[7:0]:Vertical start point of array																										
              OV3640_write_cmos_sensor(0x3023,0x06);					// ;05 VS[7:0]:Vertical start point of array																												
		OV3640_write_cmos_sensor(0x3026,0x03);					//VH[15:8]:Vertical height,  																									
		OV3640_write_cmos_sensor(0x3027,0x04);					//VH[7:0]	VH=772																										
		OV3640_write_cmos_sensor(0x302a,0x03);					//VTS[15:8]:Vertical total size for 1 frame 																										
		OV3640_write_cmos_sensor(0x302b,0x10);					//VTS[7:0]	VTS=784																									
		OV3640_write_cmos_sensor(0x3075,0x24);				  //Vsync start point and width 																										
		OV3640_write_cmos_sensor(0x300d,0x01);				  // pclk always output,																											
		//OV3640_write_cmos_sensor(0x30d7,0x90);				  //reserve 																									
		OV3640_write_cmos_sensor(0x30d7,0x80);				  //reserve 																									
		OV3640_write_cmos_sensor(0x3069,0x04);				  //;44 ;BLC																									
		OV3640_write_cmos_sensor(0x303e,0x00);				  //AVH[11:8]:Average window vertical height																											
		OV3640_write_cmos_sensor(0x303f,0xc0);				  //AVH[7:0]	,AVH=192(192*4=768)
		//need check if need write this register 0x304c
		OV3640_write_cmos_sensor(0x304c,0x85);				  //reserve 			
		
		OV3640_write_cmos_sensor(0x3302,0xef);				  //  sde, uv_adj, gam, awb,scale_en																				
		OV3640_write_cmos_sensor(0x335f,0x34);				  // SIZE_IN_MISC:Vsize_in[10:8],Hsize_in[11:8] 																										
		OV3640_write_cmos_sensor(0x3360,0x0c);				  // Hsize_in[7:0]	,Hsize_in=1036																									
		OV3640_write_cmos_sensor(0x3361,0x04);				  // Vsize_in[7:0]	,Vsize_in=772																									
		OV3640_write_cmos_sensor(0x3362,0x12);				  // SIZE_OUT_MISC:Zoom_out output size:Vsize_out[10:8],Hsize_out[11:8] 																				
		OV3640_write_cmos_sensor(0x3363,0x88);				  // Hsize_out[7:0] for zoom_out	Hsize_out=648																										
		OV3640_write_cmos_sensor(0x3364,0xE4);				  // Vsize_out[7:0] for zoom_out	Vsize_out=484																									
		OV3640_write_cmos_sensor(0x3403,0x42);				  //bit[7:4]:x start=4,	 bit[3:0]:y start	=2																										
		OV3640_write_cmos_sensor(0x3088,0x12);				  //x_output_size, isp_xout[15:8]:																							
		OV3640_write_cmos_sensor(0x3089,0x80);				  // isp_xout[7:0]	:640																									
		OV3640_write_cmos_sensor(0x308a,0x01);				  // y_output_size,isp_yout[15:8]																						
		OV3640_write_cmos_sensor(0x308b,0xe0);				  // isp_yout[7:0]	:480	
		OV3640_write_cmos_sensor(0x3366,0x15);		//reserve
#else
              OV3640_write_cmos_sensor(0x3012, 0x10);
              OV3640_write_cmos_sensor(0x3023, 0x06);
              OV3640_write_cmos_sensor(0x3026, 0x03);
              OV3640_write_cmos_sensor(0x3027, 0x04);
              OV3640_write_cmos_sensor(0x302a, 0x03);
              OV3640_write_cmos_sensor(0x302b, 0x10);
              OV3640_write_cmos_sensor(0x3075, 0x24);
              OV3640_write_cmos_sensor(0x300d, 0x01);
              OV3640_write_cmos_sensor(0x30d7, 0x90);
              OV3640_write_cmos_sensor(0x3069, 0x04);
              OV3640_write_cmos_sensor(0x303e, 0x00);
              OV3640_write_cmos_sensor(0x303f, 0xc0);
              OV3640_write_cmos_sensor(0x3302, 0xef);
              OV3640_write_cmos_sensor(0x335f, 0x34);
              OV3640_write_cmos_sensor(0x3360, 0x0c);
              OV3640_write_cmos_sensor(0x3361, 0x04);
              OV3640_write_cmos_sensor(0x3362, 0x12);
              OV3640_write_cmos_sensor(0x3363, 0x88);
              OV3640_write_cmos_sensor(0x3364, 0xe4);
              OV3640_write_cmos_sensor(0x3403, 0x42);
              OV3640_write_cmos_sensor(0x3088, 0x02);//12->02
              OV3640_write_cmos_sensor(0x3089, 0x80);
              OV3640_write_cmos_sensor(0x308a, 0x01);
              OV3640_write_cmos_sensor(0x308b, 0xe0);
		
#endif 

    //    OV3640_write_cmos_sensor(0x308d,0x04);				  //reset block sleep enable																											
	//    OV3640_write_cmos_sensor(0x3086,0x03);				  //sleep on																											
	 //   OV3640_write_cmos_sensor(0x3086,0x00);				  // sleep off	
}

/*************************************************************************
* FUNCTION
*   OV3640_FOCUS_AD5820_Init
*
* DESCRIPTION
*   This function is to load micro code for AF function
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
//OV3640R1C_VCM_AD5820(V2.10)OVERLAY_STD.ovt

static u8 AD5820_Config[] = {   
    0x02,
    0x00,
    0x06,
    0x02,
    0x07,
    0xa9,
    0x78,
    0x7f,
    0xe4,
    0xf6,
    0xd8,
    0xfd,
    0x75,
    0x81,
    0x45,
    0x02,
    0x0a,
    0xdf,
    0x32,
    0x02,
    0x00,
    0x12,
    0xe8,
    0x8f,
    0xf0,
    0xa4,
    0xcc,
    0x8b,
    0xf0,
    0xa4,
    0x2c,
    0xfc,
    0xe9,
    0x8e,
    0xf0,
    0xa4,
    0x2c,
    0xfc,
    0x8a,
    0xf0,
    0xed,
    0xa4,
    0x2c,
    0xfc,
    0xea,
    0x8e,
    0xf0,
    0xa4,
    0xcd,
    0xa8,
    0xf0,
    0x8b,
    0xf0,
    0xa4,
    0x2d,
    0xcc,
    0x38,
    0x25,
    0xf0,
    0xfd,
    0xe9,
    0x8f,
    0xf0,
    0xa4,
    0x2c,
    0xcd,
    0x35,
    0xf0,
    0xfc,
    0xeb,
    0x8e,
    0xf0,
    0xa4,
    0xfe,
    0xa9,
    0xf0,
    0xeb,
    0x8f,
    0xf0,
    0xa4,
    0xcf,
    0xc5,
    0xf0,
    0x2e,
    0xcd,
    0x39,
    0xfe,
    0xe4,
    0x3c,
    0xfc,
    0xea,
    0xa4,
    0x2d,
    0xce,
    0x35,
    0xf0,
    0xfd,
    0xe4,
    0x3c,
    0xfc,
    0x22,
    0x75,
    0xf0,
    0x08,
    0x75,
    0x82,
    0x00,
    0xef,
    0x2f,
    0xff,
    0xee,
    0x33,
    0xfe,
    0xcd,
    0x33,
    0xcd,
    0xcc,
    0x33,
    0xcc,
    0xc5,
    0x82,
    0x33,
    0xc5,
    0x82,
    0x9b,
    0xed,
    0x9a,
    0xec,
    0x99,
    0xe5,
    0x82,
    0x98,
    0x40,
    0x0c,
    0xf5,
    0x82,
    0xee,
    0x9b,
    0xfe,
    0xed,
    0x9a,
    0xfd,
    0xec,
    0x99,
    0xfc,
    0x0f,
    0xd5,
    0xf0,
    0xd6,
    0xe4,
    0xce,
    0xfb,
    0xe4,
    0xcd,
    0xfa,
    0xe4,
    0xcc,
    0xf9,
    0xa8,
    0x82,
    0x22,
    0xb8,
    0x00,
    0xc1,
    0xb9,
    0x00,
    0x59,
    0xba,
    0x00,
    0x2d,
    0xec,
    0x8b,
    0xf0,
    0x84,
    0xcf,
    0xce,
    0xcd,
    0xfc,
    0xe5,
    0xf0,
    0xcb,
    0xf9,
    0x78,
    0x18,
    0xef,
    0x2f,
    0xff,
    0xee,
    0x33,
    0xfe,
    0xed,
    0x33,
    0xfd,
    0xec,
    0x33,
    0xfc,
    0xeb,
    0x33,
    0xfb,
    0x10,
    0xd7,
    0x03,
    0x99,
    0x40,
    0x04,
    0xeb,
    0x99,
    0xfb,
    0x0f,
    0xd8,
    0xe5,
    0xe4,
    0xf9,
    0xfa,
    0x22,
    0x78,
    0x18,
    0xef,
    0x2f,
    0xff,
    0xee,
    0x33,
    0xfe,
    0xed,
    0x33,
    0xfd,
    0xec,
    0x33,
    0xfc,
    0xc9,
    0x33,
    0xc9,
    0x10,
    0xd7,
    0x05,
    0x9b,
    0xe9,
    0x9a,
    0x40,
    0x07,
    0xec,
    0x9b,
    0xfc,
    0xe9,
    0x9a,
    0xf9,
    0x0f,
    0xd8,
    0xe0,
    0xe4,
    0xc9,
    0xfa,
    0xe4,
    0xcc,
    0xfb,
    0x22,
    0x75,
    0xf0,
    0x10,
    0xef,
    0x2f,
    0xff,
    0xee,
    0x33,
    0xfe,
    0xed,
    0x33,
    0xfd,
    0xcc,
    0x33,
    0xcc,
    0xc8,
    0x33,
    0xc8,
    0x10,
    0xd7,
    0x07,
    0x9b,
    0xec,
    0x9a,
    0xe8,
    0x99,
    0x40,
    0x0a,
    0xed,
    0x9b,
    0xfd,
    0xec,
    0x9a,
    0xfc,
    0xe8,
    0x99,
    0xf8,
    0x0f,
    0xd5,
    0xf0,
    0xda,
    0xe4,
    0xcd,
    0xfb,
    0xe4,
    0xcc,
    0xfa,
    0xe4,
    0xc8,
    0xf9,
    0x22,
    0xe8,
    0x60,
    0x0f,
    0xef,
    0xc3,
    0x33,
    0xff,
    0xee,
    0x33,
    0xfe,
    0xed,
    0x33,
    0xfd,
    0xec,
    0x33,
    0xfc,
    0xd8,
    0xf1,
    0x22,
    0xa4,
    0x25,
    0x82,
    0xf5,
    0x82,
    0xe5,
    0xf0,
    0x35,
    0x83,
    0xf5,
    0x83,
    0x22,
    0xd0,
    0x83,
    0xd0,
    0x82,
    0xf8,
    0xe4,
    0x93,
    0x70,
    0x12,
    0x74,
    0x01,
    0x93,
    0x70,
    0x0d,
    0xa3,
    0xa3,
    0x93,
    0xf8,
    0x74,
    0x01,
    0x93,
    0xf5,
    0x82,
    0x88,
    0x83,
    0xe4,
    0x73,
    0x74,
    0x02,
    0x93,
    0x68,
    0x60,
    0xef,
    0xa3,
    0xa3,
    0xa3,
    0x80,
    0xdf,
    0x75,
    0x0c,
    0x0a,
    0xa2,
    0xaf,
    0x92,
    0x22,
    0xc2,
    0xaf,
    0xc2,
    0x23,
    0x12,
    0x04,
    0x07,
    0x12,
    0x04,
    0x4d,
    0x12,
    0x04,
    0x07,
    0x75,
    0x32,
    0x05,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0xd2,
    0x18,
    0x12,
    0x04,
    0x09,
    0x75,
    0x32,
    0x0a,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0xc2,
    0x19,
    0x12,
    0x04,
    0x09,
    0x75,
    0x32,
    0x05,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0xc2,
    0x18,
    0x12,
    0x04,
    0x09,
    0x75,
    0x32,
    0x05,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0x75,
    0x0d,
    0x18,
    0x12,
    0x02,
    0xb8,
    0x75,
    0x32,
    0x0a,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0xd2,
    0x18,
    0x12,
    0x04,
    0x09,
    0x12,
    0x04,
    0x97,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0xc2,
    0x18,
    0x12,
    0x04,
    0x09,
    0x75,
    0x32,
    0x0a,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0x30,
    0x11,
    0x03,
    0x02,
    0x02,
    0x6f,
    0x12,
    0x04,
    0x07,
    0x12,
    0x04,
    0x4d,
    0xe5,
    0x0a,
    0xf5,
    0x0d,
    0x12,
    0x02,
    0xb8,
    0x75,
    0x32,
    0x0a,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0xd2,
    0x18,
    0x12,
    0x04,
    0x09,
    0x12,
    0x04,
    0x97,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0xc2,
    0x18,
    0x12,
    0x04,
    0x09,
    0x75,
    0x32,
    0x0a,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0x30,
    0x11,
    0x04,
    0x15,
    0x0c,
    0x80,
    0x45,
    0x12,
    0x04,
    0x07,
    0x12,
    0x04,
    0x4d,
    0x85,
    0x0b,
    0x0d,
    0x12,
    0x0a,
    0x9b,
    0xc2,
    0x0f,
    0x12,
    0x04,
    0x4f,
    0x75,
    0x32,
    0x0a,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0xd2,
    0x18,
    0x12,
    0x04,
    0x09,
    0x12,
    0x04,
    0x97,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0xc2,
    0x18,
    0x12,
    0x04,
    0x09,
    0x75,
    0x32,
    0x0a,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0x30,
    0x11,
    0x06,
    0x15,
    0x0c,
    0xd2,
    0x23,
    0x80,
    0x03,
    0xe4,
    0xf5,
    0x0c,
    0x12,
    0x04,
    0x07,
    0x12,
    0x04,
    0x4d,
    0xc2,
    0x19,
    0x12,
    0x04,
    0x09,
    0x75,
    0x32,
    0x05,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0xd2,
    0x18,
    0x12,
    0x04,
    0x09,
    0x75,
    0x32,
    0x05,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0x12,
    0x04,
    0x07,
    0x75,
    0x32,
    0x05,
    0xaf,
    0x32,
    0x15,
    0x32,
    0xef,
    0x70,
    0xf9,
    0xa2,
    0x22,
    0x92,
    0xaf,
    0xe5,
    0x0c,
    0xd3,
    0x94,
    0x00,
    0x40,
    0x03,
    0x02,
    0x01,
    0x7f,
    0x22,
    0x12,
    0x0a,
    0x9b,
    0xc2,
    0x0f,
    0x90,
    0x30,
    0xb1,
    0xe5,
    0x21,
    0xf0,
    0x22,
    0x90,
    0x33,
    0x5f,
    0xe0,
    0x54,
    0x0f,
    0xfe,
    0xa3,
    0xe0,
    0xfd,
    0xed,
    0xff,
    0xee,
    0x54,
    0x0f,
    0xf5,
    0x0d,
    0x8f,
    0x0e,
    0x90,
    0x33,
    0x5f,
    0xe0,
    0x54,
    0x70,
    0x75,
    0xf0,
    0x10,
    0xa4,
    0xff,
    0x90,
    0x33,
    0x61,
    0xe0,
    0xfd,
    0xef,
    0x4d,
    0xff,
    0xe5,
    0xf0,
    0x54,
    0x07,
    0xf5,
    0x0f,
    0x8f,
    0x10,
    0xe5,
    0x0e,
    0xae,
    0x0d,
    0x78,
    0x05,
    0xce,
    0xc3,
    0x13,
    0xce,
    0x13,
    0xd8,
    0xf9,
    0xf5,
    0x0e,
    0x8e,
    0x0d,
    0xe5,
    0x10,
    0xae,
    0x0f,
    0x78,
    0x03,
    0xce,
    0xc3,
    0x13,
    0xce,
    0x13,
    0xd8,
    0xf9,
    0xf5,
    0x10,
    0x8e,
    0x0f,
    0x85,
    0x2a,
    0x11,
    0x85,
    0x2b,
    0x13,
    0x85,
    0x2c,
    0x12,
    0x85,
    0x2d,
    0x14,
    0x12,
    0x04,
    0xab,
    0xaf,
    0x11,
    0x12,
    0x03,
    0xdf,
    0x8f,
    0x11,
    0x12,
    0x04,
    0xab,
    0xaf,
    0x12,
    0x12,
    0x03,
    0xdf,
    0x8f,
    0x12,
    0x12,
    0x04,
    0x56,
    0xaf,
    0x13,
    0xfc,
    0xfd,
    0xfe,
    0x12,
    0x00,
    0x16,
    0x12,
    0x03,
    0xfe,
    0x7b,
    0x1e,
    0x12,
    0x03,
    0xf7,
    0x8f,
    0x13,
    0x12,
    0x04,
    0x56,
    0xaf,
    0x14,
    0xfc,
    0xfd,
    0xfe,
    0x12,
    0x00,
    0x16,
    0x12,
    0x03,
    0xfe,
    0xe4,
    0x7b,
    0x1e,
    0x12,
    0x03,
    0xf8,
    0x8f,
    0x14,
    0xc3,
    0xe5,
    0x12,
    0x95,
    0x11,
    0xff,
    0x0f,
    0xef,
    0xc3,
    0x13,
    0xff,
    0xc3,
    0x94,
    0x02,
    0x50,
    0x27,
    0xe5,
    0x11,
    0x9f,
    0x40,
    0x06,
    0xe5,
    0x11,
    0x9f,
    0xfe,
    0x80,
    0x02,
    0x7e,
    0x00,
    0x8e,
    0x11,
    0xef,
    0xfd,
    0xe5,
    0x12,
    0x2d,
    0xfd,
    0xe4,
    0x33,
    0xfc,
    0xc3,
    0xed,
    0x95,
    0x0e,
    0xec,
    0x95,
    0x0d,
    0x50,
    0x02,
    0x80,
    0x02,
    0xad,
    0x0e,
    0x8d,
    0x12,
    0xc3,
    0xe5,
    0x14,
    0x95,
    0x13,
    0xff,
    0xc3,
    0x94,
    0x08,
    0x50,
    0x29,
    0xe5,
    0x13,
    0x9f,
    0x40,
    0x06,
    0xe5,
    0x13,
    0x9f,
    0xfe,
    0x80,
    0x02,
    0x7e,
    0x00,
    0x8e,
    0x13,
    0xef,
    0xfd,
    0xe5,
    0x14,
    0x2d,
    0xfd,
    0xe4,
    0x33,
    0xfc,
    0xc3,
    0xed,
    0x95,
    0x10,
    0xec,
    0x95,
    0x0f,
    0x50,
    0x04,
    0xaf,
    0x05,
    0x80,
    0x02,
    0xaf,
    0x10,
    0x8f,
    0x14,
    0x90,
    0x39,
    0x0a,
    0xe5,
    0x11,
    0xf0,
    0xa3,
    0xe5,
    0x13,
    0xf0,
    0xa3,
    0xe5,
    0x12,
    0xf0,
    0xa3,
    0xe5,
    0x14,
    0xf0,
    0xc2,
    0x20,
    0x22,
    0xab,
    0x0c,
    0xaa,
    0x0b,
    0xa9,
    0x0a,
    0xa8,
    0x09,
    0xfc,
    0xfd,
    0xfe,
    0x12,
    0x00,
    0x16,
    0x8f,
    0x0c,
    0x8e,
    0x0b,
    0x8d,
    0x0a,
    0x8c,
    0x09,
    0x7b,
    0x28,
    0xe4,
    0xfa,
    0xf9,
    0xf8,
    0x12,
    0x00,
    0xa1,
    0x8f,
    0x0c,
    0x8e,
    0x0b,
    0x8d,
    0x0a,
    0x8c,
    0x09,
    0x22,
    0xd2,
    0x19,
    0x90,
    0x30,
    0xb4,
    0xe5,
    0x23,
    0xf0,
    0x22,
    0x85,
    0x2f,
    0x82,
    0x85,
    0x2e,
    0x83,
    0xe5,
    0x43,
    0x75,
    0xf0,
    0x02,
    0x12,
    0x01,
    0x46,
    0xe4,
    0x93,
    0xfe,
    0x74,
    0x01,
    0x93,
    0xff,
    0x85,
    0x2f,
    0x82,
    0x85,
    0x2e,
    0x83,
    0xe4,
    0x93,
    0xfc,
    0x74,
    0x01,
    0x93,
    0xfd,
    0xc3,
    0xef,
    0x9d,
    0xff,
    0xee,
    0x9c,
    0x22,
    0x12,
    0x00,
    0x16,
    0x8f,
    0x3c,
    0x8e,
    0x3b,
    0x8d,
    0x3a,
    0x8c,
    0x39,
    0xaf,
    0x3c,
    0xae,
    0x3b,
    0xad,
    0x3a,
    0xac,
    0x39,
    0x22,
    0xd2,
    0x0f,
    0x90,
    0x30,
    0xb1,
    0xe5,
    0x21,
    0xf0,
    0x22,
    0xe4,
    0x85,
    0x10,
    0x0c,
    0x85,
    0x0f,
    0x0b,
    0xf5,
    0x0a,
    0xf5,
    0x09,
    0xab,
    0x0c,
    0xaa,
    0x0b,
    0xa9,
    0x0a,
    0xa8,
    0x09,
    0x22,
    0x85,
    0x2f,
    0x82,
    0x85,
    0x2e,
    0x83,
    0x22,
    0xff,
    0xe4,
    0x94,
    0x00,
    0xfe,
    0xe4,
    0xfc,
    0xfd,
    0x02,
    0x01,
    0x33,
    0xc2,
    0x07,
    0xc2,
    0x06,
    0xc2,
    0x02,
    0xc2,
    0x01,
    0xc2,
    0x00,
    0xc2,
    0x03,
    0xd2,
    0x04,
    0x22,
    0x90,
    0x33,
    0xb3,
    0xe4,
    0xf0,
    0xa3,
    0xf0,
    0xa3,
    0xf0,
    0xa3,
    0xf0,
    0x22,
    0xa3,
    0xe0,
    0xf5,
    0x22,
    0x75,
    0x32,
    0x0a,
    0x22,
    0x12,
    0x00,
    0xa1,
    0x8f,
    0x3c,
    0x8e,
    0x3b,
    0x8d,
    0x3a,
    0x8c,
    0x39,
    0x22,
    0xe4,
    0x85,
    0x0e,
    0x0c,
    0x85,
    0x0d,
    0x0b,
    0xf5,
    0x0a,
    0xf5,
    0x09,
    0x22,
    0xe4,
    0xfc,
    0xfd,
    0xfe,
    0x02,
    0x01,
    0x33,
    0x90,
    0x0b,
    0x4c,
    0xe4,
    0x93,
    0xff,
    0x90,
    0x30,
    0x0a,
    0xe0,
    0x22,
    0xc2,
    0x02,
    0xc2,
    0x01,
    0xd2,
    0x00,
    0xc2,
    0x03,
    0xc2,
    0x04,
    0x22,
    0x75,
    0xf0,
    0x02,
    0x02,
    0x01,
    0x46,
    0xe4,
    0x93,
    0xf5,
    0x43,
    0xa3,
    0xe4,
    0x93,
    0xf5,
    0x28,
    0x22,
    0xd2,
    0x02,
    0xd2,
    0x01,
    0xc2,
    0x00,
    0x22,
    0xd3,
    0xe5,
    0x3c,
    0x94,
    0xff,
    0xe5,
    0x3b,
    0x94,
    0x03,
    0x22,
    0x30,
    0x04,
    0x03,
    0x02,
    0x05,
    0xd5,
    0xd2,
    0x04,
    0xe5,
    0x45,
    0xb4,
    0x01,
    0x06,
    0x12,
    0x0c,
    0xfe,
    0x02,
    0x05,
    0xce,
    0xe5,
    0x45,
    0xb4,
    0x02,
    0x06,
    0x12,
    0x0d,
    0x0f,
    0x02,
    0x05,
    0xce,
    0xe5,
    0x45,
    0xb4,
    0x03,
    0x05,
    0xe4,
    0xf5,
    0x09,
    0x80,
    0x08,
    0xe5,
    0x45,
    0xb4,
    0x04,
    0x09,
    0x85,
    0x43,
    0x09,
    0x12,
    0x09,
    0xee,
    0x02,
    0x05,
    0xce,
    0xe5,
    0x45,
    0x64,
    0x0f,
    0x70,
    0x15,
    0x12,
    0x04,
    0xeb,
    0x40,
    0x06,
    0x7e,
    0x03,
    0x7f,
    0xff,
    0x80,
    0x04,
    0xae,
    0x3b,
    0xaf,
    0x3c,
    0x12,
    0x05,
    0xd6,
    0x02,
    0x05,
    0xce,
    0xe5,
    0x45,
    0x64,
    0x10,
    0x60,
    0x03,
    0x02,
    0x05,
    0xce,
    0xf5,
    0x39,
    0xf5,
    0x3a,
    0xf5,
    0x3b,
    0xab,
    0x3c,
    0xaa,
    0x3b,
    0xa9,
    0x3a,
    0xa8,
    0x39,
    0x12,
    0x04,
    0x10,
    0xfe,
    0xe4,
    0xfc,
    0xfd,
    0x12,
    0x04,
    0x39,
    0xe4,
    0x7b,
    0xff,
    0xfa,
    0xf9,
    0xf8,
    0x12,
    0x04,
    0x9f,
    0x12,
    0x04,
    0x6a,
    0xe4,
    0x93,
    0xfe,
    0x74,
    0x01,
    0x93,
    0xff,
    0xe4,
    0xfc,
    0xfd,
    0xe5,
    0x3c,
    0x2f,
    0xf5,
    0x3c,
    0xe5,
    0x3b,
    0x3e,
    0xf5,
    0x3b,
    0xed,
    0x35,
    0x3a,
    0xf5,
    0x3a,
    0xec,
    0x35,
    0x39,
    0xf5,
    0x39,
    0x12,
    0x04,
    0xeb,
    0x40,
    0x06,
    0x7e,
    0x03,
    0x7f,
    0xff,
    0x80,
    0x04,
    0xae,
    0x3b,
    0xaf,
    0x3c,
    0x12,
    0x05,
    0xd6,
    0xe4,
    0xf5,
    0x3a,
    0xf5,
    0x3a,
    0xe5,
    0x3a,
    0xd3,
    0x95,
    0x43,
    0x50,
    0x1c,
    0x12,
    0x04,
    0x6a,
    0xaf,
    0x3a,
    0x75,
    0xf0,
    0x02,
    0xef,
    0x12,
    0x01,
    0x46,
    0xc3,
    0x74,
    0x01,
    0x93,
    0x95,
    0x3c,
    0xe4,
    0x93,
    0x95,
    0x3b,
    0x50,
    0x04,
    0x05,
    0x3a,
    0x80,
    0xdd,
    0x85,
    0x3a,
    0x44,
    0x90,
    0x3f,
    0x01,
    0xe4,
    0xf0,
    0xd2,
    0x25,
    0x22,
    0x8e,
    0x3b,
    0x8f,
    0x3c,
    0x85,
    0x3b,
    0x37,
    0x85,
    0x3c,
    0x38,
    0xe5,
    0x3c,
    0xc4,
    0xf8,
    0x54,
    0x0f,
    0xc8,
    0x68,
    0xf5,
    0x3c,
    0xe5,
    0x3b,
    0xc4,
    0x54,
    0xf0,
    0x48,
    0xf5,
    0x3b,
    0x85,
    0x3b,
    0x0a,
    0x85,
    0x3c,
    0x0b,
    0x12,
    0x01,
    0x78,
    0x22,
    0xe5,
    0x29,
    0x70,
    0x03,
    0x02,
    0x06,
    0xd4,
    0xc2,
    0xaf,
    0xaf,
    0x29,
    0xe4,
    0xf5,
    0x29,
    0xd2,
    0xaf,
    0x90,
    0x3f,
    0x01,
    0xe0,
    0xf5,
    0x45,
    0xa3,
    0xe0,
    0xf5,
    0x39,
    0xa3,
    0xe0,
    0xf5,
    0x3a,
    0xa3,
    0xe0,
    0xf5,
    0x3b,
    0xa3,
    0xe0,
    0xf5,
    0x3c,
    0xef,
    0x12,
    0x01,
    0x52,
    0x06,
    0x4b,
    0x03,
    0x06,
    0x5a,
    0x05,
    0x06,
    0x81,
    0x06,
    0x06,
    0x6f,
    0x08,
    0x06,
    0xa2,
    0x09,
    0x06,
    0x8e,
    0x10,
    0x06,
    0xa2,
    0x12,
    0x06,
    0xa7,
    0x20,
    0x06,
    0xb5,
    0x21,
    0x06,
    0xba,
    0x30,
    0x06,
    0xc5,
    0xd0,
    0x00,
    0x00,
    0x06,
    0xc9,
    0x30,
    0x05,
    0x7b,
    0x20,
    0x00,
    0x78,
    0xd2,
    0x07,
    0xc2,
    0x06,
    0x12,
    0x04,
    0xc9,
    0x80,
    0x21,
    0x30,
    0x05,
    0x6c,
    0x20,
    0x00,
    0x69,
    0xc2,
    0x07,
    0xd2,
    0x06,
    0xc2,
    0x03,
    0x12,
    0x04,
    0xe4,
    0xc2,
    0x04,
    0xc2,
    0x21,
    0x80,
    0x5a,
    0x12,
    0x04,
    0x7c,
    0x30,
    0x05,
    0x06,
    0xe4,
    0xf5,
    0x09,
    0x12,
    0x09,
    0xee,
    0xc2,
    0x21,
    0xd2,
    0x25,
    0x80,
    0x48,
    0x30,
    0x07,
    0x45,
    0x30,
    0x06,
    0x42,
    0x12,
    0x04,
    0xc9,
    0xd2,
    0x21,
    0x80,
    0x3b,
    0x20,
    0x07,
    0x03,
    0x30,
    0x06,
    0x09,
    0xe5,
    0x45,
    0x64,
    0x0e,
    0x70,
    0x2f,
    0x20,
    0x00,
    0x2c,
    0x12,
    0x06,
    0xd5,
    0x80,
    0x27,
    0x12,
    0x02,
    0xc4,
    0x80,
    0x22,
    0x30,
    0x05,
    0x1f,
    0x20,
    0x07,
    0x1c,
    0x20,
    0x06,
    0x19,
    0x12,
    0x0c,
    0xa6,
    0x80,
    0x14,
    0x12,
    0x09,
    0x85,
    0x80,
    0x0f,
    0x20,
    0x07,
    0x0c,
    0x20,
    0x06,
    0x09,
    0x12,
    0x0d,
    0x3c,
    0x80,
    0x04,
    0xd2,
    0x26,
    0xc2,
    0x26,
    0x20,
    0x07,
    0x03,
    0x20,
    0x06,
    0x05,
    0x90,
    0x3f,
    0x01,
    0xe4,
    0xf0,
    0x22,
    0xe5,
    0x45,
    0x24,
    0xfe,
    0x60,
    0x19,
    0x14,
    0x60,
    0x2c,
    0x24,
    0x02,
    0x60,
    0x03,
    0x02,
    0x07,
    0xa8,
    0xe5,
    0x3c,
    0xd3,
    0x94,
    0x03,
    0x40,
    0x03,
    0x75,
    0x3c,
    0x03,
    0xe4,
    0xf5,
    0x09,
    0x80,
    0x0d,
    0xe5,
    0x3c,
    0xd3,
    0x94,
    0x05,
    0x40,
    0x03,
    0x75,
    0x3c,
    0x05,
    0x75,
    0x09,
    0x01,
    0x85,
    0x3c,
    0x0a,
    0x12,
    0x08,
    0xfa,
    0xd2,
    0x20,
    0x22,
    0xe5,
    0x39,
    0xd3,
    0x94,
    0x28,
    0x40,
    0x04,
    0x7f,
    0x28,
    0x80,
    0x02,
    0xaf,
    0x39,
    0x8f,
    0x39,
    0xe5,
    0x3a,
    0xd3,
    0x94,
    0x1e,
    0x40,
    0x04,
    0x7f,
    0x1e,
    0x80,
    0x02,
    0xaf,
    0x3a,
    0x8f,
    0x3a,
    0xe5,
    0x3b,
    0xd3,
    0x94,
    0x28,
    0x40,
    0x04,
    0x7f,
    0x28,
    0x80,
    0x02,
    0xaf,
    0x3b,
    0x8f,
    0x3b,
    0xe5,
    0x3c,
    0xd3,
    0x94,
    0x1e,
    0x40,
    0x04,
    0x7f,
    0x1e,
    0x80,
    0x02,
    0xaf,
    0x3c,
    0x8f,
    0x3c,
    0xaf,
    0x3a,
    0x78,
    0x10,
    0x12,
    0x04,
    0xb7,
    0xc0,
    0x04,
    0xc0,
    0x05,
    0xc0,
    0x06,
    0xc0,
    0x07,
    0xaf,
    0x39,
    0x78,
    0x18,
    0x12,
    0x04,
    0xb7,
    0xd0,
    0x03,
    0xd0,
    0x02,
    0xd0,
    0x01,
    0xd0,
    0x00,
    0xef,
    0x4b,
    0xff,
    0xee,
    0x4a,
    0xfe,
    0xed,
    0x49,
    0xfd,
    0xec,
    0x48,
    0xfc,
    0xc0,
    0x04,
    0xc0,
    0x05,
    0xc0,
    0x06,
    0xc0,
    0x07,
    0xaf,
    0x3b,
    0xe4,
    0xfc,
    0xfd,
    0xfe,
    0x78,
    0x08,
    0x12,
    0x01,
    0x33,
    0xd0,
    0x03,
    0xd0,
    0x02,
    0xd0,
    0x01,
    0xd0,
    0x00,
    0xef,
    0x4b,
    0xfb,
    0xee,
    0x4a,
    0xfa,
    0xed,
    0x49,
    0xf9,
    0xec,
    0x48,
    0xf8,
    0xaf,
    0x3c,
    0xeb,
    0x4f,
    0xf5,
    0x2d,
    0xea,
    0xf5,
    0x2c,
    0xe9,
    0xf5,
    0x2b,
    0xe8,
    0xf5,
    0x2a,
    0xd2,
    0x20,
    0x22,
    0xc0,
    0xe0,
    0xc0,
    0x83,
    0xc0,
    0x82,
    0xc0,
    0xd0,
    0x75,
    0xd0,
    0x00,
    0xc0,
    0x00,
    0xc0,
    0x06,
    0xc0,
    0x07,
    0x90,
    0x37,
    0x08,
    0xe0,
    0xf5,
    0x08,
    0xe5,
    0x08,
    0x30,
    0xe3,
    0x23,
    0x20,
    0x26,
    0x20,
    0x90,
    0x3a,
    0x00,
    0x74,
    0x81,
    0xf0,
    0x90,
    0x3a,
    0x03,
    0xe0,
    0xf5,
    0x3e,
    0xe0,
    0xf5,
    0x3d,
    0x90,
    0x3a,
    0x00,
    0x74,
    0x85,
    0xf0,
    0x90,
    0x3a,
    0x03,
    0xe0,
    0xf5,
    0x40,
    0xe0,
    0xf5,
    0x3f,
    0xd2,
    0x2f,
    0xe5,
    0x08,
    0x30,
    0xe5,
    0x56,
    0x90,
    0x30,
    0x1b,
    0xe0,
    0xf5,
    0x42,
    0xe5,
    0x41,
    0x24,
    0x02,
    0xff,
    0xe4,
    0x33,
    0xfe,
    0xc3,
    0xef,
    0x95,
    0x42,
    0x74,
    0x80,
    0xf8,
    0x6e,
    0x98,
    0x50,
    0x02,
    0x80,
    0x01,
    0xc3,
    0x92,
    0x24,
    0xe5,
    0x42,
    0x24,
    0x02,
    0xff,
    0xe4,
    0x33,
    0xfe,
    0xc3,
    0xef,
    0x95,
    0x41,
    0x74,
    0x80,
    0xf8,
    0x6e,
    0x98,
    0x50,
    0x02,
    0x80,
    0x02,
    0xa2,
    0x24,
    0x92,
    0x24,
    0x30,
    0x24,
    0x04,
    0xaf,
    0x42,
    0x80,
    0x02,
    0xaf,
    0x41,
    0x8f,
    0x41,
    0x30,
    0x27,
    0x11,
    0x90,
    0x33,
    0x00,
    0xe0,
    0x30,
    0x29,
    0x05,
    0x44,
    0x40,
    0xf0,
    0x80,
    0x03,
    0x54,
    0xbf,
    0xf0,
    0xc2,
    0x27,
    0xe5,
    0x08,
    0x30,
    0xe1,
    0x08,
    0x90,
    0x3f,
    0x00,
    0xe0,
    0xf5,
    0x29,
    0xe4,
    0xf0,
    0x90,
    0x37,
    0x08,
    0xe5,
    0x08,
    0xf0,
    0xd0,
    0x07,
    0xd0,
    0x06,
    0xd0,
    0x00,
    0xd0,
    0xd0,
    0xd0,
    0x82,
    0xd0,
    0x83,
    0xd0,
    0xe0,
    0x32,
    0x12,
    0x04,
    0xbe,
    0xb5,
    0x07,
    0x03,
    0xd3,
    0x80,
    0x01,
    0xc3,
    0x40,
    0x03,
    0x02,
    0x08,
    0xf9,
    0x90,
    0x31,
    0x00,
    0xe0,
    0x54,
    0xfe,
    0xf0,
    0xe0,
    0x54,
    0xfd,
    0xf0,
    0xa3,
    0xe4,
    0xf0,
    0x90,
    0x33,
    0x00,
    0xe0,
    0x54,
    0xbf,
    0xf0,
    0x12,
    0x04,
    0x8b,
    0x90,
    0x33,
    0xb0,
    0xf0,
    0xa3,
    0xf0,
    0xa3,
    0xf0,
    0x90,
    0x30,
    0xb2,
    0xe0,
    0x44,
    0x08,
    0xf0,
    0x90,
    0x30,
    0xb0,
    0xe0,
    0x44,
    0x01,
    0xf0,
    0xa3,
    0xe0,
    0x44,
    0x0c,
    0xf0,
    0x90,
    0x30,
    0xb4,
    0xe0,
    0x44,
    0x07,
    0xf0,
    0xe0,
    0xf5,
    0x23,
    0x90,
    0x30,
    0xb1,
    0xe0,
    0xf5,
    0x21,
    0x90,
    0x39,
    0x01,
    0x74,
    0x35,
    0xf0,
    0x90,
    0x39,
    0x00,
    0x74,
    0x20,
    0xf0,
    0x90,
    0x37,
    0x00,
    0x74,
    0xff,
    0xf0,
    0xa3,
    0xf0,
    0x90,
    0x37,
    0x00,
    0xe0,
    0x54,
    0xf7,
    0xf0,
    0xe0,
    0x54,
    0xdf,
    0xf0,
    0x90,
    0x31,
    0x0f,
    0x74,
    0x3f,
    0xf0,
    0xa3,
    0xe4,
    0xf0,
    0xa3,
    0x74,
    0x3f,
    0xf0,
    0xa3,
    0x74,
    0x01,
    0xf0,
    0x90,
    0x37,
    0x00,
    0xe0,
    0x54,
    0xfd,
    0xf0,
    0x90,
    0x37,
    0x08,
    0x74,
    0xff,
    0xf0,
    0xa3,
    0xf0,
    0x75,
    0xa8,
    0x01,
    0x22,
    0xe5,
    0x0a,
    0x25,
    0xe0,
    0x25,
    0xe0,
    0xf5,
    0x0b,
    0xe5,
    0x09,
    0x60,
    0x09,
    0xe5,
    0x0a,
    0x75,
    0xf0,
    0x03,
    0xa4,
    0xff,
    0x80,
    0x02,
    0xaf,
    0x0b,
    0x8f,
    0x0c,
    0xc3,
    0x74,
    0x0f,
    0x9f,
    0x78,
    0x10,
    0x12,
    0x04,
    0x71,
    0xc0,
    0x04,
    0xc0,
    0x05,
    0xc0,
    0x06,
    0xc0,
    0x07,
    0xc3,
    0x74,
    0x14,
    0x95,
    0x0b,
    0x78,
    0x18,
    0x12,
    0x04,
    0x71,
    0xd0,
    0x03,
    0xd0,
    0x02,
    0xd0,
    0x01,
    0xd0,
    0x00,
    0xef,
    0x4b,
    0xff,
    0xee,
    0x4a,
    0xfe,
    0xed,
    0x49,
    0xfd,
    0xec,
    0x48,
    0xfc,
    0xc0,
    0x04,
    0xc0,
    0x05,
    0xc0,
    0x06,
    0xc0,
    0x07,
    0xe5,
    0x0b,
    0x24,
    0x14,
    0xff,
    0xe4,
    0x33,
    0xfe,
    0xe4,
    0xfc,
    0xfd,
    0x78,
    0x08,
    0x12,
    0x01,
    0x33,
    0xd0,
    0x03,
    0xd0,
    0x02,
    0xd0,
    0x01,
    0xd0,
    0x00,
    0xef,
    0x4b,
    0xfb,
    0xee,
    0x4a,
    0xfa,
    0xed,
    0x49,
    0xf9,
    0xec,
    0x48,
    0xf8,
    0xe5,
    0x0c,
    0x24,
    0x0f,
    0xff,
    0xe4,
    0x33,
    0xfe,
    0xeb,
    0x4f,
    0xf5,
    0x2d,
    0xea,
    0x4e,
    0xf5,
    0x2c,
    0xe9,
    0xf5,
    0x2b,
    0xe8,
    0xf5,
    0x2a,
    0x22,
    0xe5,
    0x45,
    0x64,
    0x01,
    0x70,
    0x50,
    0x12,
    0x04,
    0x6a,
    0xe5,
    0x44,
    0x12,
    0x04,
    0x18,
    0xfe,
    0xe4,
    0x8f,
    0x3c,
    0x8e,
    0x3b,
    0xf5,
    0x3a,
    0xf5,
    0x39,
    0x12,
    0x04,
    0x44,
    0x7b,
    0xff,
    0xfa,
    0xf9,
    0xf8,
    0x12,
    0x04,
    0x39,
    0xc0,
    0x04,
    0xc0,
    0x05,
    0xc0,
    0x06,
    0xc0,
    0x07,
    0x12,
    0x04,
    0x10,
    0xab,
    0x07,
    0xfa,
    0xe4,
    0xf9,
    0xf8,
    0xd0,
    0x07,
    0xd0,
    0x06,
    0xd0,
    0x05,
    0xd0,
    0x04,
    0x12,
    0x04,
    0x9f,
    0x85,
    0x3c,
    0x39,
    0x85,
    0x44,
    0x3a,
    0x12,
    0x04,
    0x6a,
    0xe5,
    0x44,
    0x12,
    0x04,
    0xd4,
    0xe4,
    0x93,
    0xf5,
    0x3b,
    0x74,
    0x01,
    0x93,
    0xf5,
    0x3c,
    0x90,
    0x3f,
    0x02,
    0xe5,
    0x39,
    0xf0,
    0xa3,
    0xe5,
    0x3a,
    0xf0,
    0xa3,
    0xe5,
    0x3b,
    0xf0,
    0xa3,
    0xe5,
    0x3c,
    0xf0,
    0x22,
    0xe5,
    0x09,
    0xd3,
    0x95,
    0x43,
    0x40,
    0x01,
    0x22,
    0x12,
    0x04,
    0x6a,
    0xe5,
    0x09,
    0x12,
    0x04,
    0xd4,
    0xe4,
    0x93,
    0xfe,
    0x74,
    0x01,
    0x93,
    0xff,
    0x4e,
    0x60,
    0x21,
    0x8e,
    0x37,
    0x8f,
    0x38,
    0xef,
    0xc4,
    0xf8,
    0x54,
    0x0f,
    0xc8,
    0x68,
    0xff,
    0xee,
    0xc4,
    0x54,
    0xf0,
    0x48,
    0xfe,
    0x43,
    0x07,
    0x0d,
    0x8e,
    0x0a,
    0x8f,
    0x0b,
    0x12,
    0x01,
    0x78,
    0x30,
    0x23,
    0x22,
    0xc3,
    0x22,
    0x75,
    0x0a,
    0x00,
    0x75,
    0x0b,
    0x0d,
    0x12,
    0x01,
    0x78,
    0x30,
    0x23,
    0x02,
    0xc3,
    0x22,
    0x75,
    0x0a,
    0x00,
    0x75,
    0x0b,
    0x64,
    0x12,
    0x0c,
    0x67,
    0x75,
    0x0a,
    0x80,
    0x75,
    0x0b,
    0x00,
    0x12,
    0x01,
    0x78,
    0x85,
    0x09,
    0x44,
    0xd3,
    0x22,
    0xc2,
    0x25,
    0x20,
    0x05,
    0x05,
    0x75,
    0x09,
    0xee,
    0x80,
    0x36,
    0x20,
    0x07,
    0x08,
    0x20,
    0x06,
    0x05,
    0xe4,
    0xf5,
    0x09,
    0x80,
    0x2b,
    0x20,
    0x07,
    0x08,
    0x30,
    0x06,
    0x05,
    0x75,
    0x09,
    0x20,
    0x80,
    0x20,
    0x30,
    0x00,
    0x05,
    0x75,
    0x09,
    0x01,
    0x80,
    0x18,
    0xe5,
    0x20,
    0x54,
    0x07,
    0xff,
    0xbf,
    0x06,
    0x0d,
    0x30,
    0x21,
    0x04,
    0x7f,
    0x12,
    0x80,
    0x02,
    0x7f,
    0x02,
    0x8f,
    0x09,
    0x80,
    0x03,
    0x75,
    0x09,
    0xfe,
    0x90,
    0x3f,
    0x07,
    0xe5,
    0x09,
    0xf0,
    0x90,
    0x3f,
    0x06,
    0xe5,
    0x44,
    0xf0,
    0x22,
    0x85,
    0x0d,
    0x0e,
    0x7f,
    0x08,
    0xe5,
    0x0e,
    0x30,
    0xe7,
    0x04,
    0xd2,
    0x19,
    0x80,
    0x02,
    0xc2,
    0x19,
    0x12,
    0x04,
    0x09,
    0x75,
    0x32,
    0x0a,
    0xae,
    0x32,
    0x15,
    0x32,
    0xee,
    0x70,
    0xf9,
    0xe5,
    0x0e,
    0x25,
    0xe0,
    0xf5,
    0x0e,
    0xd2,
    0x18,
    0x12,
    0x04,
    0x09,
    0x75,
    0x32,
    0x0a,
    0xae,
    0x32,
    0x15,
    0x32,
    0xee,
    0x70,
    0xf9,
    0xc2,
    0x18,
    0x12,
    0x04,
    0x09,
    0x75,
    0x32,
    0x05,
    0xae,
    0x32,
    0x15,
    0x32,
    0xee,
    0x70,
    0xf9,
    0xdf,
    0xc2,
    0x22,
    0x90,
    0x3f,
    0x07,
    0x74,
    0xfa,
    0xf0,
    0x12,
    0x08,
    0x65,
    0x12,
    0x0b,
    0xfb,
    0xe4,
    0xf5,
    0x29,
    0xd2,
    0xaf,
    0x12,
    0x05,
    0xfc,
    0x30,
    0x20,
    0x03,
    0x12,
    0x02,
    0xc4,
    0x30,
    0x25,
    0x03,
    0x12,
    0x0a,
    0x4e,
    0x30,
    0x2f,
    0xee,
    0xc2,
    0x2f,
    0xd2,
    0x26,
    0x30,
    0x00,
    0x05,
    0x12,
    0x0b,
    0x7b,
    0x80,
    0x09,
    0x20,
    0x07,
    0x06,
    0x30,
    0x06,
    0x03,
    0x12,
    0x04,
    0xf5,
    0xc2,
    0x26,
    0x80,
    0xd5,
    0xe5,
    0x44,
    0x70,
    0x19,
    0x12,
    0x0d,
    0x4b,
    0xc2,
    0x30,
    0x12,
    0x0b,
    0xd2,
    0xc2,
    0x30,
    0x12,
    0x0c,
    0x89,
    0xc2,
    0x03,
    0x12,
    0x0c,
    0xfe,
    0xd2,
    0x02,
    0xd2,
    0x01,
    0xd2,
    0x00,
    0x22,
    0x30,
    0x03,
    0x08,
    0xc2,
    0x03,
    0xc2,
    0x04,
    0x12,
    0x04,
    0xe4,
    0x22,
    0xe4,
    0xf5,
    0x09,
    0x12,
    0x09,
    0xee,
    0xd2,
    0x03,
    0x22,
    0x36,
    0x0c,
    0x04,
    0x00,
    0x00,
    0x00,
    0xc8,
    0x01,
    0x2c,
    0x01,
    0x5e,
    0x01,
    0x8b,
    0x01,
    0xb8,
    0x01,
    0xe5,
    0x02,
    0x12,
    0x02,
    0x3f,
    0x02,
    0x6c,
    0x02,
    0x99,
    0x02,
    0xc6,
    0x02,
    0xf3,
    0x07,
    0x00,
    0x02,
    0x4e,
    0x02,
    0x6c,
    0x02,
    0x8a,
    0x02,
    0xa8,
    0x02,
    0xc6,
    0x02,
    0xe4,
    0x03,
    0x02,
    0x03,
    0x20,
    0xe5,
    0x20,
    0x54,
    0x07,
    0xff,
    0xbf,
    0x01,
    0x03,
    0x02,
    0x0b,
    0x1b,
    0xe5,
    0x20,
    0x54,
    0x07,
    0xff,
    0xbf,
    0x07,
    0x03,
    0x02,
    0x0c,
    0x44,
    0xe5,
    0x20,
    0x54,
    0x07,
    0xff,
    0xbf,
    0x03,
    0x03,
    0x02,
    0x0b,
    0xa8,
    0xe5,
    0x20,
    0x54,
    0x07,
    0xff,
    0xbf,
    0x05,
    0x03,
    0x12,
    0x0d,
    0x59,
    0x22,
    0x12,
    0x0c,
    0x21,
    0xd2,
    0x30,
    0x12,
    0x0b,
    0xd2,
    0xd2,
    0x30,
    0x12,
    0x0c,
    0x89,
    0xe5,
    0x43,
    0xd3,
    0x95,
    0x44,
    0x40,
    0x03,
    0xd3,
    0x80,
    0x01,
    0xc3,
    0x50,
    0x0c,
    0x20,
    0x28,
    0x06,
    0x30,
    0x2d,
    0x03,
    0x20,
    0x2e,
    0x03,
    0x02,
    0x0c,
    0xfe,
    0x12,
    0x0c,
    0xbf,
    0x22,
    0x30,
    0x30,
    0x09,
    0x30,
    0x2d,
    0x06,
    0xae,
    0x33,
    0xaf,
    0x34,
    0x80,
    0x04,
    0xae,
    0x3d,
    0xaf,
    0x3e,
    0x8e,
    0x33,
    0x8f,
    0x34,
    0x30,
    0x30,
    0x09,
    0x30,
    0x2e,
    0x06,
    0xae,
    0x35,
    0xaf,
    0x36,
    0x80,
    0x04,
    0xae,
    0x3f,
    0xaf,
    0x40,
    0x8e,
    0x35,
    0x8f,
    0x36,
    0x22,
    0x12,
    0x0c,
    0xec,
    0x12,
    0x0d,
    0x67,
    0x50,
    0x04,
    0xd2,
    0x05,
    0x80,
    0x02,
    0xc2,
    0x05,
    0x12,
    0x04,
    0x7c,
    0xc2,
    0x28,
    0xc2,
    0x21,
    0xd2,
    0x25,
    0x12,
    0x04,
    0xbe,
    0xb5,
    0x07,
    0x03,
    0xd3,
    0x80,
    0x01,
    0xc3,
    0x40,
    0x02,
    0xc2,
    0x05,
    0x22,
    0xd3,
    0xe5,
    0x34,
    0x95,
    0x3e,
    0xe5,
    0x33,
    0x95,
    0x3d,
    0x40,
    0x03,
    0xd3,
    0x80,
    0x01,
    0xc3,
    0x92,
    0x2d,
    0xd3,
    0xe5,
    0x36,
    0x95,
    0x40,
    0xe5,
    0x35,
    0x95,
    0x3f,
    0x40,
    0x03,
    0xd3,
    0x80,
    0x01,
    0xc3,
    0x92,
    0x2e,
    0x22,
    0x12,
    0x0c,
    0x21,
    0xd2,
    0x30,
    0x12,
    0x0b,
    0xd2,
    0xd2,
    0x30,
    0x12,
    0x0c,
    0x89,
    0x12,
    0x0c,
    0xfe,
    0xe5,
    0x28,
    0xd3,
    0x95,
    0x44,
    0x40,
    0x05,
    0xe4,
    0x95,
    0x44,
    0x40,
    0x06,
    0xc2,
    0x02,
    0xd2,
    0x01,
    0xd2,
    0x00,
    0x22,
    0xe4,
    0xff,
    0xfe,
    0xc3,
    0xef,
    0x95,
    0x0b,
    0xee,
    0x95,
    0x0a,
    0x50,
    0x15,
    0x7d,
    0x8a,
    0x7c,
    0x02,
    0xed,
    0x1d,
    0xaa,
    0x04,
    0x70,
    0x01,
    0x1c,
    0x4a,
    0x70,
    0xf6,
    0x0f,
    0xbf,
    0x00,
    0x01,
    0x0e,
    0x80,
    0xe2,
    0x22,
    0x30,
    0x30,
    0x07,
    0x30,
    0x2d,
    0x04,
    0xaf,
    0x30,
    0x80,
    0x02,
    0xaf,
    0x44,
    0x8f,
    0x30,
    0x30,
    0x30,
    0x07,
    0x30,
    0x2e,
    0x04,
    0xaf,
    0x31,
    0x80,
    0x02,
    0xaf,
    0x44,
    0x8f,
    0x31,
    0x22,
    0xe5,
    0x45,
    0xb4,
    0x01,
    0x05,
    0x12,
    0x0d,
    0x1e,
    0x80,
    0x08,
    0xe5,
    0x45,
    0xb4,
    0x02,
    0x09,
    0x12,
    0x0d,
    0x2d,
    0xe4,
    0xf5,
    0x09,
    0x12,
    0x09,
    0xee,
    0x22,
    0xaf,
    0x31,
    0xe5,
    0x44,
    0xb5,
    0x07,
    0x03,
    0x02,
    0x0c,
    0xd8,
    0x8f,
    0x09,
    0x12,
    0x09,
    0xee,
    0xd2,
    0x02,
    0xc2,
    0x01,
    0xd2,
    0x00,
    0x75,
    0x27,
    0x03,
    0x22,
    0xc2,
    0x03,
    0xd2,
    0x04,
    0x12,
    0x04,
    0xe4,
    0xc2,
    0x30,
    0x12,
    0x0b,
    0xd2,
    0xc2,
    0x30,
    0x12,
    0x0c,
    0x89,
    0xd2,
    0x25,
    0x22,
    0x12,
    0x04,
    0x8b,
    0xf5,
    0x09,
    0x75,
    0x0a,
    0x01,
    0x12,
    0x08,
    0xfa,
    0xd2,
    0x20,
    0xc2,
    0x26,
    0xc2,
    0x2f,
    0x22,
    0xe5,
    0x44,
    0xc3,
    0x95,
    0x43,
    0x40,
    0x01,
    0x22,
    0xe5,
    0x44,
    0x04,
    0xf5,
    0x09,
    0x12,
    0x09,
    0xee,
    0x22,
    0xe5,
    0x44,
    0x70,
    0x02,
    0xc3,
    0x22,
    0xe5,
    0x44,
    0x14,
    0xf5,
    0x09,
    0x12,
    0x09,
    0xee,
    0x22,
    0x75,
    0x2e,
    0x0b,
    0x75,
    0x2f,
    0x4f,
    0x90,
    0x0b,
    0x4d,
    0x12,
    0x04,
    0xda,
    0xc2,
    0x2c,
    0x22,
    0x75,
    0x2e,
    0x0b,
    0x75,
    0x2f,
    0x6b,
    0x90,
    0x0b,
    0x69,
    0x12,
    0x04,
    0xda,
    0xd2,
    0x2c,
    0x22,
    0xe5,
    0x45,
    0x24,
    0xfe,
    0x60,
    0x06,
    0x04,
    0x70,
    0x05,
    0xd2,
    0x28,
    0x22,
    0xc2,
    0x28,
    0x22,
    0xe4,
    0xf5,
    0x33,
    0xf5,
    0x34,
    0xf5,
    0x35,
    0xf5,
    0x36,
    0xc2,
    0x2d,
    0xc2,
    0x2e,
    0x22,
    0xe5,
    0x27,
    0xd3,
    0x94,
    0x00,
    0x40,
    0x03,
    0x15,
    0x27,
    0x22,
    0x12,
    0x0c,
    0xd8,
    0x22,
    0x12,
    0x0d,
    0x1e,
    0xe4,
    0xf5,
    0x09,
    0x12,
    0x09,
    0xee,
    0x22,
};

static UINT32 OV3640_FOCUS_AD5820_Init(void)
{
    

    UINT8 state;
    UINT32 iteration;
    int flag1;
    u8 buf[1024];
    int totalCnt = 0; 
    int sentCnt = 0; 
    int index=0; 
    u16 addr = 0x8000; 
    //u8 buf[256]; 
    int len = 252;
    
    //int transfer_len, transac_len=3;
    //u8 buf[1024];
    //int count=0;
    //UINT32 logtime[10], i2c_1[10],i2c_2[10];

    AF_INIT = FALSE;
    SENSORDB("OV3640_FOCUS_AD5820_Init E\n");
    KD_IMGSENSOR_PROFILE_INIT();

    OV3640_write_cmos_sensor(0x308c,0x00);
    OV3640_write_cmos_sensor(0x3104,0x02);
    OV3640_write_cmos_sensor(0x3105,0xff);
    OV3640_write_cmos_sensor(0x3106,0x00);
    OV3640_write_cmos_sensor(0x3107,0xff);    
    
    SENSORDB("[OV3640 YUV AF] multi write w/o addr start \n");
    totalCnt = ARRAY_SIZE(AD5820_Config); 
    //yokolen = 128;

    while (index < totalCnt) {        
        sentCnt = totalCnt - index > len  ? len  : totalCnt - index; 
        
        buf[0] = addr >> 8; 
        buf[1] = addr & 0xff; 
        memcpy(&buf[2], &AD5820_Config[index], len );
        OV3640_multi_write_cmos_sensor(buf, sentCnt + 2); 
        addr += len ; 
        index += len ;
    }
    SENSORDB("[OV3640 YUV AF] multi write w/o addr end \n");    
    
    OV3640_write_cmos_sensor(0x3F00,0x00);
    OV3640_write_cmos_sensor(0x3F01,0x00);
    OV3640_write_cmos_sensor(0x3F02,0x00);
    OV3640_write_cmos_sensor(0x3F03,0x00);
    OV3640_write_cmos_sensor(0x3F04,0x00);
    OV3640_write_cmos_sensor(0x3F05,0x00);
    OV3640_write_cmos_sensor(0x3F06,0x00);
    OV3640_write_cmos_sensor(0x3F07,0xFF);
    OV3640_write_cmos_sensor(0x3104,0x00);  

    flag1 = 0;
    
    /////////////////////////first time check 0xfa/////////////////////
    iteration = 100;     
    do{
        Sleep(1);    
        state = (UINT8)OV3640_read_cmos_sensor(0x3F07);
        printk("waiting S_STARTUP 0xfa STA_FOCUS = 0x%0x\n", state);
        if(iteration-- == 0)
        {
            printk("waiting S_STARTUP 0xfa STA_FOCUS out of time!!!! %x\n",state);        
            flag1 = 1; 
            break;
        }
    
    }while(state != 0xfa);//CMD_TAG
        
 
    iteration = 100;     
    do{
        state = (UINT8)OV3640_read_cmos_sensor(0x3F07);
        printk("waiting S_IDLE 0x00 STA_FOCUS = 0x%0x\n", state);
        Sleep(1);
        if(iteration-- == 0)
        {
            printk("waiting S_IDLE 0x00 STA_FOCUS out of time!!!! %x\n",state);        
            break;
        }
    
    }while(state != 0x00);//CMD_TAG    
    
    KD_IMGSENSOR_PROFILE("OV3640_FOCUS_AD5820_Init");   
    
    //SENSORDB("waiting S_IDLE 0x00 cost %d ms\n", 100 - iteration);
    STA_FOCUS = (UINT8)OV3640_read_cmos_sensor(0x3F07);
    
    if(!flag1){
        AF_INIT = TRUE;
        return 0;
    }else{        
        return 1;
    }
 
}    

static void OV3640_FOCUS_AD5820_Get_AF_Status(UINT32 *pFeatureReturnPara32)
{
    UINT32 state = OV3640_read_cmos_sensor(0x3f07);
	
    if(!AF_INIT)//download af firmware fail, always return ERROR state
    {
        *pFeatureReturnPara32 = SENSOR_AF_ERROR;
        SENSORDB("get AF status fail, 0x3F07 = 0x%x, *pFeatureReturnPara32 = %d\n",  state, *pFeatureReturnPara32);		
        return;
    }
    else
    {

        if(0x00 == state)
        {
            *pFeatureReturnPara32 = SENSOR_AF_IDLE;
    	
        }
        else if(0x01 == state)
        {
            *pFeatureReturnPara32 = SENSOR_AF_FOCUSING;
    		
        }
        else if(0x02 == state)
        {
            *pFeatureReturnPara32 = SENSOR_AF_FOCUSED;
    			
        }
        else
        {
            *pFeatureReturnPara32 = SENSOR_AF_ERROR;
    			   
        }
        //SENSORDB("lln 0x3F07 = 0x%x, *pFeatureReturnPara32 = %d\n",  state, *pFeatureReturnPara32);	
    }

}			

static void OV3640_FOCUS_AD5820_Get_AF_Inf(UINT32 *pFeatureReturnPara32)
{
    *pFeatureReturnPara32 = 0;
}

static void OV3640_FOCUS_AD5820_Get_AF_Macro(UINT32 *pFeatureReturnPara32)
{
    *pFeatureReturnPara32 = 255;
}


//update focus window
//@input zone[] addr
static void OV3640_FOCUS_AD5820_Set_AF_Window(UINT32 zone_addr)
{//update global zone

    if(!AF_INIT)
    {
        return;
    }
    else
    {
        //UINT8 state = 0x8F;
    
        //input:
        UINT32 times = 1;
        UINT32 FD_XS = 4;
        UINT32 FD_YS = 3;	
        UINT32 x0, y0, x1, y1;
        UINT32* zone = (UINT32*)zone_addr;
        x0 = *zone;
        y0 = *(zone + 1);
        x1 = *(zone + 2);
        y1 = *(zone + 3);	
        FD_XS = *(zone + 4);
        FD_YS = *(zone + 5);
        times = FD_XS / AF_XS;
        SENSORDB("x0=%d,y0=%d,x1=%d,y1=%d,FD_XS=%d,FD_YS=%d\n",
    		x0, y0, x1, y1, FD_XS, FD_YS);	
    	
        x0 = (UINT8)(x0 / times);   
        y0 = (UINT8)(y0 / times);   
        x1 = (UINT8)(x1 / times);   
        y1 = (UINT8)(y1 / times);   	
    
    
    //zone changed, update global zone.
        ZONE[0]=x0;
        ZONE[1]=y0;
        ZONE[2]=x1;
        ZONE[3]=y1;
    
    }
}

//set focus zone coordinates to sensor IC
//global zone[]
static void OV3640_FOCUS_AD5820_Set_AF_Window_to_IC(void)
{
    //UINT8 x_center, y_center;
    UINT8 state = 0x8F;    
    UINT32 iteration = 100;  
    //UINT8 cmd_reg = 0x8F;
	
    // custom mode
    OV3640_write_cmos_sensor(0x3F01,0x03);//CMD_TAG
    OV3640_write_cmos_sensor(0x3F02,ZONE[0]);//CMD_PARA3
    OV3640_write_cmos_sensor(0x3F03,ZONE[1]);//CMD_PARA2
    OV3640_write_cmos_sensor(0x3F04,ZONE[2]);//CMD_PARA1
    OV3640_write_cmos_sensor(0x3F05,ZONE[3]);//CMD_PARA0
    OV3640_write_cmos_sensor(0x3F00,0x10);//CMD_MAIN
    
    do{
        state = (UINT8)OV3640_read_cmos_sensor(0x3F01);
            SENSORDB("custom mode CMD_TAG= 0x%0x\n",state);
            Sleep(1);
            if(iteration-- == 0)
            {
                SENSORDB("float center mode wait CMD_TAG zero out of time!!!! %x\n",state);
                break;
            }

    }while(state != 0x00);	
	
    return;

}
#if 0
//0x390a---0x390d are focus window register, but there are some differences with input
static void OV3640_FOCUS_AD5820_Read_Zone()
{
    if(!AF_INIT)
    {
        return;
    }
    else
    {

        UINT8 X0,Y0,X1,Y1;
        X0 = (UINT8)OV3640_read_cmos_sensor(0x390a);
        Y0 = (UINT8)OV3640_read_cmos_sensor(0x390b);
        X0 = (UINT8)OV3640_read_cmos_sensor(0x390c);
        Y0 = (UINT8)OV3640_read_cmos_sensor(0x390d);
        SENSORDB("zone x0=0x%x, y0=0x%x, x1=0x%x, y1=0x%x\n",X0,Y0,X1,Y1);

        
    }
}

#endif
//update zone[]
static void OV3640_FOCUS_AD5820_Update_Zone(void)
{
    UINT8 state = 0x8F;    
    UINT32 iteration = 100;  
    //UINT8 cmd_reg = 0x8F;
    //send update zone cmd to firmware
    OV3640_write_cmos_sensor(0x3F01,0x01);//cmd_tag
    OV3640_write_cmos_sensor(0x3F00,0x12);

    do{
        state = (UINT8)OV3640_read_cmos_sensor(0x3F01);
            SENSORDB("update zone CMD_TAG= 0x%0x\n",state);
            Sleep(1);
            if(iteration-- == 0)
            {
                SENSORDB("update zone wait CMD_TAG zero out of time!!!! %x\n",state);
                break;
            }

    }while(state != 0x00);
    return;

}

static void OV3640_FOCUS_AD5820_Single_Focus(void)
{
     SENSORDB("OV3640_FOCUS_AD5820_Single_Focus\n");

    //UINT8 state = 0x8F;
    //UINT8 state_ack = 0x8F;	
    //UINT8 state_cmd = 0x8F;		
    //UINT32 iteration = 300;

//1.update zone
    OV3640_FOCUS_AD5820_Update_Zone();

//2.change focus window
    OV3640_FOCUS_AD5820_Set_AF_Window_to_IC();

//3.update zone
    OV3640_FOCUS_AD5820_Update_Zone();

//4.send single focus mode command to firmware
    OV3640_write_cmos_sensor(0x3F01,0x11);//CMD_TAG
    OV3640_write_cmos_sensor(0x3F00,0x03);//
    
    SENSORDB("after single focus  \n");
#if 0    
//5.after sigle focus cmd, check the STA_FOCUS until S_FOCUSED 0x02
    iteration = 1000;  
    do{
        state = (UINT8)OV3640_read_cmos_sensor(0x3F07);
        state_ack = 	(UINT8)OV3640_read_cmos_sensor(0x3F01);	
        state_cmd = 	(UINT8)OV3640_read_cmos_sensor(0x3F00);		
        SENSORDB("test,Single state = 0x%x,state_ack=0x%x,state_cmd=0x%x\n",state,state_ack,state_cmd);
        
        if(state == 0x02)
        {
            SENSORDB("single focused!\n");
            break;
        }			
        Sleep(1);
        iteration --;

    }while(iteration);
#endif    
    return;


}	
static UINT32 OV3640_FOCUS_AD5820_Cancel_Focus(void)
{
    UINT8 state = 0x8F;
    UINT8 state_ack = 0x8F; 
    UINT8 state_cmd = 0x8F;     
    UINT32 iteration = 300;

    if(!AF_INIT)
    {
        return 0;
    }
    else
    {
        SENSORDB("enter iOV3640_FOCUS_AD5820_Cancel_Focus\n");
    
        //UINT8 state = 0x8F;
        //UINT8 state_ack = 0x8F;	
        //UINT8 state_cmd = 0x8F;		
        //UINT32 iteration = 300;
    
        OV3640_write_cmos_sensor(0x3F01,0x11);  //TAG
        OV3640_write_cmos_sensor(0x3F00,0x08);   
    
        iteration = 100;  
        do{
            state = (UINT8)OV3640_read_cmos_sensor(0x3F07);
            state_ack = 	(UINT8)OV3640_read_cmos_sensor(0x3F01);	
            state_cmd = 	(UINT8)OV3640_read_cmos_sensor(0x3F00);		
            SENSORDB("cancel focus state = 0x%x,state_ack=0x%x,state_cmd=0x%x\n",state,state_ack,state_cmd);
            
            if(state == 0x00)
            {
                SENSORDB("idle!\n");
                break;
            }			
            Sleep(1);
            iteration --;
    
        }while(iteration);
    
        //STA_FOCUS = (UINT8)OV3640_read_cmos_sensor(0x3F07);	
        return 0;	
    }
}



// Step to Specified position
static UINT32 OV3640_FOCUS_AD5820_Move_to(UINT32 a_u2MovePosition)//??how many bits for ov3640??
{

    UINT8 state = 0x8F;
    UINT32 iteration = 100;
    UINT8 pos = (UINT8)a_u2MovePosition;

    //3)checking status=S_IDLE?
    do{
        state = (UINT8)OV3640_read_cmos_sensor(0x3F07);
        printk("OV3640_FOCUS_AD5820_Move_to,CMD_TAG= %x\n",state);
        Sleep(1);
        if(iteration-- == 0)
        {
             printk("OV3640_FOCUS_AD5820_Move_to,CMD_TAG check error!!!! %x\n",state);
		break;
        }

    }while(state!=0x00);

    OV3640_write_cmos_sensor(0x3F01,0x10);//CMD_TAG
    OV3640_write_cmos_sensor(0x3F05,pos);
    OV3640_write_cmos_sensor(0x3F00,0x05);//

    iteration = 100;
    do{
        state = (UINT8)OV3640_read_cmos_sensor(0x3F07);
        printk("OV3640_FOCUS_AD5820_Move_to,CMD_TAG= %x\n",state);
        Sleep(1);
        if(iteration-- == 0)
        {
             printk("OV3640_FOCUS_AD5820_Move_to,CMD_TAG check error!!!! %x\n",state);
		break;
        }

    }while(state!=0x00);

    return 0;
}

static void OV3640_FOCUS_AD5820_Get_AF_Max_Num_Focus_Areas(UINT32 *pFeatureReturnPara32)
{ 	
    
    *pFeatureReturnPara32 = 1;    
    SENSORDB(" *pFeatureReturnPara32 = %d\n",  *pFeatureReturnPara32);	
}

static void OV3640_FOCUS_AD5820_Get_AE_Max_Num_Metering_Areas(UINT32 *pFeatureReturnPara32)
{ 	
    
    *pFeatureReturnPara32 = 1;    
    SENSORDB(" *pFeatureReturnPara32 = %d\n",  *pFeatureReturnPara32);	
}


static void OV3640_YUV_sensor_initial_setting(void)
{	
	//;Sensor rest														   
	//OV3640_write_cmos_sensor(0x3012,0x80);								   
       //Sleep(10); 
	OV3640_write_cmos_sensor(0x304d,0x45);		
	OV3640_write_cmos_sensor(0x30a7,0x5e);  //add 
	OV3640_write_cmos_sensor(0x3087,0x16);								   
	OV3640_write_cmos_sensor(0x309c,0x1a);								   
	OV3640_write_cmos_sensor(0x30a2,0xe4);								   
	OV3640_write_cmos_sensor(0x30aa,0x42);								   
	OV3640_write_cmos_sensor(0x30b0,0xff);								   
	OV3640_write_cmos_sensor(0x30b1,0xff);								   
	//OV3640_write_cmos_sensor(0x30b2,0x10);	
	OV3640_write_cmos_sensor(0x30b2,0x00);
																		   
	//;26MHz Mclk,22.5Mhz Pclk											   
	OV3640_write_cmos_sensor(0x300e,0x33);								   
	OV3640_write_cmos_sensor(0x300f,0x21);								   
	OV3640_write_cmos_sensor(0x3010,0x20);								   
	OV3640_write_cmos_sensor(0x3011,0x00);								   
	OV3640_write_cmos_sensor(0x304c,0x82);								   																		  
	OV3640_write_cmos_sensor(0x30d7,0x10);								   
																		   
	OV3640_write_cmos_sensor(0x30d9,0x0d);								   
	OV3640_write_cmos_sensor(0x30db,0x08);								   
	OV3640_write_cmos_sensor(0x3016,0x82);								   
	OV3640_write_cmos_sensor(0x3016, 0x81); //disable D0 & D1 Data, OV FAE suggest 
	OV3640_write_cmos_sensor(0x30d9, 0x05);								   
	OV3640_write_cmos_sensor(0x30db, 0x08); 

	OV3640_write_cmos_sensor(0x3085, 0x20); 
																		   
	//aec/agc auto setting						
#if 1
	OV3640_write_cmos_sensor(0x3018,OV3640_Y_Target_H);			  //;38 ;aec		   
	OV3640_write_cmos_sensor(0x3019,OV3640_Y_Target_L);			  //;30 			   
	OV3640_write_cmos_sensor(0x301a,0x82);			  //;61 			   
//       OV3640_write_cmos_sensor(0x301a,0xd4);			  //;61 			   
	OV3640_write_cmos_sensor(0x307d,0x00);			  // ;aec isp 06142007 
	OV3640_write_cmos_sensor(0x3087,0x02);			  //				   
	OV3640_write_cmos_sensor(0x3082,0x20);			  //				   
#else 
	//Update 2010.05.26 OV FAE 
       OV3640_write_cmos_sensor(0x3018, 0x38);// ;aec
       OV3640_write_cmos_sensor(0x3019, 0x30);// ;06142007
       OV3640_write_cmos_sensor(0x301a, 0x61);// ;06142007
       OV3640_write_cmos_sensor(0x307d, 0x00);// ;aec isp 06142007
       OV3640_write_cmos_sensor(0x3087, 0x02);// ;06142007
       OV3640_write_cmos_sensor(0x3082, 0x20);// ;06142007
#endif 	
																		   
	//;Band filter setting												   
	//;78 3070 00														   
	OV3640_write_cmos_sensor(0x3071,0xED);         //;BD50						   
	//;78 3072 00														   
	OV3640_write_cmos_sensor(0x3073,0xC5);		 //;BD60				   
	OV3640_write_cmos_sensor(0x301c,0x02);								   
	OV3640_write_cmos_sensor(0x301d,0x03);								   
																		   
	OV3640_write_cmos_sensor(0x3015,0x21);			// ; 4x gain, auto 1/2
#if 0 	
	OV3640_write_cmos_sensor(0x3014,0x84);			// ; [7]BD selector, ni
#else 
       //Update 2010.05.26 OV FAE 
	OV3640_write_cmos_sensor(0x3014, 0x2c);              //  ;06142007 auto frame on
#endif 
	OV3640_write_cmos_sensor(0x3016,0x92);			//; point 4x.		   
	OV3640_write_cmos_sensor(0x3013,0xf7);			//					   
																		   
	//AEC Weight 
	OV3640_write_cmos_sensor(0x303c,0x08);								   
	OV3640_write_cmos_sensor(0x303d,0x18);								   
	OV3640_write_cmos_sensor(0x303e,0x06);								   
	OV3640_write_cmos_sensor(0x303F,0x0c);		
	OV3640_write_cmos_sensor(0x3030,0x62);								   
	OV3640_write_cmos_sensor(0x3031,0x26);								   
	OV3640_write_cmos_sensor(0x3032,0xe6);								   
	OV3640_write_cmos_sensor(0x3033,0x6e);								   
	OV3640_write_cmos_sensor(0x3034,0xea);								   
	OV3640_write_cmos_sensor(0x3035,0xae);								   
	OV3640_write_cmos_sensor(0x3036,0xa6);								   
	OV3640_write_cmos_sensor(0x3037,0x6a);								   
																		   
	//;CCM	2010.03.08 update											   
#if 1
	OV3640_write_cmos_sensor(0x3340,0x14);								   
	OV3640_write_cmos_sensor(0x3341,0x24);								   
	OV3640_write_cmos_sensor(0x3342,0x08);								   
	OV3640_write_cmos_sensor(0x3343,0x0f);	//0x11							   
	OV3640_write_cmos_sensor(0x3344,0x55);	//0x5c							   
	OV3640_write_cmos_sensor(0x3345,0x65);	//6d						   
	OV3640_write_cmos_sensor(0x3346,0x5c);	//63						   
	OV3640_write_cmos_sensor(0x3347,0x5f);	//66						   
	OV3640_write_cmos_sensor(0x3348,0x02);	//02						   
	OV3640_write_cmos_sensor(0x3349,0x98);								   
#else 
       //update 2010.05.26 OV FAE update 
       OV3640_write_cmos_sensor(0x3340, 0x20);
       OV3640_write_cmos_sensor(0x3341, 0x64);
       OV3640_write_cmos_sensor(0x3342, 0x08);
       OV3640_write_cmos_sensor(0x3343, 0x30);
       OV3640_write_cmos_sensor(0x3344, 0x90);
       OV3640_write_cmos_sensor(0x3345, 0xc0);//C5
       OV3640_write_cmos_sensor(0x3346, 0xa0);
       OV3640_write_cmos_sensor(0x3347, 0x98);
       OV3640_write_cmos_sensor(0x3348, 0x08);
       OV3640_write_cmos_sensor(0x3349, 0x98);
#endif 	
	OV3640_write_cmos_sensor(0x333F,0x0C);		

	OV3640_write_cmos_sensor(0x3331,0x02);         //auto denoise offset,bigger the value is,bigger denoise strength
																		   
	//;ISP Common														   
	OV3640_write_cmos_sensor(0x3104,0x02);			 // ;isp system control
	OV3640_write_cmos_sensor(0x3105,0xfd);			 // 				   
	OV3640_write_cmos_sensor(0x3106,0x00);			 // 				   
	OV3640_write_cmos_sensor(0x3107,0xff);			 // 				   
																		   
	OV3640_write_cmos_sensor(0x3300,0x12);			  //				   
	OV3640_write_cmos_sensor(0x3301,0xde);			  // ;aec gamma 	   
																		   
	//;ISP setting														   
	OV3640_write_cmos_sensor(0x3302,0xcf);		   //sde, uv_adj, gam, awb 
																		   
	//; Gamma		
#if 1
	OV3640_write_cmos_sensor(0x331b,0x0a);								   
	OV3640_write_cmos_sensor(0x331c,0x18);								   
	OV3640_write_cmos_sensor(0x331d,0x30);								   
	OV3640_write_cmos_sensor(0x331e,0x5a);								   
	OV3640_write_cmos_sensor(0x331f,0x67);								   
	OV3640_write_cmos_sensor(0x3320,0x72);								   
	OV3640_write_cmos_sensor(0x3321,0x7e);								   
	OV3640_write_cmos_sensor(0x3322,0x87);								   
	OV3640_write_cmos_sensor(0x3323,0x8f);								   
	OV3640_write_cmos_sensor(0x3324,0x96);								   
	OV3640_write_cmos_sensor(0x3325,0xa3);								   
	OV3640_write_cmos_sensor(0x3326,0xaf);								   
	OV3640_write_cmos_sensor(0x3327,0xc4);								   
	OV3640_write_cmos_sensor(0x3328,0xd7);								   
	OV3640_write_cmos_sensor(0x3329,0xe8);								   
	OV3640_write_cmos_sensor(0x332a,0x20);								   
#else 
	// Update 2010.05.26 OV FAE Update 
       OV3640_write_cmos_sensor(0x331b, 0x03);
       OV3640_write_cmos_sensor(0x331c, 0x12);
       OV3640_write_cmos_sensor(0x331d, 0x2c);
       OV3640_write_cmos_sensor(0x331e, 0x55);
       OV3640_write_cmos_sensor(0x331f, 0x67);
       OV3640_write_cmos_sensor(0x3320, 0x78);
       OV3640_write_cmos_sensor(0x3321, 0x86);
       OV3640_write_cmos_sensor(0x3322, 0x93);
       OV3640_write_cmos_sensor(0x3323, 0x9e);
       OV3640_write_cmos_sensor(0x3324, 0xa8);
       OV3640_write_cmos_sensor(0x3325, 0xb5);
       OV3640_write_cmos_sensor(0x3326, 0xc1);
       OV3640_write_cmos_sensor(0x3327, 0xd2);
       OV3640_write_cmos_sensor(0x3328, 0xe1);
       OV3640_write_cmos_sensor(0x3329, 0xec);
       OV3640_write_cmos_sensor(0x332a, 0x1a);	
#endif 																		   
	//;AWB																   
	//;update 2010.03.01												   
#if 0
	OV3640_write_cmos_sensor(0x3317,0x10);								   
	OV3640_write_cmos_sensor(0x3316,0xf0);								   
	OV3640_write_cmos_sensor(0x3312,0x31);								   
	OV3640_write_cmos_sensor(0x3314,0x49);								   
	OV3640_write_cmos_sensor(0x3313,0x2e);								   
	OV3640_write_cmos_sensor(0x3315,0x3f);								   
	OV3640_write_cmos_sensor(0x3311,0xcd);								   
	OV3640_write_cmos_sensor(0x3310,0xa6);								   
	OV3640_write_cmos_sensor(0x330c,0x13);								   
	OV3640_write_cmos_sensor(0x330d,0x18);								   
	OV3640_write_cmos_sensor(0x330e,0x5a);								   
	OV3640_write_cmos_sensor(0x330f,0x4b);								   
	OV3640_write_cmos_sensor(0x330b,0x11);								   
	OV3640_write_cmos_sensor(0x3306,0x5e);								   
	OV3640_write_cmos_sensor(0x3307,0x11);								   
	OV3640_write_cmos_sensor(0x3308,0x25);								   
#else 
	//Update 2010.05.26 OV FAE suggest  
       OV3640_write_cmos_sensor(0x3317, 0x04);
       OV3640_write_cmos_sensor(0x3316, 0xf8);
       OV3640_write_cmos_sensor(0x3312, 0x26);
       OV3640_write_cmos_sensor(0x3314, 0x42);
       OV3640_write_cmos_sensor(0x3313, 0x2b);
       OV3640_write_cmos_sensor(0x3315, 0x42);
       OV3640_write_cmos_sensor(0x3310, 0xD0);
       OV3640_write_cmos_sensor(0x3311, 0xBD);
       OV3640_write_cmos_sensor(0x330c, 0x18);
       OV3640_write_cmos_sensor(0x330d, 0x18);
       OV3640_write_cmos_sensor(0x330e, 0x56);
       OV3640_write_cmos_sensor(0x330f, 0x5C);
       OV3640_write_cmos_sensor(0x330b, 0x1C);
       OV3640_write_cmos_sensor(0x3306, 0x5C);
       OV3640_write_cmos_sensor(0x3307, 0x11);
       OV3640_write_cmos_sensor(0x3308, 0x25);
#endif 	
																		   
	//;Lens correction													   
																		   
	//;Lens correction						
#if 0
	OV3640_write_cmos_sensor(0x3367,0x34);								   
	OV3640_write_cmos_sensor(0x3368,0x00);								   
	OV3640_write_cmos_sensor(0x3369,0x00);								   
	OV3640_write_cmos_sensor(0x336a,0x52);			// ;R gain			   
	OV3640_write_cmos_sensor(0x336b,0x87);								   
	OV3640_write_cmos_sensor(0x336c,0xc2);								   
																		   
	OV3640_write_cmos_sensor(0x336d,0x34);								   
	OV3640_write_cmos_sensor(0x336e,0x00);								   
	OV3640_write_cmos_sensor(0x336f,0x00);								   
	OV3640_write_cmos_sensor(0x3370,0x46);			 // ;G gain 		   
	OV3640_write_cmos_sensor(0x3371,0x87);								   
	OV3640_write_cmos_sensor(0x3372,0xc2);								   
																		   
	OV3640_write_cmos_sensor(0x3373,0x34);								   
	OV3640_write_cmos_sensor(0x3374,0x00);								   
	OV3640_write_cmos_sensor(0x3375,0x00);								   
	OV3640_write_cmos_sensor(0x3376,0x42);			 // ;38 ;B gain 	   
	OV3640_write_cmos_sensor(0x3377,0x87);								   
	OV3640_write_cmos_sensor(0x3378,0xc2);								   
#else 

       OV3640_write_cmos_sensor(0x3367, 0x34);//;R
       OV3640_write_cmos_sensor(0x3368, 0x00);//;0c
       OV3640_write_cmos_sensor(0x3369, 0x06);
       OV3640_write_cmos_sensor(0x336a, 0x52);
       OV3640_write_cmos_sensor(0x336b, 0x87);
       OV3640_write_cmos_sensor(0x336c, 0xc2);
       
       OV3640_write_cmos_sensor(0x336d, 0x34); //;G
       OV3640_write_cmos_sensor(0x336e, 0x0c);
       OV3640_write_cmos_sensor(0x336f, 0x06);
       OV3640_write_cmos_sensor(0x3370, 0x46);
       OV3640_write_cmos_sensor(0x3371, 0x87);
       OV3640_write_cmos_sensor(0x3372, 0xc2);
       
       OV3640_write_cmos_sensor(0x3373, 0x33); //;B
       OV3640_write_cmos_sensor(0x3374, 0xf0);
       OV3640_write_cmos_sensor(0x3375, 0x06);
       OV3640_write_cmos_sensor(0x3376, 0x46);
       OV3640_write_cmos_sensor(0x3377, 0x77);//;87
       OV3640_write_cmos_sensor(0x3378, 0xc2);
#endif 
	OV3640_write_cmos_sensor(0x3366,0x15);			//10					   
	OV3640_write_cmos_sensor(0x3300,0x13);			  //;[0]-enab		   
																		   
																		   

																		   
	OV3640_write_cmos_sensor(0x3507,0x06);								   
	OV3640_write_cmos_sensor(0x350a,0x4f);								   
																		   
	//;output format					
#if 1	
	OV3640_write_cmos_sensor(0x3100,0x02);								   
	OV3640_write_cmos_sensor(0x3301,0xde);								   
	OV3640_write_cmos_sensor(0x3304,0xfc);								   
	OV3640_write_cmos_sensor(0x3400,0x00);								   
	OV3640_write_cmos_sensor(0x3404,0x00);								   
	OV3640_write_cmos_sensor(0x3600,0xc0);			 // ;c4 			   
#else 
       OV3640_write_cmos_sensor(0x3100, 0x32);
       OV3640_write_cmos_sensor(0x3304, 0x00);
       OV3640_write_cmos_sensor(0x3400, 0x02);
       OV3640_write_cmos_sensor(0x3404, 0x22);
       OV3640_write_cmos_sensor(0x3500, 0x00); 
       OV3640_write_cmos_sensor(0x3600, 0xc0); //;538 c4 
       OV3640_write_cmos_sensor(0x3610, 0x60);
       OV3640_write_cmos_sensor(0x350a, 0x4f);
#endif 
																		   
																		   
	//;3640->XGA														   
	/*OV3640_write_cmos_sensor(0x3012,0x10);							   
	OV3640_write_cmos_sensor(0x3023,0x06);			 // ;05 			   
	OV3640_write_cmos_sensor(0x3026,0x03);								   
	OV3640_write_cmos_sensor(0x3027,0x04);								   
	OV3640_write_cmos_sensor(0x302a,0x03);								   
	OV3640_write_cmos_sensor(0x302b,0x10);								   
	OV3640_write_cmos_sensor(0x3075,0x24);								   
	OV3640_write_cmos_sensor(0x300d,0x01);			 // ;				   
	OV3640_write_cmos_sensor(0x30d7,0x90);								   
	OV3640_write_cmos_sensor(0x3069,0x04);			 //;44 ;BLC 		   
	OV3640_write_cmos_sensor(0x303e,0x00);								   
	OV3640_write_cmos_sensor(0x303f,0xc0);								   
																		   
	OV3640_write_cmos_sensor(0x304c,0x85);								   
																		   
	OV3640_write_cmos_sensor(0x3302,0xef);				  // ; sde, uv_adj,
																		   
	OV3640_write_cmos_sensor(0x335f,0x34);								   
	OV3640_write_cmos_sensor(0x3360,0x0c);								   
	OV3640_write_cmos_sensor(0x3361,0x04);								   
	OV3640_write_cmos_sensor(0x3362,0x12);			   //;Zoom_out output s
	OV3640_write_cmos_sensor(0x3363,0x88);								   
	OV3640_write_cmos_sensor(0x3364,0xE4);								   
	OV3640_write_cmos_sensor(0x3403,0x42);								   
	OV3640_write_cmos_sensor(0x3088,0x12);			   //;x_output_size    
	OV3640_write_cmos_sensor(0x3089,0x80);								   
	OV3640_write_cmos_sensor(0x308a,0x01);			   //;y_output_size    
	OV3640_write_cmos_sensor(0x308b,0xe0);								   
	*/																	   
	//OV3640_write_cmos_sensor(0x308d,0x14);		//04						   
	OV3640_write_cmos_sensor(0x308d,0x04);		//04						   
	OV3640_write_cmos_sensor(0x3086,0x03);								   
	OV3640_write_cmos_sensor(0x3086,0x00);		

	//@@ UV Auto Mode gth1=2 gth2=3 offset=16 ;K=(31-offset)/(G2-G1) By OV-Allen 100705_01
	OV3640_write_cmos_sensor(0x3302,0xef);    //;enable UV adj
	OV3640_write_cmos_sensor(0x30b8,0xf0);		//;auto mode slop
	OV3640_write_cmos_sensor(0x30b9,0x16);    //;offset
	OV3640_write_cmos_sensor(0x30ba,0x02);    //;gth1
	OV3640_write_cmos_sensor(0x30bb,0x03);    //;gth2

} /* OV3640_YUV_sensor_initial_setting */

static void OV3640_set_AE_mode(kal_bool AE_enable)
{
	kal_uint8 temp_AE_reg = 0;
	
	if (AE_enable == KAL_TRUE)
	{
		// turn on AEC/AGC
		temp_AE_reg = OV3640_read_cmos_sensor(0x3013);
		OV3640_write_cmos_sensor(0x3013, temp_AE_reg| 0x05);
       { /* -------OV3640 extra line can not auto set to 0 when change from capture to preview,so set it to 0 by person------*/
            OV3640_write_cmos_sensor(0x302d,0x00);
            OV3640_write_cmos_sensor(0x302e,0x00);
       }
    }
	else
	{
		// turn off AEC/AGC
		temp_AE_reg = OV3640_read_cmos_sensor(0x3013);
		OV3640_write_cmos_sensor(0x3013, temp_AE_reg&~0x05);
	}
}


static void OV3640_set_AWB_mode(kal_bool AWB_enable)
{
	kal_uint8 temp_AWB_reg = 0;

	//return ;

	if (AWB_enable == KAL_TRUE)
	{
		//enable Auto WB
		temp_AWB_reg = OV3640_read_cmos_sensor(0x3302);
		OV3640_write_cmos_sensor(0x3302, temp_AWB_reg | 0x01);
	}
	else
	{
		//turn off AWB
		temp_AWB_reg = OV3640_read_cmos_sensor(0x3302);
		OV3640_write_cmos_sensor(0x3302, temp_AWB_reg & ~0x01);
	}
}

static u32 ov3640_calc_mipiclk(void)
{
       u32 rxpll, val, n, m, bit8div;
       u32 sdiv_inv, mipidiv;
       u32 fclk, mipiclk, mclk = 26000000;
       u8 lut1[4] = {2, 3, 4, 6};
       u8 lut2[4] = {1, 1, 4, 5};

       val = OV3640_read_cmos_sensor(0x3011); 

       mclk /= (val + 1); 

       /* Calculate fclk */
       val = OV3640_read_cmos_sensor(0x300E);
       rxpll = val & 0x3F;

      val = OV3640_read_cmos_sensor(0x300F);
      n = lut1[(val >> 6) & 0x3];
      m = lut1[val & 0x3];
      bit8div = lut2[(val >> 4) & 0x3];
      fclk = (64 - rxpll) * n * bit8div * mclk / m;

      val = OV3640_read_cmos_sensor(0x3010);
      mipidiv = ((val >> 5) & 1) + 1;
      sdiv_inv = (val & 0xF) * 2;

      if ((val & 0xF) >= 1)
              mipiclk = fclk / sdiv_inv / mipidiv;
      else
              mipiclk = fclk / mipidiv;

     
#if 0 
     printk("mipiclk=%u  fclk=%u  val&0xF=%u  sdiv_inv=%u  "
                                                      "mipidiv=%u\n",
                                                      mipiclk, fclk, val&0xF,
                                                      sdiv_inv, mipidiv);
#endif
     return mipiclk;
}

static void OV3640_set_QXGA_mode(void)
{
	
	//@@ Capture QXGA Key setting								 							 
	//99 2048 1536												 
	//98 0 0													 														 
	OV3640_write_cmos_sensor(0x3012,0x00);		//soft reset, sensor array resolution:QXGA	
	
   /* 
        fclk=(0x40-0x300E[5:0])*N*Bit8Div*MCLK/M
              N=1/1.5/2/3 for ox300F[7:6]=0~3,  M=1/1.5/2/3 for 0x300F[1:0],   Bit8Div=1/1/4/5 for 0x300F[5:4]
         DVP PClk=fclk/ScaleDiv/DVPDiv;  
              ScaleDiv=1 and 0x3010[3:0]*2 for 0x3010[3:0]=0 and other values.
              DVPDiv=1/2/8/16 for 0x3010[7:6]
    */

    //size:2048x1536	
	OV3640_write_cmos_sensor(0x3020,0x01);	   //HS[15:8]					 
	OV3640_write_cmos_sensor(0x3021,0x1d);	   //HS[7:0]				 
	OV3640_write_cmos_sensor(0x3022,0x00);	   //VS[15:8]				 
	OV3640_write_cmos_sensor(0x3023,0x0a);	   //VS[7:0]					 
	OV3640_write_cmos_sensor(0x3024,0x08);	   //HW[15:8],  HW=2072				 
	OV3640_write_cmos_sensor(0x3025,0x18);	   //HW[7:0]				 
	OV3640_write_cmos_sensor(0x3026,0x06);	   //VH[15:8],   VH=1548			 
	OV3640_write_cmos_sensor(0x3027,0x0c);		//VH[7:0]				 
	OV3640_write_cmos_sensor(0x302a,0x06);	   //VTS[15:8]:1568				 
	OV3640_write_cmos_sensor(0x302b,0x20);		//VTS[7:0]				 
	OV3640_write_cmos_sensor(0x3075,0x44);		//Vsync pulse width and start point				 
	OV3640_write_cmos_sensor(0x300d,0x00);		//pclk output control				 
	OV3640_write_cmos_sensor(0x30d7,0x10);		//reserve			 
	OV3640_write_cmos_sensor(0x3069,0x44);		//BLC control				 
	OV3640_write_cmos_sensor(0x303e,0x01);		//AVH[11:8],  AVH=384(384*4=1536)			 
	OV3640_write_cmos_sensor(0x303f,0x80);		//AVH[7:0]	
	//if(OV3640_Night_mode == KAL_TRUE)
	//OV3640_write_cmos_sensor(0x3302,0xeb);      //turn off gamma
	//else
	OV3640_write_cmos_sensor(0x3302,0xef);		//UV_adj, AWB, GMA				 
	OV3640_write_cmos_sensor(0x335f,0x68);		//SIZE_IN_MISC,Vsize_in[10:8],Hsize_in[11:8]	for zoom_out			 
	OV3640_write_cmos_sensor(0x3360,0x18);		//Hsize_in[7:0],Hsize=2072			 
	OV3640_write_cmos_sensor(0x3361,0x0C);		//Vsize_in[7:0],Vsize=1548			 
	OV3640_write_cmos_sensor(0x3362,0x68);		//SIZE_OUT_MISC,Vsize_in[10:8],Hsize_in[11:8]	for zoom_out		 
	OV3640_write_cmos_sensor(0x3363,0x08);		//Hsize_out[7:0],Hsize=2056				 
	OV3640_write_cmos_sensor(0x3364,0x04);		//Vsize_out[7:0],Vsize=1540			 
																 
	OV3640_write_cmos_sensor(0x3366,0x10);		//reserve				 
																 
	OV3640_write_cmos_sensor(0x3403,0x42);		//bit[7:4]:x start=4,  bit[3:0]:y start=2				 
	OV3640_write_cmos_sensor(0x3088,0x08);		// isp output size, x_out[15:8]	,  x_out=2048			 
	OV3640_write_cmos_sensor(0x3089,0x00);		//x_out[7:0]				 
	OV3640_write_cmos_sensor(0x308a,0x06);		//y_out[15:8],   y_out=1536			 
	OV3640_write_cmos_sensor(0x308b,0x00);		//y_out[7:0]				 
	if(OV3640_Night_mode == KAL_TRUE)        //add dummy to decrease low light frame for low light noise pass
	{OV3640_write_cmos_sensor(0x302a,0x07);
	OV3640_write_cmos_sensor(0x302b,0xe4);}
	else
	{OV3640_write_cmos_sensor(0x302a,0x06);
	 OV3640_write_cmos_sensor(0x302b,0x20);}

	 OV3640_set_AWB_mode(KAL_FALSE); 
        OV3640_set_AE_mode(KAL_FALSE);

  //OV3640_write_cmos_sensor(0x3013,0xf2);	 // ; disable AEC/AGC
	
}


/*****************************************************************************/
/* Windows Mobile Sensor Interface */
/*****************************************************************************/
/*************************************************************************
* FUNCTION
*   OV3640GetSensorID
*
* DESCRIPTION
*   This function get the sensor ID 
*
* PARAMETERS
*   *sensorID : return the sensor ID 
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 OV3640GetSensorID(UINT32 *sensorID) 
{
	volatile signed char i;

	//s_move to here from CISModulePowerOn()
	OV3640_write_cmos_sensor(0x300E, 0x39);
	OV3640_write_cmos_sensor(0x300E, 0xB9);//		;Enable bit[7]
	OV3640_write_cmos_sensor(0x308D, 0x14);// 
	OV3640_write_cmos_sensor(0x30AD, 0x82);//		;Enable bit[7]
	OV3640_write_cmos_sensor(0x3086, 0x0F);//

	mdelay(10); 
	OV3640_write_cmos_sensor(0x30AD, 0x02);//		;Clear bit[7]
	OV3640_write_cmos_sensor(0x3086, 0x08);//
	OV3640_write_cmos_sensor(0x308D, 0x14);//
	OV3640_write_cmos_sensor(0x300E, 0x32);//		;Clear bit
	//e_move to here from CISModulePowerOn()
	
	OV3640_write_cmos_sensor(0x3012,0x80);// Reset sensor
	Sleep(10);

	 //  Read sensor ID to adjust I2C is OK?
	i = 3; 
	while (i > 0)
	{
		*sensorID = (OV3640_read_cmos_sensor(0x300A) << 8) | OV3640_read_cmos_sensor(0x300B);
		if (*sensorID == OV3640_SENSOR_ID)
		{
			break;
		}
		printk("[OV3640YUV]:Error Sensor ID:0x%x\n", *sensorID); 
		i --; 
	}

	if(*sensorID != OV3640_SENSOR_ID)
	{
		printk("[OV3640YUV]:Read Sensor ID fail:0x%x\n", *sensorID);		
		*sensorID = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}

    return ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*	OV3640Open
*
* DESCRIPTION
*	This function initialize the registers of CMOS sensor
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 OV3640Open(void)
{


    int  retry = 0; 
    kal_uint16 sensor_id=0;
    sensor_id_fail = 0; 
    SENSORDB("OV3640Open E\n");
    //s_move to here from CISModulePowerOn()
    OV3640_write_cmos_sensor(0x300E, 0x39);
    OV3640_write_cmos_sensor(0x300E, 0xB9);//		;Enable bit[7]
    OV3640_write_cmos_sensor(0x308D, 0x14);// 
    OV3640_write_cmos_sensor(0x30AD, 0x82);//		;Enable bit[7]
    OV3640_write_cmos_sensor(0x3086, 0x0F);//

    mdelay(10); 
    OV3640_write_cmos_sensor(0x30AD, 0x02);//		;Clear bit[7]
    OV3640_write_cmos_sensor(0x3086, 0x08);//
    OV3640_write_cmos_sensor(0x308D, 0x14);//
    OV3640_write_cmos_sensor(0x300E, 0x32);//		;Clear bit
    //e_move to here from CISModulePowerOn()
    
    OV3640_write_cmos_sensor(0x3012,0x80);// Reset sensor
    Sleep(10);

    zoom_factor = 0; 
     
     //  Read sensor ID to adjust I2C is OK?
    retry = 3; 
    while (retry > 0)
    {
        sensor_id = (OV3640_read_cmos_sensor(0x300A) << 8) | OV3640_read_cmos_sensor(0x300B);
        if (sensor_id == OV3640_SENSOR_ID)
        {
            break;
        }
        printk("[OV3640YUV]:Error Sensor ID:0x%x\n", sensor_id); 
        retry --; 
    }

    if(sensor_id != OV3640_SENSOR_ID)
    {
        printk("[OV3640YUV]:Read Sensor ID fail:0x%x\n", sensor_id); 	    
        sensor_id_fail = 1; 
        return ERROR_SENSOR_CONNECT_FAIL;
    }

    /*9. Apply sensor initail setting*/
    OV3640_YUV_sensor_initial_setting();
    first_enter_preview = KAL_TRUE;

    if(OV3640_FOCUS_AD5820_Init())
        return ERROR_LENS_NOT_SUPPORT;
    SENSORDB("OV3640Open X\n");     
    return ERROR_NONE;
}	/* OV3640Open() */

/*************************************************************************
* FUNCTION
*	OV3640Close
*
* DESCRIPTION
*	This function is to turn off sensor module power.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 OV3640Close(void)
{
//	CISModulePowerOn(FALSE);

#if WINMO_USE
	#ifndef SOFTWARE_I2C_INTERFACE	/* software I2c MODE */
	DRV_I2CClose(OV3640hDrvI2C);
	#endif
#endif 	
	return ERROR_NONE;
}	/* OV3640Close() */

#if 0

static kal_uint16 OV3640_Reg2Gain(const kal_uint8 iReg)
{
    kal_uint8 iI;
    kal_uint16 iGain = BASEGAIN;    // 1x-gain base

    // Range: 1x to 32x
    // Gain = (GAIN[7] + 1) * (GAIN[6] + 1) * (GAIN[5] + 1) * (GAIN[4] + 1) * (1 + GAIN[3:0] / 16)
    for (iI = 7; iI >= 4; iI--) {
        iGain *= (((iReg >> iI) & 0x01) + 1);
    }

    return iGain +  iGain * (iReg & 0x0F) / 16;
}

static kal_uint8 OV3640_Gain2Reg(const kal_uint16 iGain)
{
    kal_uint8 iReg = 0x00;

    if (iGain < 2 * BASEGAIN) {
        // Gain = 1 + GAIN[3:0](0x00) / 16
        iReg = 16 * (iGain - BASEGAIN) / BASEGAIN;
    }else if (iGain < 4 * BASEGAIN) {
        // Gain = 2 * (1 + GAIN[3:0](0x00) / 16)
        iReg |= 0x10;
        iReg |= 8 * (iGain - 2 * BASEGAIN) / BASEGAIN;
    }else if (iGain < 8 * BASEGAIN) {
        // Gain = 4 * (1 + GAIN[3:0](0x00) / 16)
        iReg |= 0x30;
        iReg |= 4 * (iGain - 4 * BASEGAIN) / BASEGAIN;
    }else if (iGain < 16 * BASEGAIN) {
        // Gain = 8 * (1 + GAIN[3:0](0x00) / 16)
        iReg |= 0x70;
        iReg |= 2 * (iGain - 8 * BASEGAIN) / BASEGAIN;
    }else if (iGain < 32 * BASEGAIN) {
        // Gain = 16 * (1 + GAIN[3:0](0x00) / 16)
        iReg |= 0xF0;
        iReg |= (iGain - 16 * BASEGAIN) / BASEGAIN;
    }else {
        ASSERT(0);
    }

    return iReg;
}
#endif
kal_uint16 OV3640_write_gain(kal_uint16 gain)
{
#if 0
    kal_uint8 iReg = OV3640_Gain2Reg(iGain);
    kal_uint8 gain = iGain / BASEGAIN;
   
    OV3640_write_cmos_sensor(0x3001, iReg);   //AGC[7:0]

   // kal_prompt_trace(MOD_ENG, "Write GAIN: %d, YAvg: %x", gain, OV3640_read_cmos_sensor(0x301B));

    return gain;
#else 
    kal_uint16 temp_reg;
   
	RETAILMSG(1, (TEXT("OV3640 write gain: %d\r\n"), gain));
   
   if(gain > 248)  return 0;//ASSERT(0);
   
    temp_reg = 0;
    if (gain > 31)
    {
        temp_reg |= 0x10;
        gain = gain >> 1;
    }
    if (gain > 31)
    {
        temp_reg |= 0x20;
        gain = gain >> 1;
    }

    if (gain > 31)
    {
        temp_reg |= 0x40;
        gain = gain >> 1;
    }
    if (gain > 31)
    {
        temp_reg |= 0x80;
        gain = gain >> 1;
    }
    
    if (gain > 16)
    {
        temp_reg |= ((gain -16) & 0x0f);
    }   
  
   OV3640_write_cmos_sensor(0x3001,temp_reg);
#endif 

    return 0;

}

kal_uint16 OV3640_read_gain(void)
{
#if 0
	kal_uint8  iReg = 0;
	kal_uint16 sensor_gain = 0;
	
	iReg = OV3640_read_cmos_sensor(0x3001);
    sensor_gain = OV3640_Reg2Gain(iReg);
	
	//kal_prompt_trace(MOD_ENG, "Read Gain:%d", sensor_gain/BASEGAIN);

	return sensor_gain;
#else 
    kal_uint8  temp_reg;
    kal_uint16 sensor_gain;

    temp_reg=OV3640_read_cmos_sensor(0x3001);  


    sensor_gain=(16+(temp_reg&0x0F));
    if(temp_reg&0x10)
        sensor_gain<<=1;
    if(temp_reg&0x20)
        sensor_gain<<=1;
      
    if(temp_reg&0x40)
        sensor_gain<<=1;
      
    if(temp_reg&0x80)
        sensor_gain<<=1;
      
    return sensor_gain;
#endif 	
	
}  /* read_OV3640_gain */

static kal_uint16 OV3640ReadASDGain(void)
{
  kal_uint8 Reg = OV3640_read_cmos_sensor(0x3001);  
  return (((Reg&0x0F) << 2) + 0x40) * (1 + (Reg >> 4));
}

static void OV3640_set_mirror_flip(kal_uint8 image_mirror, kal_uint16 *iStartX, kal_uint16 *iStartY)
{
	kal_uint8 iTemp = 0;
	kal_uint8 iTemp2 = 0;
	
	iTemp = OV3640_read_cmos_sensor(0x307C)&(~0x03);
	iTemp2 = OV3640_read_cmos_sensor(0x3090)&(~0x08);

    switch (image_mirror) {
    case IMAGE_NORMAL:
        *iStartX = 1;
        *iStartY = 2;
        break;
	case IMAGE_H_MIRROR:
	    iTemp |= 2;			//bit[1] Mirror on/off
		iTemp2 |= 8;		//bit[3] Array mirror on/off
        *iStartX = 1;
        *iStartY = 2;
        break;
	case IMAGE_V_MIRROR:	//Flip Register 0x04[6] and 0x04[4] (FF = 01)
	    iTemp |= 1;		// bit[0] Flip on/off
        *iStartX = 1;
        *iStartY = 3;
        break;

    case IMAGE_HV_MIRROR:
    	iTemp |= 3;
		iTemp2 |= 8;
        *iStartX = 1;
        *iStartY = 3;
        break;

    default:
        ASSERT(0);
    }
    OV3640_write_cmos_sensor(0x307C, iTemp);
	OV3640_write_cmos_sensor(0x3090, iTemp2);		// To mirror the Array, or there is horizontal lines when turn on mirror.
 
}

/*************************************************************************
* FUNCTION
*    OV3640GetEvAwbRef
*
* DESCRIPTION
*    This function get sensor Ev/Awb (EV05/EV13) for auto scene detect
*
* PARAMETERS
*    Ref
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void OV3640GetEvAwbRef(UINT32 pSensorAEAWBRefStruct/*PSENSOR_AE_AWB_REF_STRUCT Ref*/)
{
    PSENSOR_AE_AWB_REF_STRUCT Ref = (PSENSOR_AE_AWB_REF_STRUCT)pSensorAEAWBRefStruct;
    //SENSORDB("OV3640GetEvAwbRef ref = 0x%x \n", Ref);
    	
    Ref->SensorAERef.AeRefLV05Shutter = 1503;
    Ref->SensorAERef.AeRefLV05Gain = 496 * 2; /* 7.75x, 128 base */
    Ref->SensorAERef.AeRefLV13Shutter = 49;
    Ref->SensorAERef.AeRefLV13Gain = 64 * 2; /* 1x, 128 base */
    Ref->SensorAwbGainRef.AwbRefD65Rgain = 188; /* 1.46875x, 128 base */
    Ref->SensorAwbGainRef.AwbRefD65Bgain = 128; /* 1x, 128 base */
    Ref->SensorAwbGainRef.AwbRefCWFRgain = 160; /* 1.25x, 128 base */
    Ref->SensorAwbGainRef.AwbRefCWFBgain = 164; /* 1.28125x, 128 base */
}

/*************************************************************************
* FUNCTION
*    OV3640GetCurAeAwbInfo
*
* DESCRIPTION
*    This function get sensor cur Ae/Awb for auto scene detect
*
* PARAMETERS
*    Info
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void OV3640GetCurAeAwbInfo(UINT32 pSensorAEAWBCurStruct/*PSENSOR_AE_AWB_CUR_STRUCT Info*/)
{
    PSENSOR_AE_AWB_CUR_STRUCT Info = (PSENSOR_AE_AWB_CUR_STRUCT)pSensorAEAWBCurStruct;
    //SENSORDB("OV3640GetCurAeAwbInfo Info = 0x%x \n", Info);
    	
    Info->SensorAECur.AeCurShutter = OV3640_read_shutter();
    Info->SensorAECur.AeCurGain = OV3640ReadASDGain() * 2; /* 128 base */
    OV3640_write_cmos_sensor(0x33A0, 0x00); /* read R gain, set R gain index */
    Info->SensorAwbGainCur.AwbCurRgain = OV3640_read_cmos_sensor(0x33CA) << 1; /* 128 base */
    OV3640_write_cmos_sensor(0x33A0, 0x02); /* read B gain, set B gain index */
    Info->SensorAwbGainCur.AwbCurBgain = OV3640_read_cmos_sensor(0x33CA) << 1; /* 128 base */
}

/*************************************************************************
* FUNCTION
*	OV3640Preview
*
* DESCRIPTION
*	This function start the sensor preview.
*
* PARAMETERS
*	*image_window : address pointer of pixel numbers in one period of HSYNC
*  *sensor_config_data : address pointer of line numbers in one period of VSYNC
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 OV3640Preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    //kal_uint8 iTemp = 0;
    kal_uint16 iStartX = 0, iStartY = 0;
    kal_uint16 	shutter = 0,pv_gain = 0;

#if WINMO_USE
	kal_prompt_trace(MOD_ENG, "Prev:isp_op_mode=%x,Frame rate=%x", sensor_config_data->isp_op_mode, sensor_config_data->frame_rate);
	kal_prompt_trace(MOD_ENG,"call preview function ");

   //must add this,   because in capture function reduce MCLK=13M for zoom in capture
   //so enter into preview from capure should re-set MCLK=26M.
    SET_TG_OUTPUT_CLK_DIVIDER(3);   // 104MHz/4 = 26MHz MCLK
    SET_CMOS_RISING_EDGE(0);
    SET_CMOS_FALLING_EDGE(2);
    ENABLE_CAMERA_PIXEL_CLKIN_ENABLE;	// enable PCLK latch
#endif     
    
    OV3640_sensor_cap_state = KAL_FALSE;
    OV3640_gPVmode = KAL_TRUE;
    /*Step1. set output size*/
    OV3640_set_VGA_mode();

    //Step2. Nedd to turn on the AE/AWB in preview mode.
    OV3640_set_AE_mode(OV3640_AE_ENABLE);
    OV3640_set_AWB_mode(OV3640_AWB_ENABLE);

    //Step3. ISP/Sensor clock setting
    //if (sensor_config_data->frame_rate == 0x0F) {   // vedio: MPEG4/MJPEG Encode Mode
#if 0
    if (sensor_config_data->isp_op_mode != ISP_PREVIEW_MODE
		&& sensor_config_data->isp_op_mode != ISP_WEBCAM_MODE)
	{
        //4  <2> if preview of capture VIDEO

        OV3640_VEDIO_encode_mode = KAL_TRUE;
	    //OV3640_g_iOV3640_Mode = OV3640_MODE_CIF_VIDEO;
			
		if((sensor_config_data->isp_op_mode==ISP_MJPEG_PREVIEW_MODE) \
			||	(sensor_config_data->isp_op_mode==ISP_MJPEG_ENCODE_MODE))
        {
			OV3640_VEDIO_MPEG4 = KAL_FALSE;
			OV3640_PV_dummy_pixels = 0;
			OV3640_PV_dummy_lines = 0;	
		}
		else if((sensor_config_data->isp_op_mode==ISP_MPEG4_PREVIEW_MODE) \
			||	(sensor_config_data->isp_op_mode==ISP_MPEG4_ENCODE_MODE))
		{
			OV3640_VEDIO_MPEG4 = KAL_TRUE;
			OV3640_PV_dummy_pixels = 0;
			OV3640_PV_dummy_lines = 0;
		}


	#if (defined(DRV_ISP_6235_SERIES) || defined(DRV_ISP_6238_SERIES))
		image_window->wait_stable_frame = 8;
		image_window->wait_ae_frame = 0;		// For continuous shot.
	#endif
    }
    else { //-----------------------------------------------------
#endif     

		OV3640_VEDIO_encode_mode = KAL_FALSE;
		OV3640_VEDIO_MPEG4 = KAL_FALSE;
#if 0		
    	if(sensor_config_data->frame_rate==0xF0)	// That means WEBCAM mode.
		{
			//Webcam mode.
			
		//	OV3640_g_Preview_PCLK_Frequency = 480;		//48MHz, decide by Page1:0x3d
		//	OV3640_g_Preview_PCLK_Division = 0;	//Must same as Page1:0x11

		#if (defined(DRV_ISP_6235_SERIES) || defined(DRV_ISP_6238_SERIES))
			image_window->wait_stable_frame = 8;
			image_window->wait_ae_frame = 0;		// For continuous shot.
		#endif
		}
		else
		{
			//4  <2> if preview of capture PICTURE

			OV3640_g_iOV3640_Mode = OV3640_MODE_PREVIEW;

			OV3640_PV_dummy_pixels = 0;
			OV3640_PV_dummy_lines = 0;	

			//Set preview pixel clock freqency and pixel clock division.
		//	OV3640_g_Preview_PCLK_Frequency = ;		
		//	OV3640_g_Preview_PCLK_Division = 0;	

		#if (defined(DRV_ISP_6235_SERIES) || defined(DRV_ISP_6238_SERIES))
		   if(first_enter_preview == KAL_TRUE)
		   { image_window->wait_stable_frame = 13;//13;
		    image_window->wait_ae_frame = 10;//10;		// For continuous shot.
		    }
		   else
		   {
			image_window->wait_stable_frame = 4;//13;
			image_window->wait_ae_frame = 0;//10;		// For continuous shot.
		   }
		#endif
		}
    } //-----------------------------------------------------
#endif 		
     //4  <2> if preview of capture PICTURE
    OV3640_g_iOV3640_Mode = OV3640_MODE_PREVIEW;
    OV3640_PV_dummy_pixels = 1;
    OV3640_PV_dummy_lines = 0;	

			//Set preview pixel clock freqency and pixel clock division.
		//	OV3640_g_Preview_PCLK_Frequency = ;		
		//	OV3640_g_Preview_PCLK_Division = 0;	



    //Step 3. record preview ISP_clk
     OV3640_preview_pclk = 563;//56330000;   //22500000;
     
    //4 <3> set mirror and flip
    OV3640_set_mirror_flip(sensor_config_data->SensorImageMirror, &iStartX, &iStartY);


    //4 <6> set dummy
    OV3640_set_dummy(OV3640_PV_dummy_pixels, OV3640_PV_dummy_lines);


    image_window->GrabStartX = iStartX;    
    image_window->GrabStartY = iStartY;       
    image_window->ExposureWindowWidth = OV3640_IMAGE_SENSOR_PV_WIDTH;
    image_window->ExposureWindowHeight = OV3640_IMAGE_SENSOR_PV_HEIGHT;
    
   //debug 
    shutter = OV3640_read_shutter();
	pv_gain = OV3640_read_gain();
	first_enter_preview = KAL_FALSE;

    //Enable Color bar   
    //OV3640_write_cmos_sensor(0x307B, 0x02);
   //OV3640_write_cmos_sensor(0x307D, 0x80);
    //OV3640_write_cmos_sensor(0x306C, 0x00);	


    
	//kal_prompt_trace(MOD_ENG,"Register 0x3001=0x%x",OV3640_read_cmos_sensor(0x3001));
	//kal_prompt_trace(MOD_ENG,"Register 0x3002=0x%x",OV3640_read_cmos_sensor(0x3002));
	//kal_prompt_trace(MOD_ENG,"Register 0x3003=0x%x",OV3640_read_cmos_sensor(0x3003));
/*	kal_prompt_trace(MOD_ENG, "Prev read:shutter=%d, pv_gain=%d",shutter, pv_gain);

	

	kal_prompt_trace(MOD_ENG,"out preview function ");
*/	
//	camera_wait_sensor_vd_done();		//Wait at least  one frame.

//	kal_sleep_task(150);	//Delay for apply setting.
    //mdelay(1200); 


//reset AE metering 
    SENSORDB("preview set AE !!!!E \n");

    resetPVAE_2_ARRAY();
    mapAE_2_ARRAY_To_AE_1_ARRAY();
    writeAEReg();
    printAE_2_ARRAY();

    SENSORDB("preview set AE !!!!X \n");

    return TRUE; 
}	/* OV3640_Preview */

UINT32 OV3640Capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window, MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    volatile kal_uint32 shutter = 0;  //OV3640_exposure_lines;
    //kal_uint32 extra_line = 0;
    kal_uint32 pv_gain = 0;
    kal_uint8 temp;//for cal auto uv
    int temp_shutter = 0;
    kal_uint32 exposure = 0; 
    kal_uint32 Capture_Banding_Filter = 0; 
    //kal_prompt_trace(MOD_ENG,"call capture function ");
	SENSORDB("OV3640Capture!!!!!!!!!!!!!!!!!!!!1111;\n");
	SENSORDB("ImageTargetWidth:%d;ImageTargetHeight:%d;\n",image_window->ImageTargetWidth,image_window->ImageTargetHeight);
		SENSORDB("ZoomFactor:%d;\n",image_window->ZoomFactor);
		SENSORDB("zoom_factor:%d;\n",zoom_factor);
    OV3640_sensor_cap_state = KAL_TRUE;

#if 0
    if ((image_window->ImageTargetWidth<=OV3640_IMAGE_SENSOR_PV_WIDTH)&&
        (image_window->ImageTargetHeight<=OV3640_IMAGE_SENSOR_PV_HEIGHT))
    {    /*  PV Mode setting */

//         if (sensor_config_data->frame_rate != 0xF0)	//If not webcam mode.
	  {
		OV3640_set_AE_mode(KAL_FALSE);
		OV3640_set_AWB_mode(KAL_FALSE);
		//	shutter = OV3640_read_shutter();
	      //  pv_gain = OV3640_read_gain();
	      //  kal_prompt_trace(MOD_ENG, "cap read:shutter=%d, pv_gain=%d",shutter, pv_gain);
	  }

        OV3640_gPVmode=KAL_TRUE;

       
        //Step 1.record capture clk
       
      //  OV3640_capture_pclk = 22500000;      // 0315

	    OV3640_FULL_dummy_pixels = 0;
	    OV3640_FULL_dummy_lines = 0;
      //  shutter = (shutter*OV3640_capture_pclk)/OV3640_preview_pclk;         // 0315
	//	shutter = (shutter*(OV3640_PV_PERIOD_PIXEL_NUMS+OV3640_PV_dummy_pixels))/(OV3640_PV_PERIOD_PIXEL_NUMS+OV3640_FULL_dummy_pixels); // 0315


           image_window->GrabStartX = 1;
	    image_window->GrabStartY = 2;
	    image_window->ImageTargetWidth = OV3640_IMAGE_SENSOR_PV_WIDTH;
	    image_window->ImageTargetHeight = OV3640_IMAGE_SENSOR_PV_HEIGHT;

	    // set dummy
	    OV3640_set_dummy(OV3640_FULL_dummy_pixels, OV3640_FULL_dummy_lines);
	//	OV3640_write_shutter(shutter);   // 0315
	//	OV3640_write_gain(pv_gain);      // 0315
        //kal_prompt_trace(MOD_ENG, "cap write:shutter=%d, gain=%d",shutter, pv_gain);
#if WINMO_USE        
	#if (defined(DRV_ISP_6235_SERIES) || defined(DRV_ISP_6238_SERIES))
		image_window->wait_stable_frame =0;    // 3;         // 0315
		image_window->wait_ae_frame = 0;		// For continuous shot.
	#endif
#endif 		
    }
    else 
#endif     	
    {	/* 3M FULL Mode */
#if WINMO_USE
	#if (defined(DRV_ISP_6235_SERIES) || defined(DRV_ISP_6238_SERIES))
		image_window->wait_stable_frame = 3;
		image_window->wait_ae_frame = 0;		// For continuous shot.
	//	camera_wait_sensor_vd_done();
	#endif
#endif 

 #if 0
       int i = 30; 
       int y_val = 0; 
       while (i > 0)
       {
           shutter = OV3640_read_shutter();       
           pv_gain = OV3640_read_gain();
           y_val  = OV3640_read_cmos_sensor(0x301B);
           printk("pv shtter:%d, pv_gain:%d, y_val:%d\r\n", shutter, pv_gain, y_val);            
           mdelay(33); 
           i--; 
       }
#endif 
       //disable night mode 
       temp= OV3640_read_cmos_sensor(0x3014);
	OV3640_write_cmos_sensor(0x3014,temp&0xf7);
		//Step 1.disable AE,AWB
        OV3640_set_AE_mode(KAL_FALSE);
	 OV3640_set_AWB_mode(KAL_FALSE);

        shutter = OV3640_read_shutter();
        OV3640_PV_Shutter = shutter; 
	 pv_gain = OV3640_read_gain();
	 OV3640_PV_Gain16 = pv_gain; 

//       printk("cap shtter:%d, cap_gain:%d\r\n", shutter, pv_gain); 
	 //   kal_prompt_trace(MOD_ENG, "cap read:shutter=%d, pv_gain=%d",shutter, pv_gain);
    
        //Step 2. set output size:2048x1536
  
	 OV3640_set_QXGA_mode();		

        OV3640_gPVmode = KAL_FALSE;

    			
        //Step 3.record capture clk
//	OV3640_capture_pclk = 480;//47666667;
#if WINMO_USE	       
        if ((image_window->ImageTargetWidth == OV3640_IMAGE_SENSOR_2M_WIDTH)&&
                (image_window->ImageTargetHeight == OV3640_IMAGE_SENSOR_2M_HEIGHT)&&(image_window->digital_zoom_factor >= 500))
		   {
		         SET_TG_OUTPUT_CLK_DIVIDER(7);   // 104MHz/8 = 13MHz MCLK
                       SET_CMOS_RISING_EDGE(0);
                       SET_CMOS_FALLING_EDGE(4);
	                ENABLE_CAMERA_PIXEL_CLKIN_ENABLE;	// enable PCLK latch
		         OV3640_FULL_dummy_pixels = 200;
		         OV3640_FULL_dummy_lines = 0;
		         OV3640_capture_pclk = 28;//23833334;
		   }
	else if ((image_window->image_target_width == OV3640_IMAGE_SENSOR_3M_WIDTH)&&
              (image_window->image_target_height == OV3640_IMAGE_SENSOR_3M_HEIGHT)&&(image_window->digital_zoom_factor >= 300))
               {
                       SET_TG_OUTPUT_CLK_DIVIDER(7);   // 104MHz/8 = 13MHz MCLK
                       SET_CMOS_RISING_EDGE(0);
                       SET_CMOS_FALLING_EDGE(4);
  	                ENABLE_CAMERA_PIXEL_CLKIN_ENABLE;	// enable PCLK latch
			  OV3640_FULL_dummy_pixels = 200;
		  	  OV3640_FULL_dummy_lines = 0;
			  OV3640_capture_pclk = 28;//23833334;
		  }
	else
#endif 	    
  	        {
                      //input 26Mhz, PCLK 47.67Mhz, 6.4fps
                     OV3640_write_cmos_sensor(0x300e,0x32);	   
                     OV3640_write_cmos_sensor(0x300f,0x21);						 
                     OV3640_write_cmos_sensor(0x3010,0x21);						 
                     OV3640_write_cmos_sensor(0x3011,0x02);	//01->02					 
                     OV3640_write_cmos_sensor(0x304c,0x81);	  //reserve	

		     //48Mhz Full size Capture CLK/2
		       if (zoom_factor < 2)
		       {		       
       	           OV3640_write_cmos_sensor(0x3011, 0x02);//01->02
 		           OV3640_write_cmos_sensor(0x300e, 0x32);  
     	                  OV3640_capture_pclk = 430;//606; //485;//23833334;		           
     	                  OV3640_FULL_dummy_pixels = 0; 
		       }
		       else if (zoom_factor < 5)
		       {
       	           OV3640_write_cmos_sensor(0x3011, 0x02);
 		           OV3640_write_cmos_sensor(0x300e, 0x35);  
     	                  OV3640_capture_pclk = 317;//254;//23833334;		           
     	                  OV3640_FULL_dummy_pixels = 0;//200; 
		       }
		       else 
		       {
       	     OV3640_write_cmos_sensor(0x3011, 0x03);
 		           OV3640_write_cmos_sensor(0x300e, 0x35);  
     	                  OV3640_capture_pclk = 238;//190;//23833334;		           		       
   	                  OV3640_FULL_dummy_pixels = 0;//200;

		       }
			OV3640_FULL_dummy_lines = 0;	
		 }
		
		ov3640_calc_mipiclk(); 
		//Step 4.calculate capture shutter

        //shutter = (shutter*OV3640_capture_pclk)/OV3640_preview_pclk;
		//shutter = (shutter*(OV3640_PV_PERIOD_PIXEL_NUMS+OV3640_PV_dummy_pixels))/(OV3640_FULL_PERIOD_PIXEL_NUMS+OV3640_FULL_dummy_pixels);
		//kal_prompt_trace(MOD_ENG,"shutter=%d",shutter);
#if 0
		shutter = (shutter*(OV3640_PV_PERIOD_PIXEL_NUMS+OV3640_PV_dummy_pixels))/(OV3640_FULL_PERIOD_PIXEL_NUMS+OV3640_FULL_dummy_pixels);
		//kal_prompt_trace(MOD_ENG,"shutter=%d",shutter);
		shutter = (shutter*OV3640_capture_pclk)/OV3640_preview_pclk;
        //shutter = shutter*48/56;
		//shutter =  (shutter*OV3640_capture_pclk)/OV3640_preview_pclk;//(shutter*48/56);
		//kal_prompt_trace(MOD_ENG,"shutter=%d",shutter);
		shutter = shutter*2;
#else
              temp_shutter = (shutter*(OV3640_PV_PERIOD_PIXEL_NUMS+OV3640_PV_dummy_pixels)*OV3640_capture_pclk)/(OV3640_FULL_PERIOD_PIXEL_NUMS+OV3640_FULL_dummy_pixels)/OV3640_preview_pclk;	
              shutter = (kal_uint32)(temp_shutter);
		shutter = shutter*2; 		
		
		exposure = shutter * pv_gain; 
		//printk("Shutter1 = %d\n", shutter); 
		//printk("Gain1 = %d\n", pv_gain); 
               //printk("[OV3640YUV] Exposure = %d\n", exposure); 

              if (OV3640_Banding_setting == AE_FLICKER_MODE_50HZ)
              {
//                  Capture_Banding_Filter =10 / ((OV3640_FULL_PERIOD_PIXEL_NUMS+OV3640_FULL_dummy_pixels)  * 1000 / (OV3640_capture_pclk * 100000)); 
                  
                  Capture_Banding_Filter = (10 * (OV3640_capture_pclk * 100)) / (OV3640_FULL_PERIOD_PIXEL_NUMS + OV3640_FULL_dummy_pixels);
                  //Capture_Banding_Filter = (kal_uint32)(OV3640_capture_pclk*100000/100/(2*(OV3640_FULL_PERIOD_PIXEL_NUMS+OV3640_FULL_dummy_pixels) ));
                  printk("50Hz Banding_Filter = %d\n", Capture_Banding_Filter); 

              }
              else
              {              
//                  Capture_Banding_Filter =1000/120 / ((OV3640_FULL_PERIOD_PIXEL_NUMS+OV3640_FULL_dummy_pixels)  * 1000 / (OV3640_capture_pclk * 100000)); 
                  Capture_Banding_Filter = (83 * (OV3640_capture_pclk * 100)) / (OV3640_FULL_PERIOD_PIXEL_NUMS + OV3640_FULL_dummy_pixels)/ 10;
                  //Capture_Banding_Filter = (kal_uint32)(OV3640_capture_pclk*100000/120/(2*(OV3640_FULL_PERIOD_PIXEL_NUMS+OV3640_FULL_dummy_pixels) ));
                  printk("60Hz Banding Filter =  %d\n", Capture_Banding_Filter); 
              }
              
              if (shutter > Capture_Banding_Filter)
              {
                  shutter = shutter / Capture_Banding_Filter * Capture_Banding_Filter; 
              }

              //! Avoid divide by zero 
              if (shutter == 0)
              {
                  shutter = 1; 
              }

              pv_gain = exposure / shutter; 
              
              

              //OV3640_Computer_AECAGC(OV3640_preview_pclk, OV3640_capture_pclk);
		//shutter = OV3640_Capture_Shutter + OV3640_Capture_Extra_Lines;

		//printk("Shutter2 = %d\n", shutter); 
		//printk("Gain2 = %d\n", OV3640_Capture_Gain16); 
		//printk("Exposure2 = %d\n", shutter * OV3640_Capture_Gain16); 

#endif

		//kal_prompt_trace(MOD_ENG,"shutter=%d",shutter);

		
	//	OV3640_Computer_AECAGC(1,0);	//Calculate the capture gain, shutter and extra exposure line.

		/* Config the exposure window. */
		image_window->GrabStartX = 1;
		image_window->GrabStartY = 2;
		image_window->ImageTargetWidth=OV3640_IMAGE_SENSOR_FULL_WIDTH;
		image_window->ImageTargetHeight=OV3640_IMAGE_SENSOR_FULL_HEIGHT;

		
		// set dummy
	    OV3640_set_dummy(OV3640_FULL_dummy_pixels, OV3640_FULL_dummy_lines);
           //kal_prompt_trace(MOD_ENG,"shutter=%d",shutter);
            if (OV3640_AE_ENABLE  == TRUE)
            {
                Capture_Shutter = shutter; 
                Capture_Gain = pv_gain; 
	    }
            OV3640_write_shutter(Capture_Shutter);
            OV3640_write_gain(Capture_Gain);
            //printk("Cap Shutter:%d  Gain:%d\n", Capture_Shutter, Capture_Gain);	    
	    //OV3640_write_gain(OV3640_Capture_Gain16); 
       // kal_prompt_trace(MOD_ENG, "cap write:shutter=%d, gain=%d",shutter, pv_gain);
       // kal_prompt_trace(MOD_ENG,"Register 0x3001=0x%x",OV3640_read_cmos_sensor(0x3001));
	//	kal_prompt_trace(MOD_ENG,"Register 0x3002=0x%x",OV3640_read_cmos_sensor(0x3002));
	//	kal_prompt_trace(MOD_ENG,"Register 0x3003=0x%x",OV3640_read_cmos_sensor(0x3003));
     //   kal_prompt_trace(MOD_ENG,"out capture function ");
	//	camera_wait_sensor_vd_done();		//Wait at least  one frame.
	//	kal_sleep_task(100);
		
    }
    return TRUE; 

}	/* OV3640_Capture */



UINT32 OV3640GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
//due to preview/capture view angle is different, cut off 80*60, too many? 
	pSensorResolution->SensorFullWidth=OV3640_IMAGE_SENSOR_FULL_WIDTH - 80;////2 //* IMAGE_SENSOR_START_GRAB_X - 8;  //modify by yanxu
	pSensorResolution->SensorFullHeight=OV3640_IMAGE_SENSOR_FULL_HEIGHT - 60;////2 * IMAGE_SENSOR_START_GRAB_Y - 6;
	pSensorResolution->SensorPreviewWidth=OV3640_IMAGE_SENSOR_PV_WIDTH - 2 * IMAGE_SENSOR_START_GRAB_X - 16;
	pSensorResolution->SensorPreviewHeight=OV3640_IMAGE_SENSOR_PV_HEIGHT - 2 * IMAGE_SENSOR_START_GRAB_Y - 12;

	return ERROR_NONE;
}	/* OV3640GetResolution() */



UINT32 OV3640GetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
					  MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	pSensorInfo->SensorPreviewResolutionX=OV3640_IMAGE_SENSOR_PV_WIDTH - 8;
	pSensorInfo->SensorPreviewResolutionY=OV3640_IMAGE_SENSOR_PV_HEIGHT - 6;
	pSensorInfo->SensorFullResolutionX=OV3640_IMAGE_SENSOR_FULL_WIDTH - 2 * IMAGE_SENSOR_START_GRAB_X - 16; //OV3640_IMAGE_SENSOR_FULL_WIDTH;
	pSensorInfo->SensorFullResolutionY=OV3640_IMAGE_SENSOR_FULL_HEIGHT - 2 * IMAGE_SENSOR_START_GRAB_Y - 12; //OV3640_IMAGE_SENSOR_FULL_HEIGHT;

	pSensorInfo->SensorCameraPreviewFrameRate=30;
	pSensorInfo->SensorVideoFrameRate=30;
	pSensorInfo->SensorStillCaptureFrameRate=10;
	pSensorInfo->SensorWebCamCaptureFrameRate=15;
	pSensorInfo->SensorResetActiveHigh=FALSE;
	pSensorInfo->SensorResetDelayCount=1;
	pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_YVYU;//SENSOR_OUTPUT_FORMAT_YUYV;
	pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW;	/*??? */
	pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorInterruptDelayLines = 1;
	pSensorInfo->SensroInterfaceType=SENSOR_INTERFACE_TYPE_PARALLEL;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxWidth=CAM_SIZE_5M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxHeight=CAM_SIZE_5M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxWidth=CAM_SIZE_5M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxHeight=CAM_SIZE_5M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxWidth=CAM_SIZE_5M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxHeight=CAM_SIZE_5M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxWidth=CAM_SIZE_1M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxHeight=CAM_SIZE_1M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].BinningEnable=TRUE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxWidth=CAM_SIZE_1M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxHeight=CAM_SIZE_1M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].BinningEnable=TRUE;

	pSensorInfo->CaptureDelayFrame = 2; 
	pSensorInfo->PreviewDelayFrame = 3; 
	pSensorInfo->VideoDelayFrame = 10; 
	pSensorInfo->SensorMasterClockSwitch = 0; 
       pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_6MA;//ISP_DRIVING_2MA;   		
	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			pSensorInfo->SensorClockFreq=26;
			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 3;
			pSensorInfo->SensorDataLatchCount= 2;
                     pSensorInfo->SensorGrabStartX = 3; 
                     pSensorInfo->SensorGrabStartY = 3;     			
			
		break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			pSensorInfo->SensorClockFreq=26;
			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 3;
			pSensorInfo->SensorDataLatchCount= 2;
                     pSensorInfo->SensorGrabStartX = 3; 
                     pSensorInfo->SensorGrabStartY = 3;     			
		break;
		default:
			pSensorInfo->SensorClockFreq=26;
			pSensorInfo->SensorClockDividCount=3;
			pSensorInfo->SensorClockRisingCount=0;
			pSensorInfo->SensorClockFallingCount=2;
			pSensorInfo->SensorPixelClockCount=3;
			pSensorInfo->SensorDataLatchCount=2;
                     pSensorInfo->SensorGrabStartX = 3; 
                     pSensorInfo->SensorGrabStartY = 3;     			
		break;
	}
	OV3640_PixelClockDivider=pSensorInfo->SensorPixelClockCount;
	memcpy(pSensorConfigData, &OV3640SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	return ERROR_NONE;
}	/* OV3640GetInfo() */

static kal_bool OV3640_sensor_burstcap_flag = KAL_FALSE; //Preview or Capture

UINT32 OV3640Control(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    switch (ScenarioId)
    {
    case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
    case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
    case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
        OV3640Preview(pImageWindow, pSensorConfigData);
        OV3640_sensor_burstcap_flag = KAL_FALSE;
        break;
    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
        if(!OV3640_sensor_burstcap_flag)
        {
            OV3640Capture(pImageWindow, pSensorConfigData);
            OV3640_sensor_burstcap_flag = KAL_TRUE;
        }
        break;
    default:
        break; 
    }
    return TRUE;
}	/* OV3640Control() */

/* [TC] YUV sensor */	
#if WINMO_USE
void OV3640Query(PMSDK_FEATURE_INFO_STRUCT pSensorFeatureInfo)
{
	MSDK_FEATURE_TYPE_RANGE_STRUCT *pFeatureRange;
	MSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT *pFeatureMultiSelection;
	switch (pSensorFeatureInfo->FeatureId)
	{
		case ISP_FEATURE_DSC_MODE:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_SCENE_MODE_MAX;
			pFeatureMultiSelection->DefaultSelection = CAM_AUTO_DSC_MODE;
			pFeatureMultiSelection->SupportedSelection = 
				(CAMERA_FEATURE_SUPPORT(CAM_AUTO_DSC_MODE)|
				CAMERA_FEATURE_SUPPORT(CAM_NIGHTSCENE_MODE));			
		break;
		case ISP_FEATURE_WHITEBALANCE:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_WB;
			pFeatureMultiSelection->DefaultSelection = CAM_WB_AUTO;
			pFeatureMultiSelection->SupportedSelection = 
				(CAMERA_FEATURE_SUPPORT(CAM_WB_AUTO)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_CLOUD)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_DAYLIGHT)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_INCANDESCENCE)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_TUNGSTEN)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_FLUORESCENT));
		break;
		case ISP_FEATURE_IMAGE_EFFECT:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_EFFECT_ENC;
			pFeatureMultiSelection->DefaultSelection = CAM_EFFECT_ENC_NORMAL;
			pFeatureMultiSelection->SupportedSelection = 
				(CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_NORMAL)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_GRAYSCALE)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_COLORINV)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_SEPIAGREEN)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_SEPIABLUE)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_SEPIA));	
		break;
		case ISP_FEATURE_AE_METERING_MODE:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
		case ISP_FEATURE_BRIGHTNESS:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_RANGE;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureRange = (PMSDK_FEATURE_TYPE_RANGE_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureRange);
			pFeatureRange->MinValue = CAM_EV_NEG_4_3;
			pFeatureRange->MaxValue = CAM_EV_POS_4_3;
			pFeatureRange->StepValue = CAMERA_FEATURE_ID_EV_STEP;
			pFeatureRange->DefaultValue = CAM_EV_ZERO;
		break;
		case ISP_FEATURE_BANDING_FREQ:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_BANDING;
			pFeatureMultiSelection->DefaultSelection = CAM_BANDING_50HZ;
			pFeatureMultiSelection->SupportedSelection = 
				(CAMERA_FEATURE_SUPPORT(CAM_BANDING_50HZ)|
				CAMERA_FEATURE_SUPPORT(CAM_BANDING_60HZ));
		break;
		case ISP_FEATURE_AF_OPERATION:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
		case ISP_FEATURE_AF_RANGE_CONTROL:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
		case ISP_FEATURE_FLASH:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;			
		break;
		case ISP_FEATURE_VIDEO_SCENE_MODE:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_SCENE_MODE_MAX;
			pFeatureMultiSelection->DefaultSelection = CAM_VIDEO_AUTO_MODE;
			pFeatureMultiSelection->SupportedSelection = 
				(CAMERA_FEATURE_SUPPORT(CAM_VIDEO_AUTO_MODE)|
				CAMERA_FEATURE_SUPPORT(CAM_VIDEO_NIGHT_MODE));
		break;
		case ISP_FEATURE_ISO:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;			
		break;
		default:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;			
		break;
	}
}
#endif 

/*************************************************************************
* FUNCTION
*	OV3640_set_param_wb
*
* DESCRIPTION
*	wb setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL OV3640_set_param_wb(UINT16 para)
{
	
    //This sensor need more time to balance AWB, 
    //we suggest higher fps or drop some frame to avoid garbage color when preview initial

	switch (para)
	{
	        case AWB_MODE_OFF:
	            OV3640_AWB_ENABLE = KAL_FALSE; 
                    OV3640_set_AWB_mode(OV3640_AWB_ENABLE);
	            break;             
		case AWB_MODE_AUTO:
                    OV3640_AWB_ENABLE = KAL_TRUE; 
                    OV3640_set_AWB_mode(OV3640_AWB_ENABLE);    
		    OV3640_write_cmos_sensor(0x332b, 0x00);   // Enable AWB Auto mode
		    break;
		case AWB_MODE_CLOUDY_DAYLIGHT:
		    OV3640_write_cmos_sensor(0x332b, 0x08);  // Manual AWB mode
		    OV3640_write_cmos_sensor(0x33a7, 0x68);  //R_GAIN_M
	   	    OV3640_write_cmos_sensor(0x33a8, 0x40);  //G_GAIN_M
		    OV3640_write_cmos_sensor(0x33a9, 0x4e);  //B_GAIN_M
		    break;
		case AWB_MODE_DAYLIGHT:
		    OV3640_write_cmos_sensor(0x332b, 0x08);
                  OV3640_write_cmos_sensor(0x33a7, 0x5e);
                  OV3640_write_cmos_sensor(0x33a8, 0x40);
                  OV3640_write_cmos_sensor(0x33a9, 0x46);
		    break;
		case AWB_MODE_INCANDESCENT:	
		    OV3640_write_cmos_sensor(0x332b, 0x08);
		    OV3640_write_cmos_sensor(0x33a7, 0x52);
		    OV3640_write_cmos_sensor(0x33a8, 0x40);
		    OV3640_write_cmos_sensor(0x33a9, 0x58);	
		    break;  
		case AWB_MODE_FLUORESCENT:
		    OV3640_write_cmos_sensor(0x332b, 0x08);
                  OV3640_write_cmos_sensor(0x33a7, 0x52);
                  OV3640_write_cmos_sensor(0x33a8, 0x40);
                  OV3640_write_cmos_sensor(0x33a9, 0x58);
		    break;  
		case AWB_MODE_TUNGSTEN:
		    OV3640_write_cmos_sensor(0x332b, 0x08);
                  OV3640_write_cmos_sensor(0x33a7, 0x44);
                  OV3640_write_cmos_sensor(0x33a8, 0x40);
                  OV3640_write_cmos_sensor(0x33a9, 0x70);
		    break;
#if WINMO_USE		
		case CAM_WB_MANUAL:
		    // TODO
			break;
#endif 
		default:
			return FALSE;
	}

	return TRUE;
	
} /* OV3640_set_param_wb */

/*************************************************************************
* FUNCTION
*	OV3640_set_param_effect
*
* DESCRIPTION
*	effect setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL OV3640_set_param_effect(UINT16 para)
{
    kal_uint32  ret = TRUE;
    kal_uint16 temp=OV3640_read_cmos_sensor(0x3302);
            OV3640_write_cmos_sensor(0x3302,temp|0x80);
	switch (para)
	{
		case MEFFECT_OFF:
			//OV3640_write_cmos_sensor(0x3302, 0xef); 
			OV3640_write_cmos_sensor(0x3355, 0x04);
			OV3640_write_cmos_sensor(0x335a, 0x80);
                     OV3640_write_cmos_sensor(0x335b, 0x80);
	              break;
		case MEFFECT_SEPIA:
			//OV3640_write_cmos_sensor(0x3302, 0xef);
                     OV3640_write_cmos_sensor(0x3355, 0x18);
                     OV3640_write_cmos_sensor(0x335a, 0x40);
                     OV3640_write_cmos_sensor(0x335b, 0xa6);
			break;  
		case MEFFECT_NEGATIVE:
			//OV3640_write_cmos_sensor(0x3302, 0xef);
                     OV3640_write_cmos_sensor(0x3355, 0x40);
                     OV3640_write_cmos_sensor(0x335a, 0x80);       //Ureg[7:0]
                     OV3640_write_cmos_sensor(0x335b, 0x80);       //Vreg[7:0]
			break; 
		case MEFFECT_SEPIAGREEN:		
			//OV3640_write_cmos_sensor(0x3302, 0xef);		
	              OV3640_write_cmos_sensor(0x3355, 0x18);		
		       OV3640_write_cmos_sensor(0x335a, 0x60);		
		       OV3640_write_cmos_sensor(0x335b, 0x60);		
			break;
		case MEFFECT_SEPIABLUE:
			//OV3640_write_cmos_sensor(0x3302, 0xef);
                    OV3640_write_cmos_sensor(0x3355, 0x18);
                     OV3640_write_cmos_sensor(0x335a, 0xa0);
                     OV3640_write_cmos_sensor(0x335b, 0x40);
			break;        
		case MEFFECT_MONO:			
			//OV3640_write_cmos_sensor(0x3302, 0xef);		
		       OV3640_write_cmos_sensor(0x3355, 0x18);		
			OV3640_write_cmos_sensor(0x335a, 0x80);		
			OV3640_write_cmos_sensor(0x335b, 0x80);		  
			break;
#if WINMO_USE
		case CAM_EFFECT_ENC_GRAYINV:
		case CAM_EFFECT_ENC_COPPERCARVING:
        case CAM_EFFECT_ENC_BLUECARVING:
		case CAM_EFFECT_ENC_CONTRAST:


		case CAM_EFFECT_ENC_EMBOSSMENT:
		case CAM_EFFECT_ENC_SKETCH:
		case CAM_EFFECT_ENC_BLACKBOARD:
		case CAM_EFFECT_ENC_WHITEBOARD:
		case CAM_EFFECT_ENC_JEAN:
		case CAM_EFFECT_ENC_OIL:
#endif 
		default:
			ret = FALSE;
	}

	return ret;

} /* OV3640_set_param_effect */

/*************************************************************************
* FUNCTION
*	OV3640_set_param_banding
*
* DESCRIPTION
*	banding setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL OV3640_set_param_banding(UINT16 para)
{
	kal_uint16 PV_Line_Width = 0; 
//	kal_uint16 Preview_Banding_Filter = 0;
//	kal_uint16 Exposure_Step = 0;

	PV_Line_Width = OV3640_FULL_PERIOD_PIXEL_NUMS + OV3640_PV_dummy_pixels*2; 
    //kal_prompt_trace(MOD_ENG,"call banding function ");

/*	switch (para)
	{
		case CAM_BANDING_50HZ:
		//	Preview_Banding_Filter = OV3640_g_Preview_PCLK_Frequency*1000000/100/ (2*PV_Line_Width)+0.5;    //calculate result=237=0xed
		//	Exposure_Step = OV3640_PV_EXPOSURE_LIMITATION / Preview_Banding_Filter - 1;                             //calculate result=2

		//	OV3640_write_cmos_sensor(0x3014, 0x84); //bit[7]select 50/60hz banding, 0:60hz
	   	//	OV3640_write_cmos_sensor(0x3070, Preview_Banding_Filter >> 8); //50hz banding filter control
		//	OV3640_write_cmos_sensor(0x3071, Preview_Banding_Filter & 0x00FF);
		//	OV3640_write_cmos_sensor(0x301C, Exposure_Step & 0x3F); //50hz banding max steps
    
		    OV3640_write_cmos_sensor(0x3014, 0x84); //bit[7]select 50/60hz banding, 0:60hz	
			OV3640_write_cmos_sensor(0x3070, 0x00); //50Hz banding filter 8 MSB	
		    OV3640_write_cmos_sensor(0x3071, 0xed); //50Hz banding filter value 8 LSB		
	 	    OV3640_write_cmos_sensor(0x301c, 0x02); //50Hz maximum banding step
			break;

		case CAM_BANDING_60HZ:
		//	Preview_Banding_Filter = OV3640_g_Preview_PCLK_Frequency*1000000/120/ (2*PV_Line_Width)+0.5;   //calculate result=198=0xc6
		//	Exposure_Step = OV3640_PV_EXPOSURE_LIMITATION / Preview_Banding_Filter - 1;                            //calculate result=3

			OV3640_write_cmos_sensor(0x3014, 0x04); //bit[7]select 50/60hz banding, 0:60h
			OV3640_write_cmos_sensor(0x3072, Preview_Banding_Filter >> 8); //60hz banding filter control
			OV3640_write_cmos_sensor(0x3073, Preview_Banding_Filter & 0x00FF);     
			OV3640_write_cmos_sensor(0x301D, Exposure_Step & 0x3F); //60hz banding max steps	
			break;

	     default:
	          return KAL_FALSE;
	}
*/
	switch (para)
	{
		case AE_FLICKER_MODE_50HZ:
		    if(OV3640_Night_mode == KAL_TRUE)
            {
              if(OV3640_VEDIO_encode_mode == KAL_TRUE)
              {
                OV3640_write_cmos_sensor(0x3014,0x84);
                OV3640_write_cmos_sensor(0x3070,0x00);
			    OV3640_write_cmos_sensor(0x3071,0x76);
			    OV3640_write_cmos_sensor(0x301c,0x05);
              }
              else
              {
                OV3640_write_cmos_sensor(0x3014,0xac);
                OV3640_write_cmos_sensor(0x3070,0x00);
		        OV3640_write_cmos_sensor(0x3071,0xED);
		        OV3640_write_cmos_sensor(0x301c,0x02);
              }
            }
            else
            {
              if(OV3640_VEDIO_encode_mode == KAL_TRUE)
              {
                OV3640_write_cmos_sensor(0x3014,0x84);
                OV3640_write_cmos_sensor(0x3070,0x00);
			    OV3640_write_cmos_sensor(0x3071,0xED);
			    OV3640_write_cmos_sensor(0x301c,0x02);
              }
              else
              {
                OV3640_write_cmos_sensor(0x3014,0xac);
                OV3640_write_cmos_sensor(0x3070,0x00);
			    OV3640_write_cmos_sensor(0x3071,0xED);
			    OV3640_write_cmos_sensor(0x301c,0x02);
              }
            }
            OV3640_Banding_setting = AE_FLICKER_MODE_50HZ;

			break;

		case AE_FLICKER_MODE_60HZ:
		
			 if(OV3640_Night_mode == KAL_TRUE)
            {
              if(OV3640_VEDIO_encode_mode == KAL_TRUE)
              {
                OV3640_write_cmos_sensor(0x3014,0x04);
				OV3640_write_cmos_sensor(0x3072,0x00);
			    OV3640_write_cmos_sensor(0x3073,0x62);
			    OV3640_write_cmos_sensor(0x301d,0x07);
              }
              else
              {
                OV3640_write_cmos_sensor(0x3014,0x2c);
				OV3640_write_cmos_sensor(0x3072,0x00);
                OV3640_write_cmos_sensor(0x3073,0xC5);
		        OV3640_write_cmos_sensor(0x301d,0x03);
              }
            }
            else
            {
              if(OV3640_VEDIO_encode_mode == KAL_TRUE)
              {
			    OV3640_write_cmos_sensor(0x3014,0x04);
				OV3640_write_cmos_sensor(0x3072,0x00);
				OV3640_write_cmos_sensor(0x3073,0xC5);
				OV3640_write_cmos_sensor(0x301d,0x03); 
              }
              else
              {
                OV3640_write_cmos_sensor(0x3014,0x2c);			
				OV3640_write_cmos_sensor(0x3072,0x00);
			    OV3640_write_cmos_sensor(0x3073,0xC5);
			    OV3640_write_cmos_sensor(0x301d,0x03);
              }
            }
            OV3640_Banding_setting = AE_FLICKER_MODE_60HZ; 
			break;

	     default:
	          return KAL_FALSE;
	}

#if WINMO_USE	
	    kal_prompt_trace(MOD_ENG,"Banding 50Hz");
	    kal_prompt_trace(MOD_ENG,"Register 0x3001=0x%x",OV3640_read_cmos_sensor(0x3001));
		kal_prompt_trace(MOD_ENG,"Register 0x3002=0x%x",OV3640_read_cmos_sensor(0x3002));
		kal_prompt_trace(MOD_ENG,"Register 0x3003=0x%x",OV3640_read_cmos_sensor(0x3003));
/*	    kal_prompt_trace(MOD_ENG,"Register 0x3014=0x%x",OV3640_read_cmos_sensor(0x3014));
		kal_prompt_trace(MOD_ENG,"Register 0x3015=0x%x",OV3640_read_cmos_sensor(0x3015));
		kal_prompt_trace(MOD_ENG,"Register 0x3011=0x%x",OV3640_read_cmos_sensor(0x3011));
		kal_prompt_trace(MOD_ENG,"Register 0x3071=0x%x",OV3640_read_cmos_sensor(0x3071));
		kal_prompt_trace(MOD_ENG,"Register 0x3073=0x%x",OV3640_read_cmos_sensor(0x3073));
		kal_prompt_trace(MOD_ENG,"Register 0x301c=0x%x",OV3640_read_cmos_sensor(0x301c));
		kal_prompt_trace(MOD_ENG,"Register 0x301d=0x%x",OV3640_read_cmos_sensor(0x301d));
		kal_prompt_trace(MOD_ENG,"Register 0x302d=0x%x",OV3640_read_cmos_sensor(0x302d));
		kal_prompt_trace(MOD_ENG,"Register 0x302e=0x%x",OV3640_read_cmos_sensor(0x302e));
*/
        kal_prompt_trace(MOD_ENG,"out banding function ");
#endif 
	return KAL_TRUE;
} /* OV3640_set_param_banding */




/*************************************************************************
* FUNCTION
*	OV3640_set_param_exposure
*
* DESCRIPTION
*	exposure setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL OV3640_set_param_exposure(UINT16 para)
{
    kal_uint8  temp_reg;    
   kal_uint16 temp=OV3640_read_cmos_sensor(0x3302);
   OV3640_write_cmos_sensor(0x3302,temp|0x80);

   //kal_uint8  temp_reg;
   temp_reg=OV3640_read_cmos_sensor(0x3355);
	switch (para)
	{
		case AE_EV_COMP_13:
			//OV3640_write_cmos_sensor(0x3302, 0xef);           
			OV3640_write_cmos_sensor(0x3355, temp_reg  |0x04);   //SDE_CTRL[2]:contrast enable - refer to register YGAIN(0x335D)        
			OV3640_write_cmos_sensor(0x3354, 0x01);   //SGNSET
			OV3640_write_cmos_sensor(0x335e, 0x20);    //YBright[7:0]
			break;  
		case AE_EV_COMP_10:
			//OV3640_write_cmos_sensor(0x3302, 0xef);
                     OV3640_write_cmos_sensor(0x3355, temp_reg  |0x04);
                     OV3640_write_cmos_sensor(0x3354, 0x01);
                     OV3640_write_cmos_sensor(0x335e, 0x18);
			break;    
		case AE_EV_COMP_07:
			//OV3640_write_cmos_sensor(0x3302, 0xef);
                     OV3640_write_cmos_sensor(0x3355, temp_reg  |0x04);
                     OV3640_write_cmos_sensor(0x3354, 0x01);
                     OV3640_write_cmos_sensor(0x335e, 0x10);
			break;    
		case AE_EV_COMP_03:			
			//OV3640_write_cmos_sensor(0x3302, 0xef);			
			OV3640_write_cmos_sensor(0x3355, temp_reg  |0x04);		
			OV3640_write_cmos_sensor(0x3354, 0x01);		
		       OV3640_write_cmos_sensor(0x335e, 0x08);		
			break;    
		case AE_EV_COMP_00:
			//OV3640_write_cmos_sensor(0x3302, 0xef);
                     OV3640_write_cmos_sensor(0x3355, temp_reg  |0x04);
                     OV3640_write_cmos_sensor(0x3354, 0x01);
                     OV3640_write_cmos_sensor(0x335e, 0x00);
			break;    
		case AE_EV_COMP_n03:
			//OV3640_write_cmos_sensor(0x3302, 0xef);
                     OV3640_write_cmos_sensor(0x3355, temp_reg  |0x04);
                     OV3640_write_cmos_sensor(0x3354, 0x09);						
                     OV3640_write_cmos_sensor(0x335e, 0x08);
			break;    
		case AE_EV_COMP_n07:			
			//OV3640_write_cmos_sensor(0x3302, 0xef);			
    	              OV3640_write_cmos_sensor(0x3355, temp_reg  |0x04);		
    	              OV3640_write_cmos_sensor(0x3354, 0x09);		
			OV3640_write_cmos_sensor(0x335e, 0x10);		
			break;    
		case AE_EV_COMP_n10:
			//OV3640_write_cmos_sensor(0x3302, 0xef);
                     OV3640_write_cmos_sensor(0x3355, temp_reg  |0x04);
                     OV3640_write_cmos_sensor(0x3354, 0x09);
                     OV3640_write_cmos_sensor(0x335e, 0x18);
			break;
		case AE_EV_COMP_n13:
			//OV3640_write_cmos_sensor(0x3302, 0xef);
                     OV3640_write_cmos_sensor(0x3355, temp_reg  |0x04);
                     OV3640_write_cmos_sensor(0x3354, 0x09);
                     OV3640_write_cmos_sensor(0x335e, 0x20);
			break;
		default:
			return FALSE;
	}

	return TRUE;
	
} /* OV3640_set_param_exposure */


UINT32 OV3640YUVSensorSetting(FEATURE_ID iCmd, UINT32 iPara)
{
//   if( OV3640_sensor_cap_state == KAL_TRUE)
//	   return TRUE;

	switch (iCmd) {
	case FID_SCENE_MODE:	    
//	    printk("Set Scene Mode:%d\n", iPara); 
	    if (iPara == SCENE_MODE_OFF)
	    {
	        OV3640_night_mode(0); 
	    }
	    else if (iPara == SCENE_MODE_NIGHTSCENE)
	    {
               OV3640_night_mode(1); 
	    }	    
	    break; 	    
	case FID_AWB_MODE:
//	    printk("Set AWB Mode:%d\n", iPara); 	    
           OV3640_set_param_wb(iPara);
	break;
	case FID_COLOR_EFFECT:
//	    printk("Set Color Effect:%d\n", iPara); 	    	    
           OV3640_set_param_effect(iPara);
	break;
	case FID_AE_EV:
#if WINMO_USE	    
	case ISP_FEATURE_EXPOSURE:
#endif 	    
//           printk("Set EV:%d\n", iPara); 	    	    
           OV3640_set_param_exposure(iPara);
	break;
	case FID_AE_FLICKER:
//           printk("Set Flicker:%d\n", iPara); 	    	    	    
           OV3640_set_param_banding(iPara);
	break;
	case FID_AE_SCENE_MODE: 
	    if (iPara == AE_MODE_OFF) {
                OV3640_AE_ENABLE = KAL_FALSE; 
            }
            else {
                OV3640_AE_ENABLE = KAL_TRUE; 
	    }
            OV3640_set_AE_mode(OV3640_AE_ENABLE);
            break; 

	case FID_ZOOM_FACTOR:
	    zoom_factor = iPara; 		
	break; 
	default:
	break;
	}
	return TRUE;
}   /* OV3640YUVSensorSetting */

UINT32 OV3640YUVSetVideoMode(UINT16 u2FrameRate)
{
    //kal_uint8 iTemp;
    printk("[OV2640YUV] Set Video Mode \n"); 
    OV3640_VEDIO_encode_mode = KAL_TRUE; 

    //iTemp = OV3640_read_cmos_sensor(0x3014);
    //OV3640_write_cmos_sensor(0x3014, iTemp & 0xf7); //Disable night mode

    if (u2FrameRate == 30)
    {
        /* MJPEG or MPEG4 Apps */
        // set max gain to 4x
        // OV3640_write_cmos_sensor(0x3014,0x84);
        /* to fix VSYNC, to fix frame rate */
    
        OV3640_write_cmos_sensor(0x3015,0x01);
        OV3640_write_cmos_sensor(0x3011,0x00);
        // OV3640_write_cmos_sensor(0x3071,0xED);
        // OV3640_write_cmos_sensor(0x3073,0xC5);
        // OV3640_write_cmos_sensor(0x301c,0x02);
        // OV3640_write_cmos_sensor(0x301d,0x03);
        OV3640_write_cmos_sensor(0x302d,0x00);
        OV3640_write_cmos_sensor(0x302e,0x00);
    }
    else if (u2FrameRate == 15)
    {
        // set Max gain to 16X
        OV3640_write_cmos_sensor(0x3015, 0x01); //jerry
        OV3640_write_cmos_sensor(0x3011,0x01);
        // clear extra exposure line
	 OV3640_write_cmos_sensor(0x302d, 0x00);
	 OV3640_write_cmos_sensor(0x302e, 0x00);
    }
    else 
    {
        printk("Wrong Frame Rate \n"); 
    }
    
    
    return TRUE;
}

UINT32 OV3640FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
							 UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
    //UINT16 u2Temp = 0; 
    UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
    UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
    UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
    UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
    MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
    MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;

    switch (FeatureId)
    {
        case SENSOR_FEATURE_GET_RESOLUTION:
            *pFeatureReturnPara16++=OV3640_IMAGE_SENSOR_FULL_WIDTH;
            *pFeatureReturnPara16=OV3640_IMAGE_SENSOR_FULL_HEIGHT;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_GET_PERIOD:
            *pFeatureReturnPara16++=OV3640_PV_PERIOD_PIXEL_NUMS+OV3640_PV_dummy_pixels;
            *pFeatureReturnPara16=OV3640_PV_PERIOD_LINE_NUMS+OV3640_PV_dummy_lines;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
            *pFeatureReturnPara32 = OV3640_sensor_pclk/10;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_SET_ESHUTTER:
            //			u2Temp = OV3640_read_shutter(); 
            //			printk("Shutter:%d\n", u2Temp); 			
            break;
        case SENSOR_FEATURE_SET_NIGHTMODE:
            OV3640_night_mode((BOOL) *pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_GAIN:
            //			u2Temp = OV3640_read_gain(); 
            //			printk("Gain:%d\n", u2Temp); 
            //			printk("y_val:%d\n", OV3640_read_cmos_sensor(0x301B));
            break; 
        case SENSOR_FEATURE_SET_FLASHLIGHT:
            break;
        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
            OV3640_isp_master_clock=*pFeatureData32;
            break;
        case SENSOR_FEATURE_SET_REGISTER:
            OV3640_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
            break;
        case SENSOR_FEATURE_GET_REGISTER:
            pSensorRegData->RegData = OV3640_read_cmos_sensor(pSensorRegData->RegAddr);
            break;
        case SENSOR_FEATURE_GET_CONFIG_PARA:
            memcpy(pSensorConfigData, &OV3640SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
            *pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
            break;
        case SENSOR_FEATURE_SET_CCT_REGISTER:
        case SENSOR_FEATURE_GET_CCT_REGISTER:
        case SENSOR_FEATURE_SET_ENG_REGISTER:
        case SENSOR_FEATURE_GET_ENG_REGISTER:
        case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
        
        case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
        case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
        case SENSOR_FEATURE_GET_GROUP_INFO:
        case SENSOR_FEATURE_GET_ITEM_INFO:
        case SENSOR_FEATURE_SET_ITEM_INFO:
        case SENSOR_FEATURE_GET_ENG_INFO:
            break;
        case SENSOR_FEATURE_GET_GROUP_COUNT:
            *pFeatureReturnPara32++=0;
            *pFeatureParaLen=4;
            break; 
        
        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
            // get the lens driver ID from EEPROM or just return LENS_DRIVER_ID_DO_NOT_CARE
            // if EEPROM does not exist in camera module.
            *pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_CHECK_SENSOR_ID:
            OV3640GetSensorID(pFeatureReturnPara32); 
            break;             
        case SENSOR_FEATURE_SET_YUV_CMD:
            //		       printk("OV3640 YUV sensor Setting:%d, %d \n", *pFeatureData32,  *(pFeatureData32+1));
            OV3640YUVSensorSetting((FEATURE_ID)*pFeatureData32, *(pFeatureData32+1));
            break;
        case SENSOR_FEATURE_SET_VIDEO_MODE:
            OV3640YUVSetVideoMode(*pFeatureData16);
            break; 
        case SENSOR_FEATURE_GET_EV_AWB_REF:
            OV3640GetEvAwbRef(*pFeatureData32);
            break;
        case SENSOR_FEATURE_GET_SHUTTER_GAIN_AWB_GAIN:
            OV3640GetCurAeAwbInfo(*pFeatureData32);	
            break;
        
        
        case SENSOR_FEATURE_INITIALIZE_AF:
            //ov3640, OV3640_FOCUS_AD5820_Init() may fail, 
            //MCU is off, or firmware is not correct.
            //move to the end of OV3640Open();
                      
            //OV3640_FOCUS_AD5820_Init();
            break;
        case SENSOR_FEATURE_MOVE_FOCUS_LENS:
            OV3640_FOCUS_AD5820_Move_to(*pFeatureData16);
            break;
        case SENSOR_FEATURE_GET_AF_STATUS:
            OV3640_FOCUS_AD5820_Get_AF_Status(pFeatureReturnPara32);            
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_GET_AF_INF:
            OV3640_FOCUS_AD5820_Get_AF_Inf(pFeatureReturnPara32);
            *pFeatureParaLen=4;            
            break;
        case SENSOR_FEATURE_GET_AF_MACRO:
            OV3640_FOCUS_AD5820_Get_AF_Macro(pFeatureReturnPara32);
            *pFeatureParaLen=4;            
            break;   				
        case SENSOR_FEATURE_SET_AF_WINDOW:
            //SENSORDB("SENSOR_FEATURE_SET_AF_WINDOW\n");
            //SENSORDB("get zone addr = 0x%x\n",*pFeatureData32);			
            OV3640_FOCUS_AD5820_Set_AF_Window(*pFeatureData32);
            break;
        case SENSOR_FEATURE_SINGLE_FOCUS_MODE:
            //SENSORDB("SENSOR_FEATURE_SINGLE_FOCUS_MODE\n");
            OV3640_FOCUS_AD5820_Single_Focus();
            break;	
        case SENSOR_FEATURE_CANCEL_AF:
            //SENSORDB("SENSOR_FEATURE_CANCEL_AF\n");
            OV3640_FOCUS_AD5820_Cancel_Focus();
            break;					
        case SENSOR_FEATURE_GET_AF_MAX_NUM_FOCUS_AREAS:
            OV3640_FOCUS_AD5820_Get_AF_Max_Num_Focus_Areas(pFeatureReturnPara32);            
            *pFeatureParaLen=4;
            break;        
        case SENSOR_FEATURE_GET_AE_MAX_NUM_METERING_AREAS:
            OV3640_FOCUS_AD5820_Get_AE_Max_Num_Metering_Areas(pFeatureReturnPara32);            
            *pFeatureParaLen=4;
            break;        
        case SENSOR_FEATURE_SET_AE_WINDOW:
            SENSORDB("SENSOR_FEATURE_SET_AE_WINDOW\n");
            SENSORDB("AE zone addr = 0x%x\n",*pFeatureData32);			
            OV3640_FOCUS_AD5820_Set_AE_Window(*pFeatureData32);
            break;  

        default:			
            break;			
    }
    return ERROR_NONE;
}	/* OV3640FeatureControl() */


SENSOR_FUNCTION_STRUCT	SensorFuncOV3640=
{
	OV3640Open,
	OV3640GetInfo,
	OV3640GetResolution,
	OV3640FeatureControl,
	OV3640Control,
	OV3640Close
};

UINT32 OV3640_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
	/* To Do : Check Sensor status here */
	if (pfFunc!=NULL)
		*pfFunc=&SensorFuncOV3640;

	return ERROR_NONE;
}	/* SensorInit() */


