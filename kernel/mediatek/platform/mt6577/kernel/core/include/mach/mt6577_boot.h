 /* mediatek/platform/mt6577/kernel/core/include/mach
  *
  *
  * Copyright (C) 2008,2009 MediaTek <www.mediatek.com>
  * Authors: Infinity Chen <infinity.chen@mediatek.com>  
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  */

#ifndef _MT6577_BOOT_H_
#define _MT6577_BOOT_H_

/* boot type definitions */
typedef enum 
{
    NORMAL_BOOT = 0,
    META_BOOT = 1,
    RECOVERY_BOOT = 2,    
    SW_REBOOT = 3,
    FACTORY_BOOT = 4,
    ADVMETA_BOOT = 5,
    ATE_FACTORY_BOOT = 6,
    ALARM_BOOT = 7,
    UNKNOWN_BOOT
} BOOTMODE;

/*META COM port type*/
 typedef enum
{
    META_UNKNOWN_COM = 0,
    META_UART_COM,
    META_USB_COM
} META_COM_TYPE;


#define BOOT_DEV_NAME           "BOOT"
#define BOOT_SYSFS              "boot"
#define BOOT_SYSFS_ATTR         "boot_mode"
#define MD_SYSFS_ATTR           "md"
#define INFO_SYSFS_ATTR         "info"

/* chip definitions */
typedef enum 
{
    CHIP_E1 = 0xca00,
    CHIP_E2 = 0xcb00,
    CHIP_E3 = 0xcc00, 
    CHIP_6575_E1 = CHIP_E1,
    CHIP_6575_E2 = CHIP_E2,
    CHIP_6575_E3 = CHIP_E3,
    CHIP_6577_E1 = 0xcd00,
} CHIP_VER;

/* md sw definitions */
typedef enum 
{
    MD_SW_V1 = 0xe100,
    MD_SW_V2 = 0xe201,
    MD_SW_V3 = 0xe201,    
} MD_SW_VER;



extern BOOTMODE g_boot_mode;
extern META_COM_TYPE g_meta_com_type;
extern unsigned int g_meta_com_id;

extern BOOTMODE get_boot_mode(void);
extern bool is_meta_mode(void);
extern bool is_advanced_meta_mode(void);

extern void boot_register_md_func(ssize_t (*show)(char*), ssize_t (*store)(const char*,size_t));

extern unsigned int get_chip_code(void);
extern unsigned int get_chip_sw_ver(void);
extern unsigned int get_chip_hw_ver(void);
extern unsigned int get_chip_hw_subcode(void);
extern CHIP_VER get_chip_eco_ver(void);
extern CHIP_VER get_chip_ver(void);


#endif 

