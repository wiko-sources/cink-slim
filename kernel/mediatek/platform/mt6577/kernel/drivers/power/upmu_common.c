/*****************************************************************************
*
* Filename:
* ---------
*   upmu_common.c
*
* Project:
* --------
*   Android
*
* Description:
* ------------
*   Unified PMU common utility function implementation
*
* Author:
* -------
*
****************************************************************************/

#include <linux/kernel.h>
#include <linux/xlog.h>

#include <mach/upmu_hw.h>
#include <mach/pmic_mt6329_sw.h>
#include <mach/pmic_mt6329_hw.h>
#include <mach/pmic_mt6329_sw_bank1.h>
#include <mach/pmic_mt6329_hw_bank1.h>

//#define PMIC_MT6326_FLAG
#define PMIC_MT6329_FLAG

//==============================================================================
// General APIs
//==============================================================================
void upmu_write_reg_value(kal_uint32 reg, kal_uint32 val, kal_uint32 mask, kal_uint32 shift)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface(reg, val, mask, shift);
}

void upmu_write_reg_value_bank1(kal_uint32 reg, kal_uint32 val, kal_uint32 mask, kal_uint32 shift)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface(reg, val, mask, shift);
}

kal_uint32 upmu_get_reg_value(kal_uint32 reg)
{
	kal_uint32 ret=0;
	kal_uint8 temp_val=0;

	ret=pmic_read_interface(reg, &temp_val, 0xFF, 0x0);

	return temp_val;
}

kal_uint32 upmu_get_reg_value_bank1(kal_uint32 reg)
{
	kal_uint32 ret=0;
	kal_uint8 temp_val=0;

	ret=pmic_bank1_read_interface(reg, &temp_val, 0xFF, 0x0);

	return temp_val;
}

kal_uint32 upmu_get_cid0(void)
{
	kal_uint32 ret=0;
	kal_uint8 temp_val=0;

	ret=pmic_read_interface(BANK0_CID0, &temp_val, BANK_0_CID0_MASK, BANK_0_CID0_SHIFT);

	return temp_val;
}

kal_uint32 upmu_get_cid1(void)
{
	kal_uint32 ret=0;
	kal_uint8 temp_val=0;

	ret=pmic_read_interface(BANK0_CID1, &temp_val, BANK_0_CID1_MASK, BANK_0_CID1_SHIFT);

	return temp_val;
}

kal_uint32 upmu_get_PCHR_CHRDET(void)
{
	kal_uint32 ret=0;
	kal_uint8 temp_val=0;

	#if defined(PMIC_MT6326_FLAG)
	ret = pmic_int_en_interface(PMIC_INIT_CTRL_1, INT1_EN_ALL, KAL_FALSE);
    ret = pmic_int_en_interface(PMIC_INIT_CTRL_2, INT2_EN_ALL, KAL_TRUE);
    ret = pmic_int_en_interface(PMIC_INIT_CTRL_3, INT3_EN_ALL, KAL_FALSE);
    ret = pmic_int_en_interface(PMIC_INIT_CTRL_3, INT_EN_CHRDET, KAL_TRUE);
	#endif

	ret=pmic_read_interface(BANK0_CHRSTATUS, &temp_val, BANK_0_PCHR_CHRDET_MASK, BANK_0_PCHR_CHRDET_SHIFT);

	return temp_val;
}

//==============================================================================
// LDO ON/OFF Status APIs
//==============================================================================
//CON - 0
kal_uint32 upmu_status_vcore(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCORE_STATUS_MASK),
								(kal_uint8)(BANK_0_VCORE_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vproc(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VPROC_STATUS_MASK),
								(kal_uint8)(BANK_0_VPROC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vio18(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VIO18_STATUS_MASK),
								(kal_uint8)(BANK_0_VIO18_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vrf18(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VRF18_STATUS_MASK),
								(kal_uint8)(BANK_0_VRF18_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vpa(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VPA_STATUS_MASK),
								(kal_uint8)(BANK_0_VPA_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vrf(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VRF_STATUS_MASK),
								(kal_uint8)(BANK_0_VRF_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vtcxo(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VTCXO_STATUS_MASK),
								(kal_uint8)(BANK_0_VTCXO_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_va1(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VA1_STATUS_MASK),
								(kal_uint8)(BANK_0_VA1_STATUS_SHIFT)
								);
	return val;
}

//CON 1
kal_uint32 upmu_status_vcama(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCAMA_STATUS_MASK),
								(kal_uint8)(BANK_0_VCAMA_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vm12_1(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VM12_1_STATUS_MASK),
								(kal_uint8)(BANK_0_VM12_1_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vm12_int(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VM12_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_VM12_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vio28(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VIO28_STATUS_MASK),
								(kal_uint8)(BANK_0_VIO28_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vsim1(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VSIM1_STATUS_MASK),
								(kal_uint8)(BANK_0_VSIM1_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vsim2(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VSIM2_STATUS_MASK),
								(kal_uint8)(BANK_0_VSIM2_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vusb(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VUSB_STATUS_MASK),
								(kal_uint8)(BANK_0_VUSB_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vcamd(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCAMD_STATUS_MASK),
								(kal_uint8)(BANK_0_VCAMD_STATUS_SHIFT)
								);
	return val;
}

//CON 2
kal_uint32 upmu_status_vcam_io(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCAM_IO_STATUS_MASK),
								(kal_uint8)(BANK_0_VCAM_IO_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vcam_af(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCAM_AF_STATUS_MASK),
								(kal_uint8)(BANK_0_VCAM_AF_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vmc(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VMC_STATUS_MASK),
								(kal_uint8)(BANK_0_VMC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vmch(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VMCH_STATUS_MASK),
								(kal_uint8)(BANK_0_VMCH_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vgp(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VGP_STATUS_MASK),
								(kal_uint8)(BANK_0_VGP_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vgp2(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VGP2_STATUS_MASK),
								(kal_uint8)(BANK_0_VGP2_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vm12_2(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VM12_2_STATUS_MASK),
								(kal_uint8)(BANK_0_VM12_2_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vibr(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VIBR_STATUS_MASK),
								(kal_uint8)(BANK_0_VIBR_STATUS_SHIFT)
								);
	return val;
}

//CON 3
kal_uint32 upmu_status_va2(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VA2_STATUS_MASK),
								(kal_uint8)(BANK_0_VA2_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vrtc(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VRTC_STATUS_MASK),
								(kal_uint8)(BANK_0_VRTC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vtcxo_pg(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS + STATUS_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VTCXO_PG_STATUS_MASK),
								(kal_uint8)(BANK_0_VTCXO_PG_STATUS_SHIFT)
								);
	return val;
}

//==============================================================================
// Power Good Status APIs
//==============================================================================
//CON 0
kal_uint32 upmu_status_vcore_pg(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_PG + STATUS_PG_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCORE_PG_STATUS_MASK),
								(kal_uint8)(BANK_0_VCORE_PG_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vproc_pg(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_PG + STATUS_PG_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VPROC_PG_STATUS_MASK),
								(kal_uint8)(BANK_0_VPROC_PG_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vio18_pg(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_PG + STATUS_PG_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VIO18_PG_STATUS_MASK),
								(kal_uint8)(BANK_0_VIO18_PG_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vio28_pg(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_PG + STATUS_PG_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VIO28_PG_STATUS_MASK),
								(kal_uint8)(BANK_0_VIO28_PG_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_va1_pg(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_PG + STATUS_PG_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VA1_PG_STATUS_MASK),
								(kal_uint8)(BANK_0_VA1_PG_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vm12_1_pg(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_PG + STATUS_PG_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VM12_1_PG_STATUS_MASK),
								(kal_uint8)(BANK_0_VM12_1_PG_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vm12_2_pg(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_PG + STATUS_PG_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VM12_2_PG_STATUS_MASK),
								(kal_uint8)(BANK_0_VM12_2_PG_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_vm12_int_pg(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_PG + STATUS_PG_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VM12_INT_PG_STATUS_MASK),
								(kal_uint8)(BANK_0_VM12_INT_PG_STATUS_SHIFT)
								);
	return val;
}

//==============================================================================
// Charger Status APIs
//==============================================================================
//CON 0
kal_uint32 upmu_status_chr_cv(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_CHR + STATUS_CHR_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_CV_MASK),
								(kal_uint8)(BANK_0_CV_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_chr_ro_baton_undet(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_CHR + STATUS_CHR_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RO_BATON_UNDET_MASK),
								(kal_uint8)(BANK_0_RO_BATON_UNDET_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_chr_pchr_chrdet(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_CHR + STATUS_CHR_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_PCHR_CHRDET_MASK),
								(kal_uint8)(BANK_0_PCHR_CHRDET_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_chr_vbat_ov(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_CHR + STATUS_CHR_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VBAT_OV_MASK),
								(kal_uint8)(BANK_0_VBAT_OV_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_chr_pwrkey_deb(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_CHR + STATUS_CHR_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_PWRKEY_DEB_MASK),
								(kal_uint8)(BANK_0_PWRKEY_DEB_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_chr_usb_dl(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_CHR + STATUS_CHR_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_USBDL_MASK),
								(kal_uint8)(BANK_0_USBDL_SET_MASK)
								);
	return val;
}

kal_uint32 upmu_status_chr_test_mode_por(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_CHR + STATUS_CHR_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_TEST_MODE_POR_MASK),
								(kal_uint8)(BANK_0_TEST_MODE_POR_SHIFT)
								);
	return val;
}

//==============================================================================
// OC Status APIs
//==============================================================================
//CON 0
kal_uint32 upmu_status_oc_vcama(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCAMA_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VCAMA_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vm12_1(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VM12_1_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VM12_1_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vm12_int(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VM12_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VM12_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vio28(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VIO28_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VIO28_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vsim1(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VSIM1_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VSIM1_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vsim2(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VSIM2_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VSIM2_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vusb(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VUSB_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VUSB_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vcamad(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCAMD_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VCAMD_OC_STATUS_SHIFT)
								);
	return val;
}

//CON 1
kal_uint32 upmu_status_oc_vcam_io(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCAM_IO_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VCAM_IO_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vcam_af(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCAM_AF_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VCAM_AF_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vmc(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VMC_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VMC_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vmch(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VMCH_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VMCH_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vgp(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VGP_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VGP_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vgp2(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VGP2_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VGP2_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vm12_2(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VM12_2_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VM12_2_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vibr(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VIBR_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VIBR_OC_STATUS_SHIFT)
								);
	return val;
}

//CON 2
kal_uint32 upmu_status_oc_homekey_deb(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_HOMEKEY_DEB_MASK),
								(kal_uint8)(BANK_0_HOMEKEY_DEB_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_boost(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_BOOST_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_BOOST_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_va2(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VA2_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VA2_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_ni_spk_det_d_r(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_NI_SPK_OC_DET_D_R_MASK),
								(kal_uint8)(BANK_0_NI_SPK_OC_DET_D_R_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_ni_spk_det_d_l(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_NI_SPK_OC_DET_D_L_MASK),
								(kal_uint8)(BANK_0_NI_SPK_OC_DET_D_L_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_ni_spk_det_ab_r(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_NI_SPK_OC_DET_AB_R_MASK),
								(kal_uint8)(BANK_0_NI_SPK_OC_DET_AB_R_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_ni_spk_det_ab_l(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_NI_SPK_OC_DET_AB_L_MASK),
								(kal_uint8)(BANK_0_NI_SPK_OC_DET_AB_L_SHIFT)
								);
	return val;
}

//CON 3
kal_uint32 upmu_status_oc_vcore(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCORE_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VCORE_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vproc(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VPROC_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VPROC_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vio18(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VIO18_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VIO18_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vrf18(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VRF18_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VRF18_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vpa(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VPA_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VPA_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vrf(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VRF_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VRF_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_vtcxo(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VTCXO_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VTCXO_OC_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_status_oc_va1(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STATUS_OC + STATUS_OC_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VA1_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_VA1_OC_STATUS_SHIFT)
								);
	return val;
}

//==============================================================================
// STARTUP APIs
//==============================================================================
//CON 0
void upmu_strup_thrdet_sel(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_THRDET_SEL_MASK),
								(kal_uint8)(BANK_0_RG_THRDET_SEL_SHIFT)
								);
}

void upmu_strup_thr_hwpdn_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_THR_HWPDN_EN_MASK),
								(kal_uint8)(BANK_0_RG_THR_HWPDN_EN_SHIFT)
								);
}

void upmu_strup_thr_sel(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_STRUP_THR_SEL_MASK),
								(kal_uint8)(BANK_0_RG_STRUP_THR_SEL_SHIFT)
								);
}

void upmu_strup_thr_test_mode(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_THR_TMODE_MASK),
								(kal_uint8)(BANK_0_RG_THR_TMODE_SHIFT)
								);
}

void upmu_strup_thr_force_non_thermal(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_FORCE_NON_THERMAL_MASK),
								(kal_uint8)(BANK_0_RG_FORCE_NON_THERMAL_SHIFT)
								);
}

//CON 1
void upmu_strup_vref_bg(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VREF_BG_MASK),
								(kal_uint8)(BANK_0_RG_VREF_BG_SHIFT)
								);
}

void upmu_strup_iref_trim(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_STRUP_IREF_TRIM_MASK),
								(kal_uint8)(BANK_0_RG_STRUP_IREF_TRIM_SHIFT)
								);
}

//CON 2
void upmu_strup_bb_proc_sel(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BB_PROC_SEL_MASK),
								(kal_uint8)(BANK_0_RG_BB_PROC_SEL_SHIFT)
								);
}

void upmu_strup_rst_drvsel(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_STRUP_RST_DRVSEL_MASK),
								(kal_uint8)(BANK_0_RG_STRUP_RST_DRVSEL_SHIFT)
								);
}

void upmu_strup_pmu_lev_ungate(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_PMU_LEV_UNGATE_MASK),
								(kal_uint8)(BANK_0_RG_PMU_LEV_UNGATE_SHIFT)
								);
}

void upmu_strup_usbdl_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_USBDL_EN_MASK),
								(kal_uint8)(BANK_0_RG_USBDL_EN_SHIFT)
								);
}

//CON 3
void upmu_strup_pmu_rsv(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON3_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_PMU_RSV_MASK),
								(kal_uint8)(BANK_0_RG_PMU_RSV_SHIFT)
								);
}

//CON 4
void upmu_strup_bias_gen_en_force(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON4_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BIAS_GEN_EN_FORCE_MASK),
								(kal_uint8)(BANK_0_RG_BIAS_GEN_EN_FORCE_SHIFT)
								);
}

void upmu_strup_pwron_force(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON4_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_STRUP_PWRON_FORCE_MASK),
								(kal_uint8)(BANK_0_RG_STRUP_PWRON_FORCE_SHIFT)
								);
}

void upmu_strup_test_mode(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON4_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_STRUP_TEST_MASK),
								(kal_uint8)(BANK_0_RG_STRUP_TEST_SHIFT)
								);
}

void upmu_strup_ft_ctrl_test_mode(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON4_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_STRUP_FT_CTRL_MASK),
								(kal_uint8)(BANK_0_RG_STRUP_FT_CTRL_SHIFT)
								);
}

//CON 5
kal_uint32 upmu_strup_pmu_thr_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON5_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_PMU_THR_STATUS_MASK),
								(kal_uint8)(BANK_0_PMU_THR_STATUS_SHIFT)
								);
	return val;
}

void upmu_strup_thermal_test_mode(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON5_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_THERMAL_TEST_MASK),
								(kal_uint8)(BANK_0_RG_THERMAL_TEST_SHIFT)
								);
}

//CON 6
void upmu_strup_vcore_pg_enb(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VCORE_PG_ENB_MASK),
								(kal_uint8)(BANK_0_RG_VCORE_PG_ENB_SHIFT)
								);
}

void upmu_strup_vproc_pg_enb(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VPROC_PG_ENB_MASK),
								(kal_uint8)(BANK_0_RG_VPROC_PG_ENB_SHIFT)
								);
}

void upmu_strup_vio18_pg_enb(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VIO18_PG_ENB_MASK),
								(kal_uint8)(BANK_0_RG_VIO18_PG_ENB_SHIFT)
								);
}

void upmu_strup_vio28_pg_enb(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VIO28_PG_ENB_MASK),
								(kal_uint8)(BANK_0_RG_VIO28_PG_ENB_SHIFT)
								);
}

void upmu_strup_va1_pg_enb(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VA1_PG_ENB_MASK),
								(kal_uint8)(BANK_0_RG_VA1_PG_ENB_SHIFT)
								);
}

void upmu_strup_vm12_1_pg_enb(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VM12_1_PG_ENB_MASK),
								(kal_uint8)(BANK_0_RG_VM12_1_PG_ENB_SHIFT)
								);
}

void upmu_strup_vm12_2_pg_enb(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VM12_2_PG_ENB_MASK),
								(kal_uint8)(BANK_0_RG_VM12_2_PG_ENB_SHIFT)
								);
}

void upmu_strup_vm12_int_pg_enb(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VM12_INT_PG_ENB_MASK),
								(kal_uint8)(BANK_0_RG_VM12_INT_PG_ENB_SHIFT)
								);
}

//CON 7
void upmu_strup_dig_rsv(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON7_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_STRUP_DIG_RSV_MASK),
								(kal_uint8)(BANK_0_RG_STRUP_DIG_RSV_SHIFT)
								);
}

//==============================================================================
// Interrupt control APIs
//==============================================================================
//CON 0
void upmu_interrupt_ov_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_OV_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_OV_INT_EN_SHIFT)
								);
}

void upmu_interrupt_chrdet_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_CHRDET_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_CHRDET_INT_EN_SHIFT)
								);
}

void upmu_interrupt_bvalid_det_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BVALID_DET_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_BVALID_DET_INT_EN_SHIFT)
								);
}

void upmu_interrupt_vbaton_undet_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VBATON_UNDET_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_VBATON_UNDET_INT_EN_SHIFT)
								);
}

void upmu_interrupt_thr_h_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_THR_H_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_THR_H_INT_EN_SHIFT)
								);
}

void upmu_interrupt_thr_l_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_THR_L_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_THR_L_INT_EN_SHIFT)
								);
}

void upmu_interrupt_pwrkey_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_PWRKEY_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_PWRKEY_INT_EN_SHIFT)
								);
}

void upmu_interrupt_watchdog_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_WATCHDOG_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_WATCHDOG_INT_EN_SHIFT)
								);
}

//CON 1
void upmu_interrupt_fg_bat_h_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_FG_BAT_H_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_FG_BAT_H_INT_EN_SHIFT)
								);
}

void upmu_interrupt_fg_bat_l_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_FG_BAT_L_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_FG_BAT_L_INT_EN_SHIFT)
								);
}

void upmu_interrupt_high_bat_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_HIGH_BAT_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_HIGH_BAT_INT_EN_SHIFT)
								);
}

void upmu_interrupt_low_bat_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LOW_BAT_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_LOW_BAT_INT_EN_SHIFT)
								);
}

void upmu_interrupt_spkr_d_oc_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_SPKR_D_OC_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_SPKR_D_OC_INT_EN_SHIFT)
								);
}

void upmu_interrupt_spkl_d_oc_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_SPKL_D_OC_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_SPKL_D_OC_INT_EN_SHIFT)
								);
}

void upmu_interrupt_spkr_ab_oc_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_SPKR_AB_OC_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_SPKR_AB_OC_INT_EN_SHIFT)
								);
}

void upmu_interrupt_spkl_ab_oc_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_SPKL_AB_OC_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_SPKL_AB_OC_INT_EN_SHIFT)
								);
}

//CON 2
void upmu_interrupt_homekey_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_HOMEKEY_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_HOMEKEY_INT_EN_SHIFT)
								);
}

void upmu_interrupt_vrf18_oc_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VRF18_OC_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_VRF18_OC_INT_EN_SHIFT)
								);
}

void upmu_interrupt_vpa_oc_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_VPA_OC_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_VPA_OC_INT_EN_SHIFT)
								);
}

void upmu_interrupt_ldo_oc_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LDO_OC_INT_EN_MASK),
								(kal_uint8)(BANK_0_RG_LDO_OC_INT_EN_SHIFT)
								);
}

//CON 3
kal_uint32 upmu_interrupt_get_ov_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_OV_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_OV_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_chrdet_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_CHRDET_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_CHRDET_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_bvalid_det_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_BVALID_DET_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_BVALID_DET_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_vbaton_undet_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_VBATON_UNDET_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_VBATON_UNDET_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_thr_h_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_THR_H_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_THR_H_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_thr_l_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_THR_L_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_THR_L_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_pwrkey_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_PWRKEY_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_PWRKEY_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_watchdog_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_WATCHDOG_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_WATCHDOG_INT_STATUS_SHIFT)
								);
	return val;
}

//CON 4
kal_uint32 upmu_interrupt_get_fg_bat_h_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON4_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_FG_BAT_H_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_FG_BAT_H_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_fg_bat_l_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON4_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_FG_BAT_L_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_FG_BAT_L_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_high_bat_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON4_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_HIGH_BAT_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_HIGH_BAT_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_low_bat_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON4_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_LOW_BAT_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_LOW_BAT_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_spkr_d_oc_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON4_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_SPKR_D_OC_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_SPKR_D_OC_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_spkl_d_oc_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON4_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_SPKL_D_OC_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_SPKL_D_OC_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_spkr_ab_oc_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON4_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_SPKR_AB_OC_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_SPKR_AB_OC_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_spkl_ab_oc_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON4_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_SPKL_AB_OC_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_SPKL_AB_OC_INT_STATUS_SHIFT)
								);
	return val;
}

//CON 5
kal_uint32 upmu_interrupt_get_homekey_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON5_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_HOMEKEY_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_HOMEKEY_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_vrf18_oc_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON5_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_VRF18_OC_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_VRF18_OC_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_vpa_oc_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON5_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_VPA_OC_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_VPA_OC_INT_STATUS_SHIFT)
								);
	return val;
}

kal_uint32 upmu_interrupt_get_ldo_oc_int_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_INTERRUPT + INTTERRUPT_CON5_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_LDO_OC_INT_STATUS_MASK),
								(kal_uint8)(BANK_0_RG_LDO_OC_INT_STATUS_SHIFT)
								);
	return val;
}

//==============================================================================
// Charger APIs
//==============================================================================
//CON 0
kal_uint32 upmu_chr_get_vcdt_hv_det(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCDT_HV_DET_MASK),
								(kal_uint8)(BANK_0_VCDT_HV_DET_SHIFT)
								);
	return val;
}

kal_uint32 upmu_chr_get_vcdt_lv_det(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VCDT_LV_DET_MASK),
								(kal_uint8)(BANK_0_VCDT_LV_DET_SHIFT)
								);
	return val;
}

kal_uint32 upmu_chr_get_chrdet(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_CHRDET_MASK),
								(kal_uint8)(BANK_0_CHRDET_SHIFT)
								);
	return val;
}

void upmu_chr_enable(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CHR_EN_MASK),
								(kal_uint8)(BANK_0_CHR_EN_SHIFT)
								);
}

void upmu_chr_csdac_enable(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CSDAC_EN_MASK),
								(kal_uint8)(BANK_0_CSDAC_EN_SHIFT)
								);
}

void upmu_chr_hw_auto_enable(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_PCHR_AUTO_MASK),
								(kal_uint8)(BANK_0_PCHR_AUTO_SHIFT)
								);
}

kal_uint32 upmu_chr_get_ldo_det(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_CHR_LDO_DET_MASK),
								(kal_uint8)(BANK_0_CHR_LDO_DET_SHIFT)
								);
	return val;
}

void upmu_chr_vcdt_hv_enable(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_VCDT_HV_EN_MASK),
								(kal_uint8)(BANK_0_VCDT_HV_EN_SHIFT)
								);
}

//CON1
void upmu_chr_vcdt_hv_vth(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_VCDT_HV_VTH_MASK),
								(kal_uint8)(BANK_0_VCDT_HV_VTH_SHIFT)
								);
}

void upmu_chr_vcdt_lv_vth(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_VCDT_LV_VTH_MASK),
								(kal_uint8)(BANK_0_VCDT_LV_VTH_SHIFT)
								);
}

//CON2
kal_uint32 upmu_chr_get_vbat_cc_det(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VBAT_CC_DET_MASK),
								(kal_uint8)(BANK_0_VBAT_CC_DET_SHIFT)
								);
	return val;
}

kal_uint32 upmu_chr_get_vbat_cv_det(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VBAT_CV_DET_MASK),
								(kal_uint8)(BANK_0_VBAT_CV_DET_SHIFT)
								);
	return val;
}

kal_uint32 upmu_chr_get_cs_det(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_CS_DET_MASK),
								(kal_uint8)(BANK_0_CS_DET_SHIFT)
								);
	return val;
}

void upmu_chr_cs_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CS_EN_MASK),
								(kal_uint8)(BANK_0_CS_EN_SHIFT)
								);
}

void upmu_chr_vbat_cc_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_VBAT_CC_EN_MASK),
								(kal_uint8)(BANK_0_VBAT_CC_EN_SHIFT)
								);
}

void upmu_chr_vbat_cv_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_VBAT_CV_EN_MASK),
								(kal_uint8)(BANK_0_VBAT_CV_EN_SHIFT)
								);
}

//CON3
void upmu_chr_vbat_cc_vth(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON3_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_VBAT_CC_VTH_MASK),
								(kal_uint8)(BANK_0_VBAT_CC_VTH_SHIFT)
								);
}

void upmu_chr_vbat_cv_vth(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON3_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_VBAT_CV_VTH_MASK),
								(kal_uint8)(BANK_0_VBAT_CV_VTH_SHIFT)
								);
}

//CON4
void upmu_chr_vbat_cs_vth(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON4_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CS_VTH_MASK),
								(kal_uint8)(BANK_0_CS_VTH_SHIFT)
								);
}

//CON5
void upmu_chr_toltc(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON5_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_TOLTC_MASK),
								(kal_uint8)(BANK_0_TOLTC_SHIFT)
								);
}

void upmu_chr_tohtc(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON5_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_TOHTC_MASK),
								(kal_uint8)(BANK_0_TOHTC_SHIFT)
								);
}

//CON6
kal_uint32 upmu_chr_get_vbat_ov_det(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CON6_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_VBAT_OV_DET_MASK),
								(kal_uint8)(BANK_0_VBAT_OV_DET_SHIFT)
								);
	return val;
}

void upmu_chr_vbat_ov_deg(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_VBAT_OV_DEG_MASK),
								(kal_uint8)(BANK_0_VBAT_OV_DEG_SHIFT)
								);
}

void upmu_chr_vbat_ov_vth(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_VBAT_OV_VTH_MASK),
								(kal_uint8)(BANK_0_VBAT_OV_VTH_SHIFT)
								);
}

void upmu_chr_vbat_ov_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_VBAT_OV_EN_MASK),
								(kal_uint8)(BANK_0_VBAT_OV_EN_SHIFT)
								);
}

//CON7
kal_uint32 upmu_chr_get_baton_undet(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CON7_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_BATON_UNDET_MASK),
								(kal_uint8)(BANK_0_BATON_UNDET_SHIFT)
								);
	return val;
}

void upmu_chr_baton_ht_trim(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON7_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BATON_HT_TRIM_MASK),
								(kal_uint8)(BANK_0_BATON_HT_TRIM_SHIFT)
								);
}

void upmu_chr_baton_ht_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON7_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BATON_HT_EN_MASK),
								(kal_uint8)(BANK_0_BATON_HT_EN_SHIFT)
								);
}

void upmu_chr_baton_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON7_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BATON_EN_MASK),
								(kal_uint8)(BANK_0_BATON_EN_SHIFT)
								);
}

//CON8
void upmu_chr_csdac_dat_h(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON8_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CSDAC_DAT_H_MASK),
								(kal_uint8)(BANK_0_CSDAC_DAT_H_SHIFT)
								);
}

//CON9
void upmu_chr_csdac_dat_l(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON9_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CSDAC_DAT_L_MASK),
								(kal_uint8)(BANK_0_CSDAC_DAT_L_SHIFT)
								);
}

//CONA
kal_uint32 upmu_chr_get_otg_bvalid(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CONA_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_OTG_BVALID_MASK),
								(kal_uint8)(BANK_0_OTG_BVALID_SHIFT)
								);
	return val;
}

void upmu_chr_otg_bvalid_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CONA_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_OTG_BVALID_EN_MASK),
								(kal_uint8)(BANK_0_OTG_BVALID_EN_SHIFT)
								);
}

void upmu_chr_pchr_flag_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CONA_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_PCHR_FLAG_EN_MASK),
								(kal_uint8)(BANK_0_PCHR_FLAG_EN_SHIFT)
								);
}

void upmu_chr_pchr_flag_out(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CONA_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_PCHR_FLAG_OUT_MASK),
								(kal_uint8)(BANK_0_PCHR_FLAG_OUT_SHIFT)
								);
}

//CONB
void upmu_chr_pchr_flag_sel(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CONB_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_PCHR_FLAG_SEL_MASK),
								(kal_uint8)(BANK_0_PCHR_FLAG_SEL_SHIFT)
								);
}

//CONC
void upmu_chr_pchr_ft_ctrl(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CONC_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_PCHR_FT_CTRL_MASK),
								(kal_uint8)(BANK_0_PCHR_FT_CTRL_SHIFT)
								);
}

void upmu_chr_pchr_rst(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CONC_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_PCHR_RST_MASK),
								(kal_uint8)(BANK_0_PCHR_RST_SHIFT)
								);
}

void upmu_chr_csdac_test_mode(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CONC_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CSDAC_TEST_MASK),
								(kal_uint8)(BANK_0_CSDAC_TEST_SHIFT)
								);
}

void upmu_chr_pchr_test_mode(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CONC_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_PCHR_TEST_MASK),
								(kal_uint8)(BANK_0_PCHR_TEST_SHIFT)
								);
}

//COND
void upmu_chr_chrwdt_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_COND_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CHRWDT_EN_MASK),
								(kal_uint8)(BANK_0_CHRWDT_EN_SHIFT)
								);
}

void upmu_chr_chrwdt_td(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_COND_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CHRWDT_TD_MASK),
								(kal_uint8)(BANK_0_CHRWDT_TD_SHIFT)
								);
}

//CONE
void upmu_chr_pchr_rv(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CONE_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_PCHR_RV_MASK),
								(kal_uint8)(BANK_0_PCHR_RV_SHIFT)
								);
}

//CONF
kal_uint32 upmu_chr_get_chrwdt_out(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CONF_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_CHRWDT_OUT_MASK),
								(kal_uint8)(BANK_0_CHRWDT_OUT_SHIFT)
								);
	return val;
}

void upmu_chr_chrwdt_flag_wr(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CONF_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CHRWDT_FLAG_WR_MASK),
								(kal_uint8)(BANK_0_CHRWDT_FLAG_WR_SHIFT)
								);
}

void upmu_chr_chrwdt_int_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CONF_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CHRWDT_INT_EN_MASK),
								(kal_uint8)(BANK_0_CHRWDT_INT_EN_SHIFT)
								);
}

//CON11
void upmu_chr_adcin_vchr_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON11_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_ADCIN_VCHR_EN_MASK),
								(kal_uint8)(BANK_0_ADCIN_VCHR_EN_SHIFT)
								);
}

void upmu_chr_adcin_vsen_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON11_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_ADCIN_VSEN_EN_MASK),
								(kal_uint8)(BANK_0_ADCIN_VSEN_EN_SHIFT)
								);
}

void upmu_chr_adcin_vbat_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON11_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_ADCIN_VBAT_EN_MASK),
								(kal_uint8)(BANK_0_ADCIN_VBAT_EN_SHIFT)
								);
}

void upmu_chr_usbdl_set(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON11_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_USBDL_SET_MASK),
								(kal_uint8)(BANK_0_USBDL_SET_SHIFT)
								);
}

void upmu_chr_usbdl_rst(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON11_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_USBDL_RST_MASK),
								(kal_uint8)(BANK_0_USBDL_RST_SHIFT)
								);
}

void upmu_chr_uvlo_vthl(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON11_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_UVLO_VTHL_MASK),
								(kal_uint8)(BANK_0_UVLO_VTHL_SHIFT)
								);
}

//CON12
void upmu_chr_bgr_unchop(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON12_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BGR_UNCHOP_MASK),
								(kal_uint8)(BANK_0_BGR_UNCHOP_SHIFT)
								);
}

void upmu_chr_bgr_unchop_ph(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON12_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BGR_UNCHOP_PH_MASK),
								(kal_uint8)(BANK_0_BGR_UNCHOP_PH_SHIFT)
								);
}

void upmu_chr_bgr_rsel(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON12_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BGR_RSEL_MASK),
								(kal_uint8)(BANK_0_BGR_RSEL_SHIFT)
								);
}

//CON13
kal_uint32 upmu_chr_get_bc11_cmp_out(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface(	(kal_uint8)(BANK0_CHARGER + CHR_CON13_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_BC11_CMP_OUT_MASK),
								(kal_uint8)(BANK_0_BC11_CMP_OUT_SHIFT)
								);
	return val;
}

void upmu_chr_bc11_vsrc_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON13_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BC11_VSRC_EN_MASK),
								(kal_uint8)(BANK_0_RG_BC11_VSRC_EN_SHIFT)
								);
}

void upmu_chr_bc11_rst(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON13_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BC11_RST_MASK),
								(kal_uint8)(BANK_0_BC11_RST_SHIFT)
								);
}

void upmu_chr_bc11_bb_ctrl(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON13_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BC11_BB_CTRL_MASK),
								(kal_uint8)(BANK_0_BC11_BB_CTRL_SHIFT)
								);
}

//CON14
void upmu_chr_bc11_bias_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON14_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BC11_BIAS_EN_MASK),
								(kal_uint8)(BANK_0_BC11_BIAS_EN_SHIFT)
								);
}

void upmu_chr_bc11_ipu_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON14_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BC11_IPU_EN_MASK),
								(kal_uint8)(BANK_0_BC11_IPU_EN_SHIFT)
								);
}

void upmu_chr_bc11_ipd_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON14_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BC11_IPD_EN_MASK),
								(kal_uint8)(BANK_0_BC11_IPD_EN_SHIFT)
								);
}

void upmu_chr_bc11_cmp_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON14_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BC11_CMP_EN_MASK),
								(kal_uint8)(BANK_0_BC11_CMP_EN_SHIFT)
								);
}

void upmu_chr_bc11_vref_vth(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON14_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BC11_VREF_VTH_MASK),
								(kal_uint8)(BANK_0_BC11_VREF_VTH_SHIFT)
								);
}

//CON15
void upmu_chr_csdac_stp_dec(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON15_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CSDAC_STP_DEC_MASK),
								(kal_uint8)(BANK_0_CSDAC_STP_DEC_SHIFT)
								);
}

void upmu_chr_csdac_stp_inc(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON15_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CSDAC_STP_INC_MASK),
								(kal_uint8)(BANK_0_CSDAC_STP_INC_SHIFT)
								);
}

//CON16
void upmu_chr_csdac_stp(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON16_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CSDAC_STP_MASK),
								(kal_uint8)(BANK_0_CSDAC_STP_SHIFT)
								);
}

void upmu_chr_csdac_dly(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON16_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CSDAC_DLY_MASK),
								(kal_uint8)(BANK_0_CSDAC_DLY_SHIFT)
								);
}

//CON17
void upmu_chr_chrind_dimming(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON17_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_CHRIND_DIMMING_MASK),
								(kal_uint8)(BANK_0_RG_CHRIND_DIMMING_SHIFT)
								);
}

void upmu_chr_chrind_on(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON17_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_CHRIND_ON_MASK),
								(kal_uint8)(BANK_0_RG_CHRIND_ON_SHIFT)
								);
}

void upmu_chr_low_ich_db(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON17_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LOW_ICH_DB_MASK),
								(kal_uint8)(BANK_0_RG_LOW_ICH_DB_SHIFT)
								);
}

//CON18
void upmu_chr_ulc_det_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON18_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_ULC_DET_EN_MASK),
								(kal_uint8)(BANK_0_RG_ULC_DET_EN_SHIFT)
								);
}

void upmu_chr_hw_cv_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON18_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_HWCV_EN_MASK),
								(kal_uint8)(BANK_0_RG_HWCV_EN_SHIFT)
								);
}

void upmu_chr_baton_tdet_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON18_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BATON_TDET_EN_MASK),
								(kal_uint8)(BANK_0_BATON_TDET_EN_SHIFT)
								);
}

void upmu_chr_tracking_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON18_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_TRACKING_EN_MASK),
								(kal_uint8)(BANK_0_TRACKING_EN_SHIFT)
								);
}

void upmu_chr_csdac_mode(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON18_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CSDAC_MODE_MASK),
								(kal_uint8)(BANK_0_CSDAC_MODE_SHIFT)
								);
}

void upmu_chr_vcdt_mode(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON18_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_VCDT_MODE_MASK),
								(kal_uint8)(BANK_0_VCDT_MODE_SHIFT)
								);
}

void upmu_chr_cv_mode(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON18_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CV_MODE_MASK),
								(kal_uint8)(BANK_0_CV_MODE_SHIFT)
								);
}

//CON19
void upmu_chr_ichrg_trim(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON19_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_ICHRG_TRIM_MASK),
								(kal_uint8)(BANK_0_RG_ICHRG_TRIM_SHIFT)
								);
}

void upmu_chr_bgr_trim_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON19_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BGR_TRIM_EN_MASK),
								(kal_uint8)(BANK_0_RG_BGR_TRIM_EN_SHIFT)
								);
}

//CON1A
void upmu_chr_bgr_trim(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON1A_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BGR_TRIM_MASK),
								(kal_uint8)(BANK_0_RG_BGR_TRIM_SHIFT)
								);
}

//CON1B
void upmu_chr_ovp_trim(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON1B_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_OVP_TRIM_MASK),
								(kal_uint8)(BANK_0_RG_OVP_TRIM_SHIFT)
								);
}

//CON1C
void upmu_chr_rsv0(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON1C_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CHR_RSV0_MASK),
								(kal_uint8)(BANK_0_CHR_RSV0_SHIFT)
								);
}

void upmu_chr_bgr_test_rstb(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON1C_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BGR_TEST_RSTB_MASK),
								(kal_uint8)(BANK_0_RG_BGR_TEST_RSTB_SHIFT)
								);
}

void upmu_chr_bgr_test_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON1C_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BGR_TEST_EN_MASK),
								(kal_uint8)(BANK_0_RG_BGR_TEST_EN_SHIFT)
								);
}

void upmu_chr_osc_trim(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON1C_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_CHR_OSC_TRIM_MASK),
								(kal_uint8)(BANK_0_RG_CHR_OSC_TRIM_SHIFT)
								);
}

//CON1D
void upmu_chr_rsv1(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_CHARGER + CHR_CON1D_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_CHR_RSV1_MASK),
								(kal_uint8)(BANK_0_CHR_RSV1_SHIFT)
								);
}

//==============================================================================
// BUCK APIs
//==============================================================================
//CON 0
void upmu_buck_csl(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_CSL_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_CSL_SHIFT)
								);
}

void upmu_buck_csr(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_CSR_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_CSR_SHIFT)
								);
}

void upmu_buck_cc(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_CC_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_CC_SHIFT)
								);
}

void upmu_buck_rzsel(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_RZSEL_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_RZSEL_SHIFT)
								);
}

//CON 1
void upmu_buck_slp(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_SLP_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_SLP_SHIFT)
								);
}

void upmu_buck_zx_os(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_ZX_OS_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_ZX_OS_SHIFT)
								);
}

void upmu_buck_slew(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_SLEW_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_SLEW_SHIFT)
								);
}

void upmu_buck_slew_nmos(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_SLEW_NMOS_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_SLEW_NMOS_SHIFT)
								);
}

//CON 2
void upmu_buck_avp_os(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_AVP_OS_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_AVP_OS_SHIFT)
								);
}

void upmu_buck_avp_en(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_AVP_EN_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_AVP_EN_SHIFT)
								);
}

void upmu_buck_modeset(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON2_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_MODESET_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_MODESET_SHIFT)
								);
}

//CON 3
void upmu_buck_qi_en(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON3_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_QI_BUCK_EN_MASK),
								(kal_uint8)(BANK_0_QI_BUCK_EN_SHIFT)
								);
}

void upmu_buck_on_ctrl(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON3_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_ON_CTRL_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_ON_CTRL_SHIFT)
								);
}

void upmu_buck_en(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON3_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_EN_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_EN_SHIFT)
								);
}

//CON 4
void upmu_buck_stb_td(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON4_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_STBTD_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_STBTD_SHIFT)
								);
}

void upmu_buck_burst(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON4_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_BURST_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_BURST_SHIFT)
								);
}

void upmu_buck_vfbadj(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON4_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_VFBADJ_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_VFBADJ_SHIFT)
								);
}

void upmu_buck_ocfb_en(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON4_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_OCFB_EN_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_OCFB_EN_SHIFT)
								);
}

void upmu_buck_ndis_en(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON4_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_NDIS_EN_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_NDIS_EN_SHIFT)
								);
}

//CON 5
void upmu_buck_vosel(upmu_buck_list_enum buck, upmu_buck_vol_enum vol)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];
	kal_uint32 i;
	kal_uint32 vol_list_num;
	upmu_buck_vol_enum *p_buck_vol;

	// Read value once here
	vol_list_num = p_upmu_entry->vol_list_num;

	// Use pointer operation to speed up
	p_buck_vol = &p_upmu_entry->vol_list[0];

	for (i=0;i<vol_list_num;i++)
	{
		if (*p_buck_vol == vol)
		{
			ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON5_OFFSET), 
										(kal_uint8)(i),
										(kal_uint8)(BANK_0_RG_BUCK_VOSEL_MASK),
										(kal_uint8)(BANK_0_RG_BUCK_VOSEL_SHIFT)
										);			
			return;
		}
		p_buck_vol++;
	}
}

//CON 6
void upmu_buck_vosel_srclken_0(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BUCK_VOSEL_SRCLKEN0_MASK),
								(kal_uint8)(BANK_0_BUCK_VOSEL_SRCLKEN0_SHIFT)
								);
}

void upmu_buck_vosel_con_1(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_VOSEL_CON1_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_VOSEL_CON1_SHIFT)
								);
}

void upmu_buck_pasel_set_0(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON6_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_PASEL_SET0_MASK),
								(kal_uint8)(BANK_0_RG_PASEL_SET0_SHIFT)
								);
}

//CON 7
void upmu_buck_vosel_srclken_1(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON7_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BUCK_VOSEL_SRCLKEN1_MASK),
								(kal_uint8)(BANK_0_BUCK_VOSEL_SRCLKEN1_SHIFT)
								);
}

kal_uint32 upmu_buck_qi_oc_status(upmu_buck_list_enum buck)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON7_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RO_QI_BUCK_OC_STATUS_MASK),
								(kal_uint8)(BANK_0_RO_QI_BUCK_OC_STATUS_SHIFT)
								);
	return val;
}

void upmu_buck_pasel_set_1(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON7_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_PASEL_SET1_MASK),
								(kal_uint8)(BANK_0_RG_PASEL_SET1_SHIFT)
								);
}

//CON 8
void upmu_buck_vosel_dvs_00(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON8_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BUCK_VOSEL_DVS00_MASK),
								(kal_uint8)(BANK_0_BUCK_VOSEL_DVS00_SHIFT)
								);
}

void upmu_buck_pasel_set_2(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON8_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_PASEL_SET2_MASK),
								(kal_uint8)(BANK_0_RG_PASEL_SET2_SHIFT)
								);
}

//CON 9
void upmu_buck_vosel_dvs_01(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON9_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BUCK_VOSEL_DVS01_MASK),
								(kal_uint8)(BANK_0_BUCK_VOSEL_DVS01_SHIFT)
								);
}

void upmu_buck_pasel_set_3(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CON9_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_PASEL_SET3_MASK),
								(kal_uint8)(BANK_0_RG_PASEL_SET3_SHIFT)
								);
}

//CON A
void upmu_buck_vosel_dvs_10(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CONA_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BUCK_VOSEL_DVS10_MASK),
								(kal_uint8)(BANK_0_BUCK_VOSEL_DVS10_SHIFT)
								);
}

void upmu_buck_pasel_set_4(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CONA_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_PASEL_SET4_MASK),
								(kal_uint8)(BANK_0_RG_PASEL_SET4_SHIFT)
								);
}

//CON B
void upmu_buck_vosel_dvs_11(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CONB_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_BUCK_VOSEL_DVS11_MASK),
								(kal_uint8)(BANK_0_BUCK_VOSEL_DVS11_SHIFT)
								);
}

void upmu_buck_pasel_set_5(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CONB_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_PASEL_SET5_MASK),
								(kal_uint8)(BANK_0_RG_PASEL_SET5_SHIFT)
								);
}

//CON C
void upmu_buck_vosel_sfchg_en(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];
	kal_uint32 buck_offset=0;

	if(buck == BUCK_VPROC) 
	{
		buck_offset = BUCK_CONC_OFFSET;
	} 
	else if(buck == BUCK_VCORE) 
	{
		buck_offset = BUCK_CON7_OFFSET;
	} 
	else {
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "error buck_offset\n");
		return;
	}

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+buck_offset), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_VOSEL_SFCHG_EN_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_VOSEL_SFCHG_EN_SHIFT)
								);
}

void upmu_buck_ctrl(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];
	kal_uint32 buck_offset=0;

	if(buck == BUCK_VPROC) 
	{
		buck_offset = BUCK_CONC_OFFSET;
	} 
	else if(buck == BUCK_VCORE) 
	{
		buck_offset = BUCK_CON7_OFFSET;
	} 
	else if(buck == BUCK_VPA) 
	{
		buck_offset = BUCK_CONE_OFFSET;
	}
	else {
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "error buck_offset\n");
		return;
	}	

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+buck_offset), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_CTRL_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_CTRL_SHIFT)
								);
}

void upmu_buck_pasel_set_6(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_CONC_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_PASEL_SET6_MASK),
								(kal_uint8)(BANK_0_RG_PASEL_SET6_SHIFT)
								);
}

//CON D
void upmu_buck_smps_testmode(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_COND_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUCK_SMPS_TESTMODE_MASK),
								(kal_uint8)(BANK_0_RG_BUCK_SMPS_TESTMODE_SHIFT)
								);
}

void upmu_buck_pasel_set_7(upmu_buck_list_enum buck, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+BUCK_COND_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_PASEL_SET7_MASK),
								(kal_uint8)(BANK_0_RG_PASEL_SET7_SHIFT)
								);
}

//CON E

//CON F
kal_uint32 upmu_buck_qi_vosel(upmu_buck_list_enum buck)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;
	kal_uint32 buck_offset=0;
	upmu_buck_profile_entry *p_upmu_entry = &upmu_buck_profile[(kal_uint32)buck];

	if(buck == BUCK_VPROC) 
	{
		buck_offset = BUCK_CONF_OFFSET;
	} 
	else if(buck == BUCK_VCORE) 
	{
		buck_offset = BUCK_CON8_OFFSET;
	} 
	else if(buck == BUCK_VPA) 
	{
		buck_offset = BUCK_CONF_OFFSET;
	}
	else {
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "error buck_offset\n");
		return 0;
	}

	ret=pmic_read_interface( 	(kal_uint8)(p_upmu_entry->addr+buck_offset), 
								(&val),
								(kal_uint8)(BANK_0_QI_BUCK_VOSEL_MASK),
								(kal_uint8)(BANK_0_QI_BUCK_VOSEL_SHIFT)
								);
	return val;
}

//==============================================================================
// LDO APIs
//==============================================================================
//CON 0
void upmu_ldo_cal(upmu_ldo_list_enum ldo, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+LDO_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LDO_CAL_MASK),
								(kal_uint8)(BANK_0_RG_LDO_CAL_SHIFT)
								);
}

//CON 1
kal_uint32 upmu_ldo_qi_en(upmu_ldo_list_enum ldo)
{	
	kal_uint32 ret=0;
	kal_uint8 val=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_offset=0;

	if(ldo == LDO_VM12_INT) 
	{
		ldo_offset = LDO_CON3_OFFSET;
	}	
	else 
	{
		ldo_offset = LDO_CON1_OFFSET;
	}

	ret=pmic_read_interface( 	(kal_uint8)(p_upmu_entry->addr+ldo_offset), 
								(&val),
								(kal_uint8)(BANK_0_QI_LDO_EN_MASK),
								(kal_uint8)(BANK_0_QI_LDO_EN_SHIFT)
								);
	return val;
}

void upmu_ldo_vosel(upmu_ldo_list_enum ldo, upmu_ldo_vol_enum vol)
{
	kal_uint32 ret=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 i;
	kal_uint32 vol_list_num;
	upmu_ldo_vol_enum *p_ldo_vol;

	if(ldo == LDO_VM12_INT)
	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "LDO_VM12_INT has no vosel\n");
		return;
	}

	// Read value once here
	vol_list_num = p_upmu_entry->vol_list_num;

	// Use pointer operation to speed up
	p_ldo_vol = &p_upmu_entry->vol_list[0];

	for (i=0;i<vol_list_num;i++)
	{
		if (*p_ldo_vol == vol)
		{
			ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+LDO_CON1_OFFSET), 
										(kal_uint8)(i),
										(kal_uint8)(BANK_0_RG_LDO_VOSEL_MASK),
										(kal_uint8)(BANK_0_RG_LDO_VOSEL_SHIFT)
										);			
			return;
		}
		p_ldo_vol++;
	}
}

void upmu_ldo_on_ctrl(upmu_ldo_list_enum ldo, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_offset=0;

	if(ldo == LDO_VM12_INT) 
	{
		ldo_offset = LDO_CON3_OFFSET;
	}	
	else 
	{
		ldo_offset = LDO_CON1_OFFSET;
	}

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+ldo_offset), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_LDO_ON_CTRL_MASK),
								(kal_uint8)(BANK_0_LDO_ON_CTRL_SHIFT)
								);
}

void upmu_ldo_en(upmu_ldo_list_enum ldo, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_offset=0;

	if(ldo == LDO_VM12_INT) 
	{
		ldo_offset = LDO_CON3_OFFSET;
	}	
	else 
	{
		ldo_offset = LDO_CON1_OFFSET;
	}

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+ldo_offset), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_LDO_EN_MASK),
								(kal_uint8)(BANK_0_LDO_EN_SHIFT)
								);
}

//CON 2
void upmu_ldo_stbtd(upmu_ldo_list_enum ldo, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_offset=0;
	
	if(ldo == LDO_VRTC)
	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "LDO_VRTC has no stbtd\n");
		return;
	}

	if(ldo == LDO_VM12_INT) 
	{
		ldo_offset = LDO_CON4_OFFSET;
	}	
	else 
	{
		ldo_offset = LDO_CON2_OFFSET;
	}

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+ldo_offset), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LDO_STBTD_MASK),
								(kal_uint8)(BANK_0_RG_LDO_STBTD_SHIFT)
								);
}

void upmu_ldo_thr_shdn_en(upmu_ldo_list_enum ldo, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_offset=0;
	
	if(ldo == LDO_VRTC)
	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "LDO_VRTC has no std sel\n");
		return;
	}

	if(ldo == LDO_VM12_INT) 
	{
		ldo_offset = LDO_CON4_OFFSET;
	}	
	else 
	{
		ldo_offset = LDO_CON2_OFFSET;
	}

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+ldo_offset), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LDO_THR_SHDN_EN_MASK),
								(kal_uint8)(BANK_0_RG_LDO_THR_SHDN_EN_SHIFT)
								);
}

void upmu_ldo_stb_sel(upmu_ldo_list_enum ldo, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_offset=0;
	
	if(ldo == LDO_VRTC)
	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "LDO_VRTC has no std sel\n");
		return;
	}

	if(ldo == LDO_VM12_INT) 
	{
		ldo_offset = LDO_CON4_OFFSET;
	}	
	else 
	{
		ldo_offset = LDO_CON2_OFFSET;
	}

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+ldo_offset), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LDO_STB_SEL_MASK),
								(kal_uint8)(BANK_0_RG_LDO_STB_SEL_SHIFT)
								);
}

void upmu_ldo_ocfb_en(upmu_ldo_list_enum ldo, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_offset=0;
	
	if(ldo == LDO_VRTC)
	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "LDO_VRTC has no ndis_en\n");
		return;
	}

	if(ldo == LDO_VM12_INT) 
	{
		ldo_offset = LDO_CON4_OFFSET;
	}	
	else 
	{
		ldo_offset = LDO_CON2_OFFSET;
	}

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+ldo_offset), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LDO_OCFB_EN_MASK),
								(kal_uint8)(BANK_0_RG_LDO_OCFB_EN_SHIFT)
								);
}

void upmu_ldo_ndis_en(upmu_ldo_list_enum ldo, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_offset=0;
	
	if(ldo == LDO_VRTC)
	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "LDO_VRTC has no ndis_en\n");
		return;
	}

	if(ldo == LDO_VM12_INT) 
	{
		ldo_offset = LDO_CON4_OFFSET;
	}	
	else 
	{
		ldo_offset = LDO_CON2_OFFSET;
	}

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+ldo_offset), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LDO_NDIS_EN_MASK),
								(kal_uint8)(BANK_0_RG_LDO_NDIS_EN_SHIFT)
								);
}

//CON 3
kal_uint32 upmu_ldo_get_qi_mode(upmu_ldo_list_enum ldo)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_offset=0;
	
	if( (ldo==LDO_VIO28) 	||
		(ldo==LDO_VSIM) 	||
		(ldo==LDO_VSIM2)	||
		(ldo==LDO_VUSB) 	||
		(ldo==LDO_VCAMD) 	||
		(ldo==LDO_VCAM_IO) 	||
		(ldo==LDO_VCAM_AF) 	||
		(ldo==LDO_VGP) 		||
		(ldo==LDO_VGP2) 	||
		(ldo==LDO_VIBR) 	||
		(ldo==LDO_VRF) 		||
		(ldo==LDO_VTCXO) 	||
		(ldo==LDO_VA2) 		||
		(ldo==LDO_VCAMA) 	||
		(ldo==LDO_VRTC) 
		)
	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "LDO has no qi mode\n");
		return 0;
	}

	if(ldo == LDO_VM12_INT) 
	{
		ldo_offset = LDO_CON6_OFFSET;
	}	
	else 
	{
		ldo_offset = LDO_CON3_OFFSET;
	}

	ret=pmic_read_interface( 	(kal_uint8)(p_upmu_entry->addr+ldo_offset), 
								(&val),
								(kal_uint8)(BANK_0_QI_LDO_MODE_MASK),
								(kal_uint8)(BANK_0_QI_LDO_MODE_SHIFT)
								);
	return val;
}

void upmu_ldo_lp_set(upmu_ldo_list_enum ldo, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_offset=0;
	
	if( (ldo==LDO_VIO28) 	||
		(ldo==LDO_VSIM) 	||
		(ldo==LDO_VSIM2)	||
		(ldo==LDO_VUSB) 	||
		(ldo==LDO_VCAMD) 	||
		(ldo==LDO_VCAM_IO) 	||
		(ldo==LDO_VCAM_AF) 	||
		(ldo==LDO_VGP) 		||
		(ldo==LDO_VGP2) 	||
		(ldo==LDO_VIBR) 	||
		(ldo==LDO_VRF) 		||
		(ldo==LDO_VTCXO) 	||
		(ldo==LDO_VA2) 		||
		(ldo==LDO_VCAMA) 	||
		(ldo==LDO_VRTC) 
		)
	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "LDO has no lp set\n");
		return;
	}

	if(ldo == LDO_VM12_INT) 
	{
		ldo_offset = LDO_CON6_OFFSET;
	}	
	else 
	{
		ldo_offset = LDO_CON3_OFFSET;
	}

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+ldo_offset), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_LDO_LP_SET_MASK),
								(kal_uint8)(BANK_0_LDO_LP_SET_SHIFT)
								);
}

void upmu_ldo_lp_sel(upmu_ldo_list_enum ldo, kal_uint32 val)
{
	kal_uint32 ret=0;
	upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_offset=0;
	
	if( (ldo==LDO_VIO28) 	||
		(ldo==LDO_VSIM) 	||
		(ldo==LDO_VSIM2)	||
		(ldo==LDO_VUSB) 	||
		(ldo==LDO_VCAMD) 	||
		(ldo==LDO_VCAM_IO) 	||
		(ldo==LDO_VCAM_AF) 	||
		(ldo==LDO_VGP) 		||
		(ldo==LDO_VGP2) 	||
		(ldo==LDO_VIBR) 	||
		(ldo==LDO_VRF) 		||
		(ldo==LDO_VTCXO) 	||
		(ldo==LDO_VA2) 		||
		(ldo==LDO_VCAMA) 	||
		(ldo==LDO_VRTC) 
		)
	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "LDO has no lp sel\n");
		return;
	}

	if(ldo == LDO_VM12_INT) 
	{
		ldo_offset = LDO_CON6_OFFSET;
	}	
	else 
	{
		ldo_offset = LDO_CON3_OFFSET;
	}

	ret=pmic_config_interface( 	(kal_uint8)(p_upmu_entry->addr+ldo_offset), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_LDO_LP_SEL_MASK),
								(kal_uint8)(BANK_0_LDO_LP_SEL_SHIFT)
								);
}

// LDO OC Status
kal_uint32 upmu_ldo_get_qi_oc_status(upmu_ldo_list_enum ldo)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;
	//upmu_ldo_profile_entry *p_upmu_entry = &upmu_ldo_profile[(kal_uint32)ldo];
	kal_uint32 ldo_address=0;
	kal_uint32 ldo_mask=0x1;
	kal_uint32 ldo_shift=0;

	if(ldo < LDO_VCAMD)
	{
		ldo_address = BANK0_DIGLDO_CON35;
	}
	else if(ldo < LDO_VRF)
	{
		ldo_address = BANK0_DIGLDO_CON36;
	}
	else
	{
		ldo_address = BANK0_ANALDO_CON13; 
	}

	if(ldo == LDO_VM12_1)			{ 		ldo_shift = 0x0;
	} else if(ldo == LDO_VM12_2)	{		ldo_shift = 0x1;
	} else if(ldo == LDO_VM12_INT)	{		ldo_shift = 0x2;
	} else if(ldo == LDO_VIO28)		{		ldo_shift = 0x3;
	} else if(ldo == LDO_VSIM)		{		ldo_shift = 0x4;
	} else if(ldo == LDO_VSIM2)		{		ldo_shift = 0x5;
	} else if(ldo == LDO_VUSB)		{		ldo_shift = 0x6;
	
	} else if(ldo == LDO_VCAMD)		{		ldo_shift = 0x0;
	} else if(ldo == LDO_VCAM_IO)	{		ldo_shift = 0x1;
	} else if(ldo == LDO_VCAM_AF)	{		ldo_shift = 0x2;
	} else if(ldo == LDO_VMC)		{		ldo_shift = 0x3;
	} else if(ldo == LDO_VMCH)		{		ldo_shift = 0x4;
	} else if(ldo == LDO_VGP)		{		ldo_shift = 0x5;
	} else if(ldo == LDO_VGP2)		{		ldo_shift = 0x6;
	} else if(ldo == LDO_VIBR)		{		ldo_shift = 0x7;
	
	} else if(ldo == LDO_VRF)		{		ldo_shift = 0x0;
	} else if(ldo == LDO_VTCXO)		{		ldo_shift = 0x1;
	} else if(ldo == LDO_VA1)		{		ldo_shift = 0x2;
	} else if(ldo == LDO_VA2)		{		ldo_shift = 0x3;
	} else if(ldo == LDO_VCAMA)		{		ldo_shift = 0x4;
	
	} else 	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "error LDO number\n");
		return 0;
	}

	ret=pmic_read_interface( 	(kal_uint8)(ldo_address), 
								(&val),
								(kal_uint8)(ldo_mask),
								(kal_uint8)(ldo_shift)
								);
	return val;
}

//==============================================================================
// BUCK Calibration APIs
//==============================================================================
//CON 0
void upmu_buck_calibration_auto_k(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_BUCK_CALIBRATION + BUCK_K_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_rg_auto_k_MASK),
								(kal_uint8)(BANK_0_rg_auto_k_SHIFT)
								);
}

void upmu_buck_calibration_k_src_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_BUCK_CALIBRATION + BUCK_K_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_rg_k_src_sel_MASK),
								(kal_uint8)(BANK_0_rg_k_src_sel_SHIFT)
								);
}

void upmu_buck_calibration_k_start_manual(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_BUCK_CALIBRATION + BUCK_K_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_rg_k_start_manual_MASK),
								(kal_uint8)(BANK_0_rg_k_start_manual_SHIFT)
								);
}

void upmu_buck_calibration_k_once(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_BUCK_CALIBRATION + BUCK_K_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_rg_k_once_MASK),
								(kal_uint8)(BANK_0_rg_k_once_SHIFT)
								);
}

void upmu_buck_calibration_k_once_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_BUCK_CALIBRATION + BUCK_K_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_rg_k_once_en_MASK),
								(kal_uint8)(BANK_0_rg_k_once_en_SHIFT)
								);
}

void upmu_buck_calibration_k_map_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_BUCK_CALIBRATION + BUCK_K_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_rg_k_map_sel_MASK),
								(kal_uint8)(BANK_0_rg_k_map_sel_SHIFT)
								);
}

void upmu_buck_calibration_k_rst_done(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_BUCK_CALIBRATION + BUCK_K_CON0_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_rg_k_rst_done_MASK),
								(kal_uint8)(BANK_0_rg_k_rst_done_SHIFT)
								);
}

//CON 1
void upmu_buck_calibration_k_control_smps(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_BUCK_CALIBRATION + BUCK_K_CON1_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_rg_k_control_smps_MASK),
								(kal_uint8)(BANK_0_rg_k_control_smps_SHIFT)
								);
}

//CON 2
kal_uint32 upmu_buck_calibration_get_k_control(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_BUCK_CALIBRATION + BUCK_K_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_k_control_MASK),
								(kal_uint8)(BANK_0_k_control_SHIFT)
								);
	return val;
}

kal_uint32 upmu_buck_calibration_get_k_done(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_BUCK_CALIBRATION + BUCK_K_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_k_done_MASK),
								(kal_uint8)(BANK_0_k_done_SHIFT)
								);
	return val;
}

kal_uint32 upmu_buck_calibration_get_k_result(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_BUCK_CALIBRATION + BUCK_K_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_k_result_MASK),
								(kal_uint8)(BANK_0_k_result_SHIFT)
								);
	return val;
}

//==============================================================================
// AUXADC APIs
//==============================================================================
//CON 0
kal_uint32 upmu_auxadc_get_ch0_data_7_0(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON0_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C0_7_0_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C0_7_0_SHIFT)
								);
	return val;
}

//CON 1
kal_uint32 upmu_auxadc_get_ch0_ready(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_RDY_C0_MASK),
								(kal_uint8)(BANK_0_RG_ADC_RDY_C0_SHIFT)
								);
	return val;
}

kal_uint32 upmu_auxadc_get_ch0_data_9_8(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON1_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C0_9_8_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C0_9_8_SHIFT)
								);
	return val;
}

//CON 2
kal_uint32 upmu_auxadc_get_ch1_data_7_0(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON2_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C1_7_0_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C1_7_0_SHIFT)
								);
	return val;
}

//CON 3
kal_uint32 upmu_auxadc_get_ch1_ready(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_RDY_C1_MASK),
								(kal_uint8)(BANK_0_RG_ADC_RDY_C1_SHIFT)
								);
	return val;
}

kal_uint32 upmu_auxadc_get_ch1_data_9_8(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON3_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C1_9_8_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C1_9_8_SHIFT)
								);
	return val;
}

//CON 4
kal_uint32 upmu_auxadc_get_ch2_data_7_0(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON4_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C2_7_0_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C2_7_0_SHIFT)
								);
	return val;
}

//CON 5
kal_uint32 upmu_auxadc_get_ch2_ready(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON5_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_RDY_C2_MASK),
								(kal_uint8)(BANK_0_RG_ADC_RDY_C2_SHIFT)
								);
	return val;
}

kal_uint32 upmu_auxadc_get_ch2_data_9_8(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON5_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C2_9_8_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C2_9_8_SHIFT)
								);
	return val;
}

//CON 6
kal_uint32 upmu_auxadc_get_ch3_data_7_0(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON6_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C3_7_0_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C3_7_0_SHIFT)
								);
	return val;
}

//CON 7
kal_uint32 upmu_auxadc_get_ch3_ready(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON7_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_RDY_C3_MASK),
								(kal_uint8)(BANK_0_RG_ADC_RDY_C3_SHIFT)
								);
	return val;
}

kal_uint32 upmu_auxadc_get_ch3_data_9_8(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON7_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C3_9_8_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_C3_9_8_SHIFT)
								);
	return val;
}

//CON 8
kal_uint32 upmu_auxadc_get_wakeup_date_7_0(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON8_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_WAKEUP_7_0_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_WAKEUP_7_0_SHIFT)
								);
	return val;
}

//CON 9
kal_uint32 upmu_auxadc_get_wakeup_date_ready(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON9_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_RDY_WAKEUP_MASK),
								(kal_uint8)(BANK_0_RG_ADC_RDY_WAKEUP_SHIFT)
								);
	return val;
}

kal_uint32 upmu_auxadc_get_wakeup_date_9_8(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON9_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_WAKEUP_9_8_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_WAKEUP_9_8_SHIFT)
								);
	return val;
}

//CON 10
kal_uint32 upmu_auxadc_get_low_battery_data_7_0(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON10_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_LBAT_7_0_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_LBAT_7_0_SHIFT)
								);
	return val;
}

//CON 11
kal_uint32 upmu_auxadc_get_low_battery_data_ready(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON11_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_RDY_LBAT_MASK),
								(kal_uint8)(BANK_0_RG_ADC_RDY_LBAT_SHIFT)
								);
	return val;
}

kal_uint32 upmu_auxadc_get_low_battery_data_9_8(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON11_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_LBAT_9_8_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_LBAT_9_8_SHIFT)
								);
	return val;
}

//CON 12
kal_uint32 upmu_auxadc_get_trimming_data_7_0(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON12_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_TRIM_7_0_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_TRIM_7_0_SHIFT)
								);
	return val;
}

//CON 13
kal_uint32 upmu_auxadc_get_trimming_data_ready(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON13_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_RDY_TRIM_MASK),
								(kal_uint8)(BANK_0_RG_ADC_RDY_TRIM_SHIFT)
								);
	return val;
}

kal_uint32 upmu_auxadc_get_trimming_data_9_8(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON13_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_ADC_OUT_TRIM_9_8_MASK),
								(kal_uint8)(BANK_0_RG_ADC_OUT_TRIM_9_8_SHIFT)
								);
	return val;
}

//CON 14
void upmu_auxadc_ch_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON14_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_AUXADC_CHSEL_MASK),
								(kal_uint8)(BANK_0_RG_AUXADC_CHSEL_SHIFT)
								);
}

void upmu_auxadc_trim_comp(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON14_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_ADC_TRIM_COMP_MASK),
								(kal_uint8)(BANK_0_RG_ADC_TRIM_COMP_SHIFT)
								);
}

void upmu_auxadc_bist_enb(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON14_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_AUXADC_BIST_ENB_MASK),
								(kal_uint8)(BANK_0_RG_AUXADC_BIST_ENB_SHIFT)
								);
}

void upmu_auxadc_start(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON14_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_AUXADC_START_MASK),
								(kal_uint8)(BANK_0_RG_AUXADC_START_SHIFT)
								);
}

//CON 15
void upmu_auxadc_spl_num(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON15_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_SPL_NUM_MASK),
								(kal_uint8)(BANK_0_RG_SPL_NUM_SHIFT)
								);
}

void upmu_auxadc_avg_num(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON15_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_AVG_NUM_MASK),
								(kal_uint8)(BANK_0_RG_AVG_NUM_SHIFT)
								);
}

void upmu_auxadc_buf_pwd_b(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON15_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUF_PWD_B_MASK),
								(kal_uint8)(BANK_0_RG_BUF_PWD_B_SHIFT)
								);
}

void upmu_auxadc_adc_pwd_b(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON15_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_ADC_PWD_B_MASK),
								(kal_uint8)(BANK_0_RG_ADC_PWD_B_SHIFT)
								);
}

//CON 16
void upmu_auxadc_lowbat_debtime_max(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON16_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_DEBT_MAX_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_DEBT_MAX_SHIFT)
								);
}

//CON 17
void upmu_auxadc_lowbat_debtime_min(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON17_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_DEBT_MIN_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_DEBT_MIN_SHIFT)
								);
}

//CON 18
void upmu_auxadc_lowbat_period_7_0(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON18_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_DET_PRD_7_0_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_DET_PRD_7_0_SHIFT)
								);
}

//CON 19
void upmu_auxadc_lowbat_period_15_8(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON19_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_DET_PRD_15_8_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_DET_PRD_15_8_SHIFT)
								);
}

//CON 20
void upmu_auxadc_lowbat_period_19_16(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON20_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_DET_PRD_19_16_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_DET_PRD_19_16_SHIFT)
								);
}

//CON 21
void upmu_auxadc_lowbat_set_max_voltage_7_0(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON21_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_VOLT_MAX_7_0_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_VOLT_MAX_7_0_SHIFT)
								);
}

//CON 22
void upmu_auxadc_lowbat_set_max_voltage_9_8(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON22_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_VOLT_MAX_9_8_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_VOLT_MAX_9_8_SHIFT)
								);
}

void upmu_auxadc_lowbat_en_max_volt(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON22_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_EN_MAX_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_EN_MAX_SHIFT)
								);
}

void upmu_auxadc_lowbat_irq_en_max_volt(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON22_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_IRQ_EN_MAX_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_IRQ_EN_MAX_SHIFT)
								);
}

//CON 23
void upmu_auxadc_lowbat_set_min_voltage_7_0(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON23_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_VOLT_MIN_7_0_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_VOLT_MIN_7_0_SHIFT)
								);
}

//CON 24
void upmu_auxadc_lowbat_set_min_voltage_9_8(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON24_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_VOLT_MIN_9_8_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_VOLT_MIN_9_8_SHIFT)
								);
}

void upmu_auxadc_lowbat_en_min_volt(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON24_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_EN_MIN_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_EN_MIN_SHIFT)
								);
}

void upmu_auxadc_lowbat_irq_en_min_volt(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON24_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_LBAT_IRQ_EN_MIN_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_IRQ_EN_MIN_SHIFT)
								);
}

//CON 25
kal_uint32 upmu_auxadc_lowbat_max_irq_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON25_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_LBAT_MAX_IRQ_B_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_MAX_IRQ_B_SHIFT)
								);
	return val;
}

kal_uint32 upmu_auxadc_lowbat_min_irq_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON25_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_LBAT_MIN_IRQ_B_MASK),
								(kal_uint8)(BANK_0_RG_LBAT_MIN_IRQ_B_SHIFT)
								);
	return val;
}

//CON 26
void upmu_auxadc_da_dac_value_7_0(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON26_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_DA_DAC_7_0_MASK),
								(kal_uint8)(BANK_0_RG_DA_DAC_7_0_SHIFT)
								);
}

//CON 27
void upmu_auxadc_da_dac_value_9_8(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON27_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_DA_DAC_9_8_MASK),
								(kal_uint8)(BANK_0_RG_DA_DAC_9_8_SHIFT)
								);
}

kal_uint32 upmu_auxadc_ni_comp(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON27_OFFSET), 
								(&val),
								(kal_uint8)(BANK_0_RG_NI_COMP_MASK),
								(kal_uint8)(BANK_0_RG_NI_COMP_SHIFT)
								);
	return val;
}

//CON 28
void upmu_auxadc_da_dac_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON28_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_DA_DAC_SEL_MASK),
								(kal_uint8)(BANK_0_RG_DA_DAC_SEL_SHIFT)
								);
}

void upmu_auxadc_aux_out_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON28_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_AUX_OUT_SEL_MASK),
								(kal_uint8)(BANK_0_RG_AUX_OUT_SEL_SHIFT)
								);
}

void upmu_auxadc_arb_prio_2(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON28_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_ARB_PRIO_2_MASK),
								(kal_uint8)(BANK_0_RG_ARB_PRIO_2_SHIFT)
								);
}

void upmu_auxadc_arb_prio_1(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON28_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_ARB_PRIO_1_MASK),
								(kal_uint8)(BANK_0_RG_ARB_PRIO_1_SHIFT)
								);
}

void upmu_auxadc_arb_prio_0(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON28_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_ARB_PRIO_0_MASK),
								(kal_uint8)(BANK_0_RG_ARB_PRIO_0_SHIFT)
								);
}

//CON 29
void upmu_auxadc_cali(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON29_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_AUXADC_CALI_MASK),
								(kal_uint8)(BANK_0_RG_AUXADC_CALI_SHIFT)
								);
}

void upmu_buf_cali(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_AUXADC + AUXADC_CON29_OFFSET), 
								(kal_uint8)(val),
								(kal_uint8)(BANK_0_RG_BUF_CALI_MASK),
								(kal_uint8)(BANK_0_RG_BUF_CALI_SHIFT)
								);
}

//==============================================================================
// TEST MODE APIs
//==============================================================================
//TEST MODE - CON 0
void upmu_testmode_mon_grp_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_MON_GRP_SEL_MASK),
										(kal_uint8)(BANK_1_RG_MON_GRP_SEL_SHIFT)
										);
}

//TEST MODE - CON 1
void upmu_testmode_mon_flag_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_MON_FLAG_SEL_MASK),
										(kal_uint8)(BANK_1_RG_MON_FLAG_SEL_SHIFT)
										);
}

//TEST MODE - CON 2
void upmu_testmode_io_pasel0_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_IO_PASEL0_SEL_MASK),
										(kal_uint8)(BANK_1_RG_IO_PASEL0_SEL_SHIFT)
										);
}

void upmu_testmode_io_pasel1_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_IO_PASEL1_SEL_MASK),
										(kal_uint8)(BANK_1_RG_IO_PASEL1_SEL_SHIFT)
										);
}

//TEST MODE - CON 3
void upmu_testmode_io_pasel2_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_IO_PASEL2_SEL_MASK),
										(kal_uint8)(BANK_1_RG_IO_PASEL2_SEL_SHIFT)
										);
}

void upmu_testmode_io_int_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_IO_INT_SEL_MASK),
										(kal_uint8)(BANK_1_RG_IO_INT_SEL_SHIFT)
										);
}

//TEST MODE - CON 4
void upmu_testmode_sck_pu(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_DIO_SMT_MASK),
										(kal_uint8)(BANK_1_RG_DIO_SMT_SHIFT)
										);
}

void upmu_testmode_sck_pd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_DIO_E2_MASK),
										(kal_uint8)(BANK_1_RG_DIO_E2_SHIFT)
										);
}

void upmu_testmode_dio_smt(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_DIO_E4_MASK),
										(kal_uint8)(BANK_1_RG_DIO_E4_SHIFT)
										);
}

void upmu_testmode_dio_e2(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_DIO_SR_MASK),
										(kal_uint8)(BANK_1_RG_DIO_SR_SHIFT)
										);
}

void upmu_testmode_dio_e4(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SCK_PU_MASK),
										(kal_uint8)(BANK_1_RG_SCK_PU_SHIFT)
										);
}

void upmu_testmode_dio_sr(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SCK_PD_MASK),
										(kal_uint8)(BANK_1_RG_SCK_PD_SHIFT)
										);
}

//TEST MODE - CON 5
void upmu_testmode_sda_e2(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SDA_E2_MASK),
										(kal_uint8)(BANK_1_RG_SDA_E2_SHIFT)
										);
}

void upmu_testmode_sda_e4(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SDA_E4_MASK),
										(kal_uint8)(BANK_1_RG_SDA_E4_SHIFT)
										);
}

void upmu_testmode_sda_pu(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SDA_PU_MASK),
										(kal_uint8)(BANK_1_RG_SDA_PU_SHIFT)
										);
}

void upmu_testmode_sda_pd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SDA_PD_MASK),
										(kal_uint8)(BANK_1_RG_SDA_PD_SHIFT)
										);
}

void upmu_testmode_int_e2(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_INT_E2_MASK),
										(kal_uint8)(BANK_1_RG_INT_E2_SHIFT)
										);
}

void upmu_testmode_int_e4(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_INT_E4_MASK),
										(kal_uint8)(BANK_1_RG_INT_E4_SHIFT)
										);
}

void upmu_testmode_int_pu(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_INT_PU_MASK),
										(kal_uint8)(BANK_1_RG_INT_PU_SHIFT)
										);
}

void upmu_testmode_int_pd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_INT_PD_MASK),
										(kal_uint8)(BANK_1_RG_INT_PD_SHIFT)
										);
}

//TEST MODE - CON 6
void upmu_testmode_dvs_pu(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_DVS_PU_MASK),
										(kal_uint8)(BANK_1_RG_DVS_PU_SHIFT)
										);
}

void upmu_testmode_dvs_pd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_DVS_PD_MASK),
										(kal_uint8)(BANK_1_RG_DVS_PD_SHIFT)
										);
}

void upmu_testmode_pasel_pu(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_PASEL_PU_MASK),
										(kal_uint8)(BANK_1_RG_PASEL_PU_SHIFT)
										);
}

void upmu_testmode_pasel_pd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_PASEL_PD_MASK),
										(kal_uint8)(BANK_1_RG_PASEL_PD_SHIFT)
										);
}

void upmu_testmode_sysrstb_pu(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SYSRSTB_PU_MASK),
										(kal_uint8)(BANK_1_RG_SYSRSTB_PU_SHIFT)
										);
}

void upmu_testmode_sysrstb_pd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SYSRSTB_PD_MASK),
										(kal_uint8)(BANK_1_RG_SYSRSTB_PD_SHIFT)
										);
}

//TEST MODE - CON 7
void upmu_testmode_srclken_pu(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SRCLKEN_PU_MASK),
										(kal_uint8)(BANK_1_RG_SRCLKEN_PU_SHIFT)
										);
}

void upmu_testmode_srclken_pd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SRCLKEN_PD_MASK),
										(kal_uint8)(BANK_1_RG_SRCLKEN_PD_SHIFT)
										);
}

void upmu_testmode_bl_pwm_pu(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_BL_PWM_PU_MASK),
										(kal_uint8)(BANK_1_RG_BL_PWM_PU_SHIFT)
										);
}

void upmu_testmode_bl_pwm_pd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_BL_PWM_PD_MASK),
										(kal_uint8)(BANK_1_RG_BL_PWM_PD_SHIFT)
										);
}

//TEST MODE - CON 8
void upmu_testmode_test_rsv(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON8_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_TEST_RSV_MASK),
										(kal_uint8)(BANK_1_RG_TEST_RSV_SHIFT)
										);
}

void upmu_testmode_test_strup(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON8_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_TEST_STRUP_MASK),
										(kal_uint8)(BANK_1_RG_TEST_STRUP_SHIFT)
										);
}

void upmu_testmode_otp_w_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON8_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_W_MODE_MASK),
										(kal_uint8)(BANK_1_RG_OTP_W_MODE_SHIFT)
										);
}

void upmu_testmode_nandtree_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON8_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_NANDTREE_MODE_MASK),
										(kal_uint8)(BANK_1_RG_NANDTREE_MODE_SHIFT)
										);
}

//TEST MODE - CON 9
void upmu_testmode_test_auxadc(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_TEST_AUXADC_MASK),
										(kal_uint8)(BANK_1_RG_TEST_AUXADC_SHIFT)
										);
}

void upmu_testmode_test_fgpll(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_TEST_FGPLL_MASK),
										(kal_uint8)(BANK_1_RG_TEST_FGPLL_SHIFT)
										);
}

void upmu_testmode_test_fg1(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_TEST_FG1_MASK),
										(kal_uint8)(BANK_1_RG_TEST_FG1_SHIFT)
										);
}

void upmu_testmode_test_fg2(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_TEST_FG2_MASK),
										(kal_uint8)(BANK_1_RG_TEST_FG2_SHIFT)
										);
}

void upmu_testmode_test_io_fg_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_TEST_IO_FG_SEL_MASK),
										(kal_uint8)(BANK_1_RG_TEST_IO_FG_SEL_SHIFT)
										);
}

void upmu_testmode_test_classd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_TEST_CLASSD_MASK),
										(kal_uint8)(BANK_1_RG_TEST_CLASSD_SHIFT)
										);
}

void upmu_testmode_test_driver(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_TEST_DRIVER_MASK),
										(kal_uint8)(BANK_1_RG_TEST_DRIVER_SHIFT)
										);
}

void upmu_testmode_test_boost(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_TEST_BOOST_MASK),
										(kal_uint8)(BANK_1_RG_TEST_BOOST_SHIFT)
										);
}

//TEST MODE - CON 10
kal_uint32 upmu_testmode_mon_flag_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON10_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RO_MON_MASK),
									(kal_uint8)(BANK_1_RO_MON_SHIFT)
									);
	return val;
}

//TEST MODE - CON 11
kal_uint32 upmu_testmode_dvs1_in_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON11_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RO_DVS1_IN_MASK),
									(kal_uint8)(BANK_1_RO_DVS1_IN_SHIFT)
									);
	return val;
}

kal_uint32 upmu_testmode_dvs2_in_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON11_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RO_DVS2_IN_MASK),
									(kal_uint8)(BANK_1_RO_DVS2_IN_SHIFT)
									);
	return val;
}

kal_uint32 upmu_testmode_pasel0_in_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON11_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RO_PASEL0_IN_MASK),
									(kal_uint8)(BANK_1_RO_PASEL0_IN_SHIFT)
									);
	return val;
}

kal_uint32 upmu_testmode_pasel1_in_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON11_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RO_PASEL1_IN_MASK),
									(kal_uint8)(BANK_1_RO_PASEL1_IN_SHIFT)
									);
	return val;
}

kal_uint32 upmu_testmode_pasel2_in_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON11_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RO_PASEL2_IN_MASK),
									(kal_uint8)(BANK_1_RO_PASEL2_IN_SHIFT)
									);
	return val;
}

kal_uint32 upmu_testmode_sysrstb_in_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON11_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RO_SYSRSTB_IN_MASK),
									(kal_uint8)(BANK_1_RO_SYSRSTB_IN_SHIFT)
									);
	return val;
}

kal_uint32 upmu_testmode_srclken_in_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON11_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RO_SRCLKEN_IN_MASK),
									(kal_uint8)(BANK_1_RO_SRCLKEN_IN_SHIFT)
									);
	return val;
}

kal_uint32 upmu_testmode_blpwm_in_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON11_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RO_BLPWM_IN_MASK),
									(kal_uint8)(BANK_1_RO_BLPWM_IN_SHIFT)
									);
	return val;
}

//TEST MODE - CON 12
void upmu_testmode_pwrkey_rst_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON12_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_PWRKEY_RST_EN_MASK),
										(kal_uint8)(BANK_1_RG_PWRKEY_RST_EN_SHIFT)
										);
}

void upmu_testmode_homekey_rst_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON12_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_HOMEKEY_RST_EN_MASK),
										(kal_uint8)(BANK_1_RG_HOMEKEY_RST_EN_SHIFT)
										);
}

void upmu_testmode_pwrkey_rst_td(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON12_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_PWRKEY_RST_TD_MASK),
										(kal_uint8)(BANK_1_RG_PWRKEY_RST_TD_SHIFT)
										);
}

void upmu_testmode_pwrrst_tmr_dis(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON12_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_PWRRST_TMR_DIS_MASK),
										(kal_uint8)(BANK_1_RG_PWRRST_TMR_DIS_SHIFT)
										);
}

//TEST MODE - CON 13
void upmu_testmode_rst_part_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_RST_PART_SEL_MASK),
										(kal_uint8)(BANK_1_RG_RST_PART_SEL_SHIFT)
										);
}

void upmu_testmode_otp_man_rst(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_MAN_RST_MASK),
										(kal_uint8)(BANK_1_RG_OTP_MAN_RST_SHIFT)
										);
}

void upmu_testmode_pchr_man_rst_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_PCHR_MAN_RST_EN_MASK),
										(kal_uint8)(BANK_1_RG_PCHR_MAN_RST_EN_SHIFT)
										);
}

void upmu_testmode_pchr_man_rst(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_PCHR_MAN_RST_MASK),
										(kal_uint8)(BANK_1_RG_PCHR_MAN_RST_SHIFT)
										);
}

void upmu_testmode_strup_man_rst_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_STRUP_MAN_RST_EN_MASK),
										(kal_uint8)(BANK_1_RG_STRUP_MAN_RST_EN_SHIFT)
										);
}

void upmu_testmode_sif_tst_ck_dis(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SIF_TST_CK_DIS_MASK),
										(kal_uint8)(BANK_1_RG_SIF_TST_CK_DIS_SHIFT)
										);
}

void upmu_testmode_sysrstb_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TEST_MODE + TESTMODE_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SYSRSTB_EN_MASK),
										(kal_uint8)(BANK_1_RG_SYSRSTB_EN_SHIFT)
										);
}

//==============================================================================
// TOP2 APIs
//==============================================================================
//TOP2 - CON 0
void upmu_top2_75k_ext_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_75K_EXT_SEL_MASK),
										(kal_uint8)(BANK_1_RG_75K_EXT_SEL_SHIFT)
										);
}

void upmu_top2_fg_tst_ck_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_FG_TST_CK_SEL_MASK),
										(kal_uint8)(BANK_1_RG_FG_TST_CK_SEL_SHIFT)
										);
}

void upmu_top2_chr1m_tst_ck_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_CHR1M_TST_CK_SEL_MASK),
										(kal_uint8)(BANK_1_RG_CHR1M_TST_CK_SEL_SHIFT)
										);
}

void upmu_top2_clk_tst(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_CLK_TST_MASK),
										(kal_uint8)(BANK_1_RG_CLK_TST_SHIFT)
										);
}

void upmu_top2_auxadc_ck_rst(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_AUXADC_CK_RST_MASK),
										(kal_uint8)(BANK_1_RG_AUXADC_CK_RST_SHIFT)
										);
}

void upmu_top2_auxadc_ck_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_AUXADC_CK_SEL_MASK),
										(kal_uint8)(BANK_1_RG_AUXADC_CK_SEL_SHIFT)
										);
}

//TOP2 - CON 1
void upmu_top2_fgadc_ck_pdn(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_FGADC_CK_PDN_MASK),
										(kal_uint8)(BANK_1_RG_FGADC_CK_PDN_SHIFT)
										);
}

void upmu_top2_otpc_ck_pdn(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTPC_CK_PDN_MASK),
										(kal_uint8)(BANK_1_RG_OTPC_CK_PDN_SHIFT)
										);
}

void upmu_top2_bst_drv_ck_pdn(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_BST_DRV_CK_PDN_MASK),
										(kal_uint8)(BANK_1_RG_BST_DRV_CK_PDN_SHIFT)
										);
}

void upmu_top2_spk_ck_pdn(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_SPK_CK_PDN_MASK),
										(kal_uint8)(BANK_1_RG_SPK_CK_PDN_SHIFT)
										);
}

void upmu_top2_pwmoc_ck_pdn(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_PWMOC_CK_PDN_MASK),
										(kal_uint8)(BANK_1_RG_PWMOC_CK_PDN_SHIFT)
										);
}

//TOP2 - CON 2
void upmu_top2_oc_gear_bvalid_det(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_OC_GEAR_BVALID_DET_MASK),
										(kal_uint8)(BANK_1_OC_GEAR_BVALID_DET_SHIFT)
										);
}

void upmu_top2_oc_gear_vbaton_undet(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_OC_GEAR_VBATON_UNDET_MASK),
										(kal_uint8)(BANK_1_OC_GEAR_VBATON_UNDET_SHIFT)
										);
}

void upmu_top2_oc_gear_ldo(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_OC_GEAR_LDO_MASK),
										(kal_uint8)(BANK_1_OC_GEAR_LDO_SHIFT)
										);
}

//TOP2 - CON 3
void upmu_top2_int_rsv(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_INT_RSV_MASK),
										(kal_uint8)(BANK_1_RG_INT_RSV_SHIFT)
										);
}

void upmu_top2_polarity_bvalid_det(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_POLARITY_BVALID_DET_MASK),
										(kal_uint8)(BANK_1_POLARITY_BVALID_DET_SHIFT)
										);
}

void upmu_top2_polarity_vbaton_undet(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_POLARITY_VBATON_UNDET_MASK),
										(kal_uint8)(BANK_1_POLARITY_VBATON_UNDET_SHIFT)
										);
}

void upmu_top2_polarity_select(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_POLARITY_MASK),
										(kal_uint8)(BANK_1_POLARITY_SHIFT)
										);
}

//TOP2 - CON 4
void upmu_top2_vpa_oc_wnd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_VPA_OC_WND_MASK),
										(kal_uint8)(BANK_1_VPA_OC_WND_SHIFT)
										);
}

void upmu_top2_vpa_oc_thd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_VPA_OC_THD_MASK),
										(kal_uint8)(BANK_1_VPA_OC_THD_SHIFT)
										);
}

void upmu_top2_vrf18_oc_wnd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_VRF18_OC_WND_MASK),
										(kal_uint8)(BANK_1_VRF18_OC_WND_SHIFT)
										);
}

void upmu_top2_vrf18_oc_thd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_TOP2_CTRL + TOP2_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_VRF18_OC_THD_MASK),
										(kal_uint8)(BANK_1_VRF18_OC_THD_SHIFT)
										);
}

//==============================================================================
// FLASH APIs
//==============================================================================
//FLASH - CON 0
void upmu_flash_dim_duty(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FLASH_CTRL + FLASH_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FLASH_DIM_DUTY_MASK),
										(kal_uint8)(BANK_1_FLASH_DIM_DUTY_SHIFT)
										);
}

//FLASH - CON 1
void upmu_flash_ther_shdn_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FLASH_CTRL + FLASH_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FLASH_THER_SHDN_EN_MASK),
										(kal_uint8)(BANK_1_FLASH_THER_SHDN_EN_SHIFT)
										);
}

void upmu_flash_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FLASH_CTRL + FLASH_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FLASH_EN_MASK),
										(kal_uint8)(BANK_1_FLASH_EN_SHIFT)
										);
}

//FLASH - CON 2
void upmu_flash_dim_div(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FLASH_CTRL + FLASH_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FLASH_DIM_DIV_MASK),
										(kal_uint8)(BANK_1_FLASH_DIM_DIV_SHIFT)
										);
}

//FLASH - CON 3
void upmu_flash_step_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FLASH_CTRL + FLASH_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FLASH_SEL_MASK),
										(kal_uint8)(BANK_1_FLASH_SEL_SHIFT)
										);
}

//FLASH - CON 4
void upmu_flash_sfstren(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FLASH_CTRL + FLASH_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FLASH_SFSTREN_MASK),
										(kal_uint8)(BANK_1_FLASH_SFSTREN_SHIFT)
										);
}

void upmu_flash_sfstr_time(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FLASH_CTRL + FLASH_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FLASH_SFSTR_MASK),
										(kal_uint8)(BANK_1_FLASH_SFSTR_SHIFT)
										);
}

void upmu_flash_mode_select(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FLASH_CTRL + FLASH_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FLASH_MODE_MASK),
										(kal_uint8)(BANK_1_FLASH_MODE_SHIFT)
										);
}

//==============================================================================
// KPLED APIs
//==============================================================================
//KPLED - CON 0
void upmu_kpled_dim_duty(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_KPLED_CTRL + KPLED_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_KPLED_DIM_DUTY_MASK),
										(kal_uint8)(BANK_1_KPLED_DIM_DUTY_SHIFT)
										);
}

//KPLED - CON 1
void upmu_kpled_ther_shdn_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_KPLED_CTRL + KPLED_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_KPLED_THER_SHDN_EN_MASK),
										(kal_uint8)(BANK_1_KPLED_THER_SHDN_EN_SHIFT)
										);
}

void upmu_kpled_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_KPLED_CTRL + KPLED_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_KPLED_EN_MASK),
										(kal_uint8)(BANK_1_KPLED_EN_SHIFT)
										);
}

//KPLED - CON 2
void upmu_kpled_dim_div(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_KPLED_CTRL + KPLED_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_KPLED_DIM_DIV_MASK),
										(kal_uint8)(BANK_1_KPLED_DIM_DIV_SHIFT)
										);
}

//KPLED - CON 3
void upmu_kpled_step_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_KPLED_CTRL + KPLED_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_KPLED_SEL_MASK),
										(kal_uint8)(BANK_1_KPLED_SEL_SHIFT)
										);
}

//KPLED - CON 4
void upmu_kpled_sfstren(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_KPLED_CTRL + KPLED_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_KPLED_SFSTREN_MASK),
										(kal_uint8)(BANK_1_KPLED_SFSTREN_SHIFT)
										);
}

void upmu_kpled_sfstr_time(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_KPLED_CTRL + KPLED_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_KPLED_SFSTR_MASK),
										(kal_uint8)(BANK_1_KPLED_SFSTR_SHIFT)
										);
}

void upmu_kpled_mode_select(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_KPLED_CTRL + KPLED_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_KPLED_MODE_MASK),
										(kal_uint8)(BANK_1_KPLED_MODE_SHIFT)
										);
}

//==============================================================================
// ISINKS APIs
//==============================================================================
//ISINKS - CON 0
void upmu_isinks_dim0_duty(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINK_DIM0_DUTY_MASK),
										(kal_uint8)(BANK_1_ISINK_DIM0_DUTY_SHIFT)
										);
}

//ISINKS - CON 1
void upmu_isinks_dim0_fsel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINK_DIM0_FSEL_MASK),
										(kal_uint8)(BANK_1_ISINK_DIM0_FSEL_SHIFT)
										);
}

//ISINKS - CON 2
void upmu_isinks_dim1_duty(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINK_DIM1_DUTY_MASK),
										(kal_uint8)(BANK_1_ISINK_DIM1_DUTY_SHIFT)
										);
}

//ISINKS - CON 3
void upmu_isinks_dim1_fsel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINK_DIM1_FSEL_MASK),
										(kal_uint8)(BANK_1_ISINK_DIM1_FSEL_SHIFT)
										);
}

//ISINKS - CON 4
void upmu_isinks_dim2_duty(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINK_DIM2_DUTY_MASK),
										(kal_uint8)(BANK_1_ISINK_DIM2_DUTY_SHIFT)
										);
}

//ISINKS - CON 5
void upmu_isinks_dim2_fsel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINK_DIM2_FSEL_MASK),
										(kal_uint8)(BANK_1_ISINK_DIM2_FSEL_SHIFT)
										);
}

//ISINKS - CON 6
void upmu_isinks_ch5_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH5_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH5_EN_SHIFT)
										);
}

void upmu_isinks_ch4_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH4_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH4_EN_SHIFT)
										);
}

void upmu_isinks_ch3_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH3_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH3_EN_SHIFT)
										);
}

void upmu_isinks_ch2_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH2_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH2_EN_SHIFT)
										);
}

void upmu_isinks_ch1_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH1_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH1_EN_SHIFT)
										);
}

void upmu_isinks_ch0_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH0_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH0_EN_SHIFT)
										);
}

//ISINKS - CON 7
void upmu_isinks_ch5_cabc_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH5_CABC_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH5_CABC_EN_SHIFT)
										);
}

void upmu_isinks_ch4_cabc_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH4_CABC_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH4_CABC_EN_SHIFT)
										);
}

void upmu_isinks_ch3_cabc_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH3_CABC_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH3_CABC_EN_SHIFT)
										);
}

void upmu_isinks_ch2_cabc_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH2_CABC_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH2_CABC_EN_SHIFT)
										);
}

void upmu_isinks_ch1_cabc_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH1_CABC_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH1_CABC_EN_SHIFT)
										);
}

void upmu_isinks_ch0_cabc_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH0_CABC_EN_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH0_CABC_EN_SHIFT)
										);
}

//ISINKS - CON 8
void upmu_isinks_ch0_step(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON8_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH0_STEP_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH0_STEP_SHIFT)
										);
}

void upmu_isinks_ch0_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON8_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH0_MODE_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH0_MODE_SHIFT)
										);
}

//ISINKS - CON 9
void upmu_isinks_ch1_step(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH1_STEP_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH1_STEP_SHIFT)
										);
}

void upmu_isinks_ch1_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH1_MODE_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH1_MODE_SHIFT)
										);
}

//ISINKS - CON 10
void upmu_isinks_ch2_step(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON10_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH2_STEP_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH2_STEP_SHIFT)
										);
}

void upmu_isinks_ch2_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON10_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH2_MODE_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH2_MODE_SHIFT)
										);
}

//ISINKS - CON 11
void upmu_isinks_ch3_step(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH3_STEP_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH3_STEP_SHIFT)
										);
}

void upmu_isinks_ch3_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH3_MODE_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH3_MODE_SHIFT)
										);
}

//ISINKS - CON 12
void upmu_isinks_ch4_step(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON12_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH4_STEP_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH4_STEP_SHIFT)
										);
}

void upmu_isinks_ch4_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON12_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH4_MODE_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH4_MODE_SHIFT)
										);
}

//ISINKS - CON 13
void upmu_isinks_ch5_step(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH5_STEP_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH5_STEP_SHIFT)
										);
}

void upmu_isinks_ch5_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_CH5_MODE_MASK),
										(kal_uint8)(BANK_1_ISINKS_CH5_MODE_SHIFT)
										);
}

//ISINKS - CON 14
void upmu_isinks_ibias_trim_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON14_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_IBIAS_TRIM_EN_MASK),
										(kal_uint8)(BANK_1_IBIAS_TRIM_EN_SHIFT)
										);
}

void upmu_isinks_vref_cal(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ISINKS_CTRL + ISINKS_CON14_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_ISINKS_VREF_CAL_MASK),
										(kal_uint8)(BANK_1_ISINKS_VREF_CAL_SHIFT)
										);
}

//==============================================================================
// BOOST APIs
//==============================================================================
//BOOST - CON 0
void upmu_boost_isink_hw_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_ISINK_HW_SEL_MASK),
										(kal_uint8)(BANK_1_BOOST_ISINK_HW_SEL_SHIFT)
										);
}

void upmu_boost_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_MODE_MASK),
										(kal_uint8)(BANK_1_BOOST_MODE_SHIFT)
										);
}

void upmu_boost_cabc_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_CABC_EN_MASK),
										(kal_uint8)(BANK_1_BOOST_CABC_EN_SHIFT)
										);
}

void upmu_boost_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_EN_MASK),
										(kal_uint8)(BANK_1_BOOST_EN_SHIFT)
										);
}

//BOOST - CON 1
void upmu_boost_sr_nmos(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_SR_NMOS_MASK),
										(kal_uint8)(BANK_1_BOOST_SR_NMOS_SHIFT)
										);
}

void upmu_boost_vrsel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_VRSEL_MASK),
										(kal_uint8)(BANK_1_BOOST_VRSEL_SHIFT)
										);
}

//BOOST - CON 2
void upmu_boost_rc(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_RC_MASK),
										(kal_uint8)(BANK_1_BOOST_RC_SHIFT)
										);
}

void upmu_boost_cs(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_CS_MASK),
										(kal_uint8)(BANK_1_BOOST_CS_SHIFT)
										);
}

void upmu_boost_cc(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_CC_MASK),
										(kal_uint8)(BANK_1_BOOST_CC_SHIFT)
										);
}

//BOOST - CON 3
void upmu_boost_slp(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_SLP_MASK),
										(kal_uint8)(BANK_1_BOOST_SLP_SHIFT)
										);
}

void upmu_boost_cl(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_CL_MASK),
										(kal_uint8)(BANK_1_BOOST_CL_SHIFT)
										);
}

//BOOST - CON 4
void upmu_boost_ss(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_SS_MASK),
										(kal_uint8)(BANK_1_BOOST_SS_SHIFT)
										);
}

void upmu_boost_rsv(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_BOOST_CTRL + BOOST_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_BOOST_RSV_MASK),
										(kal_uint8)(BANK_1_BOOST_RSV_SHIFT)
										);
}

//==============================================================================
// SPK APIs
//==============================================================================
//SPK - CON 0
void upmu_spk_ther_shdn_l_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_THER_SHDN_L_EN_MASK),
										(kal_uint8)(BANK_1_SPK_THER_SHDN_L_EN_SHIFT)
										);
}

void upmu_spk_trim_en_l(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_TRIM_EN_L_MASK),
										(kal_uint8)(BANK_1_SPK_TRIM_EN_L_SHIFT)
										);
}

void upmu_spk_mode_l(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPKMODE_L_MASK),
										(kal_uint8)(BANK_1_SPKMODE_L_SHIFT)
										);
}

void upmu_spk_en_l(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_EN_L_MASK),
										(kal_uint8)(BANK_1_SPK_EN_L_SHIFT)
										);
}

//SPK - CON 1
void upmu_spk_oc_en_l(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OC_EN_L_MASK),
										(kal_uint8)(BANK_1_SPK_OC_EN_L_SHIFT)
										);
}

void upmu_spkab_oc_en_l(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPKAB_OC_EN_L_MASK),
										(kal_uint8)(BANK_1_SPKAB_OC_EN_L_SHIFT)
										);
}

void upmu_spk_oc_shdn_dl(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OC_SHDN_DL_MASK),
										(kal_uint8)(BANK_1_SPK_OC_SHDN_DL_SHIFT)
										);
}

void upmu_spk_vol_l(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_VOL_L_MASK),
										(kal_uint8)(BANK_1_SPK_VOL_L_SHIFT)
										);
}

//SPK - CON 2
kal_uint32 upmu_spk_ab_oc_l_deg(void)
{
	kal_uint32 ret=0;	
    kal_uint32 val = 0;
	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON2_OFFSET),
										(kal_uint8*)(&val),
										(kal_uint8)(BANK_1_SPK_AB_OC_L_DEG_MASK),
										(kal_uint8)(BANK_1_SPK_AB_OC_L_DEG_SHIFT)
										);
	return val;
}

kal_uint32 upmu_spk_d_oc_l_deg(void)
{
	kal_uint32 ret=0;	
	kal_uint32 val = 0;
	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON2_OFFSET),
										(kal_uint8*)(&val),
										(kal_uint8)(BANK_1_SPK_D_OC_L_DEG_MASK),
										(kal_uint8)(BANK_1_SPK_D_OC_L_DEG_SHIFT)
										);
    return val;
}

//SPK - CON 3
void upmu_spk_offset_l_ov(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OFFSET_L_OV_MASK),
										(kal_uint8)(BANK_1_SPK_OFFSET_L_OV_SHIFT)
										);
}

void upmu_spk_offset_l_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OFFSET_L_MODE_MASK),
										(kal_uint8)(BANK_1_SPK_OFFSET_L_MODE_SHIFT)
										);
}

void upmu_spk_lead_l_sw(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_LEAD_L_SW_MASK),
										(kal_uint8)(BANK_1_SPK_LEAD_L_SW_SHIFT)
										);
}

void upmu_spk_offset_l_sw(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OFFSET_L_SW_MASK),
										(kal_uint8)(BANK_1_SPK_OFFSET_L_SW_SHIFT)
										);
}

//SPK - CON 4
kal_uint32 upmu_spk_trim_done_l(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON4_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_SPK_TRIM_DONE_L_MASK),
									(kal_uint8)(BANK_1_SPK_TRIM_DONE_L_SHIFT)
									);
	return val;
}

kal_uint32 upmu_spk_lead_l_flag(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON4_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_SPK_LEAD_L_FLAG_MASK),
									(kal_uint8)(BANK_1_SPK_LEAD_L_FLAG_SHIFT)
									);
	return val;
}

kal_uint32 upmu_spk_lead_l_flag_deg(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON4_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_SPK_LEAD_L_FLAG_DEG_MASK),
									(kal_uint8)(BANK_1_SPK_LEAD_L_FLAG_DEG_SHIFT)
									);
	return val;
}

kal_uint32 upmu_spk_offset_l(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON4_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_SPK_OFFSET_L_MASK),
									(kal_uint8)(BANK_1_SPK_OFFSET_L_SHIFT)
									);
	return val;
}

//SPK - CON 5
void upmu_spkrcv_en_l(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPKRCV_EN_L_MASK),
										(kal_uint8)(BANK_1_SPKRCV_EN_L_SHIFT)
										);
}

void upmu_spkab_obias_l(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPKAB_OBIAS_L_MASK),
										(kal_uint8)(BANK_1_SPKAB_OBIAS_L_SHIFT)
										);
}

void upmu_spk_slew_l(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_SLEW_L_MASK),
										(kal_uint8)(BANK_1_SPK_SLEW_L_SHIFT)
										);
}

void upmu_spk_force_en_l(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_FORCE_EN_L_MASK),
										(kal_uint8)(BANK_1_SPK_FORCE_EN_L_SHIFT)
										);
}

void upmu_spk_intg_rst_l(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_INTG_RST_L_MASK),
										(kal_uint8)(BANK_1_SPK_INTG_RST_L_SHIFT)
										);
}

//SPK - CON 6
void upmu_spk_ther_shdn_r_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_THER_SHDN_R_EN_MASK),
										(kal_uint8)(BANK_1_SPK_THER_SHDN_R_EN_SHIFT)
										);
}

void upmu_spk_trim_en_r(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_TRIM_EN_R_MASK),
										(kal_uint8)(BANK_1_SPK_TRIM_EN_R_SHIFT)
										);
}

void upmu_spk_mode_r(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPKMODE_R_MASK),
										(kal_uint8)(BANK_1_SPKMODE_R_SHIFT)
										);
}

void upmu_spk_en_r(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_EN_R_MASK),
										(kal_uint8)(BANK_1_SPK_EN_R_SHIFT)
										);
}

//SPK - CON 7
void upmu_spk_oc_en_r(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OC_EN_R_MASK),
										(kal_uint8)(BANK_1_SPK_OC_EN_R_SHIFT)
										);
}

void upmu_spkab_oc_en_r(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPKAB_OC_EN_R_MASK),
										(kal_uint8)(BANK_1_SPKAB_OC_EN_R_SHIFT)
										);
}

void upmu_spk_oc_shdn_dr(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OC_SHDN_DR_MASK),
										(kal_uint8)(BANK_1_SPK_OC_SHDN_DR_SHIFT)
										);
}

void upmu_spk_vol_r(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON7_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_VOL_R_MASK),
										(kal_uint8)(BANK_1_SPK_VOL_R_SHIFT)
										);
}

//SPK - CON 8
kal_uint32 upmu_spk_ab_oc_r_deg(void)
{
	kal_uint32 ret=0;	
    kal_uint32 val = 0;
	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON8_OFFSET),
										(kal_uint8*)(&val),
										(kal_uint8)(BANK_1_SPK_AB_OC_R_DEG_MASK),
										(kal_uint8)(BANK_1_SPK_AB_OC_R_DEG_SHIFT)
										);
	return val;
}

kal_uint32 upmu_spk_d_oc_r_deg(void)
{
	kal_uint32 ret=0;	
    kal_uint32 val = 0;
	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON8_OFFSET),
										(kal_uint8*)(&val),
										(kal_uint8)(BANK_1_SPK_D_OC_R_DEG_MASK),
										(kal_uint8)(BANK_1_SPK_D_OC_R_DEG_SHIFT)
										);
	return val;
}

//SPK - CON 9
void upmu_spk_offset_r_ov(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OFFSET_R_OV_MASK),
										(kal_uint8)(BANK_1_SPK_OFFSET_R_OV_SHIFT)
										);
}

void upmu_spk_offset_r_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OFFSET_R_MODE_MASK),
										(kal_uint8)(BANK_1_SPK_OFFSET_R_MODE_SHIFT)
										);
}

void upmu_spk_lead_r_sw(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_LEAD_R_SW_MASK),
										(kal_uint8)(BANK_1_SPK_LEAD_R_SW_SHIFT)
										);
}

void upmu_spk_offset_r_sw(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OFFSET_R_SW_MASK),
										(kal_uint8)(BANK_1_SPK_OFFSET_R_SW_SHIFT)
										);
}

//SPK - CON 10
kal_uint32 upmu_spk_trim_done_r(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON10_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_SPK_TRIM_DONE_R_MASK),
									(kal_uint8)(BANK_1_SPK_TRIM_DONE_R_SHIFT)
									);
	return val;
}

kal_uint32 upmu_spk_lead_r_flag(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON10_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_SPK_LEAD_R_FLAG_MASK),
									(kal_uint8)(BANK_1_SPK_LEAD_R_FLAG_SHIFT)
									);
	return val;
}

kal_uint32 upmu_spk_lead_r_flag_deg(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON10_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_SPK_LEAD_R_FLAG_DEG_MASK),
									(kal_uint8)(BANK_1_SPK_LEAD_R_FLAG_DEG_SHIFT)
									);
	return val;
}

kal_uint32 upmu_spk_offset_r(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON10_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_SPK_OFFSET_R_MASK),
									(kal_uint8)(BANK_1_SPK_OFFSET_R_SHIFT)
									);
	return val;
}

//SPK - CON 11
void upmu_spkrcv_en_r(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPKRCV_EN_R_MASK),
										(kal_uint8)(BANK_1_SPKRCV_EN_R_SHIFT)
										);
}

void upmu_spkab_obias_r(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPKAB_OBIAS_R_MASK),
										(kal_uint8)(BANK_1_SPKAB_OBIAS_R_SHIFT)
										);
}

void upmu_spk_slew_r(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_SLEW_R_MASK),
										(kal_uint8)(BANK_1_SPK_SLEW_R_SHIFT)
										);
}

void upmu_spk_force_en_r(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_FORCE_EN_R_MASK),
										(kal_uint8)(BANK_1_SPK_FORCE_EN_R_SHIFT)
										);
}

void upmu_spk_intg_rst_r(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_INTG_RST_R_MASK),
										(kal_uint8)(BANK_1_SPK_INTG_RST_R_SHIFT)
										);
}

//SPK - CON 12
void upmu_spk_oc_autoff(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON12_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OC_AUTOFF_MASK),
										(kal_uint8)(BANK_1_SPK_OC_AUTOFF_SHIFT)
										);
}

void upmu_spk_oc_dglh(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON12_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OC_DGLH_MASK),
										(kal_uint8)(BANK_1_SPK_OC_DGLH_SHIFT)
										);
}

void upmu_spk_octh_d(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON12_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OCTH_D_MASK),
										(kal_uint8)(BANK_1_SPK_OCTH_D_SHIFT)
										);
}

//SPK - CON 13
void upmu_spk_oc_wnd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OC_WND_MASK),
										(kal_uint8)(BANK_1_SPK_OC_WND_SHIFT)
										);
}

void upmu_spk_oc_thd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_OC_THD_MASK),
										(kal_uint8)(BANK_1_SPK_OC_THD_SHIFT)
										);
}

//SPK - CON 14
void upmu_spk_trim_div(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON14_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_TRIM_DIV_MASK),
										(kal_uint8)(BANK_1_SPK_TRIM_DIV_SHIFT)
										);
}

void upmu_spk_trim_deg(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON14_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_TRIM_DEG_MASK),
										(kal_uint8)(BANK_1_SPK_TRIM_DEG_SHIFT)
										);
}

//SPK - CON 15
void upmu_spkab_fb_att(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON15_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPKAB_FB_ATT_MASK),
										(kal_uint8)(BANK_1_SPKAB_FB_ATT_SHIFT)
										);
}

void upmu_spkab_ovdrv(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON15_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPKAB_OVDRV_MASK),
										(kal_uint8)(BANK_1_SPKAB_OVDRV_SHIFT)
										);
}

//SPK - CON 16
void upmu_spk_fbrc_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON16_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_FBRC_EN_MASK),
										(kal_uint8)(BANK_1_SPK_FBRC_EN_SHIFT)
										);
}

void upmu_spk_ibias_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON16_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_IBIAS_SEL_MASK),
										(kal_uint8)(BANK_1_SPK_IBIAS_SEL_SHIFT)
										);
}

void upmu_spk_vcm_ibsel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON16_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_VCM_IBSEL_MASK),
										(kal_uint8)(BANK_1_SPK_VCM_IBSEL_SHIFT)
										);
}

void upmu_spk_vcm_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON16_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_VCM_SEL_MASK),
										(kal_uint8)(BANK_1_SPK_VCM_SEL_SHIFT)
										);
}

void upmu_spk_en_view_clk(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON16_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_EN_VIEW_CLK_MASK),
										(kal_uint8)(BANK_1_SPK_EN_VIEW_CLK_SHIFT)
										);
}

void upmu_spk_en_view_vcm(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON16_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_EN_VIEW_VCM_MASK),
										(kal_uint8)(BANK_1_SPK_EN_VIEW_VCM_SHIFT)
										);
}

//SPK - CON 17
void upmu_spk_ccode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON17_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_CCODE_MASK),
										(kal_uint8)(BANK_1_SPK_CCODE_SHIFT)
										);
}

void upmu_spk_btl_set(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON17_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_BTL_SET_MASK),
										(kal_uint8)(BANK_1_SPK_BTL_SET_SHIFT)
										);
}

//SPK - CON 18
void upmu_spk_rsv(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON18_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_RSV_MASK),
										(kal_uint8)(BANK_1_SPK_RSV_SHIFT)
										);
}

//SPK - CON 19
void upmu_spk_td1(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON19_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_TD1_MASK),
										(kal_uint8)(BANK_1_SPK_TD1_SHIFT)
										);
}

void upmu_spk_td2(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON19_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_TD2_MASK),
										(kal_uint8)(BANK_1_SPK_TD2_SHIFT)
										);
}

//==============================================================================
// ASW APIs
//==============================================================================
//ASW - CON 0
void upmu_asw_ana_sw_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_ASW_CTRL + ASW_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_ANA_SW_SEL_MASK),
										(kal_uint8)(BANK_1_RG_ANA_SW_SEL_SHIFT)
										);
}

//==============================================================================
// FGPLL APIs
//==============================================================================
//FGPLL - CON 0
void upmu_fgpll_pdiv1(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_PDIV1_MASK),
										(kal_uint8)(BANK_1_FGPLL_PDIV1_SHIFT)
										);
}

void upmu_fgpll_pdiv1_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_PDIV1_EN_MASK),
										(kal_uint8)(BANK_1_FGPLL_PDIV1_EN_SHIFT)
										);
}

void upmu_fgpll_pllbs_rst(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_BS_RST_MASK),
										(kal_uint8)(BANK_1_FGPLL_BS_RST_SHIFT)
										);
}

void upmu_fgpll_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_EN_MASK),
										(kal_uint8)(BANK_1_FGPLL_EN_SHIFT)
										);
}

//FGPLL - CON 1
void upmu_fgpll_div1(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_DIV1_MASK),
										(kal_uint8)(BANK_1_FGPLL_DIV1_SHIFT)
										);
}

//FGPLL - CON 2
void upmu_fgpll_bc(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_BC_MASK),
										(kal_uint8)(BANK_1_FGPLL_BC_SHIFT)
										);
}

void upmu_fgpll_bp(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_BP_MASK),
										(kal_uint8)(BANK_1_FGPLL_BP_SHIFT)
										);
}

void upmu_fgpll_br(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_BR_MASK),
										(kal_uint8)(BANK_1_FGPLL_BR_SHIFT)
										);
}

//FGPLL - CON 3
void upmu_fgpll_cdiv(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_CDIV_MASK),
										(kal_uint8)(BANK_1_FGPLL_CDIV_SHIFT)
										);
}

void upmu_fgpll_vcoband(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_VCOBAND_MASK),
										(kal_uint8)(BANK_1_FGPLL_VCOBAND_SHIFT)
										);
}

void upmu_fgpll_cko_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON3_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_CKO_SEL_MASK),
										(kal_uint8)(BANK_1_FGPLL_CKO_SEL_SHIFT)
										);
}

//FGPLL - CON 4
void upmu_fgpll_ibsel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_IBSEL_MASK),
										(kal_uint8)(BANK_1_FGPLL_IBSEL_SHIFT)
										);
}

void upmu_fgpll_rlatch_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_RLATCH_EN_MASK),
										(kal_uint8)(BANK_1_FGPLL_RLATCH_EN_SHIFT)
										);
}

void upmu_fgpll_ckdrv_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_CKDRV_EN_MASK),
										(kal_uint8)(BANK_1_FGPLL_CKDRV_EN_SHIFT)
										);
}

void upmu_fgpll_vct_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_VCT_EN_MASK),
										(kal_uint8)(BANK_1_FGPLL_VCT_EN_SHIFT)
										);
}

void upmu_fgpll_ckt_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_CKT_SEL_MASK),
										(kal_uint8)(BANK_1_FGPLL_CKT_SEL_SHIFT)
										);
}

void upmu_fgpll_ckt_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON4_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_CKT_EN_MASK),
										(kal_uint8)(BANK_1_FGPLL_CKT_EN_SHIFT)
										);
}

//FGPLL - CON 5
void upmu_fgpll_rsva(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON5_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_RSVA_MASK),
										(kal_uint8)(BANK_1_FGPLL_RSVA_SHIFT)
										);
}

//FGPLL - CON 6
void upmu_fgpll_rsvb(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGPLL_CTRL + FGPLL_CON6_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FGPLL_RSVB_MASK),
										(kal_uint8)(BANK_1_FGPLL_RSVB_SHIFT)
										);
}

//==============================================================================
// FGADC APIs
//==============================================================================
//FGADC - CON 0
void upmu_fgadc_clksrc(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_CLKSRC_MASK),
										(kal_uint8)(BANK_1_FG_CLKSRC_SHIFT)
										);
}

void upmu_fgadc_autocalrate(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_AUTOCALRATE_MASK),
										(kal_uint8)(BANK_1_FG_AUTOCALRATE_SHIFT)
										);
}

void upmu_fgadc_cal(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_CAL_MASK),
										(kal_uint8)(BANK_1_FG_CAL_SHIFT)
										);
}

void upmu_fgadc_vmode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_VMODE_MASK),
										(kal_uint8)(BANK_1_FG_VMODE_SHIFT)
										);
}

void upmu_fgadc_on(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_ON_MASK),
										(kal_uint8)(BANK_1_FG_ON_SHIFT)
										);
}

//FGADC - CON 1
void upmu_fgadc_sw_rst_clr(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_SW_RSTCLR_MASK),
										(kal_uint8)(BANK_1_FG_SW_RSTCLR_SHIFT)
										);
}

void upmu_fgadc_charge_rst(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_CHARGE_RST_MASK),
										(kal_uint8)(BANK_1_FG_CHARGE_RST_SHIFT)
										);
}

void upmu_fgadc_time_rst(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_TIME_RST_MASK),
										(kal_uint8)(BANK_1_FG_TIME_RST_SHIFT)
										);
}

void upmu_fgadc_offset_rst(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_OFFSET_RST_MASK),
										(kal_uint8)(BANK_1_FG_OFFSET_RST_SHIFT)
										);
}

void upmu_fgadc_sw_clear(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_SW_CLEAR_MASK),
										(kal_uint8)(BANK_1_FG_SW_CLEAR_SHIFT)
										);
}

void upmu_fgadc_latchdata_st(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_LATCHDATA_ST_MASK),
										(kal_uint8)(BANK_1_FG_LATCHDATA_ST_SHIFT)
										);
}

void upmu_fgadc_sw_read_pre(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_SW_READ_PRE_MASK),
										(kal_uint8)(BANK_1_FG_SW_READ_PRE_SHIFT)
										);
}

void upmu_fgadc_sw_cr(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_SW_CR_MASK),
										(kal_uint8)(BANK_1_FG_SW_CR_SHIFT)
										);
}

//FGADC - CON 2
kal_uint32 upmu_fgadc_car_35_32(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON2_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_CAR_35_32_MASK),
									(kal_uint8)(BANK_1_FG_CAR_35_32_SHIFT)
									);
	return val;
}

//FGADC - CON 3
kal_uint32 upmu_fgadc_car_31_24(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON3_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_CAR_31_24_MASK),
									(kal_uint8)(BANK_1_FG_CAR_31_24_SHIFT)
									);
	return val;
}

//FGADC - CON 4
kal_uint32 upmu_fgadc_car_23_16(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON4_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_CAR_23_16_MASK),
									(kal_uint8)(BANK_1_FG_CAR_23_16_SHIFT)
									);
	return val;
}

//FGADC - CON 5
kal_uint32 upmu_fgadc_car_15_08(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON5_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_CAR_15_08_MASK),
									(kal_uint8)(BANK_1_FG_CAR_15_08_SHIFT)
									);
	return val;
}

//FGADC - CON 6
kal_uint32 upmu_fgadc_car_07_00(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON6_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_CAR_07_00_MASK),
									(kal_uint8)(BANK_1_FG_CAR_07_00_SHIFT)
									);
	return val;
}

//FGADC - CON 7
kal_uint32 upmu_fgadc_nter_29_24(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON7_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_NTER_29_24_MASK),
									(kal_uint8)(BANK_1_FG_NTER_29_24_SHIFT)
									);
	return val;
}

//FGADC - CON 8
kal_uint32 upmu_fgadc_nter_23_16(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON8_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_NTER_23_16_MASK),
									(kal_uint8)(BANK_1_FG_NTER_23_16_SHIFT)
									);
	return val;
}

//FGADC - CON 9
kal_uint32 upmu_fgadc_nter_15_08(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON9_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_NTER_15_08_MASK),
									(kal_uint8)(BANK_1_FG_NTER_15_08_SHIFT)
									);
	return val;
}

//FGADC - CON 10
kal_uint32 upmu_fgadc_nter_07_00(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON10_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_NTER_07_00_MASK),
									(kal_uint8)(BANK_1_FG_NTER_07_00_SHIFT)
									);
	return val;
}

//FGADC - CON 11
void upmu_fgadc_bltr_15_08(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_BLTR_15_08_MASK),
										(kal_uint8)(BANK_1_FG_BLTR_15_08_SHIFT)
										);
}

//FGADC - CON 12
void upmu_fgadc_bltr_07_00(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON12_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_BLTR_07_00_MASK),
										(kal_uint8)(BANK_1_FG_BLTR_07_00_SHIFT)
										);
}

//FGADC - CON 13
void upmu_fgadc_bftr_15_08(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_BFTR_15_08_MASK),
										(kal_uint8)(BANK_1_FG_BFTR_15_08_SHIFT)
										);
}

//FGADC - CON 14
void upmu_fgadc_bftr_07_00(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON14_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_BFTR_07_00_MASK),
										(kal_uint8)(BANK_1_FG_BFTR_07_00_SHIFT)
										);
}

//FGADC - CON 15
kal_uint32 upmu_fgadc_current_out_15_08(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON15_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_CURRENT_OUT_15_08_MASK),
									(kal_uint8)(BANK_1_FG_CURRENT_OUT_15_08_SHIFT)
									);
	return val;
}

//FGADC - CON 16
kal_uint32 upmu_fgadc_current_out_07_00(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON16_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_CURRENT_OUT_07_00_MASK),
									(kal_uint8)(BANK_1_FG_CURRENT_OUT_07_00_SHIFT)
									);
	return val;
}

//FGADC - CON 17
void upmu_fgadc_adjust_offset_value_15_08(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON17_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_ADJUST_OFFSET_VALUE_15_08_MASK),
										(kal_uint8)(BANK_1_FG_ADJUST_OFFSET_VALUE_15_08_SHIFT)
										);
}

//FGADC - CON 18
void upmu_fgadc_adjust_offset_value_07_00(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON18_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_ADJUST_OFFSET_VALUE_07_00_MASK),
										(kal_uint8)(BANK_1_FG_ADJUST_OFFSET_VALUE_07_00_SHIFT)
										);
}

//FGADC - CON 19
kal_uint32 upmu_fgadc_offset_15_08(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON19_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_OFFSET_15_08_MASK),
									(kal_uint8)(BANK_1_FG_OFFSET_15_08_SHIFT)
									);
	return val;
}

//FGADC - CON 20
kal_uint32 upmu_fgadc_offset_07_00(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON20_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_OFFSET_07_00_MASK),
									(kal_uint8)(BANK_1_FG_OFFSET_07_00_SHIFT)
									);
	return val;
}

//FGADC - CON 21
void upmu_fgadc_analogtest(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON21_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_ANALOGTEST_MASK),
										(kal_uint8)(BANK_1_FG_ANALOGTEST_SHIFT)
										);
}

//FGADC - CON 22
void upmu_fgadc_spare(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON22_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_SPARE_MASK),
										(kal_uint8)(BANK_1_FG_SPARE_SHIFT)
										);
}

//FGADC - CON 23
void upmu_fgadc_bltr_bftr_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON23_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_BLTR_BFTR_EN_MASK),
										(kal_uint8)(BANK_1_FG_BLTR_BFTR_EN_SHIFT)
										);
}

void upmu_fgadc_adc_autorst(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON23_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_ADC_AUTORST_MASK),
										(kal_uint8)(BANK_1_FG_ADC_AUTORST_SHIFT)
										);
}

void upmu_fgadc_adj_offset_en(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON23_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_ADJ_OFFSET_EN_MASK),
										(kal_uint8)(BANK_1_FG_ADJ_OFFSET_EN_SHIFT)
										);
}

void upmu_fgadc_osr(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON23_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_OSR_MASK),
										(kal_uint8)(BANK_1_FG_OSR_SHIFT)
										);
}

//FGADC - CON 24
void upmu_fgadc_vol_osr(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON24_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_VOL_OSR_MASK),
										(kal_uint8)(BANK_1_VOL_OSR_SHIFT)
										);
}

//FGADC - CON 25
kal_uint32 upmu_fgadc_adc_rstdetect(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON25_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_ADC_RSTDETECT_MASK),
									(kal_uint8)(BANK_1_FG_ADC_RSTDETECT_SHIFT)
									);
	return val;
}

kal_uint32 upmu_fgadc_h_int_sts(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON25_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_H_INT_STS_MASK),
									(kal_uint8)(BANK_1_FG_H_INT_STS_SHIFT)
									);
	return val;
}

kal_uint32 upmu_fgadc_l_int_sts(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON25_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_L_INT_STS_MASK),
									(kal_uint8)(BANK_1_FG_L_INT_STS_SHIFT)
									);
	return val;
}

void upmu_fgadc_vol_fir1bypass(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON25_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_VOL_FIR1BYPASS_MASK),
										(kal_uint8)(BANK_1_VOL_FIR1BYPASS_SHIFT)
										);
}

void upmu_fgadc_fg_fir2bypass(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON25_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_FIR2BYPASS_MASK),
										(kal_uint8)(BANK_1_FG_FIR2BYPASS_SHIFT)
										);
}

void upmu_fgadc_fg_fir1bypass(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON25_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_FIR1BYPASS_MASK),
										(kal_uint8)(BANK_1_FG_FIR1BYPASS_SHIFT)
										);
}

//FGADC - CON 26
kal_uint32 upmu_fgadc_vol_current_out_15_08(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON26_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_VOL_CURRENT_OUT_15_08_MASK),
									(kal_uint8)(BANK_1_VOL_CURRENT_OUT_15_08_SHIFT)
									);
	return val;
}

//FGADC - CON 27
kal_uint32 upmu_fgadc_vol_current_out_07_00(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON27_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_VOL_CURRENT_OUT_07_00_MASK),
									(kal_uint8)(BANK_1_VOL_CURRENT_OUT_07_00_SHIFT)
									);
	return val;
}

//FGADC - CON 28
kal_uint32 upmu_fgadc_cic2_15_08(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON28_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_CIC2_15_08_MASK),
									(kal_uint8)(BANK_1_FG_CIC2_15_08_SHIFT)
									);
	return val;
}

//FGADC - CON 29
kal_uint32 upmu_fgadc_cic2_07_00(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON29_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_FG_CIC2_07_00_MASK),
									(kal_uint8)(BANK_1_FG_CIC2_07_00_SHIFT)
									);
	return val;
}

//FGADC - CON 30
void upmu_fgadc_vmode_sw(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON30_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_VMODE_SW_MASK),
										(kal_uint8)(BANK_1_FG_VMODE_SW_SHIFT)
										);
}

void upmu_fgadc_fgadc_en_sw(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON30_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_FGADC_EN_SW_MASK),
										(kal_uint8)(BANK_1_FG_FGADC_EN_SW_SHIFT)
										);
}

void upmu_fgadc_fgcal_en_sw(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON30_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_FGCAL_EN_SW_MASK),
										(kal_uint8)(BANK_1_FG_FGCAL_EN_SW_SHIFT)
										);
}

void upmu_fgadc_rst_sw(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON30_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_RST_SW_MASK),
										(kal_uint8)(BANK_1_FG_RST_SW_SHIFT)
										);
}

void upmu_fgadc_mode(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_FGADC_CTRL + FGADC_CON30_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_FG_MODE_MASK),
										(kal_uint8)(BANK_1_FG_MODE_SHIFT)
										);
}

//==============================================================================
// OTPC APIs
//==============================================================================
//OTPC - CON 0
kal_uint32 upmu_otpc_otp_pdo_7_0(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON0_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RG_OTP_PDO_7_0_MASK),
									(kal_uint8)(BANK_1_RG_OTP_PDO_7_0_SHIFT)
									);
	return val;
}

//OTPC - CON 1
kal_uint32 upmu_otpc_otp_pdo_15_8(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON1_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RG_OTP_PDO_15_8_MASK),
									(kal_uint8)(BANK_1_RG_OTP_PDO_15_8_SHIFT)
									);
	return val;
}

//OTPC - CON 2
kal_uint32 upmu_otpc_otp_pdo_23_16(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON2_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RG_OTP_PDO_23_16_MASK),
									(kal_uint8)(BANK_1_RG_OTP_PDO_23_16_SHIFT)
									);
	return val;
}

//OTPC - CON 3
kal_uint32 upmu_otpc_otp_pdo_31_24(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON3_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RG_OTP_PDO_31_24_MASK),
									(kal_uint8)(BANK_1_RG_OTP_PDO_31_24_SHIFT)
									);
	return val;
}

//OTPC - CON 4
kal_uint32 upmu_otpc_otp_pdo_39_32(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON4_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RG_OTP_PDO_39_32_MASK),
									(kal_uint8)(BANK_1_RG_OTP_PDO_39_32_SHIFT)
									);
	return val;
}

//OTPC - CON 5
kal_uint32 upmu_otpc_otp_pdo_47_40(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON5_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RG_OTP_PDO_47_40_MASK),
									(kal_uint8)(BANK_1_RG_OTP_PDO_47_40_SHIFT)
									);
	return val;
}

//OTPC - CON 6
kal_uint32 upmu_otpc_otp_pdo_55_48(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON6_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RG_OTP_PDO_55_48_MASK),
									(kal_uint8)(BANK_1_RG_OTP_PDO_55_48_SHIFT)
									);
	return val;
}

//OTPC - CON 7
kal_uint32 upmu_otpc_otp_pdo_63_56(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON7_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RG_OTP_PDO_63_56_MASK),
									(kal_uint8)(BANK_1_RG_OTP_PDO_63_56_SHIFT)
									);
	return val;
}

//OTPC - CON 8
void upmu_otpc_otp_pdin(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON8_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_PDIN_MASK),
										(kal_uint8)(BANK_1_RG_OTP_PDIN_SHIFT)
										);
}

//OTPC - CON 9
void upmu_otpc_otp_pa(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_PA_MASK),
										(kal_uint8)(BANK_1_RG_OTP_PA_SHIFT)
										);
}

void upmu_otpc_otp_ptm(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_PTM_MASK),
										(kal_uint8)(BANK_1_RG_OTP_PTM_SHIFT)
										);
}

void upmu_otpc_otp_pwe(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_PWE_MASK),
										(kal_uint8)(BANK_1_RG_OTP_PWE_SHIFT)
										);
}

void upmu_otpc_otp_pprog(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_PPROG_MASK),
										(kal_uint8)(BANK_1_RG_OTP_PPROG_SHIFT)
										);
}

void upmu_otpc_otp_read(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON9_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_READ_MASK),
										(kal_uint8)(BANK_1_RG_OTP_READ_SHIFT)
										);
}

//OTPC - CON 10
void upmu_otpc_otp_read_prd(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON10_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_READ_PRD_MASK),
										(kal_uint8)(BANK_1_RG_OTP_READ_PRD_SHIFT)
										);
}

//OTPC - CON 11
void upmu_otpc_otp_test_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_TEST_SEL_MASK),
										(kal_uint8)(BANK_1_RG_OTP_TEST_SEL_SHIFT)
										);
}

void upmu_otpc_otp_skip_out(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_SKIP_OUT_MASK),
										(kal_uint8)(BANK_1_RG_OTP_SKIP_OUT_SHIFT)
										);
}

void upmu_otpc_otp_out_sel(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_OUT_SEL_MASK),
										(kal_uint8)(BANK_1_RG_OTP_OUT_SEL_SHIFT)
										);
}

void upmu_otpc_otp_rsv(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON11_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_RSV_MASK),
										(kal_uint8)(BANK_1_RG_OTP_RSV_SHIFT)
										);
}

//OTPC - CON 12
void upmu_otpc_otp_val_7_0(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON12_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_VAL_7_0_MASK),
										(kal_uint8)(BANK_1_RG_OTP_VAL_7_0_SHIFT)
										);
}

//OTPC - CON 13
void upmu_otpc_otp_val_15_8(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON13_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_VAL_15_8_MASK),
										(kal_uint8)(BANK_1_RG_OTP_VAL_15_8_SHIFT)
										);
}

//OTPC - CON 14
void upmu_otpc_otp_val_23_16(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON14_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_VAL_23_16_MASK),
										(kal_uint8)(BANK_1_RG_OTP_VAL_23_16_SHIFT)
										);
}

//OTPC - CON 15
void upmu_otpc_otp_val_31_24(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON15_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_VAL_31_24_MASK),
										(kal_uint8)(BANK_1_RG_OTP_VAL_31_24_SHIFT)
										);
}

//OTPC - CON 16
void upmu_otpc_otp_val_39_32(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON16_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_VAL_39_32_MASK),
										(kal_uint8)(BANK_1_RG_OTP_VAL_39_32_SHIFT)
										);
}

//OTPC - CON 17
void upmu_otpc_otp_val_47_40(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON17_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_VAL_47_40_MASK),
										(kal_uint8)(BANK_1_RG_OTP_VAL_47_40_SHIFT)
										);
}

//OTPC - CON 18
void upmu_otpc_otp_val_55_48(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON18_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_VAL_55_48_MASK),
										(kal_uint8)(BANK_1_RG_OTP_VAL_55_48_SHIFT)
										);
}

//OTPC - CON 19
void upmu_otpc_otp_val_63_56(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON19_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_VAL_63_56_MASK),
										(kal_uint8)(BANK_1_RG_OTP_VAL_63_56_SHIFT)
										);
}

//OTPC - CON 20
kal_uint32 upmu_otpc_otp_busy(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON20_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RG_OTP_BUSY_MASK),
									(kal_uint8)(BANK_1_RG_OTP_BUSY_SHIFT)
									);
	return val;
}

kal_uint32 upmu_otpc_otp_vld(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON20_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RG_OTP_VLD_MASK),
									(kal_uint8)(BANK_1_RG_OTP_VLD_SHIFT)
									);
	return val;
}

void upmu_otpc_otp_read_rdy_bypass(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON20_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_READ_RDY_BYPASS_MASK),
										(kal_uint8)(BANK_1_RG_OTP_READ_RDY_BYPASS_SHIFT)
										);
}

//OTPC - CON 21
kal_uint32 upmu_otpc_otp_w_lock(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_bank1_read_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON21_OFFSET), 
									(&val),
									(kal_uint8)(BANK_1_RG_OTP_W_LOCK_MASK),
									(kal_uint8)(BANK_1_RG_OTP_W_LOCK_SHIFT)
									);
	return val;
}

void upmu_otpc_otp_w_lock_key_tog(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON21_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_W_LOCK_KEY_TOG_MASK),
										(kal_uint8)(BANK_1_RG_OTP_W_LOCK_KEY_TOG_SHIFT)
										);
}

void upmu_otpc_otp_w_lock_key(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_OTPC_CTRL + OTPC_CON21_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_RG_OTP_W_LOCK_KEY_MASK),
										(kal_uint8)(BANK_1_RG_OTP_W_LOCK_KEY_SHIFT)
										);
}

//==============================================================================
// I2C APIs
//==============================================================================
//I2C - CON 0
void upmu_i2c_scl_deglitch(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_I2C_CTRL + I2C_CON0_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SCL_DE_MASK),
										(kal_uint8)(BANK_1_SCL_DE_SHIFT)
										);
}

//I2C - CON 1
void upmu_i2c_sdin_deglitch(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_I2C_CTRL + I2C_CON1_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SDIN_DE_MASK),
										(kal_uint8)(BANK_1_SDIN_DE_SHIFT)
										);
}

//==============================================================================
// E2 new APIs - BANK0
//==============================================================================
kal_uint32 upmu_RO_JUST_PWRKEY_RST(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=pmic_read_interface((kal_uint8)(0x15),(&val),(kal_uint8)(0x01),(kal_uint8)(0x07) );
	
	return val;
}

void upmu_RG_VPROC_SFCHG_RATE(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0x15),(kal_uint8)(val),(kal_uint8)(0x07),(kal_uint8)(0x04) );
}

void upmu_VBAT_OV_VTH(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0x27),(kal_uint8)(val),(kal_uint8)(0x03),(kal_uint8)(0x01) );
}

void upmu_RG_OSC_SEL_AUTO(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0x4F),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x07) );
}

void upmu_RG_BUCK_2M_SEL_EN(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0x4F),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x06) );
}

void upmu_RG_DRV_1M_SEL_EN(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0x4F),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x05) );
}

void upmu_RG_BST_DRV_10M_CK_PDN(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0x4F),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x04) );
}

void upmu_RG_VPROC_DIS_DCR_SFCHG(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0x4F),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x03) );
}

void upmu_RG_VCORE_DVS_SEL(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0x4F),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x02) );
}

void upmu_RG_VCORE_DIS_DCR_SFCHG(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0x4F),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x01) );
}

void upmu_RG_SPK_1M_SEL_EN(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0x4F),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x00) );
}

void upmu_RG_VM12_2_VOSEL(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0xBA),(kal_uint8)(val),(kal_uint8)(0x03),(kal_uint8)(0x02) );
}

void upmu_RG_VM12_2_SLEEP(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0xBA),(kal_uint8)(val),(kal_uint8)(0x03),(kal_uint8)(0x00) );
}

void upmu_VA2_LP_SET(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0xD1),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x03) );
}

void upmu_VA2_LP_SEL(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0xD1),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x02) );
}

void upmu_RG_VM12_INT_DVS_SEL(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0xD1),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x01) );
}

void upmu_RG_VM12_INT_DIS_DCR_SFCHG(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_config_interface( (kal_uint8)(0xD1),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x00) );
}

//==============================================================================
// E2 new APIs - BANK1
//==============================================================================

void upmu_RG_SDA_IO_CONFIG(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x08),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x03) );
}

void upmu_IVGEN_EXT_EN(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x17),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x07) );
}

void upmu_DDUVLO_DEB_EN(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x17),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x06) );
}

void upmu_PWRBB_DEB_SEL(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x17),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x05) );
}

void upmu_RG_PWRKEY_INT_SEL(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x17),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x04) );
}

void upmu_RG_HOMEKEY_INT_SEL(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x17),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x03) );
}

void upmu_CLR_JUST_RST(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x22),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x07) );
}

void upmu_I2C_SYNC_EN(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x3C),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x04) );
}

void upmu_ISINKS_DIM2_FON(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x3C),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x02) );
}

void upmu_ISINKS_DIM1_FON(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x3C),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x01) );
}

void upmu_ISINKS_DIM0_FON(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x3C),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x00) );
}

void upmu_RG_VM12_2_DIS_SRCLKEN(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x89),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x07) );
}

void upmu_RG_VM12_2_DIS_DISCHR(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x89),(kal_uint8)(val),(kal_uint8)(0x01),(kal_uint8)(0x06) );
}

void upmu_RG_VM12_INT_DIS_WID(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=pmic_bank1_config_interface( (kal_uint8)(0x89),(kal_uint8)(val),(kal_uint8)(0x03),(kal_uint8)(0x04) );
} 

