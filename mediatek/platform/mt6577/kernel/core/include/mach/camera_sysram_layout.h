#ifndef _CAMERA_SYSRAM_LAYOUT_H
#define _CAMERA_SYSRAM_LAYOUT_H

/*******************************************************************************
* Banks.
********************************************************************************/
#define MM_SYSRAM_BASE_PA       (0xC2000000)
#define Base_Bank0              (MM_SYSRAM_BASE_PA)
#define Size_Bank0              (0x18000)       //96KB
#define Base_Bank1              (Base_Bank0+Size_Bank0)
#define Size_Bank1              (0x8000)        //32KB
#define Base_Bank2              (Base_Bank1+Size_Bank1)
#define Size_Bank2              (0x20000)       //128KB
#define MM_SYSRAM_SIZE          (Size_Bank0+Size_Bank1+Size_Bank2)

/****************************************************************************************************
MEM_POOL_BANK0  //  bank 0
        VDO_ROT0/1, RDMA0/1, JPEG_DMA(*dedicate bank), TVOUT(*dedicate bank), Flicker, FD

MEM_POOL_BANK1  //  bank 1
        TV_ROT(*dedicate bank), Shading, Defect, PCA, LCE, EIS

MEM_POOL_BANK2  //  bank 2
        BRZ, VRZ0/1, RGB_ROT0/1/2, JPEG_CODEC, VIDEO
    0x18000~0x1F800 (30720B)        Audio
    0x1F800~0x20000 (2048B)         Descriptor
        2KB of bank2 is reserved for (?? RGB_ROT0/1/2, VDO_ROT0/1, RDMA0/1, TV_ROT ??)

*****************************************************************************************************/
typedef struct MemNode
{
    ESysramUser_T eOwner;
    unsigned long u4Offset;
    unsigned long u4Length;
    unsigned long u4Index;
    struct MemNode* pNext;
    struct MemNode* pPrev;
} MemNode_T;

//------------------------------------------------------------------------------
//  Memory Pool Bank
typedef enum {
    MEM_POOL_BANK0  = 0,            //  bank 0
    MEM_POOL_BANK1,                 //  bank 1
    MEM_POOL_BANK2,                 //  bank 2
    MEM_POOL_CNT,                   //  memory pool count.
    MEM_POOL_BAD    = 0x80000000,   //  bad memory pool
    MEM_POOL_STATIC,                //  static map.
} MEM_POOL_BANK_NO_T;

#define SYSRAM_SIZE_AUDIO           (30*1024)
#define SYSRAM_SIZE_DESC_VOD_ROT0   (3*4*16)
#define SYSRAM_SIZE_DESC_VOD_ROT1   (14*4*16)
#define SYSRAM_SIZE_DESC_RGB_ROT0   (3*4*16)
#define SYSRAM_SIZE_DESC_RGB_ROT1   (3*4*16)
#define SYSRAM_SIZE_DESC_RGB_ROT2   (3*4*16)
#define SYSRAM_SIZE_DESC_RDMA0      (3*4*16)
#define SYSRAM_SIZE_DESC_RDMA1      (3*4*16)
//#define SYSRAM_SIZE_DESC_OVL         (0)
#define SYSRAM_SIZE_FLICKER         (29832)
#define SYSRAM_SIZE_SHADING         (19200)
#define SYSRAM_SIZE_DEFECT          (4096)
#define SYSRAM_SIZE_LCE0            (3072)
#define SYSRAM_SIZE_LCE1            (3072)
#define SYSRAM_SIZE_FD              (20480)
#define SYSRAM_SIZE_PCA             (720)
#define SYSRAM_SIZE_EIS             (184)
#define SYSRAM_SIZE_VIDEO           (20480)
#define SYSRAM_SIZE_TVOUT           (98304)
#define SYSRAM_SIZE_TV_ROT          (32768-SYSRAM_SIZE_PCA)
#define SYSRAM_SIZE_JPEG_DMA        (98304)
#define SYSRAM_SIZE_JPEG_CODEC      (4096)
#define SYSRAM_SIZE_BRZ             (52800)
#define SYSRAM_SIZE_VRZ0            (7680)
#define SYSRAM_SIZE_VRZ1            (7680)
#define SYSRAM_SIZE_RGB_ROT0        (34680)
#define SYSRAM_SIZE_RGB_ROT1        (11560)
#define SYSRAM_SIZE_RGB_ROT2        (34680)
#define SYSRAM_SIZE_RGB_GROUP       ((30+28)*1024)
#define SYSRAM_SIZE_VDO_ROT0        (46240)
#define SYSRAM_SIZE_VDO_ROT1        SYSRAM_SIZE_VDO_ROT0
#define SYSRAM_SIZE_VDO_ROT1_SUB    (SYSRAM_SIZE_VDO_ROT1/2)
#define SYSRAM_SIZE_RDMA0           (5800)
//#define SYSRAM_SIZE_RDMA1           (0)


#define Pool0StartAddr  (Base_Bank0)
#define Pool0Size       (Size_Bank0)        //96KB

#define Pool1StartAddr  (Base_Bank1)
#define Pool1Size       (Size_Bank1)        //32KB

#define Pool2StartAddr  (Base_Bank2)
#define Pool2Size       (Size_Bank2-(SYSRAM_SIZE_AUDIO+SYSRAM_SIZE_DESC_VOD_ROT0+SYSRAM_SIZE_DESC_VOD_ROT1+SYSRAM_SIZE_DESC_RGB_ROT0+SYSRAM_SIZE_DESC_RGB_ROT1+SYSRAM_SIZE_DESC_RGB_ROT2+SYSRAM_SIZE_DESC_RDMA0+SYSRAM_SIZE_DESC_RDMA1))

#define MEM_NODE_CNT_PER_POOL   (ESysramUser_CNT*2 + 2)

//------------------------------------------------------------------------------
//  Memory Pool Info
typedef struct MemPoolInfo
{
    MemNode_T*const         paNodes;
    unsigned long const     u4OwnerCnt;
    unsigned long const     u4StartAddr;
    unsigned long const     u4Size;

    unsigned long           u4IndexTbl;
    unsigned long           u4UserCount;
} MemPoolInfo_T;

static MemNode_T g_aMemPoolBank0Tbl[MEM_NODE_CNT_PER_POOL];
static MemNode_T g_aMemPoolBank1Tbl[MEM_NODE_CNT_PER_POOL];
static MemNode_T g_aMemPoolBank2Tbl[MEM_NODE_CNT_PER_POOL];

static MemPoolInfo_T g_aMemPoolInfo[MEM_POOL_CNT] = 
{
    [MEM_POOL_BANK0] = {
        .paNodes    = &g_aMemPoolBank0Tbl[0], 
        .u4OwnerCnt = MEM_NODE_CNT_PER_POOL, 
        .u4StartAddr= Pool0StartAddr, 
        .u4Size     = Pool0Size, 
        .u4IndexTbl = (~0x1), 
        .u4UserCount= 0, 
    }, 
    [MEM_POOL_BANK1] = {
        .paNodes    = &g_aMemPoolBank1Tbl[0], 
        .u4OwnerCnt = MEM_NODE_CNT_PER_POOL, 
        .u4StartAddr= Pool1StartAddr, 
        .u4Size     = Pool1Size, 
        .u4IndexTbl = (~0x1), 
        .u4UserCount= 0, 
    }, 
    [MEM_POOL_BANK2] = {
        .paNodes    = &g_aMemPoolBank2Tbl[0], 
        .u4OwnerCnt = MEM_NODE_CNT_PER_POOL, 
        .u4StartAddr= Pool2StartAddr, 
        .u4Size     = Pool2Size, 
        .u4IndexTbl = (~0x1), 
        .u4UserCount= 0, 
    }, 
};

static inline MemPoolInfo_T* GetMemPoolInfo(MEM_POOL_BANK_NO_T const eMemPoolBankNo)
{
    if  ( MEM_POOL_CNT > eMemPoolBankNo )
        return  &g_aMemPoolInfo[eMemPoolBankNo];
    return  NULL;
}

enum
{
    EMPoolBank0OwnersMask   =
                         (1<<ESysramUser_VDO_ROT0)
                        |(1<<ESysramUser_VDO_ROT1)
                        |(1<<ESysramUser_RDMA0)
                        //|(1<<ESysramUser_RDMA1)
                        |(1<<ESysramUser_JPEG_DMA)
                        |(1<<ESysramUser_TVOUT)
                        |(1<<ESysramUser_FD)
                        |(1<<ESysramUser_FLICKER)
                        , 
    EMPoolBank1OwnersMask   =
                         (1<<ESysramUser_SHADING)
                        |(1<<ESysramUser_DEFECT)
                        |(1<<ESysramUser_LCE0)
                        |(1<<ESysramUser_LCE1)
                        |(1<<ESysramUser_PCA)
                        |(1<<ESysramUser_EIS)
                        |(1<<ESysramUser_TV_ROT)
                        , 
    EMPoolBank2OwnersMask   =
                         (1<<ESysramUser_BRZ)
                        |(1<<ESysramUser_VRZ0)
                        |(1<<ESysramUser_VRZ1)
                        |(1<<ESysramUser_RGB_ROT0)
                        |(1<<ESysramUser_RGB_ROT1)
                        |(1<<ESysramUser_RGB_ROT2)
                        |(1<<ESysramUser_JPEG_CODEC)
                        |(1<<ESysramUser_VIDEO)
                        |(1<<ESysramUser_VDO_ROT1_SUB)
                        , 
    EStaticOwnersMask       =
                         (1<<ESysramUser_AUDIO)
                        |(1<<ESysramUser_DESC_VDO_ROT0)
                        |(1<<ESysramUser_DESC_VDO_ROT1)
                        |(1<<ESysramUser_DESC_RGB_ROT0)
                        |(1<<ESysramUser_DESC_RGB_ROT1)
                        |(1<<ESysramUser_DESC_RGB_ROT2)
                        |(1<<ESysramUser_DESC_RDMA0)
                        |(1<<ESysramUser_DESC_RDMA1)
                        //|(1<<ESysramUser_DESC_OVL)
                        , 
    EDynamicOwnersMask      = 
                        (~EStaticOwnersMask)
                        ,
    //Marx Chiu: This need to be mopdified in the future to prevent too many logs.
    ELogOwnersMask          =
                        // (1<<ESysramUser_VDO_ROT1)
                        //|(1<<ESysramUser_RDMA0)
                         (1<<ESysramUser_JPEG_DMA)
                        |(1<<ESysramUser_TVOUT)
                        |(1<<ESysramUser_FD)
                        |(1<<ESysramUser_FLICKER)
                        |(1<<ESysramUser_SHADING)
                        |(1<<ESysramUser_DEFECT)
                        |(1<<ESysramUser_LCE0)
                        |(1<<ESysramUser_LCE1)
                        |(1<<ESysramUser_PCA)
                        |(1<<ESysramUser_EIS)
                        |(1<<ESysramUser_TV_ROT)
                        |(1<<ESysramUser_BRZ)
                        //|(1<<ESysramUser_VRZ0)
                        //|(1<<ESysramUser_VRZ1)
                        //|(1<<ESysramUser_RGB_ROT0)
                        //|(1<<ESysramUser_RGB_ROT1)
                        //|(1<<ESysramUser_RGB_ROT2)
                        |(1<<ESysramUser_JPEG_CODEC)
                        |(1<<ESysramUser_VIDEO)
};

static MEM_POOL_BANK_NO_T GetMemPoolNo(ESysramUser_T const eOwner)
{
    unsigned long const u4OwnerMask = (1<<eOwner);
    if  ( u4OwnerMask & EStaticOwnersMask )
    {
        return  MEM_POOL_STATIC;
    }

    if  ( u4OwnerMask & EMPoolBank0OwnersMask )
    {
        return  MEM_POOL_BANK0;
    }

    if  ( u4OwnerMask & EMPoolBank1OwnersMask )
    {
        return  MEM_POOL_BANK1;
    }

    if  ( u4OwnerMask & EMPoolBank2OwnersMask )
    {
        return  MEM_POOL_BANK2;
    }

    return  MEM_POOL_BAD;
}


static inline unsigned long ReevaluateUserSizeToAlloc(ESysramUser_T const eOwner, unsigned long u4Size)
{
    //  Dedicated-bank user?
    switch  (eOwner)
    {
    case ESysramUser_JPEG_DMA:
    case ESysramUser_TVOUT:
        //  Jpeg DMA must occupy the whole bank0.
        //  TVOUT must occupy the whole bank0.
        u4Size = Size_Bank0;
        break;
    case ESysramUser_TV_ROT:
        //  TV ROT must occupy the whole bank1.
        //u4Size = Size_Bank1;
        break;
    default:
        break;
    }

    return  u4Size;
}


/*******************************************************************************
* Array of Owner Name String
********************************************************************************/
static char const*const g_apszOwnerName[ESysramUser_CNT] = 
{
    [ESysramUser_AUDIO      ] = "Audio", 
    //[ESysramUser_DESCRIPTOR ] = "Descriptor", 
    [ESysramUser_DESC_VDO_ROT0  ] = "Desc VDO ROT0", 
    [ESysramUser_DESC_VDO_ROT1  ] = "Desc VDO ROT1",
    [ESysramUser_DESC_RGB_ROT0  ] = "Desc RGB ROT0",
    [ESysramUser_DESC_RGB_ROT1  ] = "Desc RGB ROT1",
    [ESysramUser_DESC_RGB_ROT2  ] = "Desc RGB ROT2",
    [ESysramUser_DESC_RDMA0     ] = "Desc RDMA0",
    [ESysramUser_DESC_RDMA1     ] = "Desc RDMA1",
    //[ESysramUser_DESC_OVL       ] = "Desc OVL",
    [ESysramUser_FLICKER    ] = "Flicker", 
    [ESysramUser_SHADING    ] = "Shading", 
    [ESysramUser_DEFECT     ] = "Defect", 
    [ESysramUser_LCE0       ] = "LCE0",
    [ESysramUser_LCE1       ] = "LCE1",
    [ESysramUser_FD         ] = "FD", 
    [ESysramUser_PCA        ] = "PCA", 
    [ESysramUser_EIS        ] = "EIS", 
    [ESysramUser_VIDEO      ] = "Video", 
    [ESysramUser_TVOUT      ] = "TVOUT", 
    [ESysramUser_TV_ROT     ] = "TV ROT", 
    [ESysramUser_JPEG_DMA   ] = "Jpeg DMA", 
    [ESysramUser_JPEG_CODEC ] = "Jpeg Codec", 
    [ESysramUser_BRZ        ] = "BRZ", 
    [ESysramUser_VRZ0       ] = "VRZ0", 
    [ESysramUser_VRZ1       ] = "VRZ1", 
    [ESysramUser_RGB_ROT0   ] = "RGB ROT0", 
    [ESysramUser_RGB_ROT1   ] = "RGB ROT1", 
    [ESysramUser_RGB_ROT2   ] = "RGB ROT2", 
    [ESysramUser_VDO_ROT0   ] = "VDO ROT0", 
    [ESysramUser_VDO_ROT1   ] = "VDO ROT1", 
    [ESysramUser_VDO_ROT1_SUB   ] = "VDO ROT1 Sub", 
    [ESysramUser_RDMA0      ] = "RDMA0", 
    //[ESysramUser_RDMA1      ] = "RDMA1", 
};

/*******************************************************************************
* Max Size: Static/Dynamic Users
********************************************************************************/
static unsigned long const g_au4MaxUserSize[ESysramUser_CNT] = 
{
    [ESysramUser_AUDIO      ] = (3 + SYSRAM_SIZE_AUDIO ) / 4 * 4, 
    //[ESysramUser_DESCRIPTOR ] = (3 + 2048  ) / 4 * 4, 
    [ESysramUser_DESC_VDO_ROT0  ] = (3 + SYSRAM_SIZE_DESC_VOD_ROT0   ) / 4 * 4,
    [ESysramUser_DESC_VDO_ROT1  ] = (3 + SYSRAM_SIZE_DESC_VOD_ROT1   ) / 4 * 4,
    [ESysramUser_DESC_RGB_ROT0  ] = (3 + SYSRAM_SIZE_DESC_RGB_ROT0   ) / 4 * 4,
    [ESysramUser_DESC_RGB_ROT1  ] = (3 + SYSRAM_SIZE_DESC_RGB_ROT1   ) / 4 * 4,
    [ESysramUser_DESC_RGB_ROT2  ] = (3 + SYSRAM_SIZE_DESC_RGB_ROT2   ) / 4 * 4,
    [ESysramUser_DESC_RDMA0     ] = (3 + SYSRAM_SIZE_DESC_RDMA0      ) / 4 * 4,
    [ESysramUser_DESC_RDMA1     ] = (3 + SYSRAM_SIZE_DESC_RDMA1      ) / 4 * 4,
    [ESysramUser_FLICKER    ] = (3 + SYSRAM_SIZE_FLICKER ) / 4 * 4, 
    [ESysramUser_SHADING    ] = (3 + SYSRAM_SIZE_SHADING ) / 4 * 4, 
    [ESysramUser_DEFECT     ] = (3 + SYSRAM_SIZE_DEFECT  ) / 4 * 4, 
    [ESysramUser_LCE0       ] = (3 + SYSRAM_SIZE_LCE0  ) / 4 * 4,
    [ESysramUser_LCE1       ] = (3 + SYSRAM_SIZE_LCE1  ) / 4 * 4,
    [ESysramUser_FD         ] = (3 + SYSRAM_SIZE_FD ) / 4 * 4, 
    [ESysramUser_PCA        ] = (3 + SYSRAM_SIZE_PCA   ) / 4 * 4, 
    [ESysramUser_EIS        ] = (3 + SYSRAM_SIZE_EIS   ) / 4 * 4, 
    [ESysramUser_VIDEO      ] = (3 + SYSRAM_SIZE_VIDEO ) / 4 * 4, 
    [ESysramUser_TVOUT      ] = (3 + SYSRAM_SIZE_TVOUT ) / 4 * 4, 
    [ESysramUser_TV_ROT     ] = (3 + SYSRAM_SIZE_TV_ROT ) / 4 * 4, 
    [ESysramUser_JPEG_DMA   ] = (3 + SYSRAM_SIZE_JPEG_DMA ) / 4 * 4, 
    [ESysramUser_JPEG_CODEC ] = (3 + SYSRAM_SIZE_JPEG_CODEC  ) / 4 * 4, 
    [ESysramUser_BRZ        ] = (3 + SYSRAM_SIZE_BRZ ) / 4 * 4, 
    [ESysramUser_VRZ0       ] = (3 + SYSRAM_SIZE_VRZ0  ) / 4 * 4, 
    [ESysramUser_VRZ1       ] = (3 + SYSRAM_SIZE_VRZ1  ) / 4 * 4, 
    [ESysramUser_RGB_ROT0   ] = (3 + SYSRAM_SIZE_RGB_ROT0 ) / 4 * 4, 
    [ESysramUser_RGB_ROT1   ] = (3 + SYSRAM_SIZE_RGB_ROT1 ) / 4 * 4, 
    [ESysramUser_RGB_ROT2   ] = (3 + SYSRAM_SIZE_RGB_ROT2 ) / 4 * 4, 
    [ESysramUser_VDO_ROT0   ] = (3 + SYSRAM_SIZE_VDO_ROT0 ) / 4 * 4, 
    [ESysramUser_VDO_ROT1   ] = (3 + SYSRAM_SIZE_VDO_ROT1 ) / 4 * 4, 
    [ESysramUser_VDO_ROT1_SUB   ] = (3 + SYSRAM_SIZE_VDO_ROT1_SUB   ) / 4 * 4, 
    [ESysramUser_RDMA0      ] = (3 + SYSRAM_SIZE_RDMA0  ) / 4 * 4, 
    //[ESysramUser_RDMA1      ] = (3 + SYSRAM_SIZE_RDMA1     ) / 4 * 4, 
};


/*******************************************************************************
* Physical Address: Static Users
********************************************************************************/
static unsigned long const g_au4StaticUserAddr[ESysramUser_CNT] = 
{
    [ESysramUser_AUDIO      ] = (Pool2StartAddr + Pool2Size), 
    //[ESysramUser_DESCRIPTOR ] = (Base_Bank2 + 0x1F800), 
    [ESysramUser_DESC_VDO_ROT0  ] = (Pool2StartAddr + Pool2Size+SYSRAM_SIZE_AUDIO),
    [ESysramUser_DESC_VDO_ROT1  ] = (Pool2StartAddr + Pool2Size+SYSRAM_SIZE_AUDIO+SYSRAM_SIZE_DESC_VOD_ROT0),
    [ESysramUser_DESC_RGB_ROT0  ] = (Pool2StartAddr + Pool2Size+SYSRAM_SIZE_AUDIO+SYSRAM_SIZE_DESC_VOD_ROT0+SYSRAM_SIZE_DESC_VOD_ROT1),
    [ESysramUser_DESC_RGB_ROT1  ] = (Pool2StartAddr + Pool2Size+SYSRAM_SIZE_AUDIO+SYSRAM_SIZE_DESC_VOD_ROT0+SYSRAM_SIZE_DESC_VOD_ROT1+SYSRAM_SIZE_DESC_RGB_ROT0),
    [ESysramUser_DESC_RGB_ROT2  ] = (Pool2StartAddr + Pool2Size+SYSRAM_SIZE_AUDIO+SYSRAM_SIZE_DESC_VOD_ROT0+SYSRAM_SIZE_DESC_VOD_ROT1+SYSRAM_SIZE_DESC_RGB_ROT0+SYSRAM_SIZE_DESC_RGB_ROT1),
    [ESysramUser_DESC_RDMA0     ] = (Pool2StartAddr + Pool2Size+SYSRAM_SIZE_AUDIO+SYSRAM_SIZE_DESC_VOD_ROT0+SYSRAM_SIZE_DESC_VOD_ROT1+SYSRAM_SIZE_DESC_RGB_ROT0+SYSRAM_SIZE_DESC_RGB_ROT1+SYSRAM_SIZE_DESC_RGB_ROT2),
    [ESysramUser_DESC_RDMA1     ] = (Pool2StartAddr + Pool2Size+SYSRAM_SIZE_AUDIO+SYSRAM_SIZE_DESC_VOD_ROT0+SYSRAM_SIZE_DESC_VOD_ROT1+SYSRAM_SIZE_DESC_RGB_ROT0+SYSRAM_SIZE_DESC_RGB_ROT1+SYSRAM_SIZE_DESC_RGB_ROT2+SYSRAM_SIZE_DESC_RDMA0),
    //[ESysramUser_DESC_OVL       ] = (Pool2StartAddr + Pool2Size+SYSRAM_SIZE_AUDIO+g_au4MaxUserSize[ESysramUser_DESC_VD0_ROT0]+SYSRAM_SIZE_DESC_VOD_ROT1+SYSRAM_SIZE_DESC_RGB_ROT0+SYSRAM_SIZE_DESC_RGB_ROT1+SYSRAM_SIZE_DESC_RGB_ROT2+SYSRAM_SIZE_DESC_RDMA0+SYSRAM_SIZE_DESC_RDMA1),
    [ESysramUser_FLICKER    ] = 0, 
    [ESysramUser_SHADING    ] = 0, 
    [ESysramUser_DEFECT     ] = 0, 
    [ESysramUser_LCE0       ] = 0,
    [ESysramUser_LCE1       ] = 0,
    [ESysramUser_FD         ] = 0, 
    [ESysramUser_PCA        ] = 0, 
    [ESysramUser_EIS        ] = 0, 
    [ESysramUser_VIDEO      ] = 0, 
    [ESysramUser_TVOUT      ] = 0, 
    [ESysramUser_TV_ROT     ] = 0, 
    [ESysramUser_JPEG_DMA   ] = 0, 
    [ESysramUser_JPEG_CODEC ] = 0, 
    [ESysramUser_BRZ        ] = 0, 
    [ESysramUser_VRZ0       ] = 0, 
    [ESysramUser_VRZ1       ] = 0, 
    [ESysramUser_RGB_ROT0   ] = 0, 
    [ESysramUser_RGB_ROT1   ] = 0, 
    [ESysramUser_RGB_ROT2   ] = 0, 
    [ESysramUser_VDO_ROT0   ] = 0, 
    [ESysramUser_VDO_ROT1   ] = 0, 
    [ESysramUser_VDO_ROT1_SUB   ] = 0, 
    [ESysramUser_RDMA0      ] = 0, 
    //[ESysramUser_RDMA1      ] = 0, 
};


/*******************************************************************************
* Array of Pointers to the Tables of the Max User Sizes.
********************************************************************************/
typedef unsigned long const (*P_MaxUsrSizeTbl_T)[ESysramUser_CNT];

#else   //_CAMERA_SYSRAM_LAYOUT_H
#endif  //_CAMERA_SYSRAM_LAYOUT_H


