#ifndef _MTK_DEVICE_APC_H
#define _MTK_DEVICE_APC_H

#define AP_DEVAPC0_base             0xF101C000
#define AP_DEVAPC1_base             0xF101D000
#if 0
#define MD_DEVAPC0_base             0xD01A0000//0x801A0000 remapping
#define MD_DEVAPC1_base             0xD1160000//0x81160000 remapping
#define DSP_DEVAPC_base             0xFF070000
#endif
#define MM_DEVAPC0_base             0xF101E000
#define MM_DEVAPC1_base             0xF101F000

#define AP_DEVAPC0_D0_APC_0        ((volatile unsigned int*)(AP_DEVAPC0_base+0x0000))
#define AP_DEVAPC0_D0_APC_1        ((volatile unsigned int*)(AP_DEVAPC0_base+0x0004))
#define AP_DEVAPC0_D1_APC_0        ((volatile unsigned int*)(AP_DEVAPC0_base+0x0008))
#define AP_DEVAPC0_D1_APC_1        ((volatile unsigned int*)(AP_DEVAPC0_base+0x000C))
#define AP_DEVAPC0_D2_APC_0        ((volatile unsigned int*)(AP_DEVAPC0_base+0x0010))
#define AP_DEVAPC0_D2_APC_1        ((volatile unsigned int*)(AP_DEVAPC0_base+0x0014))
#define AP_DEVAPC0_D3_APC_0        ((volatile unsigned int*)(AP_DEVAPC0_base+0x0018))
#define AP_DEVAPC0_D3_APC_1        ((volatile unsigned int*)(AP_DEVAPC0_base+0x001C))
#define AP_DEVAPC0_D0_VIO_MASK     ((volatile unsigned int*)(AP_DEVAPC0_base+0x0020))
#define AP_DEVAPC0_D1_VIO_MASK     ((volatile unsigned int*)(AP_DEVAPC0_base+0x0024))
#define AP_DEVAPC0_D2_VIO_MASK     ((volatile unsigned int*)(AP_DEVAPC0_base+0x0028))
#define AP_DEVAPC0_D3_VIO_MASK     ((volatile unsigned int*)(AP_DEVAPC0_base+0x002C))
#define AP_DEVAPC0_D0_VIO_STA      ((volatile unsigned int*)(AP_DEVAPC0_base+0x0030))
#define AP_DEVAPC0_D1_VIO_STA      ((volatile unsigned int*)(AP_DEVAPC0_base+0x0034))
#define AP_DEVAPC0_D2_VIO_STA      ((volatile unsigned int*)(AP_DEVAPC0_base+0x0038))
#define AP_DEVAPC0_D3_VIO_STA      ((volatile unsigned int*)(AP_DEVAPC0_base+0x003C))
#define AP_DEVAPC0_VIO_DBG0        ((volatile unsigned int*)(AP_DEVAPC0_base+0x0040))
#define AP_DEVAPC0_VIO_DBG1        ((volatile unsigned int*)(AP_DEVAPC0_base+0x0044))
#define AP_DEVAPC0_DXS_VIO_MASK    ((volatile unsigned int*)(AP_DEVAPC0_base+0x0080))
#define AP_DEVAPC0_DXS_VIO_STA     ((volatile unsigned int*)(AP_DEVAPC0_base+0x0084))
#define AP_DEVAPC0_APC_CON         ((volatile unsigned int*)(AP_DEVAPC0_base+0x0090))
#define AP_DEVAPC0_APC_LOCK        ((volatile unsigned int*)(AP_DEVAPC0_base+0x0094))
#define AP_DEVAPC0_MAS_DOM         ((volatile unsigned int*)(AP_DEVAPC0_base+0x00A0))
#define AP_DEVAPC0_MAS_SEC         ((volatile unsigned int*)(AP_DEVAPC0_base+0x00A4))
#define AP_DEVAPC0_DEC_ERR_CON     ((volatile unsigned int*)(AP_DEVAPC0_base+0x00B4))
#define AP_DEVAPC0_DEC_ERR_ADDR    ((volatile unsigned int*)(AP_DEVAPC0_base+0x00B8))
#define AP_DEVAPC0_DEC_ERR_ID      ((volatile unsigned int*)(AP_DEVAPC0_base+0x00BC))

#define AP_DEVAPC1_D0_APC_0        ((volatile unsigned int*)(AP_DEVAPC1_base+0x0000))
#define AP_DEVAPC1_D0_APC_1        ((volatile unsigned int*)(AP_DEVAPC1_base+0x0004))
#define AP_DEVAPC1_D1_APC_0        ((volatile unsigned int*)(AP_DEVAPC1_base+0x0008))
#define AP_DEVAPC1_D1_APC_1        ((volatile unsigned int*)(AP_DEVAPC1_base+0x000C))
#define AP_DEVAPC1_D2_APC_0        ((volatile unsigned int*)(AP_DEVAPC1_base+0x0010))
#define AP_DEVAPC1_D2_APC_1        ((volatile unsigned int*)(AP_DEVAPC1_base+0x0014))
#define AP_DEVAPC1_D3_APC_0        ((volatile unsigned int*)(AP_DEVAPC1_base+0x0018))
#define AP_DEVAPC1_D3_APC_1        ((volatile unsigned int*)(AP_DEVAPC1_base+0x001C))
#define AP_DEVAPC1_D0_VIO_MASK     ((volatile unsigned int*)(AP_DEVAPC1_base+0x0020))
#define AP_DEVAPC1_D1_VIO_MASK     ((volatile unsigned int*)(AP_DEVAPC1_base+0x0024))
#define AP_DEVAPC1_D2_VIO_MASK     ((volatile unsigned int*)(AP_DEVAPC1_base+0x0028))
#define AP_DEVAPC1_D3_VIO_MASK     ((volatile unsigned int*)(AP_DEVAPC1_base+0x002C))
#define AP_DEVAPC1_D0_VIO_STA      ((volatile unsigned int*)(AP_DEVAPC1_base+0x0030))
#define AP_DEVAPC1_D1_VIO_STA      ((volatile unsigned int*)(AP_DEVAPC1_base+0x0034))
#define AP_DEVAPC1_D2_VIO_STA      ((volatile unsigned int*)(AP_DEVAPC1_base+0x0038))
#define AP_DEVAPC1_D3_VIO_STA      ((volatile unsigned int*)(AP_DEVAPC1_base+0x003C))
#define AP_DEVAPC1_VIO_DBG0        ((volatile unsigned int*)(AP_DEVAPC1_base+0x0040))
#define AP_DEVAPC1_VIO_DBG1        ((volatile unsigned int*)(AP_DEVAPC1_base+0x0044))
#define AP_DEVAPC1_DXS_VIO_MASK    ((volatile unsigned int*)(AP_DEVAPC1_base+0x0080))
#define AP_DEVAPC1_DXS_VIO_STA     ((volatile unsigned int*)(AP_DEVAPC1_base+0x0084))
#define AP_DEVAPC1_APC_CON         ((volatile unsigned int*)(AP_DEVAPC1_base+0x0090))
#define AP_DEVAPC1_APC_LOCK        ((volatile unsigned int*)(AP_DEVAPC1_base+0x0094))
#define AP_DEVAPC1_MAS_DOM         ((volatile unsigned int*)(AP_DEVAPC1_base+0x00A0))
#define AP_DEVAPC1_MAS_SEC         ((volatile unsigned int*)(AP_DEVAPC1_base+0x00A4))
#define AP_DEVAPC1_DEC_ERR_CON     ((volatile unsigned int*)(AP_DEVAPC1_base+0x00B4))
#define AP_DEVAPC1_DEC_ERR_ADDR    ((volatile unsigned int*)(AP_DEVAPC1_base+0x00B8))
#define AP_DEVAPC1_DEC_ERR_ID      ((volatile unsigned int*)(AP_DEVAPC1_base+0x00BC))

#if 0
#define MD_DEVAPC0_D0_APC_0        ((volatile unsigned int*)(MD_DEVAPC0_base+0x0000))
#define MD_DEVAPC0_D0_APC_1        ((volatile unsigned int*)(MD_DEVAPC0_base+0x0004))
#define MD_DEVAPC0_D1_APC_0        ((volatile unsigned int*)(MD_DEVAPC0_base+0x0008))
#define MD_DEVAPC0_D1_APC_1        ((volatile unsigned int*)(MD_DEVAPC0_base+0x000C))
#define MD_DEVAPC0_D2_APC_0        ((volatile unsigned int*)(MD_DEVAPC0_base+0x0010))
#define MD_DEVAPC0_D2_APC_1        ((volatile unsigned int*)(MD_DEVAPC0_base+0x0014))
#define MD_DEVAPC0_D0_VIO_MASK     ((volatile unsigned int*)(MD_DEVAPC0_base+0x0020))
#define MD_DEVAPC0_D1_VIO_MASK     ((volatile unsigned int*)(MD_DEVAPC0_base+0x0024))
#define MD_DEVAPC0_D2_VIO_MASK     ((volatile unsigned int*)(MD_DEVAPC0_base+0x0028))
#define MD_DEVAPC0_D0_VIO_STA      ((volatile unsigned int*)(MD_DEVAPC0_base+0x0030))
#define MD_DEVAPC0_D1_VIO_STA      ((volatile unsigned int*)(MD_DEVAPC0_base+0x0034))
#define MD_DEVAPC0_D2_VIO_STA      ((volatile unsigned int*)(MD_DEVAPC0_base+0x0038))
#define MD_DEVAPC0_VIO_DBG0        ((volatile unsigned int*)(MD_DEVAPC0_base+0x0040))
#define MD_DEVAPC0_VIO_DBG1        ((volatile unsigned int*)(MD_DEVAPC0_base+0x0044))
#define MD_DEVAPC0_DXS_VIO_MASK    ((volatile unsigned int*)(MD_DEVAPC0_base+0x0080))
#define MD_DEVAPC0_DXS_VIO_STA     ((volatile unsigned int*)(MD_DEVAPC0_base+0x0084))
#define MD_DEVAPC0_APC_CON         ((volatile unsigned int*)(MD_DEVAPC0_base+0x0090))
#define MD_DEVAPC0_APC_LOCK        ((volatile unsigned int*)(MD_DEVAPC0_base+0x0094))
#define MD_DEVAPC0_MAS_DOM         ((volatile unsigned int*)(MD_DEVAPC0_base+0x00A0))
#define MD_DEVAPC0_MAS_SEC         ((volatile unsigned int*)(MD_DEVAPC0_base+0x00A4))
#define MD_DEVAPC0_DEC_ERR_CON     ((volatile unsigned int*)(MD_DEVAPC0_base+0x00B4))
#define MD_DEVAPC0_DEC_ERR_ADDR    ((volatile unsigned int*)(MD_DEVAPC0_base+0x00B8))

#define MD_DEVAPC1_D0_APC_0        ((volatile unsigned int*)(MD_DEVAPC1_base+0x0000))
#define MD_DEVAPC1_D0_APC_1        ((volatile unsigned int*)(MD_DEVAPC1_base+0x0004))
#define MD_DEVAPC1_D1_APC_0        ((volatile unsigned int*)(MD_DEVAPC1_base+0x0008))
#define MD_DEVAPC1_D1_APC_1        ((volatile unsigned int*)(MD_DEVAPC1_base+0x000C))
#define MD_DEVAPC1_D2_APC_0        ((volatile unsigned int*)(MD_DEVAPC1_base+0x0010))
#define MD_DEVAPC1_D2_APC_1        ((volatile unsigned int*)(MD_DEVAPC1_base+0x0014))
#define MD_DEVAPC1_D0_VIO_MASK     ((volatile unsigned int*)(MD_DEVAPC1_base+0x0020))
#define MD_DEVAPC1_D1_VIO_MASK     ((volatile unsigned int*)(MD_DEVAPC1_base+0x0024))
#define MD_DEVAPC1_D2_VIO_MASK     ((volatile unsigned int*)(MD_DEVAPC1_base+0x0028))
#define MD_DEVAPC1_D0_VIO_STA      ((volatile unsigned int*)(MD_DEVAPC1_base+0x0030))
#define MD_DEVAPC1_D1_VIO_STA      ((volatile unsigned int*)(MD_DEVAPC1_base+0x0034))
#define MD_DEVAPC1_D2_VIO_STA      ((volatile unsigned int*)(MD_DEVAPC1_base+0x0038))
#define MD_DEVAPC1_VIO_DBG0        ((volatile unsigned int*)(MD_DEVAPC1_base+0x0040))
#define MD_DEVAPC1_VIO_DBG1        ((volatile unsigned int*)(MD_DEVAPC1_base+0x0044))
#define MD_DEVAPC1_DXS_VIO_MASK    ((volatile unsigned int*)(MD_DEVAPC1_base+0x0080))
#define MD_DEVAPC1_DXS_VIO_STA     ((volatile unsigned int*)(MD_DEVAPC1_base+0x0084))
#define MD_DEVAPC1_APC_CON         ((volatile unsigned int*)(MD_DEVAPC1_base+0x0090))
#define MD_DEVAPC1_APC_LOCK        ((volatile unsigned int*)(MD_DEVAPC1_base+0x0094))
#define MD_DEVAPC1_MAS_DOM         ((volatile unsigned int*)(MD_DEVAPC1_base+0x00A0))
#define MD_DEVAPC1_MAS_SEC         ((volatile unsigned int*)(MD_DEVAPC1_base+0x00A4))
#define MD_DEVAPC1_DEC_ERR_CON     ((volatile unsigned int*)(MD_DEVAPC1_base+0x00B4))
#define MD_DEVAPC1_DEC_ERR_ADDR    ((volatile unsigned int*)(MD_DEVAPC1_base+0x00B8))

#define DSP_DEVAPC_D0_APC_0		   ((volatile unsigned int*)(DSP_DEVAPC_base+0x0000))
#define DSP_DEVAPC_D0_APC_1        ((volatile unsigned int*)(DSP_DEVAPC_base+0x0004))
#define DSP_DEVAPC_D1_APC_0        ((volatile unsigned int*)(DSP_DEVAPC_base+0x0008))
#define DSP_DEVAPC_D1_APC_1        ((volatile unsigned int*)(DSP_DEVAPC_base+0x000C))
#define DSP_DEVAPC_D2_APC_0        ((volatile unsigned int*)(DSP_DEVAPC_base+0x0010))
#define DSP_DEVAPC_D2_APC_1        ((volatile unsigned int*)(DSP_DEVAPC_base+0x0014))
#define DSP_DEVAPC_D0_VIO_MASK     ((volatile unsigned int*)(DSP_DEVAPC_base+0x0020))
#define DSP_DEVAPC_D1_VIO_MASK     ((volatile unsigned int*)(DSP_DEVAPC_base+0x0024))
#define DSP_DEVAPC_D2_VIO_MASK     ((volatile unsigned int*)(DSP_DEVAPC_base+0x0028))
#define DSP_DEVAPC_D0_VIO_STA      ((volatile unsigned int*)(DSP_DEVAPC_base+0x0030))
#define DSP_DEVAPC_D1_VIO_STA      ((volatile unsigned int*)(DSP_DEVAPC_base+0x0034))
#define DSP_DEVAPC_D2_VIO_STA      ((volatile unsigned int*)(DSP_DEVAPC_base+0x0038))
#define DSP_DEVAPC_VIO_DBG0        ((volatile unsigned int*)(DSP_DEVAPC_base+0x0040))
#define DSP_DEVAPC_VIO_DBG1        ((volatile unsigned int*)(DSP_DEVAPC_base+0x0044))
#define DSP_DEVAPC_DXS_VIO_MASK    ((volatile unsigned int*)(DSP_DEVAPC_base+0x0080))
#define DSP_DEVAPC_DXS_VIO_STA     ((volatile unsigned int*)(DSP_DEVAPC_base+0x0084))
#define DSP_DEVAPC_APC_CON         ((volatile unsigned int*)(DSP_DEVAPC_base+0x0090))
#define DSP_DEVAPC_APC_LOCK        ((volatile unsigned int*)(DSP_DEVAPC_base+0x0094))
#define DSP_DEVAPC_MAS_DOM         ((volatile unsigned int*)(DSP_DEVAPC_base+0x00A0))
#define DSP_DEVAPC_MAS_SEC         ((volatile unsigned int*)(DSP_DEVAPC_base+0x00A4))
#define DSP_DEVAPC_DEC_ERR_CON     ((volatile unsigned int*)(DSP_DEVAPC_base+0x00B4))
#define DSP_DEVAPC_DEC_ERR_ADDR    ((volatile unsigned int*)(DSP_DEVAPC_base+0x00B8))
#endif

#define MM_DEVAPC0_D0_APC_0 	    ((volatile unsigned int*)(MM_DEVAPC0_base+0x0000))
#define MM_DEVAPC0_D0_APC_1        ((volatile unsigned int*)(MM_DEVAPC0_base+0x0004))
#define MM_DEVAPC0_D1_APC_0        ((volatile unsigned int*)(MM_DEVAPC0_base+0x0008))
#define MM_DEVAPC0_D1_APC_1        ((volatile unsigned int*)(MM_DEVAPC0_base+0x000C))
#define MM_DEVAPC0_D2_APC_0        ((volatile unsigned int*)(MM_DEVAPC0_base+0x0010))
#define MM_DEVAPC0_D2_APC_1        ((volatile unsigned int*)(MM_DEVAPC0_base+0x0014))
#define MM_DEVAPC0_D0_VIO_MASK     ((volatile unsigned int*)(MM_DEVAPC0_base+0x0020))
#define MM_DEVAPC0_D1_VIO_MASK     ((volatile unsigned int*)(MM_DEVAPC0_base+0x0024))
#define MM_DEVAPC0_D2_VIO_MASK     ((volatile unsigned int*)(MM_DEVAPC0_base+0x0028))
#define MM_DEVAPC0_D0_VIO_STA      ((volatile unsigned int*)(MM_DEVAPC0_base+0x0030))
#define MM_DEVAPC0_D1_VIO_STA      ((volatile unsigned int*)(MM_DEVAPC0_base+0x0034))
#define MM_DEVAPC0_D2_VIO_STA      ((volatile unsigned int*)(MM_DEVAPC0_base+0x0038))
#define MM_DEVAPC0_VIO_DBG0        ((volatile unsigned int*)(MM_DEVAPC0_base+0x0040))
#define MM_DEVAPC0_VIO_DBG1        ((volatile unsigned int*)(MM_DEVAPC0_base+0x0044))
#define MM_DEVAPC0_APC_CON         ((volatile unsigned int*)(MM_DEVAPC0_base+0x0090))
#define MM_DEVAPC0_APC_LOCK        ((volatile unsigned int*)(MM_DEVAPC0_base+0x0094))
#define MM_DEVAPC0_DEC_ERR_CON     ((volatile unsigned int*)(MM_DEVAPC0_base+0x00B4))
#define MM_DEVAPC0_DEC_ERR_ADDR    ((volatile unsigned int*)(MM_DEVAPC0_base+0x00B8))

#define MM_DEVAPC1_D0_APC_0 	    ((volatile unsigned int*)(MM_DEVAPC1_base+0x0000))
#define MM_DEVAPC1_D1_APC_0        ((volatile unsigned int*)(MM_DEVAPC1_base+0x0008))
#define MM_DEVAPC1_D2_APC_0        ((volatile unsigned int*)(MM_DEVAPC1_base+0x0010))
#define MM_DEVAPC1_D0_VIO_MASK     ((volatile unsigned int*)(MM_DEVAPC1_base+0x0020))
#define MM_DEVAPC1_D1_VIO_MASK     ((volatile unsigned int*)(MM_DEVAPC1_base+0x0024))
#define MM_DEVAPC1_D2_VIO_MASK     ((volatile unsigned int*)(MM_DEVAPC1_base+0x0028))
#define MM_DEVAPC1_D0_VIO_STA      ((volatile unsigned int*)(MM_DEVAPC1_base+0x0030))
#define MM_DEVAPC1_D1_VIO_STA      ((volatile unsigned int*)(MM_DEVAPC1_base+0x0034))
#define MM_DEVAPC1_D2_VIO_STA      ((volatile unsigned int*)(MM_DEVAPC1_base+0x0038))
#define MM_DEVAPC1_VIO_DBG0        ((volatile unsigned int*)(MM_DEVAPC1_base+0x0040))
#define MM_DEVAPC1_VIO_DBG1        ((volatile unsigned int*)(MM_DEVAPC1_base+0x0044))
#define MM_DEVAPC1_APC_CON         ((volatile unsigned int*)(MM_DEVAPC1_base+0x0090))
#define MM_DEVAPC1_APC_LOCK        ((volatile unsigned int*)(MM_DEVAPC1_base+0x0094))

/* DOMAIN_SETUP */
#define DOMAIN_AP						0
#define DOMAIN_FCORE						1
#define DOMAIN_MD						2

/* Masks for Domain Control for AP DEVAPC1 */
#define APPER_USB_1_DOM_CTRL            (0x3 << 10)  /* R/W */
#define APPER_USB_2_DOM_CTRL            (0x3 << 8)   /* R/W */
#define APPER_PWM_DOM_CTRL              (0x3 << 6)   /* R/W */
#define APPER_NFI_DOM_CTRL              (0x3 << 4)   /* R/W */
#define DEBUG_DOM_CTRL                  (0x3 << 2)   /* R/W */
#define AUDIO_DOM_CTRL                  (0x3 << 0)   /* R/W */

/* Masks for Domain Control for MD DEVAPC */
#define MDPER_LOGACC_DOM_CTRL           (0x3 << 8)   /* R/W */
#define MDPER_PFC_DOM_CTRL              (0x3 << 6)   /* R/W */
#define MD_DMA_DOM_CTRL                 (0x3 << 4)   /* R/W */
#define MD_3G_DOM_CTRL                  (0x3 << 2)   /* R/W */
#define MD_2G_DOM_CTRL                  (0x3 << 0)   /* R/W */

/* Masks for Domain Control for DSP DEVAPC */
#define FCORE_D1_PORT_DOM_CTRL          (0x3 << 4)   /* R/W */
#define FCORE_D0_PORT_DOM_CTRL          (0x3 << 2)   /* R/W */
#define FCORE_I_PORT_DOM_CTRL           (0x3 << 0)   /* R/W */

#endif
