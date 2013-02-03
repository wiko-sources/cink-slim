//
#define LOG_TAG "isp_tuning_idx"
#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif
//
#include <utils/Errors.h>
#include <cutils/log.h>
//
#include "camera_custom_nvram.h"
#define USE_CUSTOM_ISP_TUNING
#include "isp_tuning.h"
#include "isp_tuning_custom_instance.h"
namespace NSIspTuning
{
#include "cfg_isp_tuning_idx_0.h"    //  main sensor.
#include "cfg_isp_tuning_idx_1.h"    //  sub sensor.
};
//
using namespace NSIspTuning;
//


/*******************************************************************************
* 
*******************************************************************************/
namespace
{
    enum EIndexNum
    {
        NUM_DM          =   NVRAM_DM_TBL_NUM, 
        NUM_DP          =   NVRAM_DP_TBL_NUM, 
        NUM_NR1         =   NVRAM_NR1_TBL_NUM, 
        NUM_NR2         =   NVRAM_NR2_TBL_NUM, 
        NUM_Saturation  =   NVRAM_SAT_TBL_NUM, 
        NUM_Contrast    =   NVRAM_CONTRAST_TBL_NUM, 
        NUM_Hue         =   NVRAM_HUE_TBL_NUM, 
        NUM_Gamma       =   NVRAM_GAMMA_TBL_NUM, 
        NUM_EE          =   NVRAM_EE_TBL_NUM, 
    };

    template <EIndexNum Num>
    inline MBOOL setIdx(UINT8 &rIdxTgt, UINT8 const IdxSrc)
    {
        if  (IdxSrc < Num)
        {
            rIdxTgt = IdxSrc;
            return  MTRUE;
        }
        return  MFALSE;
    }

};  //  namespace


/*******************************************************************************
*
*******************************************************************************/
MBOOL IndexMgr::setIdx_DM(UINT8 const idx)         { return setIdx<NUM_DM>(DM, idx); }
MBOOL IndexMgr::setIdx_DP(UINT8 const idx)         { return setIdx<NUM_DP>(DP, idx); }
MBOOL IndexMgr::setIdx_NR1(UINT8 const idx)        { return setIdx<NUM_NR1>(NR1, idx); }
MBOOL IndexMgr::setIdx_NR2(UINT8 const idx)        { return setIdx<NUM_NR2>(NR2, idx); }
MBOOL IndexMgr::setIdx_Saturation(UINT8 const idx) { return setIdx<NUM_Saturation>(Saturation, idx); }
MBOOL IndexMgr::setIdx_Contrast(UINT8 const idx)   { return setIdx<NUM_Contrast>(Contrast, idx); }
MBOOL IndexMgr::setIdx_Hue(UINT8 const idx)        { return setIdx<NUM_Hue>(Hue, idx); }
MBOOL IndexMgr::setIdx_Gamma(UINT8 const idx)      { return setIdx<NUM_Gamma>(Gamma, idx); }
MBOOL IndexMgr::setIdx_EE(UINT8 const idx)         { return setIdx<NUM_EE>(EE, idx); }


/*******************************************************************************
* 
*******************************************************************************/
void
IndexMgr::
dump() const
{
    LOGD(
        "[IndexMgr][dump]"
        " DM:%d, DP:%d, NR1:%d, NR2:%d, Saturation:%d, Contrast:%d, Hue:%d, Gamma:%d, EE:%d"
        , DM, DP, NR1, NR2, Saturation, Contrast, Hue, Gamma, EE
    );    
}


/*******************************************************************************
* 
*******************************************************************************/
template <ESensorRole_T eSensorRole>
IdxSetMgrBase&
IdxSetMgrBase::
getInstance()
{
    static IdxSetMgr_MT6575 singleton;
    static struct link
    {
        link(IdxSetMgr_MT6575& r)
        {
            r.linkIndexSet<eSensorRole>();
        }
    } link_singleton(singleton);
    return  singleton;
}


/*******************************************************************************
* 
*******************************************************************************/
template <ESensorRole_T eSensorRole>
MVOID
IdxSetMgr_MT6575::
linkIndexSet()
{
    ::memset(m_pOnlinePreview, 0, sizeof(m_pOnlinePreview));
    ::memset(m_pOnlineCapture, 0, sizeof(m_pOnlineCapture));
    ::memset(m_pOfflineCapturePass1, 0, sizeof(m_pOfflineCapturePass1));
    ::memset(m_pOfflineCapturePass2, 0, sizeof(m_pOfflineCapturePass2));
    ::memset(m_pHdrCapPass1SF, 0, sizeof(m_pHdrCapPass1SF));
    ::memset(m_pHdrCapPass1MF1,0, sizeof(m_pHdrCapPass1MF1));
    ::memset(m_pHdrCapPass1MF2,0, sizeof(m_pHdrCapPass1MF2));
    ::memset(m_pHdrCapPass2,   0, sizeof(m_pHdrCapPass2));

#define LINK_ONE_SCENE_ISOs(link, scene)\
    link(scene, eIDX_ISO_100);\
    link(scene, eIDX_ISO_200);\
    link(scene, eIDX_ISO_400);\
    link(scene, eIDX_ISO_800);\
    link(scene, eIDX_ISO_1600)

#define LINK_SCENEs_ISOs(link)\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_OFF);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_NORMAL);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_ACTION);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_PORTRAIT);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_LANDSCAPE);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_NIGHTSCENE);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_NIGHTPORTRAIT);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_THEATRE);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_BEACH);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_SNOW);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_SUNSET);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_STEADYPHOTO);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_FIREWORKS);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_SPORTS);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_PARTY);\
    LINK_ONE_SCENE_ISOs(link, SCENE_MODE_CANDLELIGHT)

#define LINK_ONLINE_PREVIEW(scene, iso)\
m_pOnlinePreview[scene][iso] = &IdxSet<eSensorRole, ECamMode_Online_Preview, scene, iso>::idx

#define LINK_ONLINE_CAPTURE(scene, iso)\
m_pOnlineCapture[scene][iso] = &IdxSet<eSensorRole, ECamMode_Online_Capture, scene, iso>::idx

#define LINK_OFFLINE_CAPTURE_PASS1(scene, iso)\
m_pOfflineCapturePass1[scene][iso] = &IdxSet<eSensorRole, ECamMode_Offline_Capture_Pass1, scene, iso>::idx

#define LINK_OFFLINE_CAPTURE_PASS2(scene, iso)\
m_pOfflineCapturePass2[scene][iso] = &IdxSet<eSensorRole, ECamMode_Offline_Capture_Pass2, scene, iso>::idx

    //......................................................................
    LINK_SCENEs_ISOs(LINK_ONLINE_PREVIEW);
    LINK_SCENEs_ISOs(LINK_ONLINE_CAPTURE);
    LINK_SCENEs_ISOs(LINK_OFFLINE_CAPTURE_PASS1);
    LINK_SCENEs_ISOs(LINK_OFFLINE_CAPTURE_PASS2);
    //======================================================================


#define LINK_HDR_CAPTURE(scene, iso)\
m_pHdrCapPass1SF [iso] = &IdxSet<eSensorRole, ECamMode_HDR_Cap_Pass1_SF, iso>::idx;\
m_pHdrCapPass1MF1[iso] = &IdxSet<eSensorRole, ECamMode_HDR_Cap_Pass1_MF1,iso>::idx;\
m_pHdrCapPass1MF2[iso] = &IdxSet<eSensorRole, ECamMode_HDR_Cap_Pass2, iso>::idx;\
m_pHdrCapPass2   [iso] = &IdxSet<eSensorRole, ECamMode_HDR_Cap_Pass2, iso>::idx

    //......................................................................
    LINK_ONE_SCENE_ISOs(LINK_HDR_CAPTURE, "unused scene");
    //======================================================================

}


/*******************************************************************************
* 
*******************************************************************************/
INDEX_T const*
IdxSetMgr_MT6575::
get(MUINT32 const mode, MUINT32 const i/*=-1*/, MUINT32 const j/*=-1*/) const
{
    switch  (mode)
    {
    //  Normal
    case ECamMode_Online_Preview:
    case ECamMode_Video:
        return  get_OnlinePreview(i, j);
    case ECamMode_Online_Capture:
    case ECamMode_Online_Capture_ZSD:
        return  get_OnlineCapture(i, j);
    case ECamMode_Offline_Capture_Pass1:
        return  get_OfflineCapturePass1(i, j);
    case ECamMode_Offline_Capture_Pass2:
    case ECamMode_YUV2JPG_ZSD: // share the same NR2 index with offline capture pass2
        return  get_OfflineCapturePass2(i, j);
    //  HDR
    case ECamMode_HDR_Cap_Pass1_SF:
        return  get_HdrCapPass1SF(i, j);
    case ECamMode_HDR_Cap_Pass1_MF1:
        return  get_HdrCapPass1MF1(i, j);
    case ECamMode_HDR_Cap_Pass1_MF2:
        return  get_HdrCapPass1MF2(i, j);
    case ECamMode_HDR_Cap_Pass2:
        return  get_HdrCapPass2(i, j);
    default:
        break;
    }
    return  NULL;
}


/*******************************************************************************
* 
*******************************************************************************/


