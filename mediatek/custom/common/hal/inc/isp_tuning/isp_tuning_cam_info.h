#ifndef _ISP_TUNING_CAM_INFO_H_
#define _ISP_TUNING_CAM_INFO_H_


namespace NSIspTuning
{


/*******************************************************************************
* 
*******************************************************************************/

//  Scene index
typedef Fid2Type<FID_SCENE_MODE>::Type      EIndex_Scene_T;
enum { eNUM_OF_SCENE_IDX = Fid2Type<FID_SCENE_MODE>::Num };


//  Color Effect Index
typedef Fid2Type<FID_COLOR_EFFECT>::Type    EIndex_Effect_T;


//  ISP End-User-Define Tuning Index: 
//  Edge, Hue, Saturation, Brightness, Contrast
typedef Fid2Type<FID_ISP_EDGE    >::Type    EIndex_Isp_Edge_T;
typedef Fid2Type<FID_ISP_HUE     >::Type    EIndex_Isp_Hue_T;
typedef Fid2Type<FID_ISP_SAT     >::Type    EIndex_Isp_Saturation_T;
typedef Fid2Type<FID_ISP_BRIGHT  >::Type    EIndex_Isp_Brightness_T;
typedef Fid2Type<FID_ISP_CONTRAST>::Type    EIndex_Isp_Contrast_T;
typedef struct IspUsrSelectLevel
{
    EIndex_Isp_Edge_T           eIdx_Edge;
    EIndex_Isp_Hue_T            eIdx_Hue;
    EIndex_Isp_Saturation_T     eIdx_Sat;
    EIndex_Isp_Brightness_T     eIdx_Bright;
    EIndex_Isp_Contrast_T       eIdx_Contrast;

    IspUsrSelectLevel()
        : eIdx_Edge     (ISP_EDGE_MIDDLE)
        , eIdx_Hue      (ISP_HUE_MIDDLE)
        , eIdx_Sat      (ISP_SAT_MIDDLE)
        , eIdx_Bright   (ISP_BRIGHT_MIDDLE)
        , eIdx_Contrast (ISP_CONTRAST_MIDDLE)
    {}
} IspUsrSelectLevel_T;


//  ISO index.
typedef enum EIndex_ISO
{
    eIDX_ISO_100 = 0, 
    eIDX_ISO_200, 
    eIDX_ISO_400, 
    eIDX_ISO_800, 
    eIDX_ISO_1600, 
    eNUM_OF_ISO_IDX
} EIndex_ISO_T;


//  Correlated color temperature index for CCM.
typedef enum EIndex_CCM_CCT
{
    eIDX_CCM_CCT_BEGIN  = 0, 
    eIDX_CCM_CCT_TL84   = eIDX_CCM_CCT_BEGIN, 
    eIDX_CCM_CCT_CWF, 
    eIDX_CCM_CCT_D65
} EIndex_CCM_CCT_T;


//  Correlated color temperature index for shading.
typedef enum EIndex_Shading_CCT
{
    eIDX_Shading_CCT_BEGIN  = 0, 
    eIDX_Shading_CCT_ALight   = eIDX_Shading_CCT_BEGIN, 
    eIDX_Shading_CCT_CWF, 
    eIDX_Shading_CCT_D65
} EIndex_Shading_CCT_T;

// CCT estimation
typedef struct
{
	MINT32 i4CCT; // CCT
	MINT32 i4FluorescentIndex; // Fluorescent index
	MINT32 i4DaylightFluorescentIndex; // Daylight fluorescent index
	MINT32 i4DaylightProb; // Daylight probability
	MINT32 i4DaylightFluorescentProb; // Daylight fluorescent probability
	MINT32 i4SceneLV; // Scene LV
} ISP_CCT_T;


/*******************************************************************************
* 
*******************************************************************************/
struct IspCamInfo
{
public:
    ECamMode_T          eCamMode;   //  camera mode.

    EIndex_Scene_T      eIdx_Scene; //  scene mode.

public:
    IspCamInfo()
        : eCamMode(ECamMode_Online_Preview)
        , eIdx_Scene(SCENE_MODE_OFF)
    {}

public:
    void dump() const
    {
        LOGD("[IspCamInfo][dump](eCamMode, eIdx_Scene)=(%d, %d)", eCamMode, eIdx_Scene);
    }
};


/*******************************************************************************
* 
*******************************************************************************/
struct RAWIspCamInfo : public IspCamInfo
{
public:
    MUINT32             u4ISOValue;         //  iso value
    EIndex_ISO_T        eIdx_ISO;           //  iso index

    MINT32              i4CCT;              //  color temperature.
    EIndex_CCM_CCT_T    eIdx_CCM_CCT;       //  CT index for CCM.
    EIndex_Shading_CCT_T    eIdx_Shading_CCT;       //  CT index for Shading.

    /*
        x100 Zoom Ratio
    */
    MUINT32             u4ZoomRatio_x100;

    /*
        x10 "light value" or ¡§light level¡¨
    
        In photography, light value has been used to refer to a ¡§light level¡¨ 
        for either incident or reflected light, often on a base-2 logarithmic scale.
        The term does not derive from a published standard, and has had several 
        different meanings:
    */
    MINT32              i4LightValue_x10;

public:
    RAWIspCamInfo()
        : IspCamInfo()
        , u4ISOValue(0)
        , eIdx_ISO(eIDX_ISO_100)
        , i4CCT(0)
        , eIdx_CCM_CCT(eIDX_CCM_CCT_BEGIN)
        , eIdx_Shading_CCT(eIDX_Shading_CCT_CWF)
        , u4ZoomRatio_x100(0)
        , i4LightValue_x10(0)
    {}

public:
    void dump() const
    {
        IspCamInfo::dump();
        LOGD(
            "[RAWIspCamInfo][dump]"
            "(eIdx_ISO, u4ISOValue, i4CCT, eIdx_CCM_CCT, u4ZoomRatio_x100, i4LightValue_x10)"
            "=(%d, %d, %d, %d, %d, %d)"
            , eIdx_ISO, u4ISOValue, i4CCT, eIdx_CCM_CCT, u4ZoomRatio_x100, i4LightValue_x10
        );
    }
};


/*******************************************************************************
* 
*******************************************************************************/
struct YUVIspCamInfo : public IspCamInfo
{
public:
    YUVIspCamInfo()
        : IspCamInfo()
    {}
};


/*******************************************************************************
* 
*******************************************************************************/
};  //  NSIspTuning
#endif //  _ISP_TUNING_CAM_INFO_H_

