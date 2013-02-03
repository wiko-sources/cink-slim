/** 
 * @file 
 *   val_api.h 
 *
 * @par Project:
 *   MFlexVideo 
 *
 * @par Description:
 *   Video Abstraction Layer APIs
 *
 * @par Author:
 *   Jackal Chen (mtk02532)
 *
 * @par $Revision: #18 $
 * @par $Modtime:$
 * @par $Log:$
 *
 */

#ifndef _VAL_API_H_
#define _VAL_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "val_types.h"

/*=============================================================================
 *                             Function Declaration
 *===========================================================================*/

VAL_UINT32_T eVideoDecInitMVA(VAL_VOID_T** a_pvHandle);
VAL_UINT32_T eVideoDecAllocMVA(VAL_VOID_T* a_pvHandle, VAL_UINT32_T a_u4Va, VAL_UINT32_T* ap_u4Pa, VAL_UINT32_T a_u4Size);
VAL_UINT32_T eVideoDecFreeMVA(VAL_VOID_T* a_pvHandle, VAL_UINT32_T a_u4Va, VAL_UINT32_T a_u4Pa, VAL_UINT32_T a_u4Size);
VAL_UINT32_T eVideoDecDeInitMVA(VAL_VOID_T* a_pvHandle);

/**
* @par Function       
*   eVideoMp4EncAllocMVA
* @par Description    
*   alloc m4u modified memory for mpeg4 encoder
* @param              
*   a_u4Va              [IN]        The virtual address of malloc memory 
* @param              
*   a_u4Pa              [IN/OUT]    The modified virtual address of malloc memory
* @param             
*   a_u4Size            [IN]        The size of virtual address of malloc memory
* @par Returns        
*   VAL_RESULT_T
*/
VAL_UINT32_T eVideoMp4EncAllocMVA(
    VAL_VOID_T** a_pvHandle,
    VAL_UINT32_T a_u4Va, 
    VAL_UINT32_T* ap_u4Pa, 
    VAL_UINT32_T a_u4Size
);

/**
* @par Function       
*   eVideoMp4EncFreeMVA
* @par Description    
*   alloc m4u modified memory for mpeg4 encoder
* @param              
*   a_u4Va              [IN]        The virtual address of malloc memory 
* @param              
*   a_u4Pa              [IN]        The modified virtual address of malloc memory
* @param             
*   a_u4Size            [IN]        The size of virtual address of malloc memory
* @par Returns        
*   VAL_RESULT_T
*/
VAL_UINT32_T eVideoMp4EncFreeMVA(
    VAL_VOID_T* a_pvHandle,
    VAL_UINT32_T a_u4Va, 
    VAL_UINT32_T ap_u4Pa, 
    VAL_UINT32_T a_u4Size
);

/**
* @par Function       
*   eVideoMemAlloc
* @par Description    
*   alloc memory for video codec driver
* @param              
*   a_prParam           [IN/OUT]    The VAL_MEMORY_T structure
* @param             
*   a_u4ParamSize       [IN]        The size of VAL_MEMORY_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoMemAlloc(
    VAL_MEMORY_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoMemFree
* @par Description    
*   free memory for video codec driver 
* @param              
*   a_prParam           [IN/OUT]    The VAL_MEMORY_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_MEMORY_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoMemFree(
    VAL_MEMORY_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoMemSet
* @par Description    
*   set a specific value to a range of memory 
* @param              
*   a_prParam           [IN]        The VAL_MEMORY_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of a_prParam
* @param              
*   a_u4Value           [IN]        The specific value
* @param              
*   a_u4Size            [IN]        The range of memory
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoMemSet(
    VAL_MEMORY_T *a_prParam,
    VAL_UINT32_T a_u4ParamSize,
    VAL_INT32_T a_u4Value,
    VAL_UINT32_T a_u4Size
);

/**
* @par Function       
*   eVideoMemCpy
* @par Description    
*   copy a range of memory from src memory to dst memory 
* @param              
*   a_prParamDst        [IN]        The VAL_MEMORY_T structure
* @param              
*   a_u4ParamDstSize    [IN]        The size of a_prParamDst
* @param              
*   a_prParamSrc        [IN]        The VAL_MEMORY_T structure
* @param              
*   a_u4ParamSrcSize    [IN]        The size of a_prParamSrc
* @param              
*   a_u4Size            [IN]        The range of memory
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoMemCpy(
    VAL_MEMORY_T *a_prParamDst,
    VAL_UINT32_T a_u4ParamDstSize,
    VAL_MEMORY_T *a_prParamSrc,
    VAL_UINT32_T a_u4ParamSrcSize,
    VAL_UINT32_T a_u4Size
);

/**
* @par Function       
*   eVideoMemCmp
* @par Description    
*   compare a range of memory with Src1 memory and Src2 memory 
* @param              
*   a_prParamSrc1       [IN]        The VAL_MEMORY_T structure
* @param              
*   a_u4ParamSrc1Size   [IN]        The size of a_prParamSrc1
* @param              
*   a_prParamSrc2       [IN]        The VAL_MEMORY_T structure
* @param              
*   a_u4ParamSrc2Size   [IN]        The size of a_prParamSrc2
* @param              
*   a_u4Size            [IN]        The range of memory
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoMemCmp(
    VAL_MEMORY_T *a_prParamSrc1,
    VAL_UINT32_T a_u4ParamSrc1Size,
    VAL_MEMORY_T *a_prParamSrc2,
    VAL_UINT32_T a_u4ParamSrc2Size,
    VAL_UINT32_T a_u4Size
);

/**
* @par Function       
*   eVideoIntMemUsed
* @par Description    
*   used Internal memory
* @param              
*   a_prParam           [IN/OUT]    The VAL_INTMEM_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_INTMEM_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoIntMemUsed(
    VAL_INTMEM_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoIntMemAlloc
* @par Description    
*   Allocate Internal memory
* @param              
*   a_prParam           [IN/OUT]    The VAL_INTMEM_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_INTMEM_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoIntMemAlloc(
    VAL_INTMEM_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoIntMemFree
* @par Description    
*   Free Internal memory
* @param              
*   a_prParam           [IN/OUT]    The VAL_INTMEM_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_INTMEM_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoIntMemFree(
    VAL_INTMEM_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoCreateEvent
* @par Description    
*   create event  
* @param              
*   a_prParam           [IN/OUT]    The VAL_EVENT_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_EVENT_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoCreateEvent(
    VAL_EVENT_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoCloseEvent
* @par Description    
*   close event  
* @param              
*   a_prParam           [IN/OUT]    The VAL_EVENT_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_EVENT_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoCloseEvent(
    VAL_EVENT_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoWaitEvent
* @par Description    
*   wait event (ex. wait hw decode done)  
* @param              
*   a_prParam           [IN/OUT]    The VAL_EVENT_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_EVENT_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoWaitEvent(
    VAL_EVENT_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoSetEvent
* @par Description    
*   set event (ex. when hw decode done, ant then do something depend on this event...)  
* @param              
*   a_prParam           [IN/OUT]    The VAL_EVENT_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_EVENT_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoSetEvent(
    VAL_EVENT_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoCreateMutex
* @par Description    
*   create mutex
* @param              
*   a_prParam           [IN/OUT]    The VAL_MUTEX_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_MUTEX_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoCreateMutex(
    VAL_MUTEX_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoCloseMutex
* @par Description    
*   create mutex
* @param              
*   a_prParam           [IN/OUT]    The VAL_MUTEX_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_MUTEX_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoCloseMutex(
    VAL_MUTEX_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoWaitMutex
* @par Description    
*   create mutex
* @param              
*   a_prParam           [IN/OUT]    The VAL_MUTEX_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_MUTEX_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoWaitMutex(
    VAL_MUTEX_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoReleaseMutex
* @par Description    
*   create mutex
* @param              
*   a_prParam           [IN/OUT]    The VAL_MUTEX_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_MUTEX_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoReleaseMutex(
    VAL_MUTEX_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoGetTimeOfDay
* @par Description    
*   Get current system time
* @param              
*   a_prParam         [IN/OUT]   The VAL_TIME_T structure
* @param              
*   a_u4ParamSize     [IN]       The size of VAL_TIME_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoGetTimeOfDay(
    VAL_TIME_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function     
*   eVideoStrStr
* @par Description    
*   Return a pointer to the first occurrence of a search string in a string
* @param              
*   a_prParam           [IN/OUT]    The VAL_STRSTR_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_STRSTR_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoStrStr(
    VAL_STRSTR_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoAtoi
* @par Description    
*   Convert a string to integer.
* @param              
*   a_prParam           [IN/OUT]    The VAL_ATOI_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_ATOI_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoAtoi(
    VAL_ATOI_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);




#if defined(MT6575) || defined(MT6577)

/**
* @par Function       
*   eVideoCacheSwitch
* @par Description    
*   switch memory into another cache type
* @param              
*   a_prParam           [IN]    The VAL_MEMORY_T structure
* @param              
*   a_u4ParamSize       [IN]    The size of VAL_MEMORY_T
* @param              
*   a_eCacheType       [IN]        CacheType,  cachable or noncachable
* @par Returns        
*   VAL_RESULT_T
*/ 
 VAL_RESULT_T eVideoCacheSwitch(
    VAL_MEMORY_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize,
    VAL_CACHE_TYPE_T a_eCacheType
);	
 	

 	

 /**
 * @par Function	   
 *	 eVideoInvalidateCache
 * @par Description    
 *	 invalid cache of memory range
 * @param			   
 *	 a_prParam			 [IN/OUT]	 The VAL_MEMORY_T structure
 * @param			   
 *	 a_u4ParamSize		 [IN]		 The size of VAL_MEMORY_T
 * @par Returns 	   
 *	 VAL_RESULT_T
 */  
 VAL_RESULT_T eVideoInvalidateCache(
	 VAL_MEMORY_T *a_prParam,
	 VAL_UINT32_T a_u4ParamSize,
	 VAL_UINT32_T optype	 
 );
 
 
 /**
 * @par Function	   
 *	 eVideoFlushCache
 * @par Description    
 *	 Flush Cache of memory range
 * @param			   
 *	 a_prParam			 [IN/OUT]	 The VAL_MEMORY_T structure
 * @param			   
 *	 a_u4ParamSize		 [IN]		 The size of VAL_MEMORY_T
 * @par Returns 	   
 *	 VAL_RESULT_T
 */
 VAL_RESULT_T eVideoFlushCache(
	 VAL_MEMORY_T *a_prParam,
	 VAL_UINT32_T a_u4ParamSize,
 	 VAL_UINT32_T optype
 );

 	
 
 /*=============================================================================
 *                             WAIT/ISR Function Declaration
 *===========================================================================*/
 // Note:  here a_u4ParamSize is reserved for future used.


/**
* @par Function       
*   WaitISR
* @par Description    
*   wait ISR (ex. wait for hw done interrupt)  
* @param              
*   a_prParam           [IN/OUT]    The VAL_ISR_T structure
* @param              
*   a_u4ParamSize       [IN]        The size of VAL_ISR_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T WaitISR(
    VAL_ISR_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);




 /*=============================================================================
 *                             TIME Function Declaration
 *===========================================================================*/

/**
* @par Function       
*   eVideoGetTimeOfDay
* @par Description    
*   Get current system time
* @param              
*   a_prParam         [IN/OUT]   The VAL_TIME_T structure
* @param              
*   a_u4ParamSize     [IN]       The size of VAL_TIME_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoGetTimeOfDay(
    VAL_TIME_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);


 /*=============================================================================
 *                             POWER/CLOCK Function Declaration
 *===========================================================================*/
 // global power control
VAL_RESULT_T eHalHwPowerCtrl(
    VAL_POWER_T     *a_prParam,
    VAL_UINT32_T    a_u4ParamSize
);

//may not necessary since hw clock can be controlled by library.
/*
VAL_RESULT_T eHalHwEnableClock(
    HAL_CLOCK_T *a_prParam,
    VAL_UINT32_T a_u4ParamSize
);


VAL_RESULT_T eHalHwDisableClock(
    HAL_CLOCK_T *a_prParam,
    VAL_UINT32_T a_u4ParamSize
)
*/

 /*=============================================================================
 *                             MMAP Function Declaration
 *===========================================================================*/

/**
* @par Function       
*   eVideoMMAP
* @par Description    
*   memory map 
* @param              
*   a_prParam         [IN/OUT]   The VAL_MMAP_T structure
* @param              
*   a_u4ParamSize     [IN]       The size of VAL_MMAP_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoMMAP(
    VAL_MMAP_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);

/**
* @par Function       
*   eVideoUnMMAP
* @par Description    
*   un-map memroy
* @param              
*   a_prParam         [IN/OUT]   The VAL_MMAP_T structure
* @param              
*   a_u4ParamSize     [IN]       The size of VAL_MMAP_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoUnMMAP(
    VAL_MMAP_T *a_prParam, 
    VAL_UINT32_T a_u4ParamSize
);



 /*=============================================================================
 *                              HW Resource Function Declaration
 *===========================================================================*/
/**
* @par Function       
*   eVideoLockHW
* @par Description    
*   get HW lock
* @param              
*   a_prParam         [IN/OUT]   The VAL_HW_LOCK_T structure
* @param              
*   a_u4ParamSize     [IN]       The size of VAL_HW_LOCK_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoLockHW(
    VAL_HW_LOCK_T *a_prParam, 
    VAL_UINT32_T  a_u4ParamSize
);

/**
* @par Function       
*   eVideoUnLockHW
* @par Description    
*   unlock HW 
* @param              
*   a_prParam         [IN/OUT]   The VAL_HW_LOCK_T structure
* @param              
*   a_u4ParamSize     [IN]       The size of VAL_HW_LOCK_T
* @par Returns        
*   VAL_RESULT_T
*/
VAL_RESULT_T eVideoUnLockHW(
    VAL_HW_LOCK_T *a_prParam, 
    VAL_UINT32_T  a_u4ParamSize
);

VAL_RESULT_T eVideoInitLockHW( VAL_VCODEC_OAL_HW_REGISTER_T *prParam, int size);
VAL_RESULT_T eVideoDeInitLockHW( VAL_VCODEC_OAL_HW_REGISTER_T *prParam, int size);
VAL_RESULT_T eVideoVcodecSetThreadID (VAL_VCODEC_THREAD_ID_T *a_prThreadID);

VAL_RESULT_T eVideoVCodecCoreLoading(int CPUid, int *Loading);
VAL_RESULT_T eVideoVCodecCoreNumber(int *CPUNums);

#endif

#ifdef __cplusplus
}
#endif

#endif // #ifndef _VAL_API_H_
