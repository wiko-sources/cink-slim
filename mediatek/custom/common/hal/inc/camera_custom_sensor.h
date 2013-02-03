#ifndef _CAMERA_CUSTOM_SENSOR_H_
#define _CAMERA_CUSTOM_SENSOR_H_

#include "camera_custom_types.h"
#include "camera_custom_nvram.h"


namespace NSFeature
{


struct FeatureInfoProvider;
class SensorInfoBase
{
public:     ////    Feature Type.
    typedef enum
    {
        EType_RAW =   0,  //  RAW Sensor
        EType_YUV,        //  YUV Sensor
    }   EType;

    typedef NSFeature::FeatureInfoProvider FeatureInfoProvider_T;

public:
    virtual ~SensorInfoBase(){}

public:     //// Interface.
    virtual EType       GetType() const = 0;
    virtual MUINT32     GetID()   const = 0;
    virtual MBOOL       GetFeatureProvider(
        FeatureInfoProvider_T& rFInfoProvider
    ) = 0;

    virtual MUINT32 impGetDefaultData(CAMERA_DATA_TYPE_ENUM const CameraDataType, MVOID*const pDataBuf, MUINT32 const size) const = 0;
};


template <SensorInfoBase::EType _sensor_type, MUINT32 _sensor_id>
class SensorInfo : public SensorInfoBase
{
public:     //// Interface.
    virtual EType       GetType() const { return _sensor_type; }
    virtual MUINT32     GetID()   const { return _sensor_id;   }
};


template <MUINT32 _sensor_id>
class YUVSensorInfo : public SensorInfo<SensorInfoBase::EType_YUV, _sensor_id>
{
public:
    typedef SensorInfoBase::FeatureInfoProvider_T FeatureInfoProvider_T;
    typedef YUVSensorInfo<_sensor_id> SensorInfo_T;

private:    //// Instanciation from outside is disallowed.
    YUVSensorInfo(){}

public:     //// Interface.
    static SensorInfoBase* GetInstance();
    virtual MBOOL GetFeatureProvider(FeatureInfoProvider_T& rFInfoProvider);

    static MUINT32 GetDefaultData(CAMERA_DATA_TYPE_ENUM const CameraDataType, MVOID*const pDataBuf, MUINT32 const size)
    {
        return  GetInstance()->impGetDefaultData(CameraDataType, pDataBuf, size);
    }

    virtual MUINT32 impGetDefaultData(CAMERA_DATA_TYPE_ENUM const CameraDataType, MVOID*const pDataBuf, MUINT32 const size) const;
};


template <MUINT32 _sensor_id>
class RAWSensorInfo : public SensorInfo<SensorInfoBase::EType_RAW, _sensor_id>
{
public:
    typedef SensorInfoBase::FeatureInfoProvider_T FeatureInfoProvider_T;
    typedef RAWSensorInfo<_sensor_id> SensorInfo_T;

private:    //// Instanciation from outside is disallowed.
    RAWSensorInfo(){}

public:     //// Interface.
    static SensorInfoBase* GetInstance()
    {
        static SensorInfo_T singleton;
        return &singleton;
    }
    virtual MBOOL GetFeatureProvider(FeatureInfoProvider_T& rFInfoProvider)
    {
        return MFALSE;
    }

    static MUINT32 GetDefaultData(CAMERA_DATA_TYPE_ENUM const CameraDataType, MVOID*const pDataBuf, MUINT32 const size)
    {
        return  GetInstance()->impGetDefaultData(CameraDataType, pDataBuf, size);
    }

    virtual MUINT32 impGetDefaultData(CAMERA_DATA_TYPE_ENUM const CameraDataType, MVOID*const pDataBuf, MUINT32 const size) const;
};


};  //  NSFeature


typedef struct
{
    MUINT32 SensorId;
    MUINT8  drvname[32];
    NSFeature::SensorInfoBase* (*pfGetSensorInfoInstance)();
    MUINT32 (*getCameraDefault)(CAMERA_DATA_TYPE_ENUM CameraDataType, MVOID *pDataBuf, MUINT32 size);
    MUINT32 (*getCameraCalData)(MUINT32* pGetCalData);
} MSDK_SENSOR_INIT_FUNCTION_STRUCT, *PMSDK_SENSOR_INIT_FUNCTION_STRUCT;

MUINT32 GetSensorInitFuncList(MSDK_SENSOR_INIT_FUNCTION_STRUCT **ppSensorList);


#endif  //  _CAMERA_CUSTOM_SENSOR_H_

