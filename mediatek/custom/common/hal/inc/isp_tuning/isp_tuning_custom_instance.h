#ifndef MT6516
#ifndef _ISP_TUNING_CUSTOM_INSTANCE_H_
#define _ISP_TUNING_CUSTOM_INSTANCE_H_


namespace NSIspTuning
{


/*******************************************************************************
* 
*******************************************************************************/
template <ESensorRole_T eSensorRole>
class CTIspTuningCustom : public IspTuningCustom
{
public:
    static
    IspTuningCustom*
    getInstance()
    {
        static CTIspTuningCustom<eSensorRole> singleton;
        return &singleton;
    }
    virtual void destroyInstance() {}

    CTIspTuningCustom()
        : IspTuningCustom()
        , m_rIdxSetMgr(IdxSetMgrBase::getInstance<eSensorRole>())
    {}

public:     ////    Attributes
    virtual ESensorRole_T   getSensorRole() const { return eSensorRole; }
    virtual
    INDEX_T const*
    getDefaultIndex(
        ECamMode_T const eCamMode, 
        EIndex_Scene_T const eIdx_Scene, 
        EIndex_ISO_T const eIdx_ISO
    ) const
    {
        return m_rIdxSetMgr.get(eCamMode, eIdx_Scene, eIdx_ISO);
    }

protected:  ////    Data Members.
    IdxSetMgrBase&  m_rIdxSetMgr;

};


/*******************************************************************************
* Customers can specialize CTIspTuningCustom<xxx> 
* and then override default behaviors if needed.
*******************************************************************************/
#if 0
/*
    ps: where ESensorRole_xxx = ESensorRole_Main/ESensorRole_Sub
*/
template <>
class CTIspTuningCustom< ESensorRole_Main > : public IspTuningCustom
{
public:
    static
    IspTuningCustom*
    getInstance()
    {
        static CTIspTuningCustom< ESensorRole_Main > singleton;
        return &singleton;
    }
    virtual void destroyInstance() {}

public:     ////    Overrided Interfaces.
    //......
};

#endif


/*******************************************************************************
* 
*******************************************************************************/
#define INSTANTIATE(_role_id) \
    case _role_id: return  CTIspTuningCustom<_role_id>::getInstance()

IspTuningCustom*
IspTuningCustom::
createInstance(ESensorRole_T const eSensorRole)
{
    switch  (eSensorRole)
    {
    INSTANTIATE(ESensorRole_Main);  //  Main Sensor
    INSTANTIATE(ESensorRole_Sub);   //  Sub Sensor
    default:
        break;
    }

    return  NULL;
}


};  //  NSIspTuning
#endif  //  _ISP_TUNING_CUSTOM_INSTANCE_H_
#endif  //  ! MT6516

