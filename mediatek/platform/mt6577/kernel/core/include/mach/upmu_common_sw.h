/*****************************************************************************
*
* Filename:
* ---------
*   upmu_common_sw.h
*
* Project:
* --------
*   Android
*
* Description:
* ------------
*   Unified PMU common APIs header file
*
* Author:
* -------
*
****************************************************************************/

#ifndef _MT6577_PMIC_COMMON_SW_H_
#define _MT6577_PMIC_COMMON_SW_H_

//==============================================================================
// General APIs
//==============================================================================
//General
extern void upmu_write_reg_value(kal_uint32 reg, kal_uint32 val, kal_uint32 mask, kal_uint32 shift);
extern void upmu_write_reg_value_bank1(kal_uint32 reg, kal_uint32 val, kal_uint32 mask, kal_uint32 shift);
extern kal_uint32 upmu_get_reg_value(kal_uint32 reg);
extern kal_uint32 upmu_get_reg_value_bank1(kal_uint32 reg);
extern kal_uint32 upmu_get_cid0(void);
extern kal_uint32 upmu_get_cid1(void);
extern kal_uint32 upmu_get_PCHR_CHRDET(void);

//==============================================================================
// STATUS APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_status_vcore(void);
extern kal_uint32 upmu_status_vproc(void);
extern kal_uint32 upmu_status_vio18(void);
extern kal_uint32 upmu_status_vrf18(void);
extern kal_uint32 upmu_status_vpa(void);
extern kal_uint32 upmu_status_vrf(void);
extern kal_uint32 upmu_status_vtcxo(void);
extern kal_uint32 upmu_status_va1(void);
//CON 1
extern kal_uint32 upmu_status_vcama(void);
extern kal_uint32 upmu_status_vm12_1(void);
extern kal_uint32 upmu_status_vm12_int(void);
extern kal_uint32 upmu_status_vio28(void);
extern kal_uint32 upmu_status_vsim1(void);
extern kal_uint32 upmu_status_vsim2(void);
extern kal_uint32 upmu_status_vusb(void);
extern kal_uint32 upmu_status_vcamd(void);
//CON 2
extern kal_uint32 upmu_status_vcam_io(void);
extern kal_uint32 upmu_status_vcam_af(void);
extern kal_uint32 upmu_status_vmc(void);
extern kal_uint32 upmu_status_vmch(void);
extern kal_uint32 upmu_status_vgp(void);
extern kal_uint32 upmu_status_vgp2(void);
extern kal_uint32 upmu_status_vm12_2(void);
extern kal_uint32 upmu_status_vibr(void);
//CON 3
extern kal_uint32 upmu_status_va2(void);
extern kal_uint32 upmu_status_vrtc(void);
extern kal_uint32 upmu_status_vtcxo_pg(void);

//==============================================================================
// Power Good Status APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_status_vcore_pg(void);
extern kal_uint32 upmu_status_vproc_pg(void);
extern kal_uint32 upmu_status_vio18_pg(void);
extern kal_uint32 upmu_status_vio28_pg(void);
extern kal_uint32 upmu_status_va1_pg(void);
extern kal_uint32 upmu_status_vm12_1_pg(void);
extern kal_uint32 upmu_status_vm12_2_pg(void);
extern kal_uint32 upmu_status_vm12_int_pg(void);

//==============================================================================
// Charger Status APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_status_chr_cv(void);
extern kal_uint32 upmu_status_chr_ro_baton_undet(void);
extern kal_uint32 upmu_status_chr_pchr_chrdet(void);
extern kal_uint32 upmu_status_chr_vbat_ov(void);
extern kal_uint32 upmu_status_chr_pwrkey_deb(void);
extern kal_uint32 upmu_status_chr_usb_dl(void);
extern kal_uint32 upmu_status_chr_test_mode_por(void);

//==============================================================================
// OC Status APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_status_oc_vcama(void);
extern kal_uint32 upmu_status_oc_vm12_1(void);
extern kal_uint32 upmu_status_oc_vm12_int(void);
extern kal_uint32 upmu_status_oc_vio28(void);
extern kal_uint32 upmu_status_oc_vsim1(void);
extern kal_uint32 upmu_status_oc_vsim2(void);
extern kal_uint32 upmu_status_oc_vusb(void);
extern kal_uint32 upmu_status_oc_vcamad(void);
//CON 1
extern kal_uint32 upmu_status_oc_vcam_io(void);
extern kal_uint32 upmu_status_oc_vcam_af(void);
extern kal_uint32 upmu_status_oc_vmc(void);
extern kal_uint32 upmu_status_oc_vmch(void);
extern kal_uint32 upmu_status_oc_vgp(void);
extern kal_uint32 upmu_status_oc_vgp2(void);
extern kal_uint32 upmu_status_oc_vm12_2(void);
extern kal_uint32 upmu_status_oc_vibr(void);
//CON 2
extern kal_uint32 upmu_status_oc_homekey_deb(void);
extern kal_uint32 upmu_status_oc_boost(void);
extern kal_uint32 upmu_status_oc_va2(void);
extern kal_uint32 upmu_status_oc_ni_spk_det_d_r(void);
extern kal_uint32 upmu_status_oc_ni_spk_det_d_l(void);
extern kal_uint32 upmu_status_oc_ni_spk_det_ab_r(void);
extern kal_uint32 upmu_status_oc_ni_spk_det_ab_l(void);
//CON 3
extern kal_uint32 upmu_status_oc_vcore(void);
extern kal_uint32 upmu_status_oc_vproc(void);
extern kal_uint32 upmu_status_oc_vio18(void);
extern kal_uint32 upmu_status_oc_vrf18(void);
extern kal_uint32 upmu_status_oc_vpa(void);
extern kal_uint32 upmu_status_oc_vrf(void);
extern kal_uint32 upmu_status_oc_vtcxo(void);
extern kal_uint32 upmu_status_oc_va1(void);

//==============================================================================
// STARTUP APIs
//==============================================================================
//CON 0
extern void upmu_strup_thrdet_sel(kal_uint32 val);
extern void upmu_strup_thr_hwpdn_en(kal_uint32 val);
extern void upmu_strup_thr_sel(kal_uint32 val);
extern void upmu_strup_thr_test_mode(kal_uint32 val);
extern void upmu_strup_thr_force_non_thermal(kal_uint32 val);
//CON 1
extern void upmu_strup_vref_bg(kal_uint32 val);
extern void upmu_strup_iref_trim(kal_uint32 val);
//CON 2
extern void upmu_strup_bb_proc_sel(kal_uint32 val);
extern void upmu_strup_rst_drvsel(kal_uint32 val);
extern void upmu_strup_pmu_lev_ungate(kal_uint32 val);
extern void upmu_strup_usbdl_en(kal_uint32 val);
//CON 3
extern void upmu_strup_pmu_rsv(kal_uint32 val);
//CON 4
extern void upmu_strup_bias_gen_en_force(kal_uint32 val);
extern void upmu_strup_pwron_force(kal_uint32 val);
extern void upmu_strup_test_mode(kal_uint32 val);
extern void upmu_strup_ft_ctrl_test_mode(kal_uint32 val);
//CON 5
extern kal_uint32 upmu_strup_pmu_thr_status(void);
extern void upmu_strup_thermal_test_mode(kal_uint32 val);
//CON 6
extern void upmu_strup_vcore_pg_enb(kal_uint32 val);
extern void upmu_strup_vproc_pg_enb(kal_uint32 val);
extern void upmu_strup_vio18_pg_enb(kal_uint32 val);
extern void upmu_strup_vio28_pg_enb(kal_uint32 val);
extern void upmu_strup_va1_pg_enb(kal_uint32 val);
extern void upmu_strup_vm12_1_pg_enb(kal_uint32 val);
extern void upmu_strup_vm12_2_pg_enb(kal_uint32 val);
extern void upmu_strup_vm12_int_pg_enb(kal_uint32 val);
//CON 7
extern void upmu_strup_dig_rsv(kal_uint32 val);
extern void upmu_strup_vtcxo_pg_enb(kal_uint32 val);

//==============================================================================
// Interrupt control APIs
//==============================================================================
//CON 0
extern void upmu_interrupt_ov_en(kal_uint32 val);
extern void upmu_interrupt_chrdet_int_en(kal_uint32 val);
extern void upmu_interrupt_bvalid_det_int_en(kal_uint32 val);
extern void upmu_interrupt_vbaton_undet_int_en(kal_uint32 val);
extern void upmu_interrupt_thr_h_int_en(kal_uint32 val);
extern void upmu_interrupt_thr_l_int_en(kal_uint32 val);
extern void upmu_interrupt_pwrkey_int_en(kal_uint32 val);
extern void upmu_interrupt_watchdog_int_en(kal_uint32 val);
//CON 1
extern void upmu_interrupt_fg_bat_h_int_en(kal_uint32 val);
extern void upmu_interrupt_fg_bat_l_int_en(kal_uint32 val);
extern void upmu_interrupt_high_bat_int_en(kal_uint32 val);
extern void upmu_interrupt_low_bat_int_en(kal_uint32 val);
extern void upmu_interrupt_spkr_d_oc_int_en(kal_uint32 val);
extern void upmu_interrupt_spkl_d_oc_int_en(kal_uint32 val);
extern void upmu_interrupt_spkr_ab_oc_int_en(kal_uint32 val);
extern void upmu_interrupt_spkl_ab_oc_int_en(kal_uint32 val);
//CON 2
extern void upmu_interrupt_homekey_int_en(kal_uint32 val);
extern void upmu_interrupt_vrf18_oc_int_en(kal_uint32 val);
extern void upmu_interrupt_vpa_oc_int_en(kal_uint32 val);
extern void upmu_interrupt_ldo_oc_int_en(kal_uint32 val);
//CON 3
extern kal_uint32 upmu_interrupt_get_ov_status(void);
extern kal_uint32 upmu_interrupt_get_chrdet_int_status(void);
extern kal_uint32 upmu_interrupt_get_bvalid_det_int_status(void);
extern kal_uint32 upmu_interrupt_get_vbaton_undet_int_status(void);
extern kal_uint32 upmu_interrupt_get_thr_h_int_status(void);
extern kal_uint32 upmu_interrupt_get_thr_l_int_status(void);
extern kal_uint32 upmu_interrupt_get_pwrkey_int_status(void);
extern kal_uint32 upmu_interrupt_get_watchdog_int_status(void);
//CON 4
extern kal_uint32 upmu_interrupt_get_fg_bat_h_int_status(void);
extern kal_uint32 upmu_interrupt_get_fg_bat_l_int_status(void);
extern kal_uint32 upmu_interrupt_get_high_bat_int_status(void);
extern kal_uint32 upmu_interrupt_get_low_bat_int_status(void);
extern kal_uint32 upmu_interrupt_get_spkr_d_oc_int_status(void);
extern kal_uint32 upmu_interrupt_get_spkl_d_oc_int_status(void);
extern kal_uint32 upmu_interrupt_get_spkr_ab_oc_int_status(void);
extern kal_uint32 upmu_interrupt_get_spkl_ab_oc_int_status(void);
//CON 5
extern kal_uint32 upmu_interrupt_get_homekey_int_status(void);
extern kal_uint32 upmu_interrupt_get_vrf18_oc_int_status(void);
extern kal_uint32 upmu_interrupt_get_vpa_oc_int_status(void);
extern kal_uint32 upmu_interrupt_get_ldo_oc_int_status(void);

//==============================================================================
// Charger APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_chr_get_vcdt_hv_det(void);
extern kal_uint32 upmu_chr_get_vcdt_lv_det(void);
extern kal_uint32 upmu_chr_get_chrdet(void);
extern void upmu_chr_enable(kal_uint32 val);
extern void upmu_chr_csdac_enable(kal_uint32 val);
extern void upmu_chr_hw_auto_enable(kal_uint32 val);
extern kal_uint32 upmu_chr_get_ldo_det(void);
extern void upmu_chr_vcdt_hv_enable(kal_uint32 val);
//CON1
extern void upmu_chr_vcdt_hv_vth(kal_uint32 val);
extern void upmu_chr_vcdt_lv_vth(kal_uint32 val);
//CON2
extern kal_uint32 upmu_chr_get_vbat_cc_det(void);
extern kal_uint32 upmu_chr_get_vbat_cv_det(void);
extern kal_uint32 upmu_chr_get_cs_det(void);
extern void upmu_chr_cs_en(kal_uint32 val);
extern void upmu_chr_vbat_cc_en(kal_uint32 val);
extern void upmu_chr_vbat_cv_en(kal_uint32 val);
//CON3
extern void upmu_chr_vbat_cc_vth(kal_uint32 val);
extern void upmu_chr_vbat_cv_vth(kal_uint32 val);
//CON4
extern void upmu_chr_vbat_cs_vth(kal_uint32 val);
//CON5
extern void upmu_chr_toltc(kal_uint32 val);
extern void upmu_chr_tohtc(kal_uint32 val);
//CON6
extern kal_uint32 upmu_chr_get_vbat_ov_det(void);
extern void upmu_chr_vbat_ov_deg(kal_uint32 val);
extern void upmu_chr_vbat_ov_vth(kal_uint32 val);
extern void upmu_chr_vbat_ov_en(kal_uint32 val);
//CON7
extern kal_uint32 upmu_chr_get_baton_undet(void);
extern void upmu_chr_baton_ht_trim(kal_uint32 val);
extern void upmu_chr_baton_ht_en(kal_uint32 val);
extern void upmu_chr_baton_en(kal_uint32 val);
//CON8
extern void upmu_chr_csdac_dat_h(kal_uint32 val);
//CON9
extern void upmu_chr_csdac_dat_l(kal_uint32 val);
//CONA
extern kal_uint32 upmu_chr_get_otg_bvalid(void);
extern void upmu_chr_otg_bvalid_en(kal_uint32 val);
extern void upmu_chr_pchr_flag_en(kal_uint32 val);
extern void upmu_chr_pchr_flag_out(kal_uint32 val);
//CONB
extern void upmu_chr_pchr_flag_sel(kal_uint32 val);
//CONC
extern void upmu_chr_pchr_ft_ctrl(kal_uint32 val);
extern void upmu_chr_pchr_rst(kal_uint32 val);
extern void upmu_chr_csdac_test_mode(kal_uint32 val);
extern void upmu_chr_pchr_test_mode(kal_uint32 val);
//COND
extern void upmu_chr_chrwdt_en(kal_uint32 val);
extern void upmu_chr_chrwdt_td(kal_uint32 val);
//CONE
extern void upmu_chr_pchr_rv(kal_uint32 val);
//CONF
extern kal_uint32 upmu_chr_get_chrwdt_out(void);
extern void upmu_chr_chrwdt_flag_wr(kal_uint32 val);
extern void upmu_chr_chrwdt_int_en(kal_uint32 val);
//CON11
extern void upmu_chr_adcin_vchr_en(kal_uint32 val);
extern void upmu_chr_adcin_vsen_en(kal_uint32 val);
extern void upmu_chr_adcin_vbat_en(kal_uint32 val);
extern void upmu_chr_usbdl_set(kal_uint32 val);
extern void upmu_chr_usbdl_rst(kal_uint32 val);
extern void upmu_chr_uvlo_vthl(kal_uint32 val);
//CON12
extern void upmu_chr_bgr_unchop(kal_uint32 val);
extern void upmu_chr_bgr_unchop_ph(kal_uint32 val);
extern void upmu_chr_bgr_rsel(kal_uint32 val);
//CON13
extern kal_uint32 upmu_chr_get_bc11_cmp_out(void);
extern void upmu_chr_bc11_vsrc_en(kal_uint32 val);
extern void upmu_chr_bc11_rst(kal_uint32 val);
extern void upmu_chr_bc11_bb_ctrl(kal_uint32 val);
//CON14
extern void upmu_chr_bc11_bias_en(kal_uint32 val);
extern void upmu_chr_bc11_ipu_en(kal_uint32 val);
extern void upmu_chr_bc11_ipd_en(kal_uint32 val);
extern void upmu_chr_bc11_cmp_en(kal_uint32 val);
extern void upmu_chr_bc11_vref_vth(kal_uint32 val);
//CON15
extern void upmu_chr_csdac_stp_dec(kal_uint32 val);
extern void upmu_chr_csdac_stp_inc(kal_uint32 val);
//CON16
extern void upmu_chr_csdac_stp(kal_uint32 val);
extern void upmu_chr_csdac_dly(kal_uint32 val);
//CON17
extern void upmu_chr_chrind_dimming(kal_uint32 val);
extern void upmu_chr_chrind_on(kal_uint32 val);
extern void upmu_chr_low_ich_db(kal_uint32 val);
//CON18
extern void upmu_chr_ulc_det_en(kal_uint32 val);
extern void upmu_chr_hw_cv_en(kal_uint32 val);
extern void upmu_chr_baton_tdet_en(kal_uint32 val);
extern void upmu_chr_tracking_en(kal_uint32 val);
extern void upmu_chr_csdac_mode(kal_uint32 val);
extern void upmu_chr_vcdt_mode(kal_uint32 val);
extern void upmu_chr_cv_mode(kal_uint32 val);
//CON19
extern void upmu_chr_ichrg_trim(kal_uint32 val);
extern void upmu_chr_bgr_trim_en(kal_uint32 val);
//CON1A
extern void upmu_chr_bgr_trim(kal_uint32 val);
//CON1B
extern void upmu_chr_ovp_trim(kal_uint32 val);
//CON1C
extern void upmu_chr_rsv0(kal_uint32 val);
extern void upmu_chr_bgr_test_rstb(kal_uint32 val);
extern void upmu_chr_bgr_test_en(kal_uint32 val);
extern void upmu_chr_osc_trim(kal_uint32 val);
//CON1D
extern void upmu_chr_rsv1(kal_uint32 val);

//==============================================================================
// BUCK APIs
//==============================================================================
//BUCK - CON 0
extern void upmu_buck_csl(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_csr(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_cc(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_rzsel(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON 1
extern void upmu_buck_slp(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_zx_os(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_slew(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_slew_nmos(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON 2
extern void upmu_buck_avp_os(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_avp_en(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_modeset(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON 3
extern void upmu_buck_qi_en(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_on_ctrl(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_en(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON 4
extern void upmu_buck_stb_td(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_burst(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_vfbadj(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_ocfb_en(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_ndis_en(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON 5
extern void upmu_buck_vosel(upmu_buck_list_enum buck, upmu_buck_vol_enum vol);
//BUCK - CON 6
extern void upmu_buck_vosel_srclken_0(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_vosel_con_1(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_pasel_set_0(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON 7
extern void upmu_buck_vosel_srclken_1(upmu_buck_list_enum buck, kal_uint32 val);
extern kal_uint32 upmu_buck_qi_oc_status(upmu_buck_list_enum buck);
extern void upmu_buck_pasel_set_1(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON 8
extern void upmu_buck_vosel_dvs_00(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_pasel_set_2(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON 9
extern void upmu_buck_vosel_dvs_01(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_pasel_set_3(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON A
extern void upmu_buck_vosel_dvs_10(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_pasel_set_4(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON B
extern void upmu_buck_vosel_dvs_11(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_pasel_set_5(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON C
extern void upmu_buck_vosel_sfchg_en(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_ctrl(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_pasel_set_6(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON D
extern void upmu_buck_smps_testmode(upmu_buck_list_enum buck, kal_uint32 val);
extern void upmu_buck_pasel_set_7(upmu_buck_list_enum buck, kal_uint32 val);
//BUCK - CON E

//BUCK - CON F
extern kal_uint32 upmu_buck_qi_vosel(upmu_buck_list_enum buck);

//==============================================================================
// LDO APIs
//==============================================================================
//LDO - CON 0
extern void upmu_ldo_cal(upmu_ldo_list_enum ldo, kal_uint32 val);
//LDO - CON 1
extern kal_uint32 upmu_ldo_qi_en(upmu_ldo_list_enum ldo);
extern void upmu_ldo_vosel(upmu_ldo_list_enum ldo, upmu_ldo_vol_enum vol);
extern void upmu_ldo_on_ctrl(upmu_ldo_list_enum ldo, kal_uint32 val);
extern void upmu_ldo_en(upmu_ldo_list_enum ldo, kal_uint32 val);
//LDO - CON 2
extern void upmu_ldo_stbtd(upmu_ldo_list_enum ldo, kal_uint32 val);
extern void upmu_ldo_thr_shdn_en(upmu_ldo_list_enum ldo, kal_uint32 val);
extern void upmu_ldo_stb_sel(upmu_ldo_list_enum ldo, kal_uint32 val);
extern void upmu_ldo_ocfb_en(upmu_ldo_list_enum ldo, kal_uint32 val);
extern void upmu_ldo_ndis_en(upmu_ldo_list_enum ldo, kal_uint32 val);
//CON 3
extern kal_uint32 upmu_ldo_get_qi_mode(upmu_ldo_list_enum ldo);
extern void upmu_ldo_lp_set(upmu_ldo_list_enum ldo, kal_uint32 val);
extern void upmu_ldo_lp_sel(upmu_ldo_list_enum ldo, kal_uint32 val);

//==============================================================================
// BUCK CALIBRATION APIs
//==============================================================================
//CON 0
extern void upmu_buck_calibration_auto_k(kal_uint32 val);
extern void upmu_buck_calibration_k_src_sel(kal_uint32 val);
extern void upmu_buck_calibration_k_start_manual(kal_uint32 val);
extern void upmu_buck_calibration_k_once(kal_uint32 val);
extern void upmu_buck_calibration_k_once_en(kal_uint32 val);
extern void upmu_buck_calibration_k_map_sel(kal_uint32 val);
extern void upmu_buck_calibration_k_rst_done(kal_uint32 val);
//CON 1
extern void upmu_buck_calibration_k_control_smps(kal_uint32 val);
//CON 2
extern kal_uint32 upmu_buck_calibration_get_k_control(void);
extern kal_uint32 upmu_buck_calibration_get_k_done(void);
extern kal_uint32 upmu_buck_calibration_get_k_result(void);

//==============================================================================
// AUXADC APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_auxadc_get_ch0_data_7_0(void);
//CON 1
extern kal_uint32 upmu_auxadc_get_ch0_ready(void);
extern kal_uint32 upmu_auxadc_get_ch0_data_9_8(void);
//CON 2
extern kal_uint32 upmu_auxadc_get_ch1_data_7_0(void);
//CON 3
extern kal_uint32 upmu_auxadc_get_ch1_ready(void);
extern kal_uint32 upmu_auxadc_get_ch1_data_9_8(void);
//CON 4
extern kal_uint32 upmu_auxadc_get_ch2_data_7_0(void);
//CON 5
extern kal_uint32 upmu_auxadc_get_ch2_ready(void);
extern kal_uint32 upmu_auxadc_get_ch2_data_9_8(void);
//CON 6
extern kal_uint32 upmu_auxadc_get_ch3_data_7_0(void);
//CON 7
extern kal_uint32 upmu_auxadc_get_ch3_ready(void);
extern kal_uint32 upmu_auxadc_get_ch3_data_9_8(void);
//CON 8
extern kal_uint32 upmu_auxadc_get_wakeup_date_7_0(void);
//CON 9
extern kal_uint32 upmu_auxadc_get_wakeup_date_ready(void);
extern kal_uint32 upmu_auxadc_get_wakeup_date_9_8(void);
//CON 10
extern kal_uint32 upmu_auxadc_get_low_battery_data_7_0(void);
//CON 11
extern kal_uint32 upmu_auxadc_get_low_battery_data_ready(void);
extern kal_uint32 upmu_auxadc_get_low_battery_data_9_8(void);
//CON 12
extern kal_uint32 upmu_auxadc_get_trimming_data_7_0(void);
//CON 13
extern kal_uint32 upmu_auxadc_get_trimming_data_ready(void);
extern kal_uint32 upmu_auxadc_get_trimming_data_9_8(void);
//CON 14
extern void upmu_auxadc_ch_sel(kal_uint32 val);
extern void upmu_auxadc_trim_comp(kal_uint32 val);
extern void upmu_auxadc_bist_enb(kal_uint32 val);
extern void upmu_auxadc_start(kal_uint32 val);
//CON 15
extern void upmu_auxadc_spl_num(kal_uint32 val);
extern void upmu_auxadc_avg_num(kal_uint32 val);
extern void upmu_auxadc_buf_pwd_b(kal_uint32 val);
extern void upmu_auxadc_adc_pwd_b(kal_uint32 val);
//CON 16
extern void upmu_auxadc_lowbat_debtime_max(kal_uint32 val);
//CON 17
extern void upmu_auxadc_lowbat_debtime_min(kal_uint32 val);
//CON 18
extern void upmu_auxadc_lowbat_period_7_0(kal_uint32 val);
//CON 19
extern void upmu_auxadc_lowbat_period_15_8(kal_uint32 val);
//CON 20
extern void upmu_auxadc_lowbat_period_19_16(kal_uint32 val);
//CON 21
extern void upmu_auxadc_lowbat_set_max_voltage_7_0(kal_uint32 val);
//CON 22
extern void upmu_auxadc_lowbat_set_max_voltage_9_8(kal_uint32 val);
extern void upmu_auxadc_lowbat_en_max_volt(kal_uint32 val);
extern void upmu_auxadc_lowbat_irq_en_max_volt(kal_uint32 val);
//CON 23
extern void upmu_auxadc_lowbat_set_min_voltage_7_0(kal_uint32 val);
//CON 24
extern void upmu_auxadc_lowbat_set_min_voltage_9_8(kal_uint32 val);
extern void upmu_auxadc_lowbat_en_min_volt(kal_uint32 val);
extern void upmu_auxadc_lowbat_irq_en_min_volt(kal_uint32 val);
//CON 25
extern kal_uint32 upmu_auxadc_lowbat_max_irq_status(void);
extern kal_uint32 upmu_auxadc_lowbat_min_irq_status(void);
//CON 26
extern void upmu_auxadc_da_dac_value_7_0(kal_uint32 val);
//CON 27
extern void upmu_auxadc_da_dac_value_9_8(kal_uint32 val);
extern kal_uint32 upmu_auxadc_ni_comp(void);
//CON 28
extern void upmu_auxadc_da_dac_sel(kal_uint32 val);
extern void upmu_auxadc_aux_out_sel(kal_uint32 val);
extern void upmu_auxadc_arb_prio_2(kal_uint32 val);
extern void upmu_auxadc_arb_prio_1(kal_uint32 val);
extern void upmu_auxadc_arb_prio_0(kal_uint32 val);
//CON 29
extern void upmu_auxadc_cali(kal_uint32 val);
extern void upmu_buf_cali(kal_uint32 val);

//==============================================================================
// TEST MODE APIs
//==============================================================================
//TEST MODE - CON 0
extern void upmu_testmode_mon_grp_sel(kal_uint32 val);
//TEST MODE - CON 1
extern void upmu_testmode_mon_flag_sel(kal_uint32 val);
//TEST MODE - CON 2
extern void upmu_testmode_io_pasel0_sel(kal_uint32 val);
extern void upmu_testmode_io_pasel1_sel(kal_uint32 val);
//TEST MODE - CON 3
extern void upmu_testmode_io_pasel2_sel(kal_uint32 val);
extern void upmu_testmode_io_int_sel(kal_uint32 val);
//TEST MODE - CON 4
extern void upmu_testmode_sck_pu(kal_uint32 val);
extern void upmu_testmode_sck_pd(kal_uint32 val);
extern void upmu_testmode_dio_smt(kal_uint32 val);
extern void upmu_testmode_dio_e2(kal_uint32 val);
extern void upmu_testmode_dio_e4(kal_uint32 val);
extern void upmu_testmode_dio_sr(kal_uint32 val);
//TEST MODE - CON 5
extern void upmu_testmode_sda_e2(kal_uint32 val);
extern void upmu_testmode_sda_e4(kal_uint32 val);
extern void upmu_testmode_sda_pu(kal_uint32 val);
extern void upmu_testmode_sda_pd(kal_uint32 val);
extern void upmu_testmode_int_e2(kal_uint32 val);
extern void upmu_testmode_int_e4(kal_uint32 val);
extern void upmu_testmode_int_pu(kal_uint32 val);
extern void upmu_testmode_int_pd(kal_uint32 val);
//TEST MODE - CON 6
extern void upmu_testmode_dvs_pu(kal_uint32 val);
extern void upmu_testmode_dvs_pd(kal_uint32 val);
extern void upmu_testmode_pasel_pu(kal_uint32 val);
extern void upmu_testmode_pasel_pd(kal_uint32 val);
extern void upmu_testmode_sysrstb_pu(kal_uint32 val);
extern void upmu_testmode_sysrstb_pd(kal_uint32 val);
//TEST MODE - CON 7
extern void upmu_testmode_srclken_pu(kal_uint32 val);
extern void upmu_testmode_srclken_pd(kal_uint32 val);
extern void upmu_testmode_bl_pwm_pu(kal_uint32 val);
extern void upmu_testmode_bl_pwm_pd(kal_uint32 val);
//TEST MODE - CON 8
extern void upmu_testmode_test_rsv(kal_uint32 val);
extern void upmu_testmode_test_strup(kal_uint32 val);
extern void upmu_testmode_otp_w_mode(kal_uint32 val);
extern void upmu_testmode_nandtree_mode(kal_uint32 val);
//TEST MODE - CON 9
extern void upmu_testmode_test_auxadc(kal_uint32 val);
extern void upmu_testmode_test_fgpll(kal_uint32 val);
extern void upmu_testmode_test_fg1(kal_uint32 val);
extern void upmu_testmode_test_fg2(kal_uint32 val);
extern void upmu_testmode_test_io_fg_sel(kal_uint32 val);
extern void upmu_testmode_test_classd(kal_uint32 val);
extern void upmu_testmode_test_driver(kal_uint32 val);
extern void upmu_testmode_test_boost(kal_uint32 val);
//TEST MODE - CON 10
extern kal_uint32 upmu_testmode_mon_flag_status(void);
//TEST MODE - CON 11
extern kal_uint32 upmu_testmode_dvs1_in_status(void);
extern kal_uint32 upmu_testmode_dvs2_in_status(void);
extern kal_uint32 upmu_testmode_pasel0_in_status(void);
extern kal_uint32 upmu_testmode_pasel1_in_status(void);
extern kal_uint32 upmu_testmode_pasel2_in_status(void);
extern kal_uint32 upmu_testmode_sysrstb_in_status(void);
extern kal_uint32 upmu_testmode_srclken_in_status(void);
extern kal_uint32 upmu_testmode_blpwm_in_status(void);
//TEST MODE - CON 12
extern void upmu_testmode_pwrkey_rst_en(kal_uint32 val);
extern void upmu_testmode_homekey_rst_en(kal_uint32 val);
extern void upmu_testmode_pwrkey_rst_td(kal_uint32 val);
extern void upmu_testmode_pwrrst_tmr_dis(kal_uint32 val);
//TEST MODE - CON 13
extern void upmu_testmode_rst_part_sel(kal_uint32 val);
extern void upmu_testmode_otp_man_rst(kal_uint32 val);
extern void upmu_testmode_pchr_man_rst_en(kal_uint32 val);
extern void upmu_testmode_pchr_man_rst(kal_uint32 val);
extern void upmu_testmode_strup_man_rst_en(kal_uint32 val);
extern void upmu_testmode_sif_tst_ck_dis(kal_uint32 val);
extern void upmu_testmode_sysrstb_en(kal_uint32 val);

//==============================================================================
// TOP2 APIs
//==============================================================================
//TOP2 - CON 0
extern void upmu_top2_75k_ext_sel(kal_uint32 val);
extern void upmu_top2_fg_tst_ck_sel(kal_uint32 val);
extern void upmu_top2_chr1m_tst_ck_sel(kal_uint32 val);
extern void upmu_top2_clk_tst(kal_uint32 val);
extern void upmu_top2_auxadc_ck_rst(kal_uint32 val);
extern void upmu_top2_auxadc_ck_sel(kal_uint32 val);
//TOP2 - CON 1
extern void upmu_top2_fgadc_ck_pdn(kal_uint32 val);
extern void upmu_top2_otpc_ck_pdn(kal_uint32 val);
extern void upmu_top2_bst_drv_ck_pdn(kal_uint32 val);
extern void upmu_top2_spk_ck_pdn(kal_uint32 val);
extern void upmu_top2_pwmoc_ck_pdn(kal_uint32 val);
//TOP2 - CON 2
extern void upmu_top2_oc_gear_bvalid_det(kal_uint32 val);
extern void upmu_top2_oc_gear_vbaton_undet(kal_uint32 val);
extern void upmu_top2_oc_gear_ldo(kal_uint32 val);
//TOP2 - CON 3
extern void upmu_top2_int_rsv(kal_uint32 val);
extern void upmu_top2_polarity_bvalid_det(kal_uint32 val);
extern void upmu_top2_polarity_vbaton_undet(kal_uint32 val);
extern void upmu_top2_polarity_select(kal_uint32 val);
//TOP2 - CON 4
extern void upmu_top2_vpa_oc_wnd(kal_uint32 val);
extern void upmu_top2_vpa_oc_thd(kal_uint32 val);
extern void upmu_top2_vrf18_oc_wnd(kal_uint32 val);
extern void upmu_top2_vrf18_oc_thd(kal_uint32 val);

//==============================================================================
// FLASH APIs
//==============================================================================
//FLASH - CON 0
extern void upmu_flash_dim_duty(kal_uint32 val);
//FLASH - CON 1
extern void upmu_flash_ther_shdn_en(kal_uint32 val);
extern void upmu_flash_en(kal_uint32 val);
//FLASH - CON 2
extern void upmu_flash_dim_div(kal_uint32 val);
//FLASH - CON 3
extern void upmu_flash_step_sel(kal_uint32 val);
//FLASH - CON 4
extern void upmu_flash_sfstren(kal_uint32 val);
extern void upmu_flash_sfstr_time(kal_uint32 val);
extern void upmu_flash_mode_select(kal_uint32 val);

//==============================================================================
// KPLED APIs
//==============================================================================
//KPLED - CON 0
extern void upmu_kpled_dim_duty(kal_uint32 val);
//KPLED - CON 1
extern void upmu_kpled_ther_shdn_en(kal_uint32 val);
extern void upmu_kpled_en(kal_uint32 val);
//KPLED - CON 2
extern void upmu_kpled_dim_div(kal_uint32 val);
//KPLED - CON 3
extern void upmu_kpled_step_sel(kal_uint32 val);
//KPLED - CON 4
extern void upmu_kpled_sfstren(kal_uint32 val);
extern void upmu_kpled_sfstr_time(kal_uint32 val);
extern void upmu_kpled_mode_select(kal_uint32 val);

//==============================================================================
// ISINKS APIs
//==============================================================================
//ISINKS - CON 0
extern void upmu_isinks_dim0_duty(kal_uint32 val);
//ISINKS - CON 1
extern void upmu_isinks_dim0_fsel(kal_uint32 val);
//ISINKS - CON 2
extern void upmu_isinks_dim1_duty(kal_uint32 val);
//ISINKS - CON 3
extern void upmu_isinks_dim1_fsel(kal_uint32 val);
//ISINKS - CON 4
extern void upmu_isinks_dim2_duty(kal_uint32 val);
//ISINKS - CON 5
extern void upmu_isinks_dim2_fsel(kal_uint32 val);
//ISINKS - CON 6
extern void upmu_isinks_ch5_en(kal_uint32 val);
extern void upmu_isinks_ch4_en(kal_uint32 val);
extern void upmu_isinks_ch3_en(kal_uint32 val);
extern void upmu_isinks_ch2_en(kal_uint32 val);
extern void upmu_isinks_ch1_en(kal_uint32 val);
extern void upmu_isinks_ch0_en(kal_uint32 val);
//ISINKS - CON 7
extern void upmu_isinks_ch5_cabc_en(kal_uint32 val);
extern void upmu_isinks_ch4_cabc_en(kal_uint32 val);
extern void upmu_isinks_ch3_cabc_en(kal_uint32 val);
extern void upmu_isinks_ch2_cabc_en(kal_uint32 val);
extern void upmu_isinks_ch1_cabc_en(kal_uint32 val);
extern void upmu_isinks_ch0_cabc_en(kal_uint32 val);
//ISINKS - CON 8
extern void upmu_isinks_ch0_step(kal_uint32 val);
extern void upmu_isinks_ch0_mode(kal_uint32 val);
//ISINKS - CON 9
extern void upmu_isinks_ch1_step(kal_uint32 val);
extern void upmu_isinks_ch1_mode(kal_uint32 val);
//ISINKS - CON 10
extern void upmu_isinks_ch2_step(kal_uint32 val);
extern void upmu_isinks_ch2_mode(kal_uint32 val);
//ISINKS - CON 11
extern void upmu_isinks_ch3_step(kal_uint32 val);
extern void upmu_isinks_ch3_mode(kal_uint32 val);
//ISINKS - CON 12
extern void upmu_isinks_ch4_step(kal_uint32 val);
extern void upmu_isinks_ch4_mode(kal_uint32 val);
//ISINKS - CON 13
extern void upmu_isinks_ch5_step(kal_uint32 val);
extern void upmu_isinks_ch5_mode(kal_uint32 val);
//ISINKS - CON 14
extern void upmu_isinks_ibias_trim_en(kal_uint32 val);
extern void upmu_isinks_vref_cal(kal_uint32 val);

//==============================================================================
// BOOST APIs
//==============================================================================
//BOOST - CON 0
extern void upmu_boost_isink_hw_sel(kal_uint32 val);
extern void upmu_boost_mode(kal_uint32 val);
extern void upmu_boost_cabc_en(kal_uint32 val);
extern void upmu_boost_en(kal_uint32 val);
//BOOST - CON 1
extern void upmu_boost_sr_nmos(kal_uint32 val);
extern void upmu_boost_vrsel(kal_uint32 val);
//BOOST - CON 2
extern void upmu_boost_rc(kal_uint32 val);
extern void upmu_boost_cs(kal_uint32 val);
extern void upmu_boost_cc(kal_uint32 val);
//BOOST - CON 3
extern void upmu_boost_slp(kal_uint32 val);
extern void upmu_boost_cl(kal_uint32 val);
//BOOST - CON 4
extern void upmu_boost_ss(kal_uint32 val);
extern void upmu_boost_rsv(kal_uint32 val);

//==============================================================================
// SPK APIs
//==============================================================================
//SPK - CON 0
extern void upmu_spk_ther_shdn_l_en(kal_uint32 val);
extern void upmu_spk_trim_en_l(kal_uint32 val);
extern void upmu_spk_mode_l(kal_uint32 val);
extern void upmu_spk_en_l(kal_uint32 val);
//SPK - CON 1
extern void upmu_spk_oc_en_l(kal_uint32 val);
extern void upmu_spkab_oc_en_l(kal_uint32 val);
extern void upmu_spk_oc_shdn_dl(kal_uint32 val);
extern void upmu_spk_vol_l(kal_uint32 val);
//SPK - CON 2
extern kal_uint32 upmu_spk_ab_oc_l_deg(void);
extern kal_uint32 upmu_spk_d_oc_l_deg(void);
//SPK - CON 3
extern void upmu_spk_offset_l_ov(kal_uint32 val);
extern void upmu_spk_offset_l_mode(kal_uint32 val);
extern void upmu_spk_lead_l_sw(kal_uint32 val);
extern void upmu_spk_offset_l_sw(kal_uint32 val);
//SPK - CON 4
extern kal_uint32 upmu_spk_trim_done_l(void);
extern kal_uint32 upmu_spk_lead_l_flag(void);
extern kal_uint32 upmu_spk_lead_l_flag_deg(void);
extern kal_uint32 upmu_spk_offset_l(void);
//SPK - CON 5
extern void upmu_spkrcv_en_l(kal_uint32 val);
extern void upmu_spkab_obias_l(kal_uint32 val);
extern void upmu_spk_slew_l(kal_uint32 val);
extern void upmu_spk_force_en_l(kal_uint32 val);
extern void upmu_spk_intg_rst_l(kal_uint32 val);
//SPK - CON 6
extern void upmu_spk_ther_shdn_r_en(kal_uint32 val);
extern void upmu_spk_trim_en_r(kal_uint32 val);
extern void upmu_spk_mode_r(kal_uint32 val);
extern void upmu_spk_en_r(kal_uint32 val);
//SPK - CON 7
extern void upmu_spk_oc_en_r(kal_uint32 val);
extern void upmu_spkab_oc_en_r(kal_uint32 val);
extern void upmu_spk_oc_shdn_dr(kal_uint32 val);
extern void upmu_spk_vol_r(kal_uint32 val);	
//SPK - CON 8
extern kal_uint32 upmu_spk_ab_oc_r_deg(void);
extern kal_uint32 upmu_spk_d_oc_r_deg(void);
//SPK - CON 9
extern void upmu_spk_offset_r_ov(kal_uint32 val);
extern void upmu_spk_offset_r_mode(kal_uint32 val);
extern void upmu_spk_lead_r_sw(kal_uint32 val);
extern void upmu_spk_offset_r_sw(kal_uint32 val);
//SPK - CON 10
extern kal_uint32 upmu_spk_trim_done_r(void);
extern kal_uint32 upmu_spk_lead_r_flag(void);
extern kal_uint32 upmu_spk_lead_r_flag_deg(void);
extern kal_uint32 upmu_spk_offset_r(void);
//SPK - CON 11
extern void upmu_spkrcv_en_r(kal_uint32 val);
extern void upmu_spkab_obias_r(kal_uint32 val);
extern void upmu_spk_slew_r(kal_uint32 val);
extern void upmu_spk_force_en_r(kal_uint32 val);
extern void upmu_spk_intg_rst_r(kal_uint32 val);
//SPK - CON 12
extern void upmu_spk_oc_autoff(kal_uint32 val);
extern void upmu_spk_oc_dglh(kal_uint32 val);
extern void upmu_spk_octh_d(kal_uint32 val);
//SPK - CON 13
extern void upmu_spk_oc_wnd(kal_uint32 val);
extern void upmu_spk_oc_thd(kal_uint32 val);
//SPK - CON 14
extern void upmu_spk_trim_div(kal_uint32 val);
extern void upmu_spk_trim_deg(kal_uint32 val);
//SPK - CON 15
extern void upmu_spkab_fb_att(kal_uint32 val);
extern void upmu_spkab_ovdrv(kal_uint32 val);
//SPK - CON 16
extern void upmu_spk_fbrc_en(kal_uint32 val);
extern void upmu_spk_ibias_sel(kal_uint32 val);
extern void upmu_spk_vcm_ibsel(kal_uint32 val);
extern void upmu_spk_vcm_sel(kal_uint32 val);
extern void upmu_spk_en_view_clk(kal_uint32 val);
extern void upmu_spk_en_view_vcm(kal_uint32 val);
//SPK - CON 17
extern void upmu_spk_ccode(kal_uint32 val);
extern void upmu_spk_btl_set(kal_uint32 val);
//SPK - CON 18
extern void upmu_spk_rsv(kal_uint32 val);
//SPK - CON 19
extern void upmu_spk_td1(kal_uint32 val);
extern void upmu_spk_td2(kal_uint32 val);

//==============================================================================
// ASW APIs
//==============================================================================
//ASW - CON 0
extern void upmu_asw_ana_sw_sel(kal_uint32 val);

//==============================================================================
// FGPLL APIs
//==============================================================================
//FGPLL - CON 0
extern void upmu_fgpll_pdiv1(kal_uint32 val);
extern void upmu_fgpll_pdiv1_en(kal_uint32 val);
extern void upmu_fgpll_pllbs_rst(kal_uint32 val);
extern void upmu_fgpll_en(kal_uint32 val);
//FGPLL - CON 1
extern void upmu_fgpll_div1(kal_uint32 val);
//FGPLL - CON 2
extern void upmu_fgpll_bc(kal_uint32 val);
extern void upmu_fgpll_bp(kal_uint32 val);
extern void upmu_fgpll_br(kal_uint32 val);
//FGPLL - CON 3
extern void upmu_fgpll_cdiv(kal_uint32 val);
extern void upmu_fgpll_vcoband(kal_uint32 val);
extern void upmu_fgpll_cko_sel(kal_uint32 val);
//FGPLL - CON 4
extern void upmu_fgpll_ibsel(kal_uint32 val);
extern void upmu_fgpll_rlatch_en(kal_uint32 val);
extern void upmu_fgpll_ckdrv_en(kal_uint32 val);
extern void upmu_fgpll_vct_en(kal_uint32 val);
extern void upmu_fgpll_ckt_sel(kal_uint32 val);
extern void upmu_fgpll_ckt_en(kal_uint32 val);
//FGPLL - CON 5
extern void upmu_fgpll_rsva(kal_uint32 val);
//FGPLL - CON 6
extern void upmu_fgpll_rsvb(kal_uint32 val);

//==============================================================================
// FGADC APIs
//==============================================================================
//FGADC - CON 0
extern void upmu_fgadc_clksrc(kal_uint32 val);
extern void upmu_fgadc_autocalrate(kal_uint32 val);
extern void upmu_fgadc_cal(kal_uint32 val);
extern void upmu_fgadc_vmode(kal_uint32 val);
extern void upmu_fgadc_on(kal_uint32 val);
//FGADC - CON 1
extern void upmu_fgadc_sw_rst_clr(kal_uint32 val);
extern void upmu_fgadc_charge_rst(kal_uint32 val);
extern void upmu_fgadc_time_rst(kal_uint32 val);
extern void upmu_fgadc_offset_rst(kal_uint32 val);
extern void upmu_fgadc_sw_clear(kal_uint32 val);
extern void upmu_fgadc_latchdata_st(kal_uint32 val);
extern void upmu_fgadc_sw_read_pre(kal_uint32 val);
extern void upmu_fgadc_sw_cr(kal_uint32 val);
//FGADC - CON 2
extern kal_uint32 upmu_fgadc_car_35_32(void);
//FGADC - CON 3
extern kal_uint32 upmu_fgadc_car_31_24(void);
//FGADC - CON 4
extern kal_uint32 upmu_fgadc_car_23_16(void);
//FGADC - CON 5
extern kal_uint32 upmu_fgadc_car_15_08(void);
//FGADC - CON 6
extern kal_uint32 upmu_fgadc_car_07_00(void);
//FGADC - CON 7
extern kal_uint32 upmu_fgadc_nter_29_24(void);
//FGADC - CON 8
extern kal_uint32 upmu_fgadc_nter_23_16(void);
//FGADC - CON 9
extern kal_uint32 upmu_fgadc_nter_15_08(void);
//FGADC - CON 10
extern kal_uint32 upmu_fgadc_nter_07_00(void);
//FGADC - CON 11
extern void upmu_fgadc_bltr_15_08(kal_uint32 val);
//FGADC - CON 12
extern void upmu_fgadc_bltr_07_00(kal_uint32 val);
//FGADC - CON 13
extern void upmu_fgadc_bftr_15_08(kal_uint32 val);
//FGADC - CON 14
extern void upmu_fgadc_bftr_07_00(kal_uint32 val);
//FGADC - CON 15
extern kal_uint32 upmu_fgadc_current_out_15_08(void);
//FGADC - CON 16
extern kal_uint32 upmu_fgadc_current_out_07_00(void);
//FGADC - CON 17
extern void upmu_fgadc_adjust_offset_value_15_08(kal_uint32 val);
//FGADC - CON 18
extern void upmu_fgadc_adjust_offset_value_07_00(kal_uint32 val);
//FGADC - CON 19
extern kal_uint32 upmu_fgadc_offset_15_08(void);
//FGADC - CON 20
extern kal_uint32 upmu_fgadc_offset_07_00(void);
//FGADC - CON 21
extern void upmu_fgadc_analogtest(kal_uint32 val);
//FGADC - CON 22
extern void upmu_fgadc_spare(kal_uint32 val);
//FGADC - CON 23
extern void upmu_fgadc_bltr_bftr_en(kal_uint32 val);
extern void upmu_fgadc_adc_autorst(kal_uint32 val);
extern void upmu_fgadc_adj_offset_en(kal_uint32 val);
extern void upmu_fgadc_osr(kal_uint32 val);
//FGADC - CON 24
extern void upmu_fgadc_vol_osr(kal_uint32 val);
//FGADC - CON 25
extern kal_uint32 upmu_fgadc_adc_rstdetect(void);
extern kal_uint32 upmu_fgadc_h_int_sts(void);
extern kal_uint32 upmu_fgadc_l_int_sts(void);
extern void upmu_fgadc_vol_fir1bypass(kal_uint32 val);
extern void upmu_fgadc_fg_fir2bypass(kal_uint32 val);
extern void upmu_fgadc_fg_fir1bypass(kal_uint32 val);
//FGADC - CON 26
extern kal_uint32 upmu_fgadc_vol_current_out_15_08(void);
//FGADC - CON 27
extern kal_uint32 upmu_fgadc_vol_current_out_07_00(void);
//FGADC - CON 28
extern kal_uint32 upmu_fgadc_cic2_15_08(void);
//FGADC - CON 29
extern kal_uint32 upmu_fgadc_cic2_07_00(void);
//FGADC - CON 30
extern void upmu_fgadc_vmode_sw(kal_uint32 val);
extern void upmu_fgadc_fgadc_en_sw(kal_uint32 val);
extern void upmu_fgadc_fgcal_en_sw(kal_uint32 val);
extern void upmu_fgadc_rst_sw(kal_uint32 val);
extern void upmu_fgadc_mode(kal_uint32 val);

//==============================================================================
// OTPC APIs
//==============================================================================
//OTPC - CON 0
extern kal_uint32 upmu_otpc_otp_pdo_7_0(void);
//OTPC - CON 1
extern kal_uint32 upmu_otpc_otp_pdo_15_8(void);
//OTPC - CON 2
extern kal_uint32 upmu_otpc_otp_pdo_23_16(void);
//OTPC - CON 3
extern kal_uint32 upmu_otpc_otp_pdo_31_24(void);
//OTPC - CON 4
extern kal_uint32 upmu_otpc_otp_pdo_39_32(void);
//OTPC - CON 5
extern kal_uint32 upmu_otpc_otp_pdo_47_40(void);
//OTPC - CON 6
extern kal_uint32 upmu_otpc_otp_pdo_55_48(void);
//OTPC - CON 7
extern kal_uint32 upmu_otpc_otp_pdo_63_56(void);
//OTPC - CON 8
extern void upmu_otpc_otp_pdin(kal_uint32 val);
//OTPC - CON 9
extern void upmu_otpc_otp_pa(kal_uint32 val);
extern void upmu_otpc_otp_ptm(kal_uint32 val);
extern void upmu_otpc_otp_pwe(kal_uint32 val);
extern void upmu_otpc_otp_pprog(kal_uint32 val);
extern void upmu_otpc_otp_read(kal_uint32 val);
//OTPC - CON 10
extern void upmu_otpc_otp_read_prd(kal_uint32 val);
//OTPC - CON 11
extern void upmu_otpc_otp_test_sel(kal_uint32 val);
extern void upmu_otpc_otp_skip_out(kal_uint32 val);
extern void upmu_otpc_otp_out_sel(kal_uint32 val);
extern void upmu_otpc_otp_rsv(kal_uint32 val);
//OTPC - CON 12
extern void upmu_otpc_otp_val_7_0(kal_uint32 val);
//OTPC - CON 13
extern void upmu_otpc_otp_val_15_8(kal_uint32 val);
//OTPC - CON 14
extern void upmu_otpc_otp_val_23_16(kal_uint32 val);
//OTPC - CON 15
extern void upmu_otpc_otp_val_31_24(kal_uint32 val);
//OTPC - CON 16
extern void upmu_otpc_otp_val_39_32(kal_uint32 val);
//OTPC - CON 17
extern void upmu_otpc_otp_val_47_40(kal_uint32 val);
//OTPC - CON 18
extern void upmu_otpc_otp_val_55_48(kal_uint32 val);
//OTPC - CON 19
extern void upmu_otpc_otp_val_63_56(kal_uint32 val);
//OTPC - CON 20
extern kal_uint32 upmu_otpc_otp_busy(void);
extern kal_uint32 upmu_otpc_otp_vld(void);
extern void upmu_otpc_otp_read_rdy_bypass(kal_uint32 val);
//OTPC - CON 21
extern kal_uint32 upmu_otpc_otp_w_lock(void);
extern void upmu_otpc_otp_w_lock_key_tog(kal_uint32 val);
extern void upmu_otpc_otp_w_lock_key(kal_uint32 val);

//==============================================================================
// I2C APIs
//==============================================================================
//I2C - CON 0
extern void upmu_i2c_scl_deglitch(kal_uint32 val);
//I2C - CON 1
extern void upmu_i2c_sdin_deglitch(kal_uint32 val);

//==============================================================================
// E2 new APIs - BANK0
//==============================================================================
extern kal_uint32 upmu_RO_JUST_PWRKEY_RST(void);
extern void upmu_RG_VPROC_SFCHG_RATE(kal_uint32 val);
extern void upmu_VBAT_OV_VTH(kal_uint32 val);
extern void upmu_RG_OSC_SEL_AUTO(kal_uint32 val);
extern void upmu_RG_BUCK_2M_SEL_EN(kal_uint32 val);
extern void upmu_RG_DRV_1M_SEL_EN(kal_uint32 val);
extern void upmu_RG_BST_DRV_10M_CK_PDN(kal_uint32 val);
extern void upmu_RG_VPROC_DIS_DCR_SFCHG(kal_uint32 val);
extern void upmu_RG_VCORE_DVS_SEL(kal_uint32 val);
extern void upmu_RG_VCORE_DIS_DCR_SFCHG(kal_uint32 val);
extern void upmu_RG_SPK_1M_SEL_EN(kal_uint32 val);
extern void upmu_RG_VM12_2_VOSEL(kal_uint32 val);
extern void upmu_RG_VM12_2_SLEEP(kal_uint32 val);
extern void upmu_VA2_LP_SET(kal_uint32 val);
extern void upmu_VA2_LP_SEL(kal_uint32 val);
extern void upmu_RG_VM12_INT_DVS_SEL(kal_uint32 val);
extern void upmu_RG_VM12_INT_DIS_DCR_SFCHG(kal_uint32 val);

//==============================================================================
// E2 new APIs - BANK1
//==============================================================================
extern void upmu_RG_SDA_IO_CONFIG(kal_uint32 val);
extern void upmu_IVGEN_EXT_EN(kal_uint32 val);
extern void upmu_DDUVLO_DEB_EN(kal_uint32 val);
extern void upmu_PWRBB_DEB_SEL(kal_uint32 val);
extern void upmu_RG_PWRKEY_INT_SEL(kal_uint32 val);
extern void upmu_RG_HOMEKEY_INT_SEL(kal_uint32 val);
extern void upmu_CLR_JUST_RST(kal_uint32 val);
extern void upmu_I2C_SYNC_EN(kal_uint32 val);
extern void upmu_ISINKS_DIM2_FON(kal_uint32 val);
extern void upmu_ISINKS_DIM1_FON(kal_uint32 val);
extern void upmu_ISINKS_DIM0_FON(kal_uint32 val);
extern void upmu_RG_VM12_2_DIS_SRCLKEN(kal_uint32 val);
extern void upmu_RG_VM12_2_DIS_DISCHR(kal_uint32 val);
extern void upmu_RG_VM12_INT_DIS_WID(kal_uint32 val);

#endif // _MT6577_PMIC_COMMON_SW_H_

