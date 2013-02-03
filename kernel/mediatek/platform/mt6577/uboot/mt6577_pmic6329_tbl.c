//#include <mt6577.h>
//#include <mt6577_typedefs.h>
//#include <mt6577_i2c.h>
//#include <mt6577_pmic6329.h>

#include <common.h>
#include <asm/arch/mt65xx.h>
#include <asm/arch/mt65xx_typedefs.h>
#include <asm/arch/mt6577_i2c.h>
#include <asm/arch/mt6577_pmic6329.h>
#include <asm/arch/mt6577_gpio.h>

//==============================================================================
// PMIC6329 Chip Access APIs
//==============================================================================
U32 pmic6329_i2c_read (U8 chip, U8 *cmdBuffer, int cmdBufferLen, U8 *dataBuffer, int dataBufferLen)
{
    U32 ret_code = I2C_OK;

    ret_code = i2c_v1_write(chip, cmdBuffer, cmdBufferLen);    // set register command
    if (ret_code != I2C_OK)
        return ret_code;

    ret_code = i2c_v1_read(chip, dataBuffer, dataBufferLen);

    //dbg_print("[pmic6329_i2c_read] Done\n");

    return ret_code;
}

U32 pmic6329_i2c_write (U8 chip, U8 *cmdBuffer, int cmdBufferLen, U8 *dataBuffer, int dataBufferLen)
{
    U32 ret_code = I2C_OK;
    U8 write_data[I2C_FIFO_SIZE];
    int transfer_len = cmdBufferLen + dataBufferLen;
    int i=0, cmdIndex=0, dataIndex=0;

    if(I2C_FIFO_SIZE < (cmdBufferLen + dataBufferLen))
    {   return PMIC_EXCEED_I2C_FIFO_LENGTH;
    }

    //write_data[0] = cmd;
    //write_data[1] = writeData;

    while(cmdIndex < cmdBufferLen)
    {
        write_data[i] = cmdBuffer[cmdIndex];
        cmdIndex++;
        i++;
    }

    while(dataIndex < dataBufferLen)
    {
        write_data[i] = dataBuffer[dataIndex];
        dataIndex++;
        i++;
    }

    /* dump write_data for check */
    for( i=0 ; i < transfer_len ; i++ )
    {
        //dbg_print("[pmic6329_i2c_write] write_data[%d]=%x\n", i, write_data[i]);
    }

    ret_code = i2c_v1_write(chip, write_data, transfer_len);

    //dbg_print("[pmic6329_i2c_write] Done\n");
    
    return ret_code;
}

U32 pmic6329_int_ctrl_enable (U8 chip, U8 ctrl_type, U8 sel, kal_bool enable)
{
    U32 ret_code = PMIC_TEST_PASS;
    U32 delay_time_us = 50000;
    //U8 chip_slave_address = PMIC_SLAVE_ADDR;
	U8 chip_slave_address = chip;
    U8 cmd = 0x0;
    int cmd_len = 1;
    U8 data = 0x00;
    int data_len = 1;	
    U32 result_tmp;
    int i=0;

    cmd = ctrl_type;	
    if (enable){
        data |= (kal_uint8)sel;
    }else{
        data &= ~((kal_uint8)sel);
    }
    result_tmp = pmic6329_i2c_write(chip_slave_address, &cmd, cmd_len, &data, data_len);
    if(result_tmp != I2C_OK)
    {
        printf("[pmic6329_int_ctrl_enable] Reg[%x]=0x%x\n", cmd, data);
        printf("[pmic6329_int_ctrl_enable] -------------------------\n");
        return result_tmp;
    }

    /* check */
    result_tmp = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);
    if(result_tmp != I2C_OK)
    {
        printf("[pmic6329_int_ctrl_enable] Reg[%x]=0x%x\n", cmd, data);
        printf("[pmic6329_int_ctrl_enable] -------------------------\n");
        return result_tmp;
    }

    return result_tmp;
}

//==============================================================================
// PMIC6329 Usage APIs
//==============================================================================
U32 pmic6329_get_version (void)
{
    U32 ret_code = PMIC_TEST_PASS;
    U32 delay_time_us = 50000;
    U8 chip_slave_address = mt6329_BANK0_SLAVE_ADDR_WRITE;
    U8 cmd = 0x0;
    int cmd_len = 1;
    U8 data = 0xFF;
    int data_len = 1;	
    kal_uint16 eco_version = 0;
    U32 result_tmp;

#if 0
    gpt4_busy_wait_us(delay_time_us);
#endif

    /* Low part of CID */
    cmd = BANK0_CID0;	
    result_tmp = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);
    eco_version |= data;

    //dbg_print("[pmic6329_get_version] Reg(%x) = %x\n", cmd, data);

    /* High part of CID */
    cmd = BANK0_CID1;	
    result_tmp = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);	
    eco_version |= (data << 8);

    //dbg_print("[pmic6329_get_version] Reg(%x) = %x\n", cmd, data);

    if (eco_version == PMIC6329_E1_CID_CODE)
    {
        printf("[UBOOT_pmic6329_get_version] PMIC6329_E1_CID_CODE !!\n");
    }
	else if (eco_version == PMIC6329_E2_CID_CODE)
    {
        printf("[UBOOT_pmic6329_get_version] PMIC6329_E2_CID_CODE !!\n");
    } 
    else
    {
        printf("[UBOOT_pmic6329_get_version] UNKNOWN VERSION !!\n");
        ret_code = PMIC_TEST_FAIL;
    }

    return ret_code;
}

U32 get_pmic6329_version (void)
{
    U32 ret_code = PMIC_TEST_PASS;
    U32 delay_time_us = 50000;
    U8 chip_slave_address = mt6329_BANK0_SLAVE_ADDR_WRITE;
    U8 cmd = 0x0;
    int cmd_len = 1;
    U8 data = 0xFF;
    int data_len = 1;	
    kal_uint16 eco_version = 0;
    U32 result_tmp;

    /* Low part of CID */
    cmd = BANK0_CID0;	
    result_tmp = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);
    eco_version |= data;

    //dbg_print("[pmic6329_get_version] Reg(%x) = %x\n", cmd, data);

    /* High part of CID */
    cmd = BANK0_CID1;	
    result_tmp = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);	
    eco_version |= (data << 8);

    //dbg_print("[pmic6329_get_version] Reg(%x) = %x\n", cmd, data);

    return eco_version;
}

U32 pmic_IsUsbCableIn (void) 
{
    U32 ret_code = I2C_OK;
    U32 delay_time_us = 50000;
    U8 chip_slave_address = mt6329_BANK0_SLAVE_ADDR_WRITE;
    //U8 cmd = PMIC_STATUS_REG;
    U8 cmd = BANK0_CHRSTATUS;
    int cmd_len = 1;
    U8 data = 0x00;
    int data_len = 1;	
    
    //gpt4_busy_wait_us(delay_time_us);

    ret_code = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);
    //if (data & (CHRDET_MASK << CHRDET_SHIFT)){
    if (data & (BANK_0_PCHR_CHRDET_MASK << BANK_0_PCHR_CHRDET_SHIFT)){
        //printf("[PMIC_IsUsbCableIn] Get register %x : data = %x\n", cmd, data);
        printf("[UBOOT_PMIC_IsUsbCableIn] PMIC_CHRDET_EXIST\n");
        ret_code = PMIC_CHRDET_EXIST;
    }else{
        //printf("[PMIC_IsUsbCableIn] Get register %x : data = %x\n", cmd, data);
        printf("[UBOOT_PMIC_IsUsbCableIn] PMIC_CHRDET_NOT_EXIST\n");
        //ret_code = PMIC_CHRDET_NOT_EXIST;
    }

    return ret_code;
}

//==============================================================================
// PMIC6329 Init Code
//==============================================================================
void PMIC_DUMP_ALL_Register(void)
{
	kal_uint32 i=0;
	kal_uint32 ret=0;
	kal_uint8 reg_val=0;
	
	for (i=0;i<0xFF;i++)
	{
		ret=pmic_read_interface(i,&reg_val,0xFF,0);
        printf("[Bank0] Reg[0x%x]=0x%x\n", i, reg_val);        
    }
	
    for (i=0;i<0xFF;i++)
	{
		ret=pmic_bank1_read_interface(i,&reg_val,0xFF,0);
        printf("[Bank1] Reg[0x%x]=0x%x\n", i, reg_val);        
    }
}

kal_uint32 upmu_get_cid0(void)
{
	kal_uint32 ret=0;
	kal_uint32 temp_val=0;

	ret=pmic_read_interface(BANK0_CID0, &temp_val, BANK_0_CID0_MASK, BANK_0_CID0_SHIFT);

	return temp_val;
}

kal_uint32 upmu_get_cid1(void)
{
	kal_uint32 ret=0;
	kal_uint32 temp_val=0;

	ret=pmic_read_interface(BANK0_CID1, &temp_val, BANK_0_CID1_MASK, BANK_0_CID1_SHIFT);

	return temp_val;
}

void PMIC_INIT_SETTING_V1(void)
{
	kal_uint32 eco_version = 0;
    kal_uint32 tmp32;
	kal_uint32 ret=0;	
    
    /* Low part of CID */
    tmp32=upmu_get_cid0();
    eco_version |= tmp32;
    /* High part of CID */
    tmp32=upmu_get_cid1();
    eco_version |= (tmp32 << 8);

	if (eco_version == PMIC6329_E1_CID_CODE)
    {
        printf("[PMIC_INIT_SETTING_V1] PMIC6329_E1_CID_CODE !!\n");
		printf("[PMIC_INIT_SETTING_V1] Version : 2011/05/13 from CL\n");

		//<ECO_E1>
		//
		//bank0 0x3F[7:6] = 0x1
		ret=pmic_config_interface(0x3F,0x1,0x3,0x6);
		//
		//bank0 0x40[7:6] = 0x2
		ret=pmic_config_interface(0x40,0x2,0x3,0x6);
		//
		//bank0 0x43[5:4] = 0x3
		ret=pmic_config_interface(0x43,0x3,0x3,0x4);
		//
		//bank0 0x43[0:0] = 0x0
		ret=pmic_config_interface(0x43,0x0,0x1,0x0);
		//
		//bank0 0x52[5:4] = 0x3
		ret=pmic_config_interface(0x52,0x3,0x3,0x4);
		//
		//bank0 0x56[5:4] = 0x3
		ret=pmic_config_interface(0x56,0x3,0x3,0x4);
		//
		//bank0 0x56[0:0] = 0x0
		ret=pmic_config_interface(0x56,0x0,0x1,0x0);
		//
		//bank0 0x59[0:0] = 0x1
		ret=pmic_config_interface(0x59,0x1,0x1,0x0);
		//
		//bank0 0x6B[5:4] = 0x3
		ret=pmic_config_interface(0x6B,0x3,0x3,0x4);
		//
		//bank0 0x6B[0:0] = 0x0
		ret=pmic_config_interface(0x6B,0x0,0x1,0x0);
		//
		//bank0 0x70[5:4] = 0x2
		ret=pmic_config_interface(0x70,0x2,0x3,0x4);
		//
		//bank0 0x71[7:6] = 0x2
		ret=pmic_config_interface(0x71,0x2,0x3,0x6);
		//
		//bank0 0x74[5:4] = 0x3
		ret=pmic_config_interface(0x74,0x3,0x3,0x4);
		//Turn on GPIO hysteresis enable
		//bank1 0x4[5:5] = 0x1
		ret=pmic_bank1_config_interface(0x4,0x1,0x1,0x5);
		//</ECO_E1>
				
		printf("[PMIC_INIT_SETTING_V1] 20110823-Juinn-Ting update !!\n");
		//20110823-Juinn-Ting update
		ret=pmic_config_interface(0x44,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x45,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x46,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x47,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x48,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x49,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x4A,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x8A,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x8B,0x16,0xFF,0x0);
		
    }
	else if (eco_version == PMIC6329_E2_CID_CODE)
    {
        printf("[PMIC_INIT_SETTING_V1] PMIC6329_E2_CID_CODE !!\n");
		printf("[PMIC_INIT_SETTING_V1] Version : 2011/11/17 from CL\n");

		//<ECO_E2>
		//20111109
		//bank0 0xE[5:5] = 0x1
		ret=pmic_config_interface(0xE,0x1,0x1,0x5);
		//20111117
		//bank0 0x10[6:6] = 0x1
		ret=pmic_config_interface(0x10,0x1,0x1,0x6);
		//VPROC_soft change step time=4uS
		//bank0 0x15[6:4] = 0x4
		ret=pmic_config_interface(0x15,0x4,0x7,0x4);
		//20111109
		//bank0 0x17[7:7] = 0x1
		ret=pmic_config_interface(0x17,0x1,0x1,0x7);
		//20111109
		//bank0 0x17[6:6] = 0x1
		ret=pmic_config_interface(0x17,0x1,0x1,0x6);
		//
		//bank0 0x3F[7:6] = 0x1
		ret=pmic_config_interface(0x3F,0x1,0x3,0x6);
		//
		//bank0 0x40[7:6] = 0x2
		ret=pmic_config_interface(0x40,0x2,0x3,0x6);
		//
		//bank0 0x43[5:4] = 0x3
		ret=pmic_config_interface(0x43,0x3,0x3,0x4);
		//20111108 update
		//bank0 0x44[4:0] = 0x16
		ret=pmic_config_interface(0x44,0x16,0x1F,0x0);
		//VPROC_VOSEL_SRCLKEN=0.9V
		//bank0 0x45[4:0] = 0x8
		ret=pmic_config_interface(0x45,0x8,0x1F,0x0);
		//VPROC_DVS00=0.9V
		//bank0 0x47[4:0] = 0x8
		ret=pmic_config_interface(0x47,0x8,0x1F,0x0);
		//VPROC_DVS01=1.0V
		//bank0 0x48[4:0] = 0xC
		ret=pmic_config_interface(0x48,0xC,0x1F,0x0);
		//VPROC_DVS10=1.1V
		//bank0 0x49[4:0] = 0x10
		ret=pmic_config_interface(0x49,0x10,0x1F,0x0);
		//VPROC_DVS11=1.25V
		//bank0 0x4A[4:0] = 0x16
		ret=pmic_config_interface(0x4A,0x16,0x1F,0x0);
		//VPROC_soft change enable, control by DVS1/0@SRCLKEN=1
		//bank0 0x4B[2:2] = 0x1
		ret=pmic_config_interface(0x4B,0x1,0x1,0x2);
		//VPROC_soft change enable, control by DVS1/0@SRCLKEN=1
		//bank0 0x4B[1:0] = 0x2
		ret=pmic_config_interface(0x4B,0x2,0x3,0x0);
		//low_power_20111108
		//bank0 0x4F[7:7] = 0x1
		ret=pmic_config_interface(0x4F,0x1,0x1,0x7);
		//low_power_20111108
		//bank0 0x4F[6:6] = 0x1
		ret=pmic_config_interface(0x4F,0x1,0x1,0x6);
		//low_power_20111108
		//bank0 0x4F[5:5] = 0x1
		ret=pmic_config_interface(0x4F,0x1,0x1,0x5);
		//
		//bank0 0x4F[4:4] = 0x0
		ret=pmic_config_interface(0x4F,0x0,0x1,0x4);
		//low_power_20111108
		//bank0 0x4F[0:0] = 0x1
		ret=pmic_config_interface(0x4F,0x1,0x1,0x0);
		//
		//bank0 0x52[5:4] = 0x3
		ret=pmic_config_interface(0x52,0x3,0x3,0x4);
		//VCORE slow discharge
		//bank0 0x53[5:4] = 0x2
		ret=pmic_config_interface(0x53,0x2,0x3,0x4);
		//
		//bank0 0x56[5:4] = 0x3
		ret=pmic_config_interface(0x56,0x3,0x3,0x4);
		//VCORE_soft change, control by SRCLKEN  (XXXX 0x59->0x57->0x58)
		//bank0 0x59[2:2] = 0x1
		ret=pmic_config_interface(0x59,0x1,0x1,0x2);
		//
		//bank0 0x59[0:0] = 0x1
		ret=pmic_config_interface(0x59,0x1,0x1,0x0);
		//VPROC_DVS00=0.9V
		//bank0 0x57[4:0] = 0x8
		ret=pmic_config_interface(0x57,0x8,0x1F,0x0);
		//VCORE_VOSEL_CON1=1.1V
		//bank0 0x58[4:0] = 0x10
		ret=pmic_config_interface(0x58,0x10,0x1F,0x0);
		//20111110 force PWM mode
		//bank0 0x5F[0:0] = 0x1
		ret=pmic_config_interface(0x5F,0x1,0x1,0x0);
		//20111108 updated
		//bank0 0x67[5:4] = 0x2
		ret=pmic_config_interface(0x67,0x2,0x3,0x4);
		//20110608 updated
		//bank0 0x67[1:0] = 0x1
		ret=pmic_config_interface(0x67,0x1,0x3,0x0);
		//
		//bank0 0x6B[5:4] = 0x3
		ret=pmic_config_interface(0x6B,0x3,0x3,0x4);
		//20111109
		//bank0 0x71[7:6] = 0x3
		ret=pmic_config_interface(0x71,0x3,0x3,0x6);
		//20111109
		//bank0 0x76[4:0] = 0x6
		ret=pmic_config_interface(0x76,0x6,0x1F,0x0);
		//20111109
		//bank0 0x77[4:0] = 0x9
		ret=pmic_config_interface(0x77,0x9,0x1F,0x0);
		//20111109
		//bank0 0x78[4:0] = 0xB
		ret=pmic_config_interface(0x78,0xB,0x1F,0x0);
		//20111109
		//bank0 0x79[4:0] = 0xD
		ret=pmic_config_interface(0x79,0xD,0x1F,0x0);
		//20111109
		//bank0 0x7A[4:0] = 0xF
		ret=pmic_config_interface(0x7A,0xF,0x1F,0x0);
		//20111109
		//bank0 0x7B[4:0] = 0x11
		ret=pmic_config_interface(0x7B,0x11,0x1F,0x0);
		//20111109
		//bank0 0x7C[4:0] = 0x15
		ret=pmic_config_interface(0x7C,0x15,0x1F,0x0);
		//20111109
		//bank0 0x7D[4:0] = 0x19
		ret=pmic_config_interface(0x7D,0x19,0x1F,0x0);
		//20110525 VPA control setting
		//bank0 0x7E[0:0] = 0x1
		ret=pmic_config_interface(0x7E,0x1,0x1,0x0);
		//20111109
		//bank0 0xB6[3:3] = 0x1
		ret=pmic_config_interface(0xB6,0x1,0x1,0x3);
		//Turn on GPIO hysteresis enable
		//bank1 0x4[5:5] = 0x1
		ret=pmic_bank1_config_interface(0x4,0x1,0x1,0x5);
		//20111108_debeb
		//bank1 0x17[7:7] = 0x1
		ret=pmic_bank1_config_interface(0x17,0x1,0x1,0x7);
		//20111108_debeb
		//bank1 0x17[6:6] = 0x1
		ret=pmic_bank1_config_interface(0x17,0x1,0x1,0x6);
		//20111108_debeb
		//bank1 0x17[5:5] = 0x1
		ret=pmic_bank1_config_interface(0x17,0x1,0x1,0x5);
		//20111109
		//bank1 0x23[1:1] = 0x1
		ret=pmic_bank1_config_interface(0x23,0x1,0x1,0x1);
		//20111109
		//bank1 0x28[1:1] = 0x1
		ret=pmic_bank1_config_interface(0x28,0x1,0x1,0x1);
		//20111109
		//bank1 0x46[6:6] = 0x1
		ret=pmic_bank1_config_interface(0x46,0x1,0x1,0x6);
		//20111109
		//bank1 0x4C[6:6] = 0x1
		ret=pmic_bank1_config_interface(0x4C,0x1,0x1,0x6);
		//</ECO_E2> 		
 		
    }
    else
    {
    	printf("[PMIC_INIT_SETTING_V1] Unknown PMIC6329 CODE !!\n");
    }
}

void PMIC_CUSTOM_SETTING_V1(void)
{
	kal_uint32 ret=0;
	
	printf("[PMIC_CUSTOM_SETTING_V1] ...............\n");
	ret=pmic_config_interface(BANK0_DIGLDO_CON1C, 0x7, BANK_0_RG_VCAMD_VOSEL_MASK, BANK_0_RG_VCAMD_VOSEL_SHIFT);		
	ret=pmic_config_interface(BANK0_DIGLDO_CON1F, 0x2, BANK_0_RG_VCAM_IO_VOSEL_MASK, BANK_0_RG_VCAM_IO_VOSEL_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON22, 0x4, BANK_0_RG_VCAM_AF_VOSEL_MASK, BANK_0_RG_VCAM_AF_VOSEL_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON29, 0x7, BANK_0_RG_VMCH_VOSEL_MASK, BANK_0_RG_VMCH_VOSEL_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON2D, 0x2, BANK_0_RG_VGP_VOSEL_MASK, BANK_0_RG_VGP_VOSEL_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON30, 0x4, BANK_0_RG_VGP2_VOSEL_MASK, BANK_0_RG_VGP2_VOSEL_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON33, 0x4, BANK_0_RG_VIBR_VOSEL_MASK, BANK_0_RG_VIBR_VOSEL_SHIFT);
	
	ret=pmic_config_interface(BANK0_DIGLDO_CON30, 0x1, BANK_0_RG_VGP2_EN_MASK, BANK_0_RG_VGP2_EN_SHIFT);
	printf("[mtk-tpd] Turn on VGP2 2.8v \n");

	printf("[Disable All Power Good Detection] ...............\n");
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VCORE_PG_ENB_MASK, BANK_0_RG_VCORE_PG_ENB_SHIFT);	
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VPROC_PG_ENB_MASK, BANK_0_RG_VPROC_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VIO18_PG_ENB_MASK, BANK_0_RG_VIO18_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VIO28_PG_ENB_MASK, BANK_0_RG_VIO28_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VA1_PG_ENB_MASK, BANK_0_RG_VA1_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VM12_1_PG_ENB_MASK, BANK_0_RG_VM12_1_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VM12_2_PG_ENB_MASK, BANK_0_RG_VM12_2_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VM12_INT_PG_ENB_MASK, BANK_0_RG_VM12_INT_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN2, 0x1, BANK_0_RG_VTCXO_PG_ENB_MASK, BANK_0_RG_VTCXO_PG_ENB_SHIFT);	
}

U32 pmic6329_init (void)
{
    U32 ret_code = PMIC_TEST_PASS;
#if 1
	printf("\n[UBOOT_pmic6329_init] Start...................\n");

	//Put init setting here

#ifdef PMIC_SW_TEST_MODE_6326
    ret_code = pmic6329_int_ctrl_enable(mt6329_BANK0_SLAVE_ADDR_WRITE, PMIC_INIT_CTRL_1, INT1_EN_ALL, KAL_FALSE);
    if(ret_code != I2C_OK)
    {
        return ret_code;
    }
    ret_code = pmic6329_int_ctrl_enable(mt6329_BANK0_SLAVE_ADDR_WRITE, PMIC_INIT_CTRL_2, INT2_EN_ALL, KAL_TRUE);
    if(ret_code != I2C_OK)
    {
        return ret_code;
    }
    ret_code = pmic6329_int_ctrl_enable(mt6329_BANK0_SLAVE_ADDR_WRITE, PMIC_INIT_CTRL_3, INT3_EN_ALL, KAL_FALSE);
    if(ret_code != I2C_OK)
    {
        return ret_code;
    }
    ret_code = pmic6329_int_ctrl_enable(mt6329_BANK0_SLAVE_ADDR_WRITE, PMIC_INIT_CTRL_3, INT_EN_CHRDET, KAL_TRUE);
    if(ret_code != I2C_OK)
    {
        return ret_code;
    }
#endif	

	pmic6329_get_version();
	pmic_IsUsbCableIn();
	upmu_strup_usbdl_en(0);
	upmu_chr_chrind_on(0);
	printf("[UBOOT_PMIC_INIT] Turn Off chrind\n");

	PMIC_INIT_SETTING_V1();
	printf("[UBOOT_PMIC_INIT_SETTING_V1] Done\n");	
	PMIC_CUSTOM_SETTING_V1();
	printf("[UBOOT_PMIC_CUSTOM_SETTING_V1] Done\n");

	mt6577_detect_homekey();
	
	//PMIC_DUMP_ALL_Register();
#endif
	return ret_code;
}

int mt6577_detect_powerkey(void)
{
	U32 ret_code = I2C_OK;
	U8 chip_slave_address = mt6329_BANK0_SLAVE_ADDR_WRITE;
    U8 cmd = 0x09;
    int cmd_len = 1;
    U8 data = 0xFF;
    int data_len = 1;

	ret_code = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);

	if (data & (BANK_0_PWRKEY_DEB_MASK << BANK_0_PWRKEY_DEB_SHIFT)){     
        printf("[mt6577_detect_powerkey] Release\n");
        //return 1;
        return 0;
    }else{
        printf("[mt6577_detect_powerkey] Press\n");
        //return 0;
        return 1;
    }
}

int mt6577_detect_homekey(void)
{
	U32 ret_code = I2C_OK;
	U8 chip_slave_address = mt6329_BANK0_SLAVE_ADDR_WRITE;
    U8 cmd = 0x0C;
    int cmd_len = 1;
    U8 data = 0xFF;
    int data_len = 1;

	ret_code = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);

	if (data & (BANK_0_HOMEKEY_DEB_MASK << BANK_0_HOMEKEY_DEB_SHIFT)){     
        printf("[mt6577_detect_homekey] Release\n");
        return 0;
    }else{
        printf("[mt6577_detect_homekey] Press\n");
        return 1;
    }
}

//==============================================================================
// PMIC6329 Access Interface
//==============================================================================
kal_uint32 pmic_read_interface (kal_uint8 RegNum, kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT)
{
    U8 chip_slave_address = 0xC0;
    U8 cmd = 0x0;
    int cmd_len = 1;
    U8 data = 0xFF;
    int data_len = 1;	
    U32 result_tmp;

    cmd = RegNum;	
    result_tmp = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);

    data &= (MASK << SHIFT);
    *val = (data >> SHIFT);

	//printf("[pmic_read_interface] Reg[0x%x]=0x%x, val=%d\n", RegNum, data, *val);

	return 0;
}

kal_uint32 pmic_config_interface (kal_uint8 RegNum, kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT)
{
    U8 chip_slave_address = 0xC0;
    U8 cmd = 0x0;
    int cmd_len = 1;
    U8 data = 0xFF;
    int data_len = 1;	
    U32 result_tmp;

    cmd = RegNum;	
    result_tmp = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);

    data &= ~(MASK << SHIFT);
    data |= (val << SHIFT);

    result_tmp = pmic6329_i2c_write(chip_slave_address, &cmd, cmd_len, &data, data_len);

    //check 
    //result_tmp = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);
	
	//printf("[pmic_config_interface] Reg[0x%x]=0x%x\n", RegNum, data);

	return 0;
}

kal_uint32 pmic_bank1_read_interface (kal_uint8 RegNum, kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT)
{
    U8 chip_slave_address = 0xC2;
    U8 cmd = 0x0;
    int cmd_len = 1;
    U8 data = 0xFF;
    int data_len = 1;	
    U32 result_tmp;

    cmd = RegNum;	
    result_tmp = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);

    data &= (MASK << SHIFT);
    *val = (data >> SHIFT);

	//printf("[pmic_bank1_read_interface] Reg[0x%x]=0x%x, val=%d\n", RegNum, data, *val);

	return 0;
}

kal_uint32 pmic_bank1_config_interface (kal_uint8 RegNum, kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT)
{
    U8 chip_slave_address = 0xC2;
    U8 cmd = 0x0;
    int cmd_len = 1;
    U8 data = 0xFF;
    int data_len = 1;	
    U32 result_tmp;

    cmd = RegNum;	
    result_tmp = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);

    data &= ~(MASK << SHIFT);
    data |= (val << SHIFT);

    result_tmp = pmic6329_i2c_write(chip_slave_address, &cmd, cmd_len, &data, data_len);

    //check 
    //result_tmp = pmic6329_i2c_read(chip_slave_address, &cmd, cmd_len, &data, data_len);

	//printf("[pmic_bank1_config_interface] Reg[0x%x]=0x%x\n", RegNum, data);

	return 0;
}

//==============================================================================
// PMIC6329 API 
//==============================================================================

//////////////////////////////////////////////////////////////////////////////////////////
// PMIC-Charger Type Detection
//////////////////////////////////////////////////////////////////////////////////////////
CHARGER_TYPE g_ret = CHARGER_UNKNOWN;
int g_charger_in_flag = 0;
int g_first_check=0;

CHARGER_TYPE hw_charger_type_detection(void)
{
	CHARGER_TYPE ret 				= CHARGER_UNKNOWN;
	unsigned int USB_U2PHYACR3_2 	= 0xC126081C;
	unsigned int USBPHYRegs			= 0xC1260800; //U2B20_Base+0x800
	unsigned short wChargerAvail	= 0;
	unsigned short bLineState_B		= 0;
	unsigned short bLineState_C 	= 0;
	int ret_val						= 0;
	unsigned short testReadValue	= 0;

	//msleep(400);
	//printf("mt_charger_type_detection : start!\r\n");

/********* Step 0.0 : enable USB memory and clock *********/
	//hwEnableClock(MT65XX_PDN_PERI_USB1, "pmu");
	//hwEnableClock(MT65XX_PDN_PERI_USB2, "pmu");
	//mdelay(1);

/********* Step 1.0 : PMU_BC11_Detect_Init ***************/		
	SETREG8(USB_U2PHYACR3_2,0x80); //USB_U2PHYACR3[7]=1		
	
	//BC11_RST=1
	ret_val=pmic_config_interface(0x33,0x1,BANK_0_BC11_RST_MASK,BANK_0_BC11_RST_SHIFT); 
	//BC11_RST=0	
	//ret_val=pmic_config_interface(0x33,0x0,BANK_0_BC11_RST_MASK,BANK_0_BC11_RST_SHIFT); 
	//BC11_BB_CTRL=1
	ret_val=pmic_config_interface(0x33,0x1,BANK_0_BC11_BB_CTRL_MASK,BANK_0_BC11_BB_CTRL_SHIFT);
	
	//RG_BC11_BIAS_EN=1	
	ret_val=pmic_config_interface(0x34,0x1,BANK_0_BC11_BIAS_EN_MASK,BANK_0_BC11_BIAS_EN_SHIFT); 
	//RG_BC11_VSRC_EN[1:0]=00
	ret_val=pmic_config_interface(0x33,0x0,BANK_0_RG_BC11_VSRC_EN_MASK,BANK_0_RG_BC11_VSRC_EN_SHIFT); 
	//RG_BC11_VREF_VTH = 0
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_VREF_VTH_MASK,BANK_0_BC11_VREF_VTH_SHIFT); 
	//RG_BC11_CMP_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_CMP_EN_MASK,BANK_0_BC11_CMP_EN_SHIFT);
	//RG_BC11_IPU_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);
	//RG_BC11_IPD_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPD_EN_MASK,BANK_0_BC11_IPD_EN_SHIFT);

	//ret_val=pmic_read_interface(0x33,&testReadValue,0xFF,0);		
	//printf("Reg[0x33]=%x, ", testReadValue);
	//ret_val=pmic_read_interface(0x34,&testReadValue,0xFF,0);		
	//printf("Reg[0x34]=%x \r\n", testReadValue);

/********* Step A *************************************/
	//printf("mt_charger_type_detection : step A\r\n");
	//RG_BC11_IPU_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);
	
	SETREG8(USBPHYRegs+0x1C,0x1000);//RG_PUPD_BIST_EN = 1	
	CLRREG8(USBPHYRegs+0x1C,0x0400);//RG_EN_PD_DM=0
	
	//RG_BC11_VSRC_EN[1.0] = 10 
	ret_val=pmic_config_interface(0x33,0x2,BANK_0_RG_BC11_VSRC_EN_MASK,BANK_0_RG_BC11_VSRC_EN_SHIFT); 
	//RG_BC11_IPD_EN[1:0] = 01
	ret_val=pmic_config_interface(0x34,0x1,BANK_0_BC11_IPD_EN_MASK,BANK_0_BC11_IPD_EN_SHIFT);
	//RG_BC11_VREF_VTH = 0
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_VREF_VTH_MASK,BANK_0_BC11_VREF_VTH_SHIFT);
  	//RG_BC11_CMP_EN[1.0] = 01
  	ret_val=pmic_config_interface(0x34,0x1,BANK_0_BC11_CMP_EN_MASK,BANK_0_BC11_CMP_EN_SHIFT);
	//mdelay(400);
	mdelay(100);
		
	ret_val=pmic_read_interface(0x33,&wChargerAvail,BANK_0_BC11_CMP_OUT_MASK,BANK_0_BC11_CMP_OUT_SHIFT); 
	//printf("mt_charger_type_detection : step A : wChargerAvail=%x\r\n", wChargerAvail);
	
	//RG_BC11_VSRC_EN[1:0]=00
	ret_val=pmic_config_interface(0x33,0x0,BANK_0_RG_BC11_VSRC_EN_MASK,BANK_0_RG_BC11_VSRC_EN_SHIFT); 
	//RG_BC11_IPD_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPD_EN_MASK,BANK_0_BC11_IPD_EN_SHIFT);
	//RG_BC11_CMP_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_CMP_EN_MASK,BANK_0_BC11_CMP_EN_SHIFT);
	mdelay(50);
	
	if(wChargerAvail==1)
	{
/********* Step B *************************************/
		//printk("mt_charger_type_detection : step B\r\n");

		//RG_BC11_IPU_EN[1:0]=10
		ret_val=pmic_config_interface(0x34,0x2,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);		
		mdelay(80);
		
		bLineState_B = INREG8(USBPHYRegs+0x76);
		//printk("mt_charger_type_detection : step B : bLineState_B=%x\r\n", bLineState_B);
		if(bLineState_B & 0x80)
		{
			ret = STANDARD_CHARGER;
			printf("mt_charger_type_detection : step B : STANDARD CHARGER!\r\n");
		}
		else
		{
			ret = CHARGING_HOST;
			printf("mt_charger_type_detection : step B : Charging Host!\r\n");
		}
	}
	else
	{
/********* Step C *************************************/
		//printk("mt_charger_type_detection : step C\r\n");

		//RG_BC11_IPU_EN[1:0]=01
		ret_val=pmic_config_interface(0x34,0x1,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);
		//RG_BC11_CMP_EN[1.0] = 01
		ret_val=pmic_config_interface(0x34,0x1,BANK_0_BC11_CMP_EN_MASK,BANK_0_BC11_CMP_EN_SHIFT);
		//ret_val=pmic_read_interface(0x34,&testReadValue,0xFF,0);			
		//printf("mt_charger_type_detection : step C : Reg[0x34]=%x\r\n", testReadValue);		
		mdelay(80);
		
		ret_val=pmic_read_interface(0x33,&bLineState_C,0xFF,0);
		//printf("mt_charger_type_detection : step C : bLineState_C=%x\r\n", bLineState_C);
		if(bLineState_C & 0x80)
		{
			ret = NONSTANDARD_CHARGER;
			printf("mt_charger_type_detection : step C : UNSTANDARD CHARGER!!\r\n");
			//RG_BC11_IPU_EN[1:0]=10
			ret_val=pmic_config_interface(0x34,0x2,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);			
			mdelay(80);
		}
		else
		{
			ret = STANDARD_HOST;
			printf("mt_charger_type_detection : step C : Standard USB Host!!\r\n");
		}
	}
/********* Finally setting *******************************/
	//RG_BC11_VSRC_EN[1:0]=00
	ret_val=pmic_config_interface(0x33,0x0,BANK_0_RG_BC11_VSRC_EN_MASK,BANK_0_RG_BC11_VSRC_EN_SHIFT); 
	//RG_BC11_VREF_VTH = 0
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_VREF_VTH_MASK,BANK_0_BC11_VREF_VTH_SHIFT);
	//RG_BC11_CMP_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_CMP_EN_MASK,BANK_0_BC11_CMP_EN_SHIFT);
	//RG_BC11_IPU_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);
	//RG_BC11_IPD_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPD_EN_MASK,BANK_0_BC11_IPD_EN_SHIFT);
	//RG_BC11_BIAS_EN=0
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_BIAS_EN_MASK,BANK_0_BC11_BIAS_EN_SHIFT); 
	
	CLRREG8(USB_U2PHYACR3_2,0x80); //USB_U2PHYACR3[7]=0

	//hwDisableClock(MT65XX_PDN_PERI_USB1, "pmu");
	//hwDisableClock(MT65XX_PDN_PERI_USB2, "pmu");

	//step4:done, ret the type	
	return ret;
	
}

CHARGER_TYPE mt_charger_type_detection(void)
{
    if( g_first_check == 0 )
    {
		g_first_check = 1;
		g_ret = hw_charger_type_detection();
    }
    else
    {
		printf("[mt_charger_type_detection] Got data !!, %d, %d\r\n", g_charger_in_flag, g_first_check);
    }

    return g_ret;
}

//==============================================================================
// LDO ON/OFF Status APIs
//==============================================================================
//CON - 0
kal_uint32 upmu_status_vcore(void)
{
	kal_uint32 ret=0;
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

	ret=pmic_config_interface( 	(kal_uint8)(BANK0_STARTUP + STARTUP_CON5_OFFSET), 
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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
void upmu_spk_ab_oc_l_deg(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_AB_OC_L_DEG_MASK),
										(kal_uint8)(BANK_1_SPK_AB_OC_L_DEG_SHIFT)
										);
}

void upmu_spk_d_oc_l_deg(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON2_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_D_OC_L_DEG_MASK),
										(kal_uint8)(BANK_1_SPK_D_OC_L_DEG_SHIFT)
										);
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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
void upmu_spk_ab_oc_r_deg(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON8_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_AB_OC_R_DEG_MASK),
										(kal_uint8)(BANK_1_SPK_AB_OC_R_DEG_SHIFT)
										);
}

void upmu_spk_d_oc_r_deg(kal_uint32 val)
{
	kal_uint32 ret=0;	

	ret=pmic_bank1_config_interface( 	(kal_uint8)(BANK1_SPK_CTRL + SPK_CON8_OFFSET), 
										(kal_uint8)(val),
										(kal_uint8)(BANK_1_SPK_D_OC_R_DEG_MASK),
										(kal_uint8)(BANK_1_SPK_D_OC_R_DEG_SHIFT)
										);
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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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
	kal_uint32 val=0;

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

//==============================================================================
// PMIC6329 API for read value
//==============================================================================
kal_uint32 upmu_get_reg_value(kal_uint32 reg)
{
	kal_uint32 ret=0;
	kal_uint32 temp_val=0;

	ret=pmic_read_interface(reg, &temp_val, 0xFF, 0x0);

	return temp_val;
}

//==============================================================================
// PMIC6329 API for UBOOT : AUXADC
//==============================================================================
int Enable_PMIC_LOG = 1;
extern int g_R_BAT_SENSE;
extern int g_R_I_SENSE;
extern int g_R_CHARGER_1;
extern int g_R_CHARGER_2;

int PMIC_IMM_GetOneChannelValue(int dwChannel, int deCount)
{
    kal_int32 u4Sample_times = 0;
	kal_int32 u4channel[4] = {0,0,0,0};
	kal_int32 adc_result=0;
	kal_int32 adc_result_temp=0;
	kal_int32 r_val_temp=0;
	
	kal_uint32 count=0;
	kal_uint32 count_time_out=1000;
	kal_uint32 ret_data=0;
	kal_uint32 temp_data_7_0=0;
	kal_uint32 temp_data_9_8=0;
	kal_uint32 i=0;	

	kal_int32 data_55_48 = 0;
	kal_int32 data_63_56 = 0;
	kal_int32 otp_gain_trim_data = 0;
	kal_int32 otp_offset_trim_data = 0;	

	//printf("[IMM_GetOneChannelValue_PMIC] start!\r\n");

	upmu_auxadc_ch_sel(dwChannel);

	data_55_48=upmu_otpc_otp_pdo_55_48();
	data_63_56=upmu_otpc_otp_pdo_63_56();
	//if (Enable_PMIC_LOG == 1) {
	//	printf("[PMIC_ADC] data_55_48=0x%x, data_63_56=0x%x \r\n", 
	//		data_55_48, data_63_56);
	//}
	otp_gain_trim_data = (data_55_48 & 0x7F); //[54:48]
	if(otp_gain_trim_data > 64)
	{
		otp_gain_trim_data = otp_gain_trim_data - 128;
	}
	data_55_48 = (data_55_48 & 0x80) >> 7;
	data_63_56 = (data_63_56 & 0x1F) << 1;
	otp_offset_trim_data = data_63_56 | data_55_48; //[60:55]
	if(otp_offset_trim_data > 32)
	{
		otp_offset_trim_data = otp_offset_trim_data - 64;
	}
	//if (Enable_PMIC_LOG == 1) {
	//	printf("[PMIC_ADC] otp_gain_trim_data=%d, otp_offset_trim_data=%d \r\n", 
	//		otp_gain_trim_data, otp_offset_trim_data);
	//}

	if( dwChannel == AUXADC_BATTERY_VOLTAGE_CHANNEL )	
	{
		u4Sample_times=0;
		if( PMIC6329_E1_CID_CODE == get_pmic6329_version() )
		{
		upmu_chr_adcin_vbat_en(1);
		}
		upmu_auxadc_spl_num(0x1);
		do
	    {
	    	upmu_auxadc_start(0);
			upmu_auxadc_start(1);
	
	    	//reset parameter
	    	ret_data=0;
			temp_data_7_0=0;
			temp_data_9_8=0;
			
			count=0;
	        while( upmu_auxadc_get_ch0_ready() != 1 )
			{				
				if( (count++) > count_time_out)
				{
					printf("[IMM_GetOneChannelValue_PMIC_0] Time out!\r\n");
					break;
				}
			}	
			//temp_data_7_0=upmu_auxadc_get_ch0_data_7_0();
			temp_data_7_0=upmu_auxadc_get_trimming_data_7_0();
			ret_data = temp_data_7_0;
			//temp_data_9_8=upmu_auxadc_get_ch0_data_9_8();
			temp_data_9_8=upmu_auxadc_get_trimming_data_9_8();
			ret_data |= (temp_data_9_8 << 8);			
			//printf("[CH0] ret_data=%d (9_8=%x,7_0=%x)\r\n", ret_data, temp_data_9_8, temp_data_7_0);
			u4channel[0] += ret_data; 
	                
			//if (Enable_PMIC_LOG == 1) {
			//	printf("[IMM_GetOneChannelValue_PMIC_0] %d\r\n", ret_data);				
			//}
						
			//if (Enable_PMIC_LOG == 1) {				
				//HW debug
			//	ret_data=0;
			//	temp_data_7_0=0;
			//	temp_data_9_8=0;
			//	temp_data_7_0=upmu_auxadc_get_ch0_data_7_0();
			//	ret_data = temp_data_7_0;
			//	temp_data_9_8=upmu_auxadc_get_trimming_data_9_8();
			//	ret_data |= (temp_data_9_8 << 8);
			//	printf("[IMM_GetOneChannelValue_PMIC_0] not trim=%d\r\n", ret_data);				
			//}
					
	        u4Sample_times++;
	    }
	    while (u4Sample_times < deCount);

		/* Value averaging  */ 
    	u4channel[0]  = u4channel[0]  / deCount;
	
	}
	else if( dwChannel == AUXADC_REF_CURRENT_CHANNEL )	
	{
		u4Sample_times=0;
		if( PMIC6329_E1_CID_CODE == get_pmic6329_version() )
		{
		upmu_chr_adcin_vsen_en(1);
		}
		upmu_auxadc_spl_num(0x1);
		do
	    {
	    	upmu_auxadc_start(0);
			upmu_auxadc_start(1);
			
	    	//reset parameter
	    	ret_data=0;
			temp_data_7_0=0;
			temp_data_9_8=0;
			
			count=0;
	        while( upmu_auxadc_get_ch1_ready() != 1 )
			{	
				if( (count++) > count_time_out)
				{
					printf("[IMM_GetOneChannelValue_PMIC_1] Time out!\r\n");
					break;
				}
			}	
			temp_data_7_0=upmu_auxadc_get_ch1_data_7_0();
			ret_data = temp_data_7_0;
			temp_data_9_8=upmu_auxadc_get_ch1_data_9_8();
			ret_data |= (temp_data_9_8 << 8);
			u4channel[1] += ret_data; 

			//if (Enable_PMIC_LOG == 1) {		
			//	printf("[IMM_GetOneChannelValue_PMIC_1] %d\r\n", ret_data);
			//}
	                
	        u4Sample_times++;
	    }
	    while (u4Sample_times < deCount);

		/* Value averaging  */ 
    	u4channel[1]  = u4channel[1]  / deCount;
		
	}
	else if( dwChannel == AUXADC_CHARGER_VOLTAGE_CHANNEL )
	{
		u4Sample_times=0;
		upmu_chr_adcin_vchr_en(1); // for more precise - start		
		upmu_auxadc_spl_num(0xF);
		do
	    {
	    	upmu_auxadc_start(0);
			upmu_auxadc_start(1);
	    
	    	//reset parameter
	    	ret_data=0;
			temp_data_7_0=0;
			temp_data_9_8=0;
			
			count=0;
	        while( upmu_auxadc_get_ch2_ready() != 1 )
			{	
				if( (count++) > count_time_out)
				{
					printf("[IMM_GetOneChannelValue_PMIC_2] Time out !\r\n");
					break;
				}
			}			
			temp_data_7_0=upmu_auxadc_get_ch2_data_7_0();
			ret_data = temp_data_7_0;
			temp_data_9_8=upmu_auxadc_get_ch2_data_9_8();
			ret_data |= (temp_data_9_8 << 8);			
			if(u4Sample_times > 0) //skip the first value
			{
				u4channel[2] += ret_data; 
				//if (Enable_PMIC_LOG == 1) {
				//	printf("[IMM_GetOneChannelValue_PMIC_2] %d\r\n", ret_data);
				//}
			}
					
	        u4Sample_times++;
	    }
	    while (u4Sample_times < deCount);

		/* Value averaging  */ 
    	u4channel[2]  = u4channel[2]  / (deCount-1);

		upmu_chr_adcin_vchr_en(0); // for more precise - end
		upmu_auxadc_spl_num(0x1);
		
	}
	else if( dwChannel == AUXADC_TEMPERATURE_CHANNEL )
	{
		upmu_auxadc_spl_num(0xF);
		
		u4Sample_times=0;
		if( PMIC6329_E1_CID_CODE == get_pmic6329_version() )
		{
		upmu_chr_baton_tdet_en(1);
		}		
		//Workaround-----------------
		//upmu_auxadc_start(0);
		//upmu_auxadc_start(1);
		//upmu_chr_baton_tdet_en(1);
		//---------------------------
		//printf("[BATTERY] Reg[0xe8]=0x%x\r\n", upmu_get_reg_value(0xe8) );		
		//printf("[BATTERY] Reg[0xe9]=0x%x\r\n", upmu_get_reg_value(0xe9) );
		//printf("[BATTERY] Reg[0x38]=0x%x\r\n", upmu_get_reg_value(0x38) );
		do
	    {   
	    	upmu_auxadc_start(0);
			upmu_auxadc_start(1);
			
	    	//reset parameter
	    	ret_data=0;
			temp_data_7_0=0;
			temp_data_9_8=0;
			
			count=0;
	        while( upmu_auxadc_get_ch3_ready() != 1 )
			{	
				if( (count++) > count_time_out)
				{
					printf("[IMM_GetOneChannelValue_PMIC_3] Time out!\r\n");
					break;
				}
			}	
			temp_data_7_0=upmu_auxadc_get_ch3_data_7_0();
			ret_data = temp_data_7_0;
			temp_data_9_8=upmu_auxadc_get_ch3_data_9_8();
			ret_data |= (temp_data_9_8 << 8);
			//printf("[CH3] ret_data=%d (9_8=%x,7_0=%x)\r\n", ret_data, temp_data_9_8, temp_data_7_0);
			u4channel[3] += ret_data; 

			//if (Enable_PMIC_LOG == 1) {		
			//	printf("[IMM_GetOneChannelValue_PMIC_3] %d\r\n", ret_data);
			//}
	                
	        u4Sample_times++;
	    }
	    while (u4Sample_times < deCount);

		/* Value averaging  */ 
    	u4channel[3]  = u4channel[3]  / deCount;

		upmu_auxadc_spl_num(0x1);
		
	}
	else 
	{
		printf("[PMIC_ADC] Error dwChannel number 1 (%d)!\r\n", dwChannel);
	}    

	if( dwChannel == AUXADC_BATTERY_VOLTAGE_CHANNEL )	
	{
		adc_result_temp = u4channel[dwChannel];
		r_val_temp = g_R_BAT_SENSE;
		adc_result = (adc_result_temp*r_val_temp*VOLTAGE_FULL_RANGE)/ADC_PRECISE;
	}
	else if( dwChannel == AUXADC_REF_CURRENT_CHANNEL )	
	{
		adc_result_temp = u4channel[dwChannel] + ((u4channel[dwChannel]*otp_gain_trim_data)/1024) + otp_offset_trim_data;
		r_val_temp = g_R_I_SENSE;
		adc_result = (adc_result_temp*r_val_temp*VOLTAGE_FULL_RANGE)/ADC_PRECISE;
	}
	else if( dwChannel == AUXADC_CHARGER_VOLTAGE_CHANNEL )
	{
		adc_result_temp = u4channel[dwChannel] + ((u4channel[dwChannel]*otp_gain_trim_data)/1024) + otp_offset_trim_data;
		r_val_temp = (((g_R_CHARGER_1+g_R_CHARGER_2)*100)/g_R_CHARGER_2);
		adc_result = (adc_result_temp*r_val_temp*VOLTAGE_FULL_RANGE)/ADC_PRECISE;		
	}
	else if( dwChannel == AUXADC_TEMPERATURE_CHANNEL )
	{
		adc_result_temp = u4channel[dwChannel] + ((u4channel[dwChannel]*otp_gain_trim_data)/1024) + otp_offset_trim_data;
		r_val_temp = 1;
		adc_result = (adc_result_temp*r_val_temp*VOLTAGE_FULL_RANGE)/ADC_PRECISE;
	}
	else 
	{
		printf("[PMIC_ADC] Error dwChannel number 2 (%d)!\r\n", dwChannel);
	} 

	//if (Enable_PMIC_LOG == 1) {		
	//	printf("[PMIC_ADC] adc_result_temp=%d, adc_result=%d, r_val_temp=%d\r\n", 
	//		adc_result_temp, adc_result, r_val_temp);
	//}

	//printf("[PMIC_ADC] Bank0[0x31]=0x%x, Bank0[0x38]=0x%x\r\n", 
	//	upmu_get_reg_value(0x31), upmu_get_reg_value(0x38));
	
	upmu_auxadc_start(0);
	count=0;
	
	return adc_result;
	
}

//==============================================================================
// PMIC6329 API for UBOOT : Get CHRDET status
//==============================================================================
kal_uint32 upmu_get_PCHR_CHRDET(void)
{
	kal_uint32 ret=0;
	kal_uint32 temp_val=0;

	ret=pmic_read_interface(BANK0_CHRSTATUS, &temp_val, BANK_0_PCHR_CHRDET_MASK, BANK_0_PCHR_CHRDET_SHIFT);

	return temp_val;
}

kal_bool upmu_is_chr_det(void)
{
	kal_uint32 tmp32;
	tmp32=upmu_get_PCHR_CHRDET();
	if(tmp32 == 0)
	{
		//printk("[upmu_is_chr_det] No charger\n");
		return KAL_FALSE;
	}
	else
	{
		//printk("[upmu_is_chr_det] Charger exist\n");
		return KAL_TRUE;
	}
}
