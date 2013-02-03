#
#  Copyright Statement:
#  ---------------------------
#  This software/firmware and related documentation (¡§MediaTek Software¡¨) are
#  protected under relevant copyright laws. The information contained herein
#  is confidential and proprietary to MediaTek Inc. and/or its licensors.
#  Without the prior written permission of MediaTek inc. and/or its licensors,
#  any reproduction,modification, use or disclosure of MediaTek Software, and
#  information contained herein, in whole or in part, shall be strictly prohibited.
#
#  MediaTek Inc.(C)2011.All rights reserved.
#
#  BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
#  AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK
#  SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED
#  TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
#  WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
#  NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER
#  WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
#  INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER
#  AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING
#  THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER¡¦S SOLE
#  RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
#  CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR
#  ANY MEDIATEK SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO
#  CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND
#  EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT
#  TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,AT MEDIATEK'S OPTION,
#  TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,OR REFUND ANY SOFTWARE
#  LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK
#  SOFTWARE AT ISSUE.
#
# *************************************************************************

# Generated at Fri Apr 13 18:10:25 2012

# ------------------------------ ARM

RVCT_VERSION = V31
  # Description:
  #   ARM RVCT Version
  # Option Values:
  #   V31: ARM RVCT Version 3.1
  #   V22: ARM RVCT Version 2.2
  # Switch-ability:
  #   [Any] -> [Any]

RVCT_PARTIAL_LINK = FALSE
  # Description:
  #   True:turn on partial link; False:turn off partial link
  # Option Values:
  #   TRUE: In linking process, try normal link firstly, if OOM, try partiallink next
  #   FALSE: In linking process, normal link only
  # Switch-ability:
  #   [Any] -> [Any]

CUSTOM_CFLAGS = #--debug --no_debug_macros
  # Description:
  #   Add custom cflag
  # Option Values:
  #   --debug --no_debug_macros:
  # Switch-ability:
  #   Non-switchable

FEATURE_OVERLOAD = FALSE
  # Description:
  #   Enabling this option will allow a project configured with more features than its physical constraints. This option just bypass some building error checkings and generate the .bin. However, the .bin is still can NOT work in target if the physical constrain
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   TRUE -> FALSE

# ------------------------------ HW and Project
ACCDET_SUPPORT = FALSE
  # Description:
  #   This compile option is defined to indicate that out new chip support Accessory Detection module.
  # Option Values:
  #   HYBRID: accdet+eint SW Solution to detection
  #   TRUE: Support ACCDET HW Detection Solution;And on MT6276 and MT6251, it indicates the Accdet+Earbias Solution
  #   FALSE: Indicate the Old AuxADC detection solution
  #   DISABLE_AUX_ACCDET: no earphone detection on the project
  # Switch-ability:
  #   Non-switchable

BOARD_VER = TINNO77_ICS2_BB
  # Description:
  #   Name of the PCB or EVB.
  # Option Values:
  #   N/A
  # Switch-ability:
  #   Non-switchable

SUB_BOARD_VER = PCB01
  # Description:
  #   Sub-name of the PCB or EVB.
  # Option Values:
  #   N/A
  # Switch-ability:
  #   Non-switchable

CHIP_VERSION_CHECK = TRUE
  # Description:
  #   Enable/Disable HW ID check and supported in all projects with all current online project
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

RF_MODULE = MT6162_CUSTOM
  # Description:
  #   describe the RF module used in this project
  # Option Values:
  #   NA: the RF module used in this project
  # Switch-ability:
  #   Non-switchable

PHONE_TYPE = BAR
  # Description:
  #   Phone type is clamshell, bar or slide type.
  # Option Values:
  #   CLAMSHELL: Clamshell phone
  #   BAR: BAR type phone
  #   SLIDE: Slide type phone
  # Switch-ability:
  #   [Any] -> [Any]

VENDOR = NONE
  # Description:
  #   If not NONE, the project is a vendor project or is doing vendor release
  # Option Values:
  #   (other than NONE): This project is for the specified VENDOR
  #   NONE: Not a vendor project
  # Switch-ability:
  #   Non-switchable

# ------------------------------ Audio
# ------------------------------ HW and Project
# ------------------------------ Connectivity
# ------------------------------ HW and Project
# ------------------------------ Connectivity
# ------------------------------ HW and Project
# ------------------------------ LCD
LCD_MODULE = TINNO77_ICS2_LCM
  # Description:
  #   In order to select the folders of LCD module of specified project, please set the option as the folder name in custom folder.
  # Option Values:
  #   NA:
  # Switch-ability:
  #   Non-switchable

DISPLAY_TYPE = COLOR
  # Description:
  #   Support lcmmi to select BW/COLOR image
  # Option Values:
  #   COLOR:
  #   BW:
  # Switch-ability:
  #   Non-switchable

MAIN_LCD_SIZE = 240X320
  # Description:
  #   Add a MAIN_LCD_SIZE compile option in make file
  # Option Values:
  #   320x240: The main lcd size is 320X240
  #   240x320: The main lcd size is 240X320
  #   128X128: The main lcd size is 128X128
  #   96X64: The main lcd size is 96X96
  #   360X640: The main lcd size is 360X640
  #   128X160: The main lcd size is 128X160
  #   176X220: The main lcd size is 176X220
  #   240x400: The main lcd size is 240X400
  #   320x480: The main lcd size is 320X480
  # Switch-ability:
  #   Non-switchable

MAIN_LCM_PHYSICAL_SIZE = BY_LCD_SIZE_SETTING
  # Description:
  #   This is the physical size of attached main LCM, MAIN_LCD_SIZE is MMI size actually
  # Option Values:
  #   320x240:
  #   240x320:
  #   128X128:
  #   96X64:
  #   360X640:
  #   128X160:
  #   176X220:
  #   240x400:
  #   320x480:
  #   BY_LCD_SIZE_SETTING:
  # Switch-ability:
  #   [Any] -> [Any]

MAIN_MEDIA_LAYER_BITS_PER_PIXEL = DEFAULT
  # Description:
  #   If set this option to 24, Image Viewer, Slide Show, Photo Editor, Image Tile, Wallpaper would use 24-bit color format layer to show image. The image quality would be better.
  # Option Values:
  #   24: multimedia app support 24 bits LCD display
  #   DEFAULT:
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Connectivity
# ------------------------------ LCD
SUB_LCD_SIZE = NONE
  # Description:
  #   To support different SubLCD size, customer can set size to NONE, 48X64BW, 64X96, 96X64BW, 96X64, and 128X128.
  #   Resource generation will then generate different image resources for this option.
  # Option Values:
  #   NONE:
  #   128X128:
  #   96X64:
  #   64X96:
  #   48X64BW:
  #   96X64BW:
  # Switch-ability:
  #   Non-switchable

SUB_LCM_PHYSICAL_SIZE = BY_LCD_SIZE_SETTING
  # Description:
  #   This is the physical size of attached sub LCM if any, SUB_LCD_SIZE is MMI size actually
  # Option Values:
  #   320x240:
  #   240x320:
  #   128X128:
  #   96X64:
  #   360X640:
  #   128X160:
  #   176X220:
  #   240x400:
  #   320x480:
  #   BY_LCD_SIZE_SETTING:
  # Switch-ability:
  #   [Any] -> [Any]

LCM_ROTATE_SUPPORT = FALSE
  # Description:
  #   enable/disable use LCM HW's rotation,
  #   Turn on it when MT6228/29 platform and use TV-out
  # Option Values:
  #   TRUE: LCM support rotate
  #   FALSE: LCM does not support rotate
  # Switch-ability:
  #   [Any] -> [Any]

TOUCH_PANEL_SUPPORT = NONE
  # Description:
  #   Touch panel integration
  # Option Values:
  #   CTP_SITRONIX_ST1232_FAKE_MULTIPLE: support Sitroniz ST1232 capacitence touch panel controller -- fake multiple touch
  #   MT6301: use MT6301 TP controller
  #   NONE: Not support touch panel
  #   CTP_CYPRESS_TMA340_TRUE_MULTIPLE: support cypress TMA340 capacitence touch panel controller -- true multiple touch
  #   CTP_NOVATEK_NT11001_FAKE_MULTIPLE: support Novatek NT11001 capacitence touch panel controller -- fake multiple touch
  #   CTP_ITE_7250AFN_FAKE_MULTIPLE: support ITE 7250AFN capacitence touch panel controller -- fake multiple touch
  #   TP_INTERNAL: use Internal AUXADC to implement touch panel
  #   MT6301B: use MT6301B TP controller
  #   CTP_FOCALTEK_FT5206_TRUE_MULTIPLE: support FocalTech FT5206 capacitence touch panel controller -- true multiple touch
  #   CTP_GOODIX_GT818_TRUE_MULTIPLE: Support Goodix GT818 capacitence touch panel controller
  # Switch-ability:
  #   CTP_GOODIX_GT818_TRUE_MULTIPLE -> MT6301
  #   CTP_GOODIX_GT818_TRUE_MULTIPLE -> MT6301B
  #   CTP_GOODIX_GT818_TRUE_MULTIPLE -> NONE
  #   CTP_GOODIX_GT818_TRUE_MULTIPLE -> TP_INTERNAL
  #   MT6301 -> CTP_GOODIX_GT818_TRUE_MULTIPLE
  #   MT6301 -> MT6301B
  #   MT6301 -> NONE
  #   MT6301 -> TP_INTERNAL
  #   MT6301B -> CTP_GOODIX_GT818_TRUE_MULTIPLE
  #   MT6301B -> MT6301
  #   MT6301B -> NONE
  #   MT6301B -> TP_INTERNAL
  #   NONE -> CTP_GOODIX_GT818_TRUE_MULTIPLE
  #   NONE -> MT6301
  #   NONE -> MT6301B
  #   NONE -> TP_INTERNAL
  #   TP_INTERNAL -> CTP_GOODIX_GT818_TRUE_MULTIPLE
  #   TP_INTERNAL -> MT6301
  #   TP_INTERNAL -> MT6301B
  #   TP_INTERNAL -> NONE

TOUCH_PANEL_SHORTCUT_SUPPORT = FALSE
  # Description:
  #   Support touch panel shortcut functionality
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ MMI
MMI_VERSION = EMPTY_MMI
  # Description:
  #   It's MMI version type control
  #   For example:
  #   MMI_VERSION=PLUTO_MMI (It's an internal or MP at customer side project)
  #   MMI_VERSION=XXX_MMI (It's a MP at MTK side project)
  # Option Values:
  #   NEPTUNE_MMI:
  #   COSMOS_MMI:
  #   PLUTO_MMI:
  #   EMPTY_MMI:
  # Switch-ability:
  #   [Any] -> [Any]

VENUS_MMI = NONE
  # Description:
  #   Enable Venus UI Engine and related Fancy Apps
  # Option Values:
  #   NONE: Disable venus ui engine
  #   VENUS_LEVEL1: Enable venus ui engine
  # Switch-ability:
  #   [Any] -> [Any]

FINGER_TOUCH_SUPPORT = NONE
  # Description:
  #   it needs to covert to this macro
  #   __MMI_FTE_SUPPORT__
  #   for using in C program
  # Option Values:
  #   NONE: Not support finger touch
  #   FTE: Enable finger touch
  # Switch-ability:
  #   FTE -> NONE

REDUCE_PLUTO_HEADER_DEPTH = FALSE
  # Description:
  #   To distinguish if the build script will copy plutommi header files to a temporary folder in the custom release load to reduce the searching depth.
  # Option Values:
  #   TRUE: Enable header file cache mechanism for PLUTOMMI projects.
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

# ------------------------------ Connectivity
# ------------------------------ Input and Language
HAND_WRITING = NONE
  # Description:
  #   Enable Which Third Party Handwriting Engine
  # Option Values:
  #   MMI_HANWANG_DEMO:
  #   MMI_DECUMA_DEMO:
  #   NONE: No hand writing support
  #   MMI_DECUMA:
  #   MMI_PENPOWER_DEMO:
  #   MMI_HANWANG:
  #   MMI_PENPOWER:
  # Switch-ability:
  #   [Any] -> [Any]

INPUT_METHOD = NONE
  # Description:
  #   Add a INPUT_METHOD compile option in make file
  # Option Values:
  #   MMI_T9: T9 IME support
  #   NONE: No input method support
  #   MMI_ZI_V7: ZI V7 IME support
  #   MMI_KOMOXO:
  #   MMI_T9_V7_4:
  #   MMI_CSTAR_V2: CSTAR IME support
  #   MMI_CSTAR_V2_DEMO:
  #   MMI_KOMOXO_DEMO:
  #   MMI_ZI: ZI IME supoort
  #   MMI_ZI_V6: ZI V6 IME support
  # Switch-ability:
  #   [Any] -> [Any]

KEYPAD_TYPE = NORMAL_KEYPAD
  # Description:
  #   To set keypad type
  # Option Values:
  #   REDUCED_KEYPAD:
  #   EXTEND_QWERTY_KEYPAD:
  #   NORMAL_KEYPAD:
  #   QWERTY_KEYPAD:
  # Switch-ability:
  #   Non-switchable

# ------------------------------ Application
BACKGROUND_CALL_SUPPORT = FALSE
  # Description:
  #   Background call feature allows end users to launch MMI applications during in-call. End users can enter MMI main menu from in-call option menu. Background call feature also allows end user to continue the MMI application launched before incoming call. Aft
  # Option Values:
  #   TRUE: Enable background call feature
  #   FALSE: Disable background call feature
  # Switch-ability:
  #   [Any] -> [Any]

MMS_FEATURES = NONE
  # Description:
  #   Define features set of MMS solution
  # Option Values:
  #   SLIM: SLIM Feature set of MMS-V02
  #   NONE: In case MMS_SUPPORT is NONE
  #   FULL: FULL Feature set of MMS-V02
  #   LOW_TIER_SLIM: To enable low tier UI for slim project
  # Switch-ability:
  #   FULL -> SLIM
  #   SLIM -> FULL

MMS_SUPPORT = NONE
  # Description:
  #   Determine MMS solution
  # Option Values:
  #   OBIGO_Q03C_MMS_V02: MMS-V02 is supported
  #   NONE: MMS Feature is not supported
  # Switch-ability:
  #   OBIGO_Q03C_MMS_V02 -> NONE

# ------------------------------ HW and Project
# ------------------------------ Connectivity
# ------------------------------ HW and Project
# ------------------------------ Connectivity
# ------------------------------ Application
DA_MEMORY_CUT_SUPPORT = FALSE
  # Description:
  #   Reduce the data account memory.
  # Option Values:
  #   TRUE: Enable memory cut.
  #   FALSE: Disable memory cut.
  # Switch-ability:
  #   [Any] -> [Any]

DOWNLOADABLE_THEME = FALSE
  # Description:
  #   Theme is to change MMI look and feel easily;
  #   Capable of changing look and feel of the phone by downloaded theme file. Can: Change Color/Change Image/Change Tone;
  #   Dynamically downloaded on phone
  # Option Values:
  #   TRUE: Enable downloadable theme feature
  #   FALSE: Disable downloadable theme feature
  # Switch-ability:
  #   [Any] -> [Any]

DLT_ALWAYS_LOAD_BUFFER_SIZE = 0
  # Description:
  #   The basic idea is to load images into a global buffer so as to reduce file system access; the loading maybe of three types:
  #   one is "LOAD_ALWAYS images that are always loaded. Ex: Images that are part of the base control set are frequently accessed and can
  # Option Values:
  #   NA: For downloadable theme feature, define memory buffer (in KByte) for theme attributes. It is for performance acceleration. Some theme attributes will always be loaded to this buffer.
  # Switch-ability:
  #   [Any] -> [Any]

DLT_LOAD_ON_DEMAND_BUFFER_SIZE = 0
  # Description:
  #   The basic idea is to load images into a global buffer so as to reduce file system access; the loading maybe of three types:
  #   one is "LOAD_ONDEMAND images that are loaded as need arises. Images that may be frequently accessed from a screen like matrix menu
  # Option Values:
  #   NA: For downloadable theme feature, define memory buffer (in KByte) for theme attributes. It is for performance acceleration. Theme attributes will be loaded on demand to this buffer.
  # Switch-ability:
  #   [Any] -> [Any]

EMAIL_SUPPORT = NONE
  # Description:
  #   from W08.01 MAUI/07B is 2.0; before is 1.0
  # Option Values:
  #   ALL: Enable Email (include SMTP/POP3/IMAP4)
  #   NONE: Disable Email
  # Switch-ability:
  #   N/A

PUSH_MAIL = NONE
  # Description:
  #   NONE: turn off push mail support
  #   MAIL_139: turn on push mail support
  # Option Values:
  #   NONE: Turn off Push mail support
  #   MAIL_139: Turn on push mail support
  # Switch-ability:
  #   [Any] -> [Any]

GEMINI = 2
  # Description:
  #   To identify if this project uses a "Single chip-dual SIM" solution.
  # Option Values:
  #   FALSE: Disable this feature
  #   2: Enable 2 SIMs, single talk.
  #   3: Enable 3 SIMs, single talk.
  #   4: Enable 4 SIMs, single talk.
  # Switch-ability:
  #   Non-switchable

SENDKEY_MODE = SINGLE_KEY_SELECT_MENU
  # Description:
  #   Send key mode.
  #   SINGLE_KEY_SELECT_MENU : only one sendkey will popup a menu to select dail card1 or card2
  #   SENDKEY2_SUPPORT :  two sendkeys the one is for card1 and the other is for card2
  #   LSK_KEYSEND2_IN_ONE: only one sendkey and is for card1 dial ,card2 an
  # Option Values:
  #   SENDKEY2_SUPPORT: Two sendkeys , the one is for card1 and the other is for card2
  #   SINGLE_KEY_SELECT_MENU: Only one sendkey, will popup a menu to select dial card1 or card2
  # Switch-ability:
  #   SENDKEY2_SUPPORT -> SINGLE_KEY_SELECT_MENU

IMPS_SUPPORT = FALSE
  # Description:
  #   Enable OMA Instant Messaging and Presence Services(IMPS) feature
  # Option Values:
  #   TRUE: Enable IMPS feature
  #   FALSE: Disable IMPS feature
  # Switch-ability:
  #   TRUE -> FALSE

PHB_LN_ENTRY = 10
  # Description:
  #   Number of the call log with single log type from single SIM card, it 's a range better no more than 100, suggestion to be 10, 20, 30, 40, 50, 60, 80, 100 and so on. Totol log num = PHB_LN_ENTRY * log type num * sim card num.
  # Option Values:
  #   50: Number of call log with single log type from single SIM card, ex, The dialed call log from SIM1 is 50, total log num = 50 * log type num * sim card num.
  #   40: Number of call log with single log type from single SIM card, ex, The dialed call log from SIM1 is 40, total log num = 40 * log type num * sim card num.
  #   80: Number of call log with single log type from single SIM card, ex, The dialed call log from SIM1 is 80, total log num = 80 * log type num * sim card num.
  #   20: Number of call log with single log type from single SIM card, ex, The dialed call log from SIM1 is 20, total log num = 20 * log type num * sim card num.
  #   60: Number of call log with single log type from single SIM card, ex, The dialed call log from SIM1 is 60, total log num = 60 * log type num * sim card num.
  #   30: Number of call log with single log type from single SIM card, ex, The dialed call log from SIM1 is 30, total log num = 30 * log type num * sim card num.
  #   10: Number of call log with single log type from single SIM card, ex, The dialed call log from SIM1 is 10, total log num = 10 * log type num * sim card num.
  #   100: Number of call log with single log type from single SIM card, ex, The dialed call log from SIM1 is 100, total log num = 100 * log type num * sim card num.
  # Switch-ability:
  #   [Any] -> [Any]

PHB_PHONE_ENTRY = 10
  # Description:
  #   Maximum Phonebook entry in Phone,  #Switch-ability: [Any] -> [Any]
  # Option Values:
  #   NA: .
  # Switch-ability:
  #   [Any] -> [Any]

PHB_SIM_ENTRY = 1000
  # Description:
  #   Maximum Phonebook entry in SIM card.
  # Option Values:
  #   NA: .
  # Switch-ability:
  #   [Any] -> [Any]

PHB_SYNC = ON
  # Description:
  #   To enable/disable phonebook sync feature.
  # Option Values:
  #   ON: Support Sync from Phonebook
  #   OFF: Not support Sync from Phonebook
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Connectivity
# ------------------------------ Application
# ------------------------------ Connectivity
# ------------------------------ Application
SIM_PROVISIONING = MMSICP
  # Description:
  #   Enable provisioning of Internet profile settings from SIM card
  # Option Values:
  #   NONE: Disable SIM Provisioning feature
  #   MMSICP: Enable SIM provsioning feature and support 3GPP Efmmsicp file in SIM card
  # Switch-ability:
  #   MMSICP -> NONE

SMS_PHONE_ENTRY = 10
  # Description:
  #   SMS Entry Number in NVRAM
  # Option Values:
  #   NA: SMS entities on NVRAM
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Multimedia
# ------------------------------ HW and Project
# ------------------------------ Application
SMS_SIM_ENTRY = 50
  # Description:
  #   This option should be added below the feature SMS_PHONE_ENTRY.
  # Option Values:
  #   50: MAX number of SIM to save SMS, customer to set reasonable value accoring to SIM, if too bigger, waster memory, if too smaller, do not display all SMS
  #   100: MAX number of SIM to save SMS, customer to set reasonable value accoring to SIM, if too bigger, waster memory, if too smaller, do not display all SMS
  # Switch-ability:
  #   [Any] -> [Any]

SMS_TOTAL_ENTRY = 500
  # Description:
  #   total number of SMS stored in SIM and NVRAM can be read
  # Option Values:
  #   NA: SMS entities on NVRAM+SIM
  # Switch-ability:
  #   [Any] -> [Any]

BACKGROUND_SMS_SUPPORT = FALSE
  # Description:
  #   to enable unified message background sending feature
  # Option Values:
  #   TRUE: support background send: when send SMS, SMS will be saved to Outbox, outbox background send SMS one by one
  #   FALSE: DO not support background send, if user send one SMS, willl display sending screen and send this SMS directly
  # Switch-ability:
  #   FALSE -> TRUE

MSG_SMS_EMAIL_SUPPORT = FALSE
  # Description:
  #   To enable SMS Email interworking support from orange project
  # Option Values:
  #   TRUE: support SMS email interworking feature, user can send Email by SMS
  #   FALSE: only support normal SMS send
  # Switch-ability:
  #   FALSE -> TRUE

# ------------------------------ Audio
# ------------------------------ Application
QOS_PROFILE_SUPPORT = ALL
  # Description:
  #   If QOS_PROFILE_SUPPORT is NONE, this means the QoS feature is disable.
  #   If QOS_PROFILE_SUPPORT is STATIC, this means the static QoS feature is supported.
  #   If QOS_PROFILE_SUPPORT is DYNAMIC, this means the dynamic QoS feature is supported.
  #   If QOS_PROFILE_SUPPORT is ALL, this means both static and dynamic qos feature is supported.
  #   The static QoS feature is that there is a default GPRS qos default value in customer folder and customer can change default GPRS qos values. The default value will be passed to operator network to tell operator that it uses this default qos setting. If customer doesn't enable static qos feature, there may be some IOT problem. If dynamic QoS feature is another way to change default qos values passed to operator network, there is a API called cbm_set_qos_profile. customer can call this API to set default qos values. The API usage is defined in CBM SAP
  # Option Values:
  #   ALL: .
  #   NONE: .
  #   STATIC: .
  #   DYNAMIC: .
  # Switch-ability:
  #   Non-switchable

UNIFIED_MESSAGE_ADVANCE_FEATURE = NONE
  # Description:
  #   to enable unified message advance feature
  # Option Values:
  #   COMPOSER_LISTOPTION_ARCHIVE: This value already phase out
  #   NONE: Disable all message advance feature
  #   LISTOPTION: This value already phase out
  #   ARCHIVE: enable  message archive feature
  #   LISTOPTION_ARCHIVE: This value already phase out
  #   COMPOSER_LISTOPTION: This value already phase out
  #   COMPOSER: enable unified message composer feature
  #   COMPOSER_ARCHIVE: enable unified message composer and archive feature
  # Switch-ability:
  #   [Any] -> [Any]

UNIFIED_MESSAGE_FOLDER = FALSE
  # Description:
  #   to enable unified message feature
  # Option Values:
  #   TRUE: Enable unified message folder feature
  #   FALSE: Disable unified message folder feature
  # Switch-ability:
  #   TRUE -> FALSE

UNIFIED_MESSAGE_MARK_SEVERAL_FEATURE = FALSE
  # Description:
  #   "Mark several" feature is multiple operation of messages.
  #   End users can mark several messages and do the operation on them together.
  # Option Values:
  #   TRUE: Enable message mark several feature
  #   FALSE: Disable message mark several feature
  # Switch-ability:
  #   [Any] -> [Any]

UNIFIED_MESSAGE_SIMBOX_SUPPORT = FALSE
  # Description:
  #   MT SMS stored in phone only. SIM message box would be displayed to show SMS stored in SIM.
  # Option Values:
  #   TRUE: Enable message SIM box feature
  #   FALSE: Disable message SIM box feature
  # Switch-ability:
  #   [Any] -> [Any]

UNIFIED_MESSAGE_SORT_MESSAGE_LIST = FALSE
  # Description:
  #   Enable the unified message sort feature
  # Option Values:
  #   TRUE: Enable message sort feature
  #   FALSE: Disable message sort feature
  # Switch-ability:
  #   [Any] -> [Any]

WBXML_SUPPORT = FALSE
  # Description:
  #   to enable wbxml parser function
  # Option Values:
  #   TRUE: Enable WBXML.
  #   FALSE: Disable WBXML.
  # Switch-ability:
  #   TRUE -> FALSE

XML_SUPPORT = FALSE
  # Description:
  #   to enable xml parser function
  # Option Values:
  #   TRUE: Enable XML.
  #   FALSE: Disable XML.
  # Switch-ability:
  #   TRUE -> FALSE

# ------------------------------ Speech
# ------------------------------ Wifi
# ------------------------------ Application
# ------------------------------ Connectivity
BLUETOOTH_SUPPORT = NONE
  # Description:
  #   The supported Bluetooth chip
  # Option Values:
  #   BTMTK_MT6626: The MAUI load supports MT6626
  #   BTMTK_MT6611: The MAUI load supports MT6611 (phased-out in 11A)
  #   NONE: The MAUI load did not support Bluetooth
  #   BTMTK_MT6616: The MAUI load supports MT6616
  #   BTMTK_MT6612: The MAUI load supports MT6612
  #   BTMTK_MT6612_22_26: The MAUI load supports MT6622, MT6626, MT6612
  #   BTMTK_MT6276: The MAUI load supports MT6276
  #   BTMTK_MT6601: The MAUI load supports MT6601 (phased-out in 11A)
  #   BTMTK_MT6236: The MAUI load supports MT6236
  #   BTMTK_MT6255: The MAUI load supports MT6255
  #   BTMTK_MT6622: The MAUI load supports MT6622
  #   BTMTK_MT6256: The MAUI load supports MT6256
  #   BTMTK_MT6250: The MAUI load supports MT6250
  # Switch-ability:
  #   BTMTK_MT6612 -> BTMTK_MT6612_22_26
  #   BTMTK_MT6612 -> BTMTK_MT6622
  #   BTMTK_MT6612 -> BTMTK_MT6626
  #   BTMTK_MT6612_22_26 -> BTMTK_MT6612
  #   BTMTK_MT6612_22_26 -> BTMTK_MT6622
  #   BTMTK_MT6612_22_26 -> BTMTK_MT6626
  #   BTMTK_MT6622 -> BTMTK_MT6612
  #   BTMTK_MT6622 -> BTMTK_MT6612_22_26
  #   BTMTK_MT6622 -> BTMTK_MT6626
  #   BTMTK_MT6626 -> BTMTK_MT6612
  #   BTMTK_MT6626 -> BTMTK_MT6612_22_26
  #   BTMTK_MT6626 -> BTMTK_MT6622

# ------------------------------ Audio
# ------------------------------ Connectivity
BT_BPP_PROFILE = FALSE
  # Description:
  #   To enable Bluetooth BIP function, also can enable
  # Option Values:
  #   TRUE: Turn on BPP support
  #   FALSE: Turn off BPP support
  # Switch-ability:
  #   TRUE -> FALSE

BT_DUN_PROFILE = FALSE
  # Description:
  #   To turn on/off Dial-Up Networking profile
  # Option Values:
  #   TRUE: Turn on DUN support
  #   FALSE: Turn off DUN support
  # Switch-ability:
  #   TRUE -> FALSE

# ------------------------------ Application
UDX_SUPPORT = FALSE
  # Description:
  #   Enable the feature of UDX support.
  # Option Values:
  #   TRUE: Enable UDX support
  #   FALSE: Disable UDX Support
  # Switch-ability:
  #   TRUE -> FALSE

# ------------------------------ Connectivity
BT_FTC_PROFILE = FALSE
  # Description:
  #   To turn on/off FTP Client profile
  # Option Values:
  #   TRUE: Turn on FTP client role support
  #   FALSE: Turn off FTP client role support
  # Switch-ability:
  #   TRUE -> FALSE

BT_FTS_PROFILE = FALSE
  # Description:
  #   To turn on/off  FTP Server profile
  # Option Values:
  #   TRUE: Turn on FTP server role support
  #   FALSE: Turn off FTP server role support
  # Switch-ability:
  #   TRUE -> FALSE

# ------------------------------ Modem
# ------------------------------ Connectivity
BT_HFG_PROFILE = FALSE
  # Description:
  #   To turn on/off  Hands-Free Gateway profile
  # Option Values:
  #   TRUE: Turn on HFP/HSP gateway role support
  #   FALSE: Turn off HFP/HSP gateway role support
  # Switch-ability:
  #   TRUE -> FALSE

BT_HIDD_PROFILE = FALSE
  # Description:
  #   To turn on/off bluetooth HID profile
  # Option Values:
  #   TRUE: Turn on HID Device role support
  #   FALSE: Turn off HID Device role support
  # Switch-ability:
  #   TRUE -> FALSE

BT_OPP_PROFILE = FALSE
  # Description:
  #   To turn on/off Object Push profile
  # Option Values:
  #   TRUE: Turn on OPP support
  #   FALSE: Turn off OPP support
  # Switch-ability:
  #   TRUE -> FALSE

BT_PBAP_PROFILE = FALSE
  # Description:
  #   bluetooth phonebook access profile support
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   TRUE -> FALSE

# ------------------------------ Application
SMS_R8_TABLE_MECHANISM = SMS_R8_DEFAULT
  # Description:
  #   Customize the GSM R8 table mechanism
  # Option Values:
  #   SMS_R8_BOTH: Support National Locking table + National extension table mode
  #   SMS_R8_SINGLE: Support English Locking table + National extension table mode
  #   SMS_R8_LOCKING: Support National Locking table + English extension table mode
  #   SMS_R8_DEFAULT: Only support English Gsm7bit conversion table
  #   SMS_R8_AUTO: Support switch table mode automaticlly by the minimized text size
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Connectivity
BT_SPP_PROFILE = FALSE
  # Description:
  #   To turn on/off Serial Port profile
  # Option Values:
  #   TRUE: Turn on SPP support
  #   FALSE: Turn off SPP support
  # Switch-ability:
  #   TRUE -> FALSE

# ------------------------------ Application
# ------------------------------ Connectivity
# ------------------------------ MMI
# ------------------------------ Connectivity
# ------------------------------ Application
# ------------------------------ Connectivity
GPS_UART_PORT = UART3
  # Description:
  #   Set GPS UART HW usage port
  # Option Values:
  #   UART2: If this value is set, it means GPS UART port use UART2
  #   NONE: If GPS is not supported(the GPS_SUPPORT set to NONE). This option set to NONE also.
  #   UART3: If this value is set, it means GPS UART port use UART3
  # Switch-ability:
  #   [Any] -> [Any]

USB_HS_SUPPORT = FALSE
  # Description:
  #   Customer can use this compileoption to decide USB2.0 or USB1.1. TRUE:: work in USB2.0
  # Option Values:
  #   TRUE: Enable USB High-Speed
  #   FALSE: Disable USB High-Soeed mode (Run in Full Speed)
  # Switch-ability:
  #   TRUE -> FALSE

USB_MULTIPLE_COMPORT_ENABLE = FALSE
  # Description:
  #   To enable USB multiple com port feature
  # Option Values:
  #   TRUE: Enable multiple usb comport
  #   FALSE: disable multiple usb comport
  # Switch-ability:
  #   [Any] -> [Any]

USB_HIGH_SPEED_COM_PORT_SUPPORT = FALSE
  # Description:
  #   USB High Speed Interface API usage.
  # Option Values:
  #   TRUE: Enable USB high-speed comport interface API
  #   FALSE: Disable USB high-speed comport interface API
  # Switch-ability:
  #   [Any] -> [Any]

USB_IN_NORMAL_MODE_SUPPORT = FALSE
  # Description:
  #   When this option is enable, the USB Mass Storage function could be used in normal mode. That is, the targe doesn't need to reboot when the user select "Mass Storage" in the USB query menu.
  # Option Values:
  #   TRUE: Support USB mass storage in USB normal boot mode
  #   FALSE: Mass Storage only work on charging boot mode
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ File system and System service
# ------------------------------ Connectivity
USB_MASS_STORAGE_SUPPORT = FALSE
  # Description:
  #   enable USB mass storage feature
  # Option Values:
  #   TRUE: Support USB mass storage function
  #   FALSE: doesn't  support Mass Storage  function
  # Switch-ability:
  #   TRUE -> FALSE

USB_DOWNLOAD_IN_BL = NONE
  # Description:
  #   enable factory download / connection via USB cable via Bootloader
  # Option Values:
  #   NONE: USBDL in Bootloader is disabled
  #   NORMAL: USBDL in Bootloader is enabled for non-secure platform
  #   SECURE: USBDL in Bootloader is enabled for secure platform
  # Switch-ability:
  #   NORMAL -> NONE

USB_MULTI_CHARGE_CURRENT_SUPPORT = FALSE
  # Description:
  #   To make USB device mode support multiple configurations, i.e., it supports multiple charging currents defined in USB_CHARGE_CURRENT[] table.
  #   Note that it only effects when we set enable_usb_charge as false
  # Option Values:
  #   TRUE: Support multiple charging current in USB
  #   FALSE: Disable multiple charging current in USB (Only 0ma and 500ma)
  # Switch-ability:
  #   [Any] -> [Any]

EXTERNAL_ACCESSORY_DETECTION = NONE
  # Description:
  #   When the feature is NOT set to NONE, it means the platform uses external accessory detection device
  # Option Values:
  #   MAX14526:
  #   NONE:
  #   TS5USBA33402:
  # Switch-ability:
  #   Non-switchable

EXTERNAL_CHARGER_DETECTION = NONE
  # Description:
  #   When the feature is NOT set to NONE, it means the platform uses external charger detection device
  # Option Values:
  #   MAX14526: External  Device Type
  #   NONE: Not support External Charger
  #   TS5USBA33402: External  Device Type
  # Switch-ability:
  #   [Any] -> [Any]

GENERAL_EXTERNAL_CHARGER = NONE
  # Description:
  #   The feature is to indicate whether we use external charger or NOT
  # Option Values:
  #   NONE: no external charger
  #   ISL9221: external charger type is ISL9221
  #   BQ25040: external charger type is BQ25040
  #   RT9505_BQ24020: external charger type is RT9505_BQ24020
  #   STT818B: external charger type is STT818B
  # Switch-ability:
  #   [Any] -> [Any]

MTP_SUPPORT = FALSE
  # Description:
  #   Enable the feature of USB MTP protocol
  # Option Values:
  #   TRUE: Enable MTP
  #   FALSE: Disbale MTP
  # Switch-ability:
  #   [Any] -> [Any]

IRDA_SUPPORT = FALSE
  # Description:
  #   to enable IRDA function
  # Option Values:
  #   TRUE: support IRDA
  #   FALSE: not support IRDA
  # Switch-ability:
  #   TRUE -> FALSE

# ------------------------------ Camera
# ------------------------------ Connectivity
# ------------------------------ Camera
FLASHLIGHT_TYPE = NONE
  # Description:
  #   Set flashlight type
  # Option Values:
  #   NONE:
  #   LED_ONOFF_SUPPORT:
  #   LED_FLASHLIGHT_SUPPORT:
  # Switch-ability:
  #   N/A

# ------------------------------ Modem
# ------------------------------ Connectivity
# ------------------------------ Multimedia
AUDIO_CLIPPER_SUPPORT = FALSE
  # Description:
  #   The audio clipper function is to clip audio segment from an audio file. The audio clip could be used as ringtone.
  # Option Values:
  #   TRUE: Enable audio clipper
  #   FALSE: Disable audio clipper
  # Switch-ability:
  #   FALSE -> TRUE
  #   TRUE -> FALSE

CONTOUR_IMPROVEMENT_SUPPORT = FALSE
  # Description:
  #   default : FLASE
  # Option Values:
  #   TRUE: Enable CONTOUR IMPROVEMENT
  #   TRUE: Enable CONTOUR IMPROVEMENT
  #   FALSE: Disable CONTOUR IMPROVEMENT (Default)
  #   FALSE: Disable CONTOUR IMPROVEMENT (Default)
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Tools
# ------------------------------ Multimedia
INTERGRAFX_SUPPORT = NONE
  # Description:
  #   Co-work with vendor InterGrafx to support those features in QVGA/WQVGA/HVGA/WVGA LCD resolution:
  #   a) Pre-integrated InterGrafx engine
  #   b) 10 live wallpapers for MT6236/MT6276 for Venus home screen.
  #   c) Customize tools for live wallpapers
  #
  #   ( 4 3D games for MT6236/MT6276 tracked by FMS00065384)
  # Option Values:
  #   NONE: Disable intergrafx live wallpaper & game
  #   INTERGRAFX_ENGINE_EN_BASIC: Enable  4 Intergrafx wallpaper and 1 game (English version)
  #   INTERGRAFX_ENGINE_CH_BASIC: Enable  4 Intergrafx wallpaper and 1 game (Chinese version)
  #   INTERGRAFX_ENGINE_EN_VALUED: Enable  9 Intergrafx wallpaper and 2 games (English version)
  #   INTERGRAFX_ENGINE_CH_VALUED: Enable  9 Intergrafx wallpaper and 2 games (Chinese version)
  #   INTERGRAFX_ENGINE_EN_FULL: Enable 10 Intergrafx wallpaper and 2 games (English version)
  #   INTERGRAFX_ENGINE_CH_FULL: Enable 10 Intergrafx wallpaper and 2 games (Chinese version)
  # Switch-ability:
  #   [Any] -> [Any]

MELODY_BUFFER_LEN = 20480
  # Description:
  #   Defining buffer midi/imelody files to provide different number of notes supported
  # Option Values:
  #   512: Support up to 512 bytes MIDI file
  #   307200: Support up to 307200 bytes MIDI file
  #   1024: Support up to 1024 bytes MIDI file
  #   102400: Support up to 102400 bytes MIDI file
  #   20480: Support up to 20480 bytes MIDI file
  #   1300: Support up to 1300 bytes MIDI file
  #   81920: Support up to 81920 bytes MIDI file
  # Switch-ability:
  #   81920 -> 20480

# ------------------------------ Audio
# ------------------------------ LCD
# ------------------------------ Audio
# ------------------------------ Connectivity
# ------------------------------ Audio
EXT_DAC_SUPPORT = FALSE
  # Description:
  #   To enable the interface between AFE control and EXT_DAC control.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Connectivity
# ------------------------------ Audio
GAIN_TABLE_SUPPORT = FALSE
  # Description:
  #   If GAIN_TABLE_SUPPORT is set to TRUE, the project is allowed to customize gain table in custom folder.
  #   If GAIN_TABLE_SUPPORT is set to FALSE, MED will be responsible for updating gain (current MAUI design).
  # Option Values:
  #   TRUE: Enable gain table
  #   FALSE: Disable gain table
  # Switch-ability:
  #   FALSE -> TRUE
  #   TRUE -> FALSE

# ------------------------------ Speech
# ------------------------------ Connectivity
# ------------------------------ Speech
# ------------------------------ Video
PGDL_SUPPORT = FALSE
  # Description:
  #   to enable streaming function
  # Option Values:
  #   TRUE: Progressive download is supported
  #   FALSE: Progressive download is not supported
  # Switch-ability:
  #   Non-switchable

MJPG_DECODE = FALSE
  # Description:
  #   To enable Mjpeg decoder.
  # Option Values:
  #   TRUE: support motion jpeg decode
  #   FALSE: does not support motion jpeg decode
  # Switch-ability:
  #   TRUE -> FALSE

MJPG_ENCODE = FALSE
  # Description:
  #   To enable Mjpeg encoder
  # Option Values:
  #   TRUE: support motion jpeg encode
  #   FALSE: support motion jpeg encode
  # Switch-ability:
  #   TRUE -> FALSE

RM_DECODE = FALSE
  # Description:
  #   When RM_DECODE = TRUE/FALSE, turn on/off Real Media video decode support
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   N/A

RM_DECODE_MODE = NORMAL
  # Description:
  # Option Values:
  #   SLIM:
  #   NORMAL:
  # Switch-ability:
  #   [Any] -> [Any]

STREAM_SUPPORT = FALSE
  # Description:
  #   to enable streaming function
  # Option Values:
  #   TRUE: to enable video streaming
  #   FALSE: to disable video streaming
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Camera
AF_AUXI_LED_SUPPORT = FALSE
  # Description:
  #   AF auxiliary LED feature enable/disable, under low light environment, auxiliary LED will emit to let AF function better quality & performance
  #   If switch from FALSE to TRUE, FlashlightPowerOn() function must implemented in custom\drv\camera\[proj]\camera_hw.c
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

AF_SUPPORT = FALSE
  # Description:
  #   to enable Auto Focus function
  #   If switch from FALSE to TRUE, LENS_MODULE must not be NONE, except YUV sensor project.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

BACKUP_SENSOR_SUPPORT = FALSE
  # Description:
  #   support multiple sensors with a single firmware bin file. It doesn't need to change firmware when change sensor module on production line.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

CAMCORDER_SUPPORT = NONE
  # Description:
  #   Check if camcorder support. If yes, camera and video recorder will be turned off.
  # Option Values:
  #   SLIM:
  #   TRUE:
  #   NONE:
  #   CUSTOM:
  #   FULL:
  #   FALSE:
  #   STANDARD:
  # Switch-ability:
  #   FULL -> SLIM
  #   FULL -> STANDARD
  #   SLIM -> FULL
  #   SLIM -> STANDARD
  #   STANDARD -> FULL
  #   STANDARD -> SLIM

CAMCORDER_FACE_DETECTION_MODE_SUPPORT = FALSE
  # Description:
  #   To enable camcorder face detection mode
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   TRUE -> FALSE

CAMCORDER_SMILE_SHUTTER_MODE_SUPPORT = FALSE
  # Description:
  #   if this feature is enabled, it will enable the function to automatically capture while the smile in a face is detected
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

DUAL_CAMERA_SUPPORT = FALSE
  # Description:
  #   Enable the feature of dual camera support for end user
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   TRUE -> FALSE

CMOS_SENSOR = NONE
  # Description:
  #   Set sensor module of main camera
  # Option Values:
  #   NA:
  #   NONE: Disable this feature
  # Switch-ability:
  #   [Any] -> [Any]

CMOS_SENSOR_BAK1 = NONE
  # Description:
  #   Set sensor module of main backup camera
  # Option Values:
  #   NA:
  #   NONE: Disable this feature
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Security
# ------------------------------ Camera
SENSOR_TYPE = NONE
  # Description:
  #   indicate the sensor type
  # Option Values:
  #   YUV:
  #   NONE:
  #   RAW:
  # Switch-ability:
  #   Non-switchable

CMOS_SENSOR_SUB = NONE
  # Description:
  #   Set sensor module of sub camera
  # Option Values:
  #   NONE:
  #   NA:
  # Switch-ability:
  #   [Any] -> NA

CMOS_SENSOR_SUB_BAK1 = NONE
  # Description:
  #   Set sensor module of sub backup camera
  # Option Values:
  #   NA:
  #   NONE: Disable this feature
  # Switch-ability:
  #   [Any] -> [Any]

SENSOR_SUB_ROTATE = SENSOR_ROTATE_0
  # Description:
  #   Set the angle between sensor scan direction and handset.
  # Option Values:
  #   SENSOR_ROTATE_90:
  #   SENSOR_ROTATE_180:
  #   SENSOR_ROTATE_0:
  #   SENSOR_ROTATE_270:
  # Switch-ability:
  #   [Any] -> [Any]

SENSOR_TYPE_SUB = NONE
  # Description:
  #   indicate the sub sensor type
  # Option Values:
  #   YUV:
  #   NONE:
  #   RAW:
  # Switch-ability:
  #   Non-switchable

CAPTURE_SCENARIO = OFFLINE
  # Description:
  #   Indicate on the fly or off line capture
  # Option Values:
  #   OFFLINE:
  #   STANDARD:
  #   ON_THE_FLY:
  # Switch-ability:
  #   Non-switchable

# ------------------------------ Security
# ------------------------------ Camera
HORIZONTAL_CAMERA = FALSE
  # Description:
  #   To enable horizontal camera feature.
  #   Add HORIZONTAL_CAMERA to global compile option when the value is TRUE.
  # Option Values:
  #   TRUE: Camera or Camcorder with Horizontal  UI
  #   FALSE: Camera or Camcorder with Vertical UI
  # Switch-ability:
  #   [Any] -> [Any]

LENS_MODULE = NONE
  # Description:
  #   Set the lens module of main camera
  #   If switch from Any to NONE, AF_SUPPORT must set to FALSE, except YUV sensor project.
  #   If switch from NONE to Any, AF_SUPPORT must set to TRUE.
  # Option Values:
  #   NA:
  #   NONE: Disable this feature
  # Switch-ability:
  #   NONE -> [Any]
  #   [Any] -> NONE

LENS_MODULE_BAK1 = NONE
  # Description:
  #   Set the lens module of main backup camera
  # Option Values:
  #   NA:
  #   NONE: Disable this feature
  # Switch-ability:
  #   [Any] -> NONE

PANORAMA_VIEW_SUPPORT = FALSE
  # Description:
  #   Support Panorama View capture mode for camcoder
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

WEBCAM_SUPPORT = FALSE
  # Description:
  #   To enable webcam feature.
  #   If value is TRUE, define this as global compile option.
  # Option Values:
  #   TRUE: Enable Webcam
  #   FALSE: Disable Webcam
  # Switch-ability:
  #   [Any] -> [Any]

YUVCAM_ENCODE_DIRECT_WRITE_FILE = FALSE
  # Description:
  #   Enable photo capture direct write file function to save temp file buffer.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

YUVCAM_INTERPOLATION = NONE
  # Description:
  #   YUVCAM_INTERPOLATION = HW  means to use HW Resizer to do this interpolation.YUVCAM_INTERPOLATION = SW means touse SW JPEG Encoder to do this interpolation.
  # Option Values:
  #   SW:
  #   NONE:
  #   HW:
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Graphic
GDI_BLT_WITH_DOUBLE_BUF_SUPPORT = FALSE
  # Description:
  #   gdi blt with diouble buffer
  # Option Values:
  #   TRUE: Enable it for Portrait LCD landscape MMI
  #   FALSE: app have to use double buffer by itself
  # Switch-ability:
  #   [Any] -> [Any]

GDI_SCREEN_SHOT_BUFFER = ON
  # Description:
  #   If ON, preserve a global memory buffer for some debug tool to get the screen shot, the memory size is LCD_WIDTH*LCD_HEIGHT*2. If OFF, there is not such memory, then the screen shot function of those debug tools would not be able to use.
  # Option Values:
  #   ON: Support tool snapshop feature
  #   OFF: Disable tool snapshop feature
  # Switch-ability:
  #   [Any] -> [Any]

GIF_DECODE = FALSE
  # Description:
  #   Support for 11A/11B branch.
  #   This option define if SW/HW GIF decoder support or not.
  # Option Values:
  #   TRUE:
  #   NONE:
  #   FALSE:
  #   GIF_HW:
  #   GIF_SW:
  # Switch-ability:
  #   [Any] -> [Any]

SCALADO_SUPPORT = FALSE
  # Description:
  #   using SCALADO Caps engine to enhance applications: Image Viewer and Photo Editor
  # Option Values:
  #   TRUE: Turn on SCALADO Caps engine with image viewer and photo editor
  #   FALSE: not use SCALADO caps engine.
  # Switch-ability:
  #   Non-switchable

# ------------------------------ Security
# ------------------------------ Connectivity
# ------------------------------ Security
# ------------------------------ Application
# ------------------------------ Security
SECURE_VERSION = 1
  # Description:
  #   Support security system
  # Option Values:
  #   1: Secure version 1
  #   0: Secure version 0
  # Switch-ability:
  #   [Any] -> [Any]

SECURE_CUSTOM_NAME = MTK
  # Description:
  #   Support security system
  # Option Values:
  #   JRD:
  #   NONE: No customer name specified, non secure load only
  #   MTK: Customer name for MTK
  # Switch-ability:
  #   [Any] -> [Any]

SECURE_JTAG_ENABLE = TRUE
  # Description:
  #   Support security system
  # Option Values:
  #   TRUE: The JTAG on the phone will b on
  #   FALSE: The JTAG on the phone will b off
  # Switch-ability:
  #   FALSE -> TRUE
  #   TRUE -> FALSE

# ------------------------------ Application
# ------------------------------ Security
# ------------------------------ Modem
# ------------------------------ Application
# ------------------------------ Modem
BIP_SCWS_SUPPORT = FALSE
  # Description:
  #   BIP SCWS feature:
  #   Server mode: To establish channel between Browser and UICC
  #   Client mode: To establish channel between UICC and network
  # Option Values:
  #   TRUE: Enable BIP support
  #   FALSE: Disable BIP support(default)
  # Switch-ability:
  #   Non-switchable

ECC_RETRY_ENHANCEMENT = TRUE
  # Description:
  #   If this feature option is turned on, L4C would retry ECC in other RAT when the first ECC attemp is fail.
  # Option Values:
  #   TRUE: Modem will retry fail ECC on the other RAT
  #   FALSE: Modem will not retry fail ECC on the other RAT
  # Switch-ability:
  #   Non-switchable

# ------------------------------ Security
# ------------------------------ Modem
GPRS_MAX_PDP_SUPPORT = 6
  # Description:
  #   Maximum simultaneous PDP contexts (2~7)
  # Option Values:
  #   6: Maximum simultaneous PDP contexts is six
  #   4: Maximum simultaneous PDP contexts is four
  #   1: Maximum simultaneous PDP contexts is one
  #   3: Maximum simultaneous PDP contexts is three
  #   7: Maximum simultaneous PDP contexts is seven
  #   2: Maximum simultaneous PDP contexts is two
  #   5: Maximum simultaneous PDP contexts is five
  # Switch-ability:
  #   N/A

RSAT_SUPPORT = MODEM_WITH_RSAT
  # Description:
  #   To enable RSAT(STK AT command) feature
  # Option Values:
  #   NONE: Disable RSAT support(default)
  #   MMI_WITH_RSAT: Enable RSAT support in MMI project
  #   MODEM_WITH_RSAT: Enable RSAT support in modem project
  # Switch-ability:
  #   N/A

UMTS_BAND_I = TRUE
  # Description:
  #   Indicates whether UE support UMTS FDD band I
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

UMTS_BAND_II = TRUE
  # Description:
  #   Indicates whether UE support UMTS FDD band II
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   N/A

UMTS_BAND_III = FALSE
  # Description:
  #   Indicates whether UE support UMTS FDD band III
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

UMTS_BAND_IV = FALSE
  # Description:
  #   Indicates whether UE support UMTS FDD band IV
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

UMTS_BAND_V = TRUE
  # Description:
  #   Indicates whether UE support UMTS FDD band V
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

UMTS_BAND_VI = FALSE
  # Description:
  #   Indicates whether UE support UMTS FDD band VI
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

UMTS_BAND_VII = FALSE
  # Description:
  #   Indicates whether UE support UMTS FDD band VII
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

UMTS_BAND_VIII = TRUE
  # Description:
  #   Indicates whether UE support UMTS FDD band VIII
  # Option Values:
  #   TRUE: UE supports UMTS FDD band VIII
  #   FALSE: UE not supports UMTS FDD band VIII
  # Switch-ability:
  #   [Any] -> [Any]

UMTS_BAND_IX = FALSE
  # Description:
  #   Indicates whether UE support UMTS FDD band IX
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

UMTS_BAND_X = FALSE
  # Description:
  #   Indicates whether UE support UMTS FDD band X
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ TD-SCDMA
# ------------------------------ File system and System service
KAL_TRACE_OUTPUT = FULL
  # Description:
  #   This option is used to enable or disable the trace output for the concern of saving ROM size
  # Option Values:
  #   CUST_PARTIAL: Enable partial traces
  #   NONE: Disable all trace
  #   FULL: Enable all trace
  # Switch-ability:
  #   NONE -> CUST_PARTIAL
  #   NONE -> FULL

PARTIAL_TRACE_LIB = NONE
  # Description:
  #   When module was defined in PARTIAL_TRACE_LIB, the compile option, TRACE_MODULE_OFF, will be un-defined, and it will be buit with TRACE information.
  # Option Values:
  #   N/A
  # Switch-ability:
  #   Non-switchable

KAL_RECORD_BOOTUP_LOG = FALSE
  # Description:
  #   The goal is to print out log via Catcher at the initialization stage. We supply the API for customers and print out each initialization milistone by default. With this facility, customers who do not have Trace32 can easily locat where the problem is via UART at MAUI initialization stage.
  # Option Values:
  #   TRUE: enable boot up trace
  #   FALSE: disable boot up trace
  # Switch-ability:
  #   [Any] -> [Any]

DRV_DEBUG_MEMORY_TRACE_SUPPORT = TRUE
  # Description:
  #   Driver do not want to depend on MMI_FEATURES_SWITCH because sometimes we do need to use memory trace debug on memory monitored project.
  # Option Values:
  #   TRUE: driver will log the trace in memory
  #   FALSE: driver won't log the trace in memory
  # Switch-ability:
  #   TRUE -> FALSE

# ------------------------------ HW and Project
# ------------------------------ File system and System service
MULTIPLE_BINARY_FILES = FALSE
  # Description:
  #   This feature option indicate resource binary could be replaced / changed after first time download.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

ZIMAGE_SUPPORT = FALSE
  # Description:
  #   Please refer to CR MAUI_02832638 which was applied to 10AW1041OFD_53EL_SLIM
  # Option Values:
  #   TRUE: Enable code compression to utilize the free RAM space and save ROM space
  #   FALSE: Disable code compression mechanism
  # Switch-ability:
  #   Non-switchable

# ------------------------------ Flash
NAND_FLASH_BOOTING = NONE
  # Description:
  #   Specify if system has to boot from nand-flash.
  #   If yes, bootloader will be built and MAUI will be splitted into
  #   primary and secondary MAUI.
  # Option Values:
  #   NONE: Disable this feature
  #   BASIC: Only Primary MAUI and Secondary MAUI exist
  #   ONDEMAND: Use demand paging mechanism
  # Switch-ability:
  #   Non-switchable

NOR_SUPPORT_RAW_DISK = FALSE
  # Description:
  #   Purpose
  #   (1) Support direct access API for NOR flash.
  #   (2) Support at most 2 additional raw disk regions on NOR flash.
  #   (3) Support flexible settings of disk base address and length. Only need to have disk range aligned with block size.
  #   Usage
  #   (1) Set to TRUE.
  #   (2) Should modify custom_memorydevice.h NOR_BOOTING_NOR_DISK_NUM & NOR_BOOTING_NOR_DISKX_BASE_ADDRESS & NOR_BOOTING_NOR_DISKX_SIZE  to assign disk region.
  #   Limitation
  #   (1) In V32/V33 project, NOR_BOOTING_NOR_DISK_NUM should be 2 and the second disk is treated as Security Partition.
  # Option Values:
  #   TRUE: Enable NOR RAW disk support
  #   FALSE: Disable NOR RAW disk support
  # Switch-ability:
  #   [Any] -> [Any]

OTP_SUPPORT = FALSE
  # Description:
  #   Support One-Time-Programming area for the flash device
  # Option Values:
  #   TRUE: Enable One-Time Program Support
  #   FALSE: Disable One-Time Program Support
  # Switch-ability:
  #   TRUE -> FALSE

COMBO_MEMORY_SUPPORT = FALSE
  # Description:
  #   To support different memory part in the same load.
  # Option Values:
  #   TRUE: Enable combo memory support
  #   FALSE: Disable combo memory support.
  #          Default setting is FALSE.
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Memory card
# ------------------------------ Application
# ------------------------------ Tools
# ------------------------------ Application
# ------------------------------ Tools
TST_WRITE_TO_FILE = FALSE
  # Description:
  #   enable tst outputting traces to files
  # Option Values:
  #   TRUE: Enable
  #   FALSE: Disable
  # Switch-ability:
  #   FALSE -> TRUE
  #   TRUE -> FALSE

DRV_CUSTOM_TOOL_SUPPORT = TRUE
  # Description:
  #   To enable using driver customization tool for GPIO/EINT/ADC/UEM/Keypad/PMU setting.
  # Option Values:
  #   TRUE: Enable
  #   FALSE: Disable
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Java
J2ME_SUPPORT = NONE
  # Description:
  #   Modify J2ME_SUPPORT options to meet future 3rd party VM solution
  # Option Values:
  #   J2ME_LIB: Not supported now
  #   NONE: Not support Java
  #   IJET: RollTech Java solution
  #   JBLENDIA: Aplix Java solution
  #   JBED: Not supported now
  #   MTK_DUMMY_MVM: J2ME_VM can be NONE. Not supported now.
  # Switch-ability:
  #   Non-switchable

J2ME_VM = NONE
  # Description:
  #   J2ME_VM     = HI_VM  # J2ME VM:  NONE, KVM, KVM_SOURCE, HI_VM, HI_VM_SOURCE, JAZELLE
  # Option Values:
  #   JAZELLE: Phase out
  #   HI_VM_SOURCE: VM sources (Used by VM vendor)
  #   NONE: Java is disable
  #   HI_VM: Hot spot VM
  #   KVM_SOURCE: VM sources (Used by VM vendor)
  #   KVM: Kilo bytes VM
  # Switch-ability:
  #   Non-switchable

J2ME_MAX_LAUNCH_NUM = 0
  # Description:
  #   Define the Maximum number of VM that can be executed concurrently
  # Option Values:
  #   4: Java concurrent VM count is 4
  #   1: Java concurrent VM count is 1
  #   3: Java concurrent VM count is 3
  #   2: Java concurrent VM count is 2
  #   5: Java concurrent VM count is 5
  # Switch-ability:
  #   Non-switchable

J2ME_SHARE_MED_EXT_MEM = FALSE
  # Description:
  #   (1) When J2ME_SHARE_MED_EXT_MEM = TRUE, Java will share Media Memory,
  #   and therefore it will decrease the required RAM size. In experiment,
  #   the RAM size decreases about 300k~400k bytes in MT6219, MT6225.
  #   (2) When J2ME_SHARE_MED_EXT_MEM = TRUE, Java can not support video/camera/recording for MMAPI. Background running will also be disabled. If we enable both J2ME_SHARE_MED_EXT_MEM and J2ME_SLIM_MEMORY_SUPPORT, Java background running will support with some limitation. Please check J2ME_SLIM_MEMORY_SUPPORT for more information.
  # Option Values:
  #   TRUE: Java will use MED memory to save overall memory. Java can not support Video in this case
  #   FALSE: Java will use ASM to allocate memory.
  # Switch-ability:
  #   Non-switchable

SUPPORT_BACKGROUND_JAVA = FALSE
  # Description:
  #   FALSE: disable background running JAVA only applied on Jblendia
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

SUPPORT_JAVA_NOKIA_UI = FALSE
  # Description:
  #   On Aplix solution (J2ME_SUPPORT = JBLENDIA), Once SUPPORT_JAVA_NOKIA_UI is TRUE, SUPPORT_JSR_75_PIM, SUPPORT_JSR_205 must be true and J2ME_FEATURE_SET must be JTWI. Aplix kernal libraries will be the files under \mcu\j2me\vm\Jblendia\lib\CLDCHI_JTWI10_FC_
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

SUPPORT_JSR_75_PIM = FALSE
  # Description:
  #   J2ME FEATURE SETTINGS
  #   MUST TURN ON JTWI FEATURESET or NO USE
  #   SUPPORT_JSR_75_PIM/SUPPORT_JSR_205/SUPPORT_INFUSIO is TRUE
  #   SUPPORT_JSR_184/SUPPORT_SURFKITCHEN is FALSE
  # Option Values:
  #   TRUE: Support JSR-75 PIM
  #   FALSE: Turn off JSR-75 PIM
  # Switch-ability:
  #   Non-switchable

SUPPORT_JSR_75_PIM_MSA = FALSE
  # Description:
  #   This feature option is used to support MSA 1.0 clarifications for JSR-75 PIM. Enabling this compile option will enable TITLE, URL, ADDR, NOTE, and 2nd EMAIL of Contact. Please note that to enable the compile option may enlarge the memory usage.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

SUPPORT_JSR_179 = FALSE
  # Description:
  #   Enable Java JSR179 location API
  #   Please add this compile option under SUPPORT_JSR_82 in project.mak
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

SUPPORT_JSR_205 = FALSE
  # Description:
  #   J2ME FEATURE SETTINGS
  #   MUST TURN ON JTWI FEATURESET or NO USE
  #   SUPPORT_JSR_75_PIM/SUPPORT_JSR_205/SUPPORT_INFUSIO is TRUE
  #   SUPPORT_JSR_184/SUPPORT_SURFKITCHEN is FALSE
  # Option Values:
  #   TRUE: Support JSR-205
  #   FALSE: Turn off JSR-205
  # Switch-ability:
  #   Non-switchable

# ------------------------------ VoIP
# ------------------------------ Wifi
WIFI_PREFER_SUPPORT = FALSE
  # Description:
  #   To enable select WiFi first if WiFi is available.
  # Option Values:
  #   TRUE: Select WiFi first if WiFi is available
  #   FALSE: close WiFi prefer
  # Switch-ability:
  #   [Any] -> [Any]

MMI_HIDE_WAPI = FALSE
  # Description:
  #   Turn off MMI WAPI support
  # Option Values:
  #   TRUE: Turn off MMI WAPI support
  #   FALSE: Turn on MMI WAPI support
  # Switch-ability:
  #   [Any] -> [Any]

# ------------------------------ Memory card
# ------------------------------ Wifi
# ------------------------------ Operator
DCD_SUPPORT = NONE
  # Description:
  #   To enable/disable DCD (dynamic content delivery) feature.
  #   NONE: to disable DCD
  #   INFRAWARE: to  enable DCD by using vendor name.
  #   If using other vendor's engine, there will be more option values.
  # Option Values:
  #   INFRAWARE: Enable DCD
  #   NONE: Disable DCD
  # Switch-ability:
  #   INFRAWARE -> NONE

# ------------------------------ TC01
# ------------------------------ Others
# =========================================================================
# Feature Options which customer can NOT modify
# =========================================================================
#[Non-modifiable]# ------------------------------ ARM
COMPILER = RVCT
  # Description:
  #   RVCT or ADS
  # Option Values:
  #   ADS: Use ARM ADS tool chain.
  #   RVCT: Use ARM RVCT tool chain
  #   GCC: Use GNU GCC tool chain (Mentor Sourcery 2010.09 , 4.5.1)
  # Switch-ability:
  #   Non-switchable

RVCT_MULTI_FILE = NONE
  # Description:
  #   Set RVCT feature MULTI_FILE. RVCT is a compiler
  # Option Values:
  #   NONE: Disable --multifile compiler argument .
  #   MULTI: Enable --multifile compiler argument to compile specified source files into one object file.
  # Switch-ability:
  #   Non-switchable

RTOS = NUCLEUS_V2
  # Description:
  #   We will support two RTOS: NUCLEUS and THREADX. The users can select one of them in their proejcts.
  # Option Values:
  #   THREADX: Use ThreadX as the OS
  #   NUCLEUS_V2: Use Nucleus 2.x as the OX
  #   NUCLEUS: Use Nucles 1.13 as the OS
  # Switch-ability:
  #   Non-switchable

RTOS_DEBUG = TRUE
  # Description:
  #   We will support two RTOS: NUCLEUS and THREADX. The users can select one of them in their proejcts.
  # Option Values:
  #   TRUE: Enable OnlineSST OS diagnosis
  #   FALSE: Disable OnlineSST OS diagnosis
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ HW and Project
MCU_CLOCK = MCU_520M
  # Description:
  #   Specify MCU clock rate,
  #   MT6205B = 26MHz (MCU_26M)
  #   MT6218B/19, MT6226/7, MT6223 = 52MHz (MCU_52M)
  #   MT6228, MT6229, MT6230, MT6225 = 104MHz (MCU_104M)
  #   MT6235, MT6238, MT6239 = 208MHz (MCU_208M)
  # Option Values:
  #   MCU_39M: CPU normal operation frequency:39MHz
  #   MCU_245_76M: CPU normal operation frequency:245.76MHz
  #   MCU_208M: CPU normal operation frequency: 208MHz
  #   MCU_312M: CPU normal operation frequency: 312MHz
  #   MCU_52M: CPU normal operation frequency:52MHz
  #   MCU_104M: CPU normal operation frequency: 104MHz
  #   MCU_520M: CPU normal operation frequency: 520MHz
  #   MCU_122_88M: CPU normal operation frequency: 122.88MHz
  #   MCU_13M: CPU normal operation frequency:13MHz
  #   MCU_650M: CPU normal operation frequency: 650MHz
  #   MCU_26M: CPU normal operation frequency:26MHz
  #   MCU_416M: CPU normal operation frequency: 416Mhz
  # Switch-ability:
  #   Non-switchable

MCU_DCM = DCM_ENABLE
  # Description:
  #   to enable Dynamic Clock Switching (MCU_DCM) function for saving the current consumption. Please set DCM_ENABLE to turn on this feature. (DCM_ENABLE <-> DCM_DISABLE) DCM_ENABLE: Need correct EMI configurations.
  # Option Values:
  #   DCM_ENABLE: Enable DCM support
  #   DCM_DISABLE: Disable DCM support
  # Switch-ability:
  #   Non-switchable

CHIP_VER = S00
  # Description:
  #   Chip version, changed according to ECO.
  # Option Values:
  #   S01:
  #   S00:
  #   S02:
  # Switch-ability:
  #   Non-switchable

PLATFORM = MT6577
  # Description:
  #   Name of BB-chip.
  # Option Values:
  #   NA:
  # Switch-ability:
  #   Non-switchable

DEMO_PROJECT = FALSE
  # Description:
  #   To define projects that are experimental or for demo purposes only. It should NEVER be enabled on MP projects, and it is used by MTK internally. Customer does not need to change it
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

FLAVOR = NONE
  # Description:
  #   To describe what flavor setting was used in internal project.
  # Option Values:
  #   NA:
  # Switch-ability:
  #   Non-switchable

UART_CHARGER_THE_SAME_INTERFACE = NONE

FM_RADIO_CHIP = NONE
  # Description:
  #   To determine which FM chip will be used.
  # Option Values:
  #   MT6189A1:
  #   MT6188A1:
  #   MT6276FM:
  #   AR1000:
  #   MT6626:
  #   MT6256FM:
  #   MT6616:
  #   MT6251FM:
  #   NONE: This project doesn't support FM radio
  # Switch-ability:
  #   Non-switchable

FM_INTERFACE_TYPE = I2C
  # Description:
  #   Before, FM Chip use I2C or SPI, but new MT6616 FM Chip support HCI Interface, we need add compile option to dedicate HCI to MT6616.
  # Option Values:
  #   HCI:
  #   I2C:
  #   NONE: other control interface: FSPI
  # Switch-ability:
  #   Non-switchable

FM_RADIO_I2S_PATH = FALSE
  # Description:
  #   Used to determine the audio path of FM radio
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

FM_RADIO_HW_SEARCH = FALSE
  # Description:
  #   if FM HW support HW search, this option can be set to true or false, or it should be set to false
  # Option Values:
  #   FALSE: FM use SW search
  #   TRUE: FM use HW search
  # Switch-ability:
  #   TRUE -> FALSE

FM_RADIO_RECORD = FALSE
  # Description:
  #   Turn on FM radio record feature
  # Option Values:
  #   TRUE: Turn on FM Radio record function
  #   FALSE: Turn off FM radio record function
  # Switch-ability:
  #   TRUE -> FALSE

AFC_VCXO_TYPE = VCTCXO
  # Description:
  #   the oscillator type used in this project
  # Option Values:
  #   VCTCXO: VCTCXO clk
  #   VCXO: VCXO clk
  # Switch-ability:
  #   Non-switchable

EXT_CLOCK = EXT_26M
  # Description:
  #   Specify external clock rate.
  # Option Values:
  #   EXT_26M:
  #   EXT_13M:
  # Switch-ability:
  #   Non-switchable

PRODUCTION_RELEASE = TRUE
  # Description:
  #   Specify if production released is turned on.
  #   If it is enabled, some system code will be effected, like
  #   1. Watchdog enabled, and must be restared each time context switch;
  #   2. Handset silent reboot if system comes across with exception;
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

DSP_SOLUTION = DUALMACDSP
  # Description:
  #   This option is used to configure whether DSP core is enabled by ARM. If  DSP core functionality is required, specify the suitable DSP type for this option. Otherwise, use NONE for this option
  # Option Values:
  #   NONE:
  #   DUALMACDSP: FCore related chips.
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Audio
MMF_DECODE = FALSE
  # Description:
  #   Control MMF
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ HW and Project
DSPIRDBG = FALSE
  # Description:
  #   To enable IRDBG interface for dsp debugging
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Connectivity
BT_FAX_PROFILE = FALSE
  # Description:
  #   Turn on/off Bluetooth FAX profile feature.
  # Option Values:
  #   FALSE: Disable this feature
  #   TRUE: Enable this feature
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ HW and Project
#[Non-modifiable]# ------------------------------ Connectivity
BT_SIM_PROFILE = FALSE
  # Description:
  #   Turn on/off Bluetooth SIMAP profile feature.
  # Option Values:
  #   FALSE: Disable this feature
  #   TRUE: Enable this feature
  # Switch-ability:
  #   TRUE -> FALSE

BT_SPP_CLIENT = NONE
  # Description:
  #   Turn on/off Bluetooth SPP CLient feature
  # Option Values:
  #   NONE: Disable this feature
  #   BT_SPP_CLI_NO_SCO: If set this, it means open normal SPP Client feature and could connect remote SPP Server.
  #   BT_SPP_CLI_WITH_SCO: If set this, it means open normal SPP Client feature with extra feature to create SCO link with remote.
  # Switch-ability:
  #   BT_SPP_CLI_NO_SCO -> NONE

#[Non-modifiable]# ------------------------------ HW and Project
HW_PFC_SUPPORT = TRUE
  # Description:
  #   PPP Framer use HW or SW
  # Option Values:
  #   TRUE: use HW PFC
  #   FALSE: use SW PFC
  # Switch-ability:
  #   Non-switchable

IC_TEST_TYPE = NONE
  # Description:
  # Option Values:
  #   N/A
  # Switch-ability:
  #   N/A

INTERNAL_ANTENNAL_SUPPORT = FALSE
  # Description:
  #   if FM HW support S.Ant, this option can be set to true or false, or it should be set to false
  # Option Values:
  #   FALSE: only support long antenna
  #   TRUE: support both long and short antenna
  # Switch-ability:
  #   TRUE -> FALSE

LOW_COST_SUPPORT = NONE
  # Description:
  #   General compile option for Low Cost features. Please not that a module can define its own low-cost levels, or it can just adopt
  # Option Values:
  #   TRUE:
  #   NONE:
  #   FALSE:
  #   ULC:
  #   COMMON:
  # Switch-ability:
  #   Non-switchable

MTK_DSP_DEBUG = FALSE

MTK_SLEEP_ENABLE = TRUE
  # Description:
  #   Sleep mode enable or not
  # Option Values:
  #   TRUE: sleep mode enable
  #   FALSE: sleep mode disable
  # Switch-ability:
  #   Non-switchable

PMIC = MT6329
  # Description:
  #   To specify which Power Management IC(PMIC) is used, or no PMIC.
  # Option Values:
  #   MT6235PMU:
  #   MT6318:
  #   MT6238PMU:
  #   MT6305:
  #   NONE: No PMU/PMIC on PCB
  #   MT6326_CCCI:
  #   MT6253PMU:
  #   MT6326:
  #   MT6258PMU:
  #   MT6253ELPMU:
  #   MT6223PMU:
  #   MT6276PMU:
  #   MT6236PMU:
  #   MT6573PMU:
  #   MT6255PMU: For MT6255 Usage
  # Switch-ability:
  #   Non-switchable

PMU_WITH_EXTERNL_CHARGER = FALSE

NOT_BENEFIT_FROM_BATTERY_CONSUMPTION_OPTIMISATION = FALSE
  # Description:
  #   FALSE: UE benefits from battery consumption optimisation mechanism from network, e.g. mobile phone   # TRUE: UE doesn't benefit from battery consumption optimisation mechanism from network, e.g. data card
  # Option Values:
  #   TRUE: UE doesn't benefit from battery consumption optimisation mechanism from network, e.g. data card
  #   FALSE: UE benefits from battery consumption optimisation mechanism from network, e.g. mobile phone
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ LCD
MDP_LCD_FW_UPDATE = FALSE

MAIN_LCM_SCANLINE_ROTATION = BY_LCD_SIZE_SETTING
  # Description:
  #   This refers to the scanline rotation supportness for Main LCM
  # Option Values:
  #   FALSE: Disable this feature
  #   FALSE: Disable this feature
  #   TRUE: Enable this feature
  #   TRUE: Enable this feature
  # Switch-ability:
  #   [Any] -> [Any]

#[Non-modifiable]# ------------------------------ Connectivity
BT_SPP_SERVER = NONE
  # Description:
  #   Bluetooth SPP proilfe Server role
  # Option Values:
  #   NONE: If set this, it means SPP Server is not support.
  #   BT_SPP_SRV_NO_SCO: If set this, it means open normal SPP server feature and could accept remote SPP connection request.
  #   BT_SPP_SRV_WITH_SCO: If set this, it means open noemal SPP feature with extra feature to create SCO link with remote device.
  # Switch-ability:
  #   BT_SPP_SRV_NO_SCO -> NONE

#[Non-modifiable]# ------------------------------ LCD
SUB_LCM_SCANLINE_ROTATION = BY_LCD_SIZE_SETTING
  # Description:
  #   This refers to the scanline rotation supportness for Sub LCM
  # Option Values:
  #   FALSE: Disable this feature
  #   FALSE: Disable this feature
  #   TRUE: Enable this feature
  #   TRUE: Enable this feature
  # Switch-ability:
  #   [Any] -> [Any]

SUBLCD_SYNC_LCM_METHOD = NONE
  # Description:
  #   Set this feature SUBLCD_SYNC_LCM_METHOD to SW or HW according to real HW layout connection
  # Option Values:
  #   SW:
  #   NONE:
  #   HW:
  # Switch-ability:
  #   [Any] -> [Any]

SUBLCD_DEDICATED_KEYS_SUPPORT = FALSE

LANDSCAPE_UI_ON_PORTRAIT_LCM = FALSE

LANDSCAPE_UI_ON_PORTRAIT_LCM_CCW_ROTATION_ANGLE = 270

LANDSCAPE_UI_ON_PORTRAIT_LCM_SLIM_VERSION = FALSE

SPATIAL_DITHERING = FALSE
  # Description:
  #   Dithering is an intentionally applied form of noise, used to randomize quantization error.
  #   Dithering is routinely used in processing of both digital audio and video data.
  #   In our case, the spatial dithering technique is applied on image decoding.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

SYNC_LCM_SUPPORT = FALSE
  # Description:
  #   COMPILER OPTION FOR SUPPORTING SYNC MODE LCM
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   TRUE -> FALSE

#[Non-modifiable]# ------------------------------ MMI
MMI_BASE = PLUTO_MMI
  # Description:
  #   Set MMI_BASE as PLUTO_MMI or NEPTUNE_MMI
  # Option Values:
  #   NEPTUNE_MMI:
  #   PLUTO_MMI:
  # Switch-ability:
  #   Non-switchable

MMI_FEATURES_SWITCH = FALSE
  # Description:
  #   MMI_FEATURES_SWITCH = TRUE means it's a memory reduction project, and the specified MMI_features_switch.h will be used, but USE_COMMON_MMI_FEATURES_SWITCH won't be defined.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

EMPTY_RESOURCE = TRUE
  # Description:
  #   Add a EMPTY_RESOURCE compile option in make file
  # Option Values:
  #   TRUE: For customer release
  #   FALSE: For internal demo project release
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Connectivity
#[Non-modifiable]# ------------------------------ Input and Language
FONT_ENGINE = NONE
  # Description:
  #   Set font engine type
  # Option Values:
  #   FONT_ENGINE_WTLE: Monotype WTLE Font
  #   FONT_ENGINE_FREETYPE_DEMO: Demo Freetype Font
  #   FONT_ENGINE_FREETYPE: Freetype Font
  #   FONT_ENGINE_MONOTYPE_DEMO: Demo Monotype Font
  #   NONE: Bitmap Font
  #   FONT_ENGINE_MONOTYPE: Monotype Font
  # Switch-ability:
  #   Non-switchable

FONT_RESOURCE = OFFICIAL
  # Description:
  #   To add compile option for demo font resource
  # Option Values:
  #   DEMO: Demo Bitmap Font Data
  #   OFFICIAL: Official Bitmap Font Data
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Application
AOC_SUPPORT = TRUE
  # Description:
  #   3GPP feature. TS.22.024, 22.086
  #   User can see the call cost in the MENU, and if call cost is larger than the upper limit stored in the SIM card , call will be ended.  (Under the constrient that NW. has sent AOCC in the peer MSG. )   . AOCC is a information about how to charge the call per uint.
  # Option Values:
  #   TRUE: Turn on AOC
  #   FALSE: Turn off AOC
  # Switch-ability:
  #   Non-switchable

APP_STORAGE_IN_SYS_DRV = FALSE
  # Description:
  # Option Values:
  #   N/A
  # Switch-ability:
  #   N/A

BROWSER_SUPPORT = NONE
  # Description:
  #   This feature option is used to set browser solution of one project.
  # Option Values:
  #   OPERA_TURBO: Set Browser solution as Opera Turbo
  #   OPERA_V10: Set browser solution as Opera Mobile V10
  #   OBIGO_Q03C: Set browser solution as Obigo Q03C Browser
  #   OBIGO_Q05A: Set browser solution as Obigo Q05A Browser
  #   NONE: Disable this feature
  # Switch-ability:
  #   Non-switchable

OBIGO_FEATURE = NONE
  # Description:
  #   It defines the capability of Obigo Q03C/Q05A WAP and Opera Mobile Browser solution. If feature option OBIGO_FEATURE is NONE, it means no WAP solution is enabled.
  # Option Values:
  #   NONE:
  #   WAP2_SEC_MMS:
  #   WAP_SEC:
  #   WAP_MMS:
  #   WAP2:
  #   WAP:
  #   WAP_SEC_MMS:
  #   WAP2_MMS:
  #   WAP2_SEC:
  # Switch-ability:
  #   Non-switchable

WAP_MEM_DEBUG = TRUE
  # Description:
  #   WAP_MEM_DEBUG = TRUE # TRUE, FALSE. Works only for WAP_SUPPORT = OBIGO_Q03C
  #   When WAP_SUPPORT=OBIGO_Q05A, WAP_MEM_DEBUG = TRUE or FALSE will not have any effect.
  #   And when WAP_SUPPORT=OBIGO_Q03C, WAP_MEM_DEBUG = TRUE or FALSE will work.
  # Option Values:
  #   TRUE: It will enable debug memory pool, so the memory requirement of Q03C browser will get increase.
  #   FALSE: It will disable debug memory pool, so the memory requirement of Q03C browser will get decrease.
  # Switch-ability:
  #   Non-switchable

UNIFIED_PUSH_FEATURES = NONE
  # Description:
  #   This feature option is used to enable or disable the push feature.
  # Option Values:
  #   SLIM: It will disable white list and black list (trust list) management
  #   NONE: WAP Push disable
  #   STANDARD: It will enable  only white list and not black list (trust list) management
  #   FULL: It will enable white list and black list (trust list) management
  # Switch-ability:
  #   Non-switchable

HTTP_INTERFACE_SUPPORT = FALSE
  # Description:
  #   If this compile option is turned on, we export the browser's HTTP protocol for the other handset's applications. Our HTTP interface is supported by the WAP Protocol Service (WPS) task. Generally, when the Obigo browser solution is turned on, WPS task is turned on such that other applications, e.g., SyncML, IMPS, and Java, can use WPS to send and receive HTTP messages. However, for slim projects, it is possible that except the browser, no application needs HTTP interface. In this situation, this compile option can be turned off to save the memory.
  # Option Values:
  #   TRUE: HTTP interface, i.e., WPS task, is turned on.
  #   FALSE: HTTP interface, i.e., WPS task, is turned off.
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ HW and Project
E_COMPASS_SENSOR_SUPPORT = NONE
  # Description:
  # Option Values:
  #   N/A
  # Switch-ability:
  #   N/A

#[Non-modifiable]# ------------------------------ Connectivity
USB_IN_META_SUPPORT = FALSE
  # Description:
  #   This option is used to turn on Meta mode bu using USB.
  # Option Values:
  #   TRUE: support by using USB
  #   FALSE: can only use UART
  # Switch-ability:
  #   TRUE -> TRUE

#[Non-modifiable]# ------------------------------ HW and Project
MOTION_SENSOR_SUPPORT = NONE
  # Description:
  #   To enable motion sensor support.
  #   And Describe which sensor chip in use.
  # Option Values:
  #   KXP74_SPI: moiton sensor is KXP74 and using SPI interface.
  #   KXP84_I2C: motion sensor is KXP84 and using I2C interface.
  #   KXP84_SPI: motion sensor is KXP84 and using SPI interface.
  #   KXTF9_I2C: motion sensor is KXTF9 and using I2C interface.
  #   BMA220_I2C: motion sensor is BMA220 and using I2C interface.
  #   MMA7455_I2C: motion sensor is MMA7455 and using I2C interface.
  #   MMA7660_I2C: motion sensor is MMA7660 and using I2C interface.
  #   MXC6202_I2C: motion sensor is MXC6202 and using I2C interface.
  #   NONE: Disable this feature
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Connectivity
#[Non-modifiable]# ------------------------------ Application
CUST_KEYPAD_TONE_SUPPORT = FALSE
  # Description:
  #   enable/disable cust keypad tone
  # Option Values:
  #   TRUE: Turn on customized keypad tone
  #   FALSE: Turn off customized keypad tone
  # Switch-ability:
  #   [Any] -> [Any]

DRM_SUPPORT = NONE
  # Description:
  #   Provides feature of DRM.
  # Option Values:
  #   NONE: If DRM not support, value will be NONE
  #   MTK: If DRM 1.0 is supported, value will be MTK
  # Switch-ability:
  #   N/A

DRM_VERSION = NONE
  # Description:
  #   Provides information about DRM version supported
  # Option Values:
  #   NONE: Value will be NONE if DRM is not supported.
  #   V01: If DRM 1.0 is supported, version will be V01
  # Switch-ability:
  #   N/A

DRM_SECURE_CLOCK = FALSE
  # Description:
  # Option Values:
  #   N/A
  # Switch-ability:
  #   N/A

DRM_SUPPORT_DB_SIZE = NONE
  # Description:
  #   Provides information about number of licenses which can be supported in license database.
  # Option Values:
  #   256: Can support upto 256 licenses
  #   512: Can support upto 512 licenses
  #   1024: Can support upto 1024 licenses
  #   64: Can support upto 64 licenses
  #   NONE: Should be NONE if DRM SUPPORT is not present.
  # Switch-ability:
  #   Non-switchable

DRM_SUPPORT_SCAN_DISK = FALSE
  # Description:
  #   DRM supports scan disk or not
  # Option Values:
  #   FALSE: Scan disk is not supported.
  #   TRUE: Scan disk is supported
  # Switch-ability:
  #   N/A

FOTA_ENABLE = NONE
  # Description:
  #   Turn on FOTA (firmware update over the air) feature, support full image firmware update now
  # Option Values:
  #   NONE: FOTA is disable
  #   FOTA_DM: Support FOTA
  # Switch-ability:
  #   Non-switchable

FOTA_UPDATE_PACKAGE_ON_NAND = FALSE
  # Description:
  #   Use NAND to place update package for NOR project
  # Option Values:
  #   TRUE: Enable FOTA update package in NAND for NOR project
  #   FALSE: Place diff package on NOR
  # Switch-ability:
  #   Non-switchable

GAME_ENGINE = NONE

SUPPORT_INFUSIO = FALSE

GADGET_SUPPORT = NONE
  # Description:
  #   Web widget feature
  # Option Values:
  #   NONE:
  #   STANDARD:
  #   SHARED_MEMORY:
  # Switch-ability:
  #   TRUE -> FALSE

MERCURY_SLAVE_BT_SUPPORT = FALSE

#[Non-modifiable]# ------------------------------ Connectivity
GPS_SUPPORT = MT3326
  # Description:
  #   Mapping with GPS CHIP, Such as MT3336       # NONE, MT3336
  #   MT3326 will be phase out after 11BW1144MP
  # Option Values:
  #   NONE: NO GPS CHIP Bind with Baseband
  #   MT3336: GPS CHIP MT3336
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Application
QQMOVIE_SUPPORT = FALSE

#[Non-modifiable]# ------------------------------ Connectivity
#[Non-modifiable]# ------------------------------ Application
#[Non-modifiable]# ------------------------------ Multimedia
IMAGE_VIEWER_VER = STANDARD
  # Description:
  #   Version of ImageViewer APP.
  # Option Values:
  #   SLIM: Simple version. Not support zoom and tool panel.
  #   STANDARD: Standard version. Support zoom and tool panel.
  #   EX: Ex version. Support zoom, tool panel with fancy effect. Use default cache memory allocation machenism.
  #   EX_DB: Ex with DB cache and cache memory use default allocation machenism.
  #   EX_SHARE_MEDMEM: Ex with cache memory allocated from MED pool.
  #   EX_DB_SHARE_MEDMEM: EX_DB with cache memory allocated from MED pool.
  #   NONE: With no imageviewer. The value cannot be set currently.
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ HW and Project
JOGBALL_SUPPORT = FALSE
  # Description:
  #   Support Jogball or OFN (Optical Finger Navigation) to replace navigation keys
  # Option Values:
  #   JOGBALL: HW support for Jogball
  #   OFN: HW support for OFN
  #   NONE: Disable this feature
  # Switch-ability:
  #   [Any] -> [Any]

#[Non-modifiable]# ------------------------------ Application
EMS_SUPPORT = EMS_NONE
  # Description:
  #   There are four level for EMS functionality, EMS 5.0, EMS 4.0, slim EMS (pure text only), and ultra low cost slim EMS (currently only can be used in PLUTO MMI). This feature option is to set what level of the EMS functionality. It will impact three library mmi (plutommi or lcmmi), ems, and custom.
  # Option Values:
  #   EMS_50: support EMS 5.0
  #   EMS_SLIM: support only pure text content.
  #   EMS_40: support EMS 4.0
  #   EMS_ULC_SLIM: remove PLUTO MMI non used part, only remain support concatenate, port, R8 language handling
  #                 (Currently only support Plutommi, other MMI version can't use!)
  # Switch-ability:
  #   Non-switchable

SQLITE3_SUPPORT = FALSE
  # Description:
  # Option Values:
  #   N/A
  # Switch-ability:
  #   N/A

TCPIP_LOOP_SUPPORT = FALSE
  # Description:
  #   When TCPIP_LOOP_SUPPORT is TRUE, __TCPIP_LOOP_BACK__ will be defined. WAP routes all the requests for "localhost" to 127.0.0.1.
  # Option Values:
  #   TRUE: Enable LOOPBACK.
  #   FALSE: Disable LOOPBACK.
  # Switch-ability:
  #   Non-switchable

TCPIP_SUPPORT = NONE
  # Description:
  #   To enable TCP/UDP/IP.
  # Option Values:
  #   NONE: Disable TCPIP
  #   TCP: Support TCP only.
  #   UDP_TCP: Support TCP and UDP.
  #   UDP: Use UDP.
  # Switch-ability:
  #   Non-switchable

DHCPD_SUPPORT = FALSE
  # Description:
  #   To enable dhcp server.
  # Option Values:
  #   TRUE: Support DHCP server.
  #   FALSE: Disable dhcp server.
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Audio
EXTERNAL_AMPLIFIER_ROUTE_CONFIGURATION = FALSE
  # Description:
  #   configure external amplifier router
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   [Any] -> [Any]

#[Non-modifiable]# ------------------------------ Application
UNIFIED_MESSAGE_LOW_MEMORY_SUPPORT = FALSE

#[Non-modifiable]# ------------------------------ Speech
NVRAM_AFE_DIGITAL_GAIN_SUPPORT = FALSE
  # Description:
  #   NVRAM afe digital gain control
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   [Any] -> [Any]

#[Non-modifiable]# ------------------------------ Wifi
HOTSPOT_SUPPORT = FALSE
  # Description:
  #   Enable or disable hotspot feature.
  # Option Values:
  #   TRUE: Enable the hotspot feature.
  #   FALSE: disable the hotspot feature.
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Application
XDM_SUPPORT = FALSE

SUPPORT_SURFKITCHEN = FALSE

#[Non-modifiable]# ------------------------------ Connectivity
BLUETOOTH_VERSION = NONE
  # Description:
  #   descides which Bleutooth version is used
  # Option Values:
  #   BT_VER_PRE21: If this value is set, it means the Bluetooth SPEC version supported by Bluetooth stack is older than 2.1.
  #   NONE: If Bluetooth is not supported(the BLUETOOTH_SUPPORT set to NONE). This option set to NONE also.
  #   BT_VER_30: If this value is set, it means Bluetooth stack supports Bluetooth SPEC version 3.0.
  #   BT_VER_21: If this value is set, it means Bluetooth stack supports Bluetooth SPEC version 2.1.
  # Switch-ability:
  #   N/A

BT_A2DP_PROFILE = FALSE
  # Description:
  #   To turn A2DP profile
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   N/A

BT_AVRCP_PROFILE = FALSE
  # Description:
  #   To turn on/off AVRCP profile
  # Option Values:
  #   TRUE: Turn on AVRCP Profile
  #   FALSE: Turn off AVRCP Profile
  # Switch-ability:
  #   TRUE -> FALSE

#[Non-modifiable]# ------------------------------ Audio
TWOMICNR_SUPPORT = NONE
  # Description:
  #   External two MIC support ( ForteMedia)
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   [Any] -> [Any]

#[Non-modifiable]# ------------------------------ Connectivity
BT_CONNECT_TO_UART2 = FALSE
  # Description:
  #   For non BT built-in project, BT chip can be connected to BB via UART2 or UART3.
  #   UART & BT drivers may need to know the information and change configurations.
  # Option Values:
  #   TRUE: BT is connected to UART2
  #   FALSE: BT is connected to UART3
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Application
#[Non-modifiable]# ------------------------------ Connectivity
#[Non-modifiable]# ------------------------------ Modem
3G_VIDEO_CALL = FALSE
  # Description:
  #   Enable 3g video call
  # Option Values:
  #   TRUE: 3G Video Telephony is supported
  #   FALSE: 3G Video Telephony is not supported
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Connectivity
#[Non-modifiable]# ------------------------------ Application
#[Non-modifiable]# ------------------------------ Connectivity
#[Non-modifiable]# ------------------------------ Application
PHB_ADDITIONAL_SUPPORT = FALSE
  # Description:
  #   USIM Phonebook Support
  # Option Values:
  #   FALSE: Disable this feature
  #   TRUE: Enable this feature
  # Switch-ability:
  #   [Any] -> [Any]

#[Non-modifiable]# ------------------------------ Connectivity
BT_SEQ_SUPPORT = FALSE
  # Description:
  # Option Values:
  #   N/A
  # Switch-ability:
  #   N/A

BT_AUDIO_VIA_SCO = FALSE
  # Description:
  #   To turn on/off Audio via Bluetooth SCO link feature
  # Option Values:
  #   TRUE: Turn on audio via SCO function
  #   FALSE: Turn off audio via SCO function
  # Switch-ability:
  #   TRUE -> FALSE

BT_FM_RADIO_VIA_SCO = FALSE
  # Description:
  #   To turn on/off FM Radio via Bluetooth SCO link feature
  # Option Values:
  #   TRUE: Turn on FM via SCO function
  #   FALSE: Turn off FM via SCO function
  # Switch-ability:
  #   TRUE -> FALSE

#[Non-modifiable]# ------------------------------ MMI
MAUI_SDK_TEST = FALSE
  # Description:
  #   If this feature set TRUE, MAUI SDK AVK test will be enabled.
  # Option Values:
  #   TRUE: enable MAUI SDK AVK test
  #   FALSE: disable MAUI SDK AVK test
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Connectivity
L1_GPS_REF_TIME_SUPPORT = TRUE
  # Description:
  #   To enable GPS-L1 reference fine time measurement
  # Option Values:
  #   TRUE: LI GPS timing synch feature is supported
  #   FALSE: LI GPS timing synch feature is not supported
  # Switch-ability:
  #   Non-switchable

AGPS_SUPPORT = CONTROL_PLANE
  # Description:
  #   It decides user plane or control plane is used.
  # Option Values:
  #   BOTH: Enable both user-plane and control-plane.
  #   NONE: Disable AGPS feature.
  #   CONTROL_PLANE: Enable AGPS control-plane. (only used for the modem load on smart phone project)
  #   USER_PLANE: Enable AGPS user-plane. (SUPL) Use IP network to get assistance data.
  # Switch-ability:
  #   Non-switchable

OMA_ULP_SUPPORT = NONE
  # Description:
  #   It decides which OMA ULP version is used.
  #   AGPS will acceralte GPS TTFF, and also provide LBS service framework.
  # Option Values:
  #   NONE: Disable SUPL feature. Only can be this value when AGPS_SUPPORT is NONE or CONTROL_PLANE.
  #   V1: Enable SUPL v1.0.
  # Switch-ability:
  #   Non-switchable

RRLP_VER_SUPPORT = R5
  # Description:
  #   To change RRLP protocol version for both AGPS user plane and/or control plane.
  # Option Values:
  #   NONE: Doesn't support RRLP (it also means both AGPS user plane and control plane are not supported)
  #   R5: Support RRLP for 3GPP R5
  # Switch-ability:
  #   Non-switchable

AGPS_UP_CP_CONFLICT_HANDLING_SUPPORT = TRUE
  # Description:
  #   To enable AT&T AGPS UP CP conflict handling requirements
  # Option Values:
  #   TRUE: UP/CP conflict handling mechanism is supported
  #   FALSE: UP/CP conflict handling mechanism is not supported
  # Switch-ability:
  #   Non-switchable

AGPS_CP_SIB15_SUPPORT = TRUE
  # Description:
  #   To enable UMTS RRC processing on SIB15 for AGPS
  # Option Values:
  #   TRUE: SIB15 series messages can be supported
  #   FALSE: SIB15 series messages can not be supported
  # Switch-ability:
  #   Non-switchable

GPS_HS_SUPPORT = FALSE

#[Non-modifiable]# ------------------------------ Application
RFC2507_SUPPORT = TRUE
  # Description:
  #   Support RFC2507 IP header compression / decompression in PDCP of 3G modem. (TRUE/FALSE)
  # Option Values:
  #   FALSE: Disable this feature
  #   TRUE: Enable this feature
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Connectivity
USB_SUPPORT = FALSE
  # Description:
  #   to enable USB function
  # Option Values:
  #   TRUE: Enable USB
  #   FALSE: Disable USB
  # Switch-ability:
  #   [Any] -> [Any]

USB_COM_PORT_SUPPORT = FALSE
  # Description:
  #   Open Com Port function
  # Option Values:
  #   TRUE: Enable USB comport
  #   FALSE: Disbale USB comport
  # Switch-ability:
  #   N/A

USB_COMPORT_PC_DRIVER_SUPPORT = MS
  # Description:
  #   Only if __TC01__  is defined
  #   then
  #   USB_COMPORT_PC_DRIVER_SUPPORT = TC01
  # Option Values:
  #   TC01: TC01 comport driver
  #   MS: Microsoft comport driver
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ File system and System service
#[Non-modifiable]# ------------------------------ Connectivity
USB_TETHERING = FALSE
  # Description:
  #   To replace ppp dialup.  However, almost no set-up is required.  User simply connects a USB cable from PC to the phone.  And then he can connect to the network.
  # Option Values:
  #   TRUE: Enable USB tethering function
  #   FALSE: Disable USB tethering function
  # Switch-ability:
  #   Non-switchable

BATTERY_CHARGING_SUPPORT = FALSE
  # Description:
  #   enable battery charging .
  # Option Values:
  #   TRUE: support BC1.1 spec
  #   FALSE: use MTK detection flow
  # Switch-ability:
  #   Non-switchable

OTG_SUPPORT = FALSE
  # Description:
  #   To enable OTG suuport  feature
  # Option Values:
  #   TRUE: Enable OTG
  #   FALSE: Disable OTG
  # Switch-ability:
  #   Non-switchable

PICTBRIDGE_SUPPORT = FALSE

UART3_SUPPORT = FALSE
  # Description:
  #   Support UART3
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Camera
FLASHLIGHT_MODULE = NONE

#[Non-modifiable]# ------------------------------ Connectivity
DMA_UART_VFIFO_TUNNEL_SUPPORT = FALSE
  # Description:
  #   enable the VFIFO support UART for separate tx or rx
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

DMA_UART_VIRTUAL_FIFO = TRUE
  # Description:
  #   Support UART Virtual FIFO
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Camera
SENSOR_ROTATE = SENSOR_ROTATE_0
  # Description:
  #   For different camera hw module mount direction.
  # Option Values:
  #   SENSOR_ROTATE_90:
  #   SENSOR_ROTATE_180:
  #   SENSOR_ROTATE_0:
  #   SENSOR_ROTATE_270:
  # Switch-ability:
  #   N/A

#[Non-modifiable]# ------------------------------ Modem
FAX_SUPPORT = FALSE
  # Description:
  #   Support FAX or not, the value should be:
  #   TRUE, FALSE
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Connectivity
SYNCML_DM_SUPPORT = FALSE
  # Description:
  #   define SYNCML_DM_SUPPORT to specify if dm is enabled.
  #   if yes, MMI and DM tasks will refer to this compile option.
  # Option Values:
  #   TRUE: enabile DM
  #   INSIGNIA_DM: enabile insignial DM
  #   FALSE: disable DM
  # Switch-ability:
  #   Non-switchable

SYNCML_DM_VERSION = NONE
  # Description:
  #   The version that project can support for DM.
  # Option Values:
  #   DM11: DM 1.1
  #   NONE: DM 1.1
  #   DM12: DM1.2
  #   DM12_SCTS: SCTS tool test
  # Switch-ability:
  #   Non-switchable

TTL_SUPPORT = LIB
  # Description:
  #   Use ttl.lib to link
  # Option Values:
  #   LIB: Link lib
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Multimedia
A8BOX_SUPPORT = FALSE

ATV_SUPPORT = FALSE
  # Description:
  #   enable mobile atv feature support
  # Option Values:
  #   SLIM:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   SLIM -> FALSE
  #   TRUE -> FALSE

ATV_CHIP = VER00
  # Description:
  #   This option is to indentify driver version.
  # Option Values:
  #   VER00: atv chip version 5192
  #   VER01: atv chip version 5193
  # Switch-ability:
  #   Non-switchable

ATV_I2S_PATH = FALSE
  # Description:
  #   turn on I2S audio path for mATV feature
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   TRUE -> FALSE

ATV_RECORD_SUPPORT = FALSE
  # Description:
  #   enable mobile atv record feature
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   TRUE -> FALSE

ATV_SMS_SUPPORT = FALSE

BACKGROUND_SOUND = TRUE
  # Description:
  #   To enable the calling background sound feature.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

DIGIT_TONE_SUPPORT = FALSE

#[Non-modifiable]# ------------------------------ Tools
LQT_SUPPORT = FALSE
  # Description:
  #   Support LCM Qualification auto-measurement
  # Option Values:
  #   TRUE: Default:
  #         can use LCM Qualification tool
  #   FALSE: Can't use LCM Qualification tool
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Multimedia
JTONE_SUPPORT = FALSE
  # Description:
  #   JTONE support is the tone interface in JAVA game or JAVA application,
  #   It should be open if J2ME_SUPPORT not equal to NONE.
  # Option Values:
  #   TRUE: JAVA Tone Sequence Support
  #   FALSE: Not support JAVA Tone Sequence
  # Switch-ability:
  #   Non-switchable

MEDIA_PLAYER_VER = SEPARATED
  # Description:
  #   This feature option is to decide if MMI use two seperate player to play video and audio files
  #   or use an integrated media player to play these files.
  #   Also, the media player can use file system or SQLite as its play list by changing this option.
  # Option Values:
  #   INTEGRATED_FTE: Integrate audio and video playback into a player (For FTE project)
  #   INTEGRATED_FTE_DB: Integrated audio and video player with auto category function by using database (For FTE project)
  #   COSMOS_MUSIC_PLY: Cosmos MMI music player with auto category function
  #   SEPARATED: Seperated audio player and video player applications
  #   INTEGRATED: Integrate audio and video playback into a player
  # Switch-ability:
  #   [Any] -> [Any]

M3GPMP4_FILE_FORMAT_SUPPORT = FALSE
  # Description:
  #   3GPMP4_FILE_FORMAT_SUPPORT is used to enable or disable to support mp4/3gp iso media file format
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

PURE_AUDIO_SUPPORT = FALSE
  # Description:
  #   To support pure audio 3GP/MP4 files in audio player
  # Option Values:
  #   TRUE: Turn on pure audio support
  #   FALSE: Turn off pure audio support
  # Switch-ability:
  #   [Any] -> [Any]

#[Non-modifiable]# ------------------------------ Audio
AAC_DECODE = FALSE
  # Description:
  #   To enable AAC decoder.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

AAC_PLUS_DECODE = FALSE
  # Description:
  #   To enable AAC PLUS decoder.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

AAC_PLUS_PS_DECODE = FALSE
  # Description:
  #   to enable aacPlus v2
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

AIFF_DECODE = FALSE
  # Description:
  #   AIFF decoder support
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

AMR_CODEC = FALSE
  # Description:
  #   AMR codec is a basic audio capability in general platforms. It supports file/stream AMR playback/record so it should be turned on in most cases. Only turn off it in ultra-low-cost platform to reduce memory usage . Consider carefully before turning off it.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

AMRWB_DECODE = FALSE
  # Description:
  #   Because MT6219 DSP can't support AMRWB_ENCODE when GPRS connection. We have to split decode and encode. MT6219 only support decode.
  # Option Values:
  #   TRUE: Turn on AWB decoder
  #   FALSE: Turn off AWB decoder
  # Switch-ability:
  #   Non-switchable

AMRWB_ENCODE = FALSE
  # Description:
  #   Because MT6219 DSP can't support AMRWB_ENCODE when GPRS connection. We have to split decode and encode. MT6219 only support decode.
  # Option Values:
  #   TRUE: Turn on AWB encoder
  #   FALSE: Turn off AWB encoder
  # Switch-ability:
  #   Non-switchable

DEDI_AMR_REC = FALSE
  # Description:
  #   To enable the AMR encoder for conversation recording.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

APE_DECODE = FALSE
  # Description:
  #   APE audio file deocde
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

AU_DECODE = FALSE
  # Description:
  #   AU decoder support
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

BSAC_DECODE = FALSE

COOK_DECODE = FALSE
  # Description:
  #   for real media cook audio decoder support
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

DAF_DECODE = FALSE
  # Description:
  #   To enable DAF decoder.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

DAF_ENCODE = FALSE
  # Description:
  #   mp3 encoder for fm recording
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

DRA_DECODE = FALSE
  # Description:
  #   DRA is a kind of audio codec for CMMB. This option is to enable or disable DRA decoder.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

M4A_DECODE = FALSE
  # Description:
  #   To support .m4a file whose content is AAC.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ LCD
SYNC_LCM_METHOD = BY_CHIP
  # Description:
  #   Specifies either HW or SW sync LCM method. Will pick either __SYNC_LCM_HW_SUPPORT__ or __SYNC_LCM_SW_SUPPORT__ option. If user is not knowledgeable with the chip capability, just set it as SYNC_LCM_METHOD = BY_CHIP.
  # Option Values:
  #   N/A
  # Switch-ability:
  #   [Any] -> [Any]

#[Non-modifiable]# ------------------------------ Audio
MUSICAM_DECODE = FALSE
  # Description:
  #   add mp2 decoder
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

WAV_CODEC = TRUE
  # Description:
  #   wav codec is a basic audio capability in general platforms. It supports linear PCM , u-law, a-law, ADPCM wav playabck/record so it should be turned on in most cases. Only turn off it in ultra-low-cost platform to reduce memory usage and it cause not only
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

WAVETABLE_VER = WT600K
  # Description:
  #   Use the compiletion to choose a suitable wavetable for the system. The bigger the wavetable is, the higher quality it can provide.
  # Option Values:
  #   WT_ROM_V2: only 6251 (one to one and onto)
  #              Wavetable Size
  #   WT100K: Wavetable Size
  #   WT600K: Wavetable Size
  #   WT150K: Wavetable Size
  #   WT70K: Wavetable Size
  #   WT20K: Wavetable Size
  #   WT_ROM_V1: only 6253
  #              Wavetable Size
  #   WT300K: Wavetable Size
  # Switch-ability:
  #   [Any] -> [Any]

WMA_DECODE = FALSE
  # Description:
  #   WMA (Window Media Audio) decoder.
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

AUDIO_COMPENSATION_DSP_IIR_SUPPORT = FALSE
  # Description:
  #   For audio IIR filter tuning. Only valid on MT6251. Otherchip could use BES_LOUDNESS_SUPPORT instead
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

AUDIO_COMPENSATION_ENABLE = FALSE
  # Description:
  #   to enable Audio_EQ and Audio_Compensation_filter
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   N/A

AUD_RECORD = TRUE
  # Description:
  #   Check if the platform support audio record.
  #   (1) Sound Recorder
  #   (2) FM Radio Recorder
  #   (3) Schedule FM Radio Record
  #   (4) Media Task record APIs
  # Option Values:
  #   TRUE: Turn on audio record function
  #   FALSE: Turn off audio record function
  # Switch-ability:
  #   TRUE -> FALSE

BITSTREAM_SUPPORT = FALSE
  # Description:
  #   (1) support bitstream audio playback interface
  #   (2) support bitstream audio record interface
  # Option Values:
  #   TRUE: Turn on bitstream interface
  #   FALSE: Turn off bitstream interface
  # Switch-ability:
  #   [Any] -> [Any]

BES_BASS_SUPPORT = FALSE
  # Description:
  #   add bass enhancement audio feature
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

BES_EQ_SUPPORT = FALSE
  # Description:
  #   to enable Audio_EQ and Audio_Compensation_filter
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

BES_LIVE_SUPPORT = FALSE
  # Description:
  #   To turn on audio post process (3D effect, time stretch) feature
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

BES_LOUDNESS_SUPPORT = FALSE

BES_SURROUND_SUPPORT = NONE
  # Description:
  #   to enable 3D surround,
  # Option Values:
  #   MODE_EARP:
  #   MODE_BOTH:
  #   NONE:
  #   MODE_LSPK:
  # Switch-ability:
  #   Non-switchable

BES_TS_SUPPORT = FALSE
  # Description:
  #   To turn on audio post process time stretch feature
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

CTM_SUPPORT = TRUE
  # Description:
  #   To turn on/off CTM, TTY  feature
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

DSP_COMPRESS = NONE
  # Description:
  #   Compress DSP ROM code. Decompress into RAM at power on.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

DUAL_MIC_SUPPORT = TRUE
  # Description:
  #   Enable/Disable the baseband dual microphone support capability. Only MT6256/6276 and later chip support it
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   FALSE -> TRUE

#[Non-modifiable]# ------------------------------ Connectivity
SYNCML_SUPPORT = FALSE
  # Description:
  #   define __SYNCML_SUPPORT__ to specify if syncml is enabled.
  #   if yes, MMI and SYNCML tasks will refer to this compile option.
  # Option Values:
  #   TRUE: Turn on SyncML support
  #   FALSE: Turn off SyncML support
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Audio
#[Non-modifiable]# ------------------------------ Connectivity
SYNCML_DEVICE_SYNC_SUPPORT = FALSE
  # Description:
  #   To enable phone to phone sync
  # Option Values:
  #   TRUE: Turn on device sync support
  #   FALSE: Turn off device sync support
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Audio
I2S_INPUT_MODE_SUPPORT = FALSE
  # Description:
  #   This feature option enables the I2S input mode
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

IP_COMMON_SUPPORT = FALSE
  # Description:
  #   To use common SWIP.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

LARGE_MIDI_FILE_SUPPORT = FALSE
  # Description:
  #   Support large size midi playback
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   TRUE -> FALSE

MED_PROFILE = MED_MODEM
  # Description:
  #   To set the MED profile, MED_LOW means not support play audio from file.
  # Option Values:
  #   MED_HIGH: Config MED task for plutommi/cosmos project
  #   MED_MODEM: Config MED task for Modem project
  #   MED_LOW: Config MED task for Neptune or low cost project
  # Switch-ability:
  #   Non-switchable

MELODY_VER = NONE
  # Description:
  #   Choose the built-in melody synthesizer.
  # Option Values:
  #   SW_SYN_16K:
  #   NONE:
  #   EXT_AUDIO_CHIP:
  #   SW_SYN_8K:
  #   DSP_WT_SYN: Midi synthesizer
  # Switch-ability:
  #   Non-switchable

RICH_AUDIO_PROFILE = TRUE

#[Non-modifiable]# ------------------------------ Speech
AEC_ENABLE = TRUE
  # Description:
  #   For Acoustic Echo Cancellation (AEC)
  # Option Values:
  #   TRUE: Support AEC in handset mode
  #   FALSE: Not Support AEC in handset mode
  # Switch-ability:
  #   Non-switchable

AMRWB_LINK_SUPPORT = TRUE
  # Description:
  #   If the compile option is true, our BB chip will support AMRWB voice call. If false, we won't support this function.
  # Option Values:
  #   TRUE: Support WB-AMR link
  #   FALSE: Not Support WB-AMR link
  # Switch-ability:
  #   Non-switchable

EES_ENABLE = TRUE
  # Description:
  #   For Enhanced Echo Suppression (EES)
  # Option Values:
  #   TRUE: Support AEC in speaker mode
  #   FALSE: Not Support AEC in speaker mode
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Connectivity
SYNCML_PC_SYNC_SUPPORT = FALSE
  # Description:
  #   to enable SyncML with PC tool
  # Option Values:
  #   TRUE: Turn on PC sync support
  #   FALSE: Turn off PC sync support
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Speech
TTS = NONE

#[Non-modifiable]# ------------------------------ Video
CMMB_SUPPORT = NONE
  # Description:
  #   To enable CMMB support
  # Option Values:
  #   NONE: CMMB Is not supported
  #   SIANO_SMS2186: CMMB is supported and vendor chip is SMS2186
  #   INNOFIDEI_IF228: CMMB is supported and vendor chip is IF228
  # Switch-ability:
  #   [Any] -> [Any]

CMMB_CAS_FULL_CARD_SUPPORT = FALSE

CMMB_CAS_MBBMS_SUPPORT = FALSE
  # Description:
  # Option Values:
  #   TRUE: MBBMS is supported
  #   FALSE: MBBMS is not supported
  # Switch-ability:
  #   [Any] -> [Any]

CMMB_CAS_SMD_SUPPORT = FALSE

H264_DECODE = FALSE
  # Description:
  #   This option is used to turn on H264  decoder.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   TRUE -> FALSE

H264_DECODE_MODE = NORMAL
  # Description:
  #   When this option is set as SLIM. The support resolution may be decreased in order to reduce memory.
  # Option Values:
  #   SLIM: Enable slim-version of this feature
  #   NORMAL: Enable normal-version of this feature
  # Switch-ability:
  #   NORMAL -> SLIM
  #   SLIM -> NORMAL

H264_OPEN_API = FALSE

MP4_DECODE = FALSE
  # Description:
  #   Turn on/off and set MP4/H264 decode/encode to SW/HW solution
  # Option Values:
  #   TRUE:
  #   MP4_SW:
  #   NONE:
  #   FALSE:
  #   MP4_HW:
  # Switch-ability:
  #   TRUE -> FALSE

MP4_DECODE_MODE = NONE
  # Description:
  #   When this option is set as SLIM. The support resolution will be decreased in order to reduce memory.
  # Option Values:
  #   SLIM:
  #   NONE:
  #   NORMAL:
  # Switch-ability:
  #   NORMAL -> SLIM
  #   SLIM -> NORMAL

MP4_ENCODE = FALSE
  # Description:
  #   Turn on/off and set MP4/H264 decode/encode to SW/HW solution
  # Option Values:
  #   TRUE: TRUE to enable MPEG-4 encoder
  #   MP4_SW: Not supported anymore
  #   NONE: Not supported anymore
  #   FALSE: FALSE to disable MPEG-4 encoder
  #   MP4_HW: Not supported anymore
  # Switch-ability:
  #   TRUE -> FALSE

MP4_ENCODE_MODE = NORMAL
  # Description:
  #   When this option is set as SLIM. The support resolution may be decreased in order to reduce memory.
  # Option Values:
  #   SLIM: Reduce MP4 encode resolution and memory usage
  #   NORMAL: No reduction of maximum resolution for MP4 encode and thus no memory reduction
  # Switch-ability:
  #   [Any] -> [Any]

MJPG_DECODE_MODE = NORMAL
  # Description:
  #   When this option is set as SLIM. The support resolution may be decreased in order to reduce memory.
  # Option Values:
  #   SLIM:
  #   NORMAL:
  # Switch-ability:
  #   [Any] -> [Any]

SUPPORT_VIDEO_RECORD_ROTATE = FALSE

STREAM_REC_SUPPORT = FALSE
  # Description:
  #   Enable record support for streaming video/audio
  # Option Values:
  #   TRUE: to enable video streaming record
  #   FALSE: to disable video streaming record
  # Switch-ability:
  #   Non-switchable

RTSP_SUPPORT = FALSE
  # Description:
  #   to enable streaming function
  # Option Values:
  #   TRUE: open this feature
  #   FALSE: not open this feature
  # Switch-ability:
  #   Non-switchable

TDMB_SUPPORT = NONE

TV_OUT_SUPPORT = FALSE

VDOEDT_SUPPORT = FALSE

#[Non-modifiable]# ------------------------------ Camera
#[Non-modifiable]# ------------------------------ Security
OCSP_SUPPORT = FALSE
  # Description:
  #   To support OCSP (Online Certificate Status Protocol) for socket SSL handshakes.
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   TRUE -> FALSE

#[Non-modifiable]# ------------------------------ Camera
JPEG_SENSOR_SUPPORT = FALSE
  # Description:
  #   enable sensor output jpeg format support
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Security
SOCKET_SSL_SUPPORT = FALSE
  # Description:
  #   To provide SSL APIs in socket API style.
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   TRUE -> FALSE

SSL_SUPPORT = NONE
  # Description:
  #   SSL library for secure connections with SSL/TLS.
  # Option Values:
  #   NONE: Disable this feature
  #   OPENSSL: Use OpenSSL library to support SSL protocol.
  #   OPENSSL_MTK: Use OpenSSL library to support SSL protocol, but replace the cipher engine with Mediatek home made ones.
  # Switch-ability:
  #   OPENSSL -> NONE
  #   OPENSSL_MTK -> NONE

#[Non-modifiable]# ------------------------------ Camera
ISO_PRIORITY_MODE_SUPPORT = FALSE

ISP_SUPPORT = FALSE
  # Description:
  #   To enable BB embeded image signal processor for camera applications.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

MSHUTTER_SUPPORT = FALSE

SENSOR_LOCATION = SENSOR_ON_BODY
  # Description:
  #   For different camera hw module mount location.
  # Option Values:
  #   SENSOR_ON_BODY:
  #   SENSOR_ON_COVER:
  # Switch-ability:
  #   N/A

#[Non-modifiable]# ------------------------------ Graphic
JPG_DECODE = FALSE
  # Description:
  #   To enable jpeg decoder.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

JPG_ENCODE = FALSE
  # Description:
  #   To enable jpeg encoder.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

EXIF_SUPPORT = FALSE
  # Description:
  #   Turn On/Off Exif feature. EXIF is abbreviation of Exchangeable Image File Format.
  #   It's a specification for the image file format sued by digital cameras.
  #   I can embedded some information into image files, such as shutter speed, explosure time, etc.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

PNG_DECODE = FALSE
  # Description:
  #   To enable png decoder.
  # Option Values:
  #   TRUE:
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

SW_FLASH = NONE

OPENGL = NONE

#[Non-modifiable]# ------------------------------ Security
BOOT_CERT_SUPPORT = FALSE
  # Description:
  #   Make hacker can not abuse download the binary and boot-up successfully
  # Option Values:
  #   NONE: Disable this feature
  #   BOOT_CERT_V1: BOOT_CERT_V1 make the built binary must combine with a BOOT_CERT file. Hacker can not download stolen binary into a empty phone and boot-up successfully.
  #   BOOT_CERT_V2: BOOT_CERT_V2 will cover BOOT_ECRT_V1 scope plus read back attack
  # Switch-ability:
  #   Non-switchable

CHE_SUPPORT = FALSE

CHE_INPUT_FROM_FSAL = FALSE
  # Description:
  #   Since TK6xxx doesn't build media module, CHE couldn't use FSAL to access file. Increase one option in TK6xxx make file, if TK6516 AP need this function in the future, it could be turn on.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Connectivity
#[Non-modifiable]# ------------------------------ Security
IPSEC_SUPPORT = FALSE
  # Description:
  #   IPSEC support (using IKEv1)
  # Option Values:
  #   IPSEC_IKEv1: Support IKEv1.
  #   FALSE: Disable IPSEC.
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Application
CBM_ALWAYS_ASK_SUPPORT = FALSE
  # Description:
  #   To support always ask user to select the way to get on network
  # Option Values:
  #   FALSE: Disable the feature
  #   TRUE: Enable the feature
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Security
RC5_SUPPORT = TRUE
  # Description:
  #   to enable RC5 crypto support in security library
  #   This is to turn off RC5 cipher algorithm used in Obigo WAP solution.
  #   Customer will need to turn off this cipher if the handset is shipped to US by US law.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

SECURE_SUPPORT = FALSE
  # Description:
  #   Support security system
  # Option Values:
  #   TRUE: Secure load
  #   FALSE: Non-secure load
  # Switch-ability:
  #   Non-switchable

SECURE_RO_ENABLE = FALSE
  # Description:
  #   Putting default values of SIM-ME-Lock and any other senitive data in put a encrypted RO.
  # Option Values:
  #   TRUE: To enable Secure RO
  #   FALSE: To disable Secure RO
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Application
CBM_BEARER_FALLBACK_SUPPORT = FALSE
  # Description:
  #   Enable bearer fallback between primary data account and secondary data account
  # Option Values:
  #   TRUE: Enable bearer switch
  #   FALSE: Disable bearer switch
  # Switch-ability:
  #   Non-switchable

CBM_CONNECTION_MANAGER_SUPPORT = FALSE
  # Description:
  #   This feature option is used to turn on/off Connection Manager (CNMGR).
  #
  #   CNMGR counts the sent/received data size of dial-up and local bearers. It also counts the connection time of current activated bearer. The information provided by CNMGR is listed below.
  #   - The sent/received data size of last connection
  #   - The connection time of last connection
  #   - The sent/received data size of all connections
  #   - The connection time of all connections
  #   - The sent/received data size of current activated connection
  #   - The connection time of current activated connection
  #
  #   The second functionality of CNMGR is user can disconnect connection from CNMGR Application UI.
  #
  #   The third functionality of CNMGR is supporting 3G Fast Dormancy (FD) feature. CNMGR monitors connection status and send FD request if no data transmission continues for a while.
  # Option Values:
  #   FALSE: Disable this feature
  #   TRUE: Enable this feature
  # Switch-ability:
  #   FALSE -> TRUE
  #   TRUE -> FALSE

#[Non-modifiable]# ------------------------------ Security
RES_PROT = FALSE
  # Description:
  #   RES_PROT is the enhanced anticlone mechenism which not only protect the code, also the data
  #   It's for customer who have concern about the own picture and ringtons that may be stolen by read-out image
  # Option Values:
  #   TRUE: Enable encryption on some important resource data
  #   FALSE: Resource protection is disabled
  # Switch-ability:
  #   Non-switchable

SW_BINDING_SUPPORT = NONE
  # Description:
  #   These 2 options are used to replace original UID_SUPPORT.
  #   SW_BINDING_SUPPORT specifies what kind of anticlone feature for the main code is enabled.
  #   BIND_TO_CHIP_BASIC is the same type as original UID_SUPPORT
  #   BIND_TO_CHIP is the new type which can support FOTA
  #   BIND_TO_KEY is the new type which can bind software to a customer defined key in the chip (SV5 only)
  #   BIND_TO_CHIP_AND_KEY is the combination of BIND_TO_CHIP and BIND_TO_KEY
  # Option Values:
  #   BIND_TO_CHIP_AND_KEY: BIND_TO_CHIP plus BIND_TO_KEY
  #   BIND_TO_CHIP_BASIC: Enable software binding. is the same type as original UID_SUPPORT
  #   NONE: Software bindnig is off
  #   BIND_TO_CHIP: Enable software binding which can support FOTA
  #   BIND_TO_KEY: Enable software binding which can support ROM writer
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Modem
BAND_SUPPORT = QUAD
  # Description:
  #   Described the support band of RF
  # Option Values:
  #   DUAL900: Support 900/1800
  #   EGSM900: Support 900
  #   RGSM900: Support 900
  #   DCS1800: Support 1800
  #   PGSM900: Support 900
  #   QUAD: Support 850/900/1800/1900
  #   DUAL850: Support 850/1800/1900
  #   PCS1900: Support 1900
  #   GSM480: Support 480
  #   GSM850: Support 850
  #   TRIPLE: Support 900/1800/1900
  #   GSM450: Support 450
  # Switch-ability:
  #   DUAL850 -> GSM850
  #   DUAL850 -> PCS1900
  #   DUAL900 -> DCS1800
  #   DUAL900 -> EGSM900
  #   QUAD -> [Any]
  #   TRIPLE -> DCS1800
  #   TRIPLE -> DUAL900
  #   TRIPLE -> EGSM900
  #   TRIPLE -> PCS1900

EDGE_SUPPORT = TRUE
  # Description:
  #   Enable EDGE support Only applicable to 6229 latter version
  # Option Values:
  #   TRUE: EGPRS enable
  #   FALSE: EGPRS enable
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Application
SMS_R8_NATION_LANGUAGE = FALSE
  # Description:
  #   The feature option is to support Turkish special character encoding method which is defined in 3GPP 23.038 Release8.
  #   New SMS UDH IEI is introduced to indicate the encoding/decoding alphabet of special Turikish character.
  #   Using the new method, SMS can avoid to use UCS2 coding scheme when the special Turkish character is present in the message.
  # Option Values:
  #   TRUE: Enable SMS R8 Nation Language (ex: Turkey)
  #   FALSE: Disable SMS R8 Nation Language (ex: Turkey)
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Modem
USIM_SUPPORT = TRUE
  # Description:
  #   Support USIM card commands or not(TRUE: support; FALSE: not support)
  # Option Values:
  #   TRUE: Enable the capability to operate USIM as USIM card, instead of SIM card.
  #   FALSE: Operate USIM as SIM
  # Switch-ability:
  #   Non-switchable

UMTS_MODE_SUPPORT = UMTS_FDD_MODE_SUPPORT
  # Description:
  #   The option is to switch the FDD or TDD mode for 3G modem
  # Option Values:
  #   UMTS_TDD128_MODE_SUPPORT: 3G modem in TDD mode
  #   UMTS_FDD_MODE_SUPPORT: 3G modem in FDD mode
  #   NONE: None of 3G modem mode is supported.
  # Switch-ability:
  #   Non-switchable

UMTS_RF_MODULE = CUSTOM_MT6162
  # Description:
  #   This option is used specify which RF module is used in UL1D RF driver
  #   "UL1D" means UMTS Layer 1 Driver.
  # Option Values:
  #   MT6162: the RF module used in this project
  # Switch-ability:
  #   Non-switchable

HSDPA_SUPPORT = TRUE
  # Description:
  #   Define if UE supports HSDPA feature. (TRUE/FALSE)
  # Option Values:
  #   TRUE: HSDPA is support
  #   FALSE: HSDPA is not support
  # Switch-ability:
  #   Non-switchable

HSUPA_SUPPORT = TRUE
  # Description:
  #   Define if UE supports HSUPA feature. (TRUE/FALSE)
  # Option Values:
  #   TRUE: HSUPA is support
  #   FALSE: HSUPA is not support
  # Switch-ability:
  #   Non-switchable

R99_SUPPORT = TRUE

R4_SUPPORT = TRUE

R5_SUPPORT = TRUE

3SET_F8F9_ENABLE = TRUE
  # Description:
  #   3 set F8/F9 HW support
  # Option Values:
  #   TRUE: Enable 3 SET F8F9 HW
  #   FALSE: Disable 3 SET F8F9 HW
  # Switch-ability:
  #   Non-switchable

AMR_LINK_SUPPORT = TRUE
  # Description:
  #   Support AMR link, channel codec and RATSCCH procedure, with base station.
  # Option Values:
  #   TRUE: AMR enable
  #   FALSE: AMR disable
  # Switch-ability:
  #   Non-switchable

AT_COMMAND_SET = FULL
  # Description:
  #   set AT_COMMAND_SET = FULL for all projects
  #   FULL means the same setting in current existed project.s
  #   So it will not impact any existed project.
  # Option Values:
  #   SLIM: support BT HF/DUN commands, phonesuite commands, dial up programs, and EM AT commands
  #   ULC: support BT HF commands and EM AT commands
  #   FULL: all AT commands are supported
  # Switch-ability:
  #   Non-switchable

CCBS_SUPPORT = FALSE

CENTRALIZED_SLEEP_MANAGER = TRUE
  # Description:
  #   This is a Chip Plaform Dependent feature.
  #   Currently, only MT6276 and MT6573 support this kind of
  #   "Centralized Sleep Mode Manager Architecture".
  # Option Values:
  #   TRUE: sleep mode enable
  #   FALSE: sleep mode enable
  # Switch-ability:
  #   Non-switchable

CMUX_SUPPORT = TRUE
  # Description:
  #   Support 07.10 multiplexer feature: TRUE or FALSE
  # Option Values:
  #   TRUE: Enable 27.010 multiplexer
  #   FALSE: Disable 27.010 multiplexer
  # Switch-ability:
  #   Non-switchable

CNAP_SUPPORT = FALSE
  # Description:
  #   Enable/Disable Name identification supplementary services feature (TS22.096) support
  # Option Values:
  #   TRUE: Turn on CNAP
  #   FALSE: Turn off CNAP
  # Switch-ability:
  #   Non-switchable

CSD_SUPPORT = NONE
  # Description:
  #   for CSD support
  # Option Values:
  #   T_NT:
  #   TRUE: CSD enable
  #   NT_ONLY:
  #   NONE:
  #   FALSE: CSD disable
  # Switch-ability:
  #   Non-switchable

EMLPP_SUPPORT = FALSE

EXT_MODEM_SUPPORT = FALSE
  # Description:
  #   If this option is enabled, the phone can connect to the internet via the other modem (ex. CDMA or PHS or GSM)
  # Option Values:
  #   TRUE: Enable external modem support
  #   FALSE: Disable external modem support
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Security
DISABLE_A5_2 = TRUE
  # Description:
  #   Disable A5/2 cipering algo
  # Option Values:
  #   N/A
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Modem
FDD_EDCH_PHYSICAL_CATEGORY = 6
  # Description:
  #   Define the HSUPA category in makefile. However, the actual category UE uses is the minimum of this one and the one queried from HW
  # Option Values:
  #   6: EDCH category = 6
  #   4: EDCH category = 4
  #   1: EDCH category = 1
  #   3: EDCH category = 3
  #   2: EDCH category = 2
  #   5: EDCH category = 5
  # Switch-ability:
  #   Non-switchable

FDD_HSDSCH_PHYSICAL_CATEGORY = 8
  # Description:
  #   Define the HSDPA category in makefile. However, the actual category UE uses is the minimum of this one and the one queried from HW
  # Option Values:
  #   6: HSDSCH category = 6
  #   11: HSDSCH category = 11
  #   3: HSDSCH category = 3
  #   7: HSDSCH category = 7
  #   9: HSDSCH category = 9
  #   12: HSDSCH category = 12
  #   2: HSDSCH category = 2
  #   8: HSDSCH category = 8
  #   1: HSDSCH category = 1
  #   4: HSDSCH category = 4
  #   10: HSDSCH category = 10
  #   5: HSDSCH category = 5
  # Switch-ability:
  #   Non-switchable

GERAN_RELEASE_SUPPORT = GERAN_R6_SUPPORT
  # Description:
  #   The option is to switch different version of release for 2G modem
  # Option Values:
  #   GERAN_R5_SUPPORT: set the 2G MODEM to 3GPP R5 baseline
  #   GERAN_R6_SUPPORT: set the 2G MODEM to 3GPP R6 baseline
  #   GERAN_R99_SUPPORT: set the 2G MODEM to 3GPP R99 baseline
  #   GERAN_R4_SUPPORT: set the 2G MODEM to 3GPP R4 baseline
  # Switch-ability:
  #   Non-switchable

GPRS_DIALUP_PPP_DROP_PACKETS_WHEN_2G_PS_SUSPEND = TRUE
  # Description:
  #   This feature makes PPP process UART data even if there is no FLC buffer available (ex: In 2G CS Call suspend PS Transfer, so FLC buffer run out)
  #   In this situation, PPP start to decode the UART data into a temp buffer, and if the decoded packet is a IP pac
  # Option Values:
  #   TRUE: Enable CS suspend PS in 2G.
  #   FALSE: Disable CS suspend PS in 2G.
  # Switch-ability:
  #   Non-switchable

GPRS_DIALUP_PPP_SUPPORT_ESCAPE_ATO = TRUE
  # Description:
  #   The fature enable the switch between command mode and data mode during GPRS dialup
  #   User can switch from data mode to command mode by using ESCAPE sequence (+++)
  #   User can then switch from command mode to data mode by using ATO command
  # Option Values:
  #   TRUE: Enable support for ATO +++ cmmands.
  #   FALSE: Disable support for ATO +++ cmmands.
  # Switch-ability:
  #   Non-switchable

GPRS_DIALUP_PPP_SUPPORT_SPEED_UP_DIALUP = TRUE

L1_3GSOLUTION = MTK_UL1_FDD
  # Description:
  #   Add an option to select 3G L1 solution in WCDMA project
  # Option Values:
  #   MTK_TL1_TDD: MTK TL1 solution is selected
  #   AST_TL1_TDD: AST TL1 solution is selected
  #   MTK_UL1_FDD: MTK UL1 solution is selected
  #   MA_L1: Modem Art 3G L1 solution is selected
  #   NONE: Disable this feature(non-3G project)
  # Switch-ability:
  #   Non-switchable

L1_EGPRS = TRUE
  # Description:
  #   this compile option is only used for L1 Stand alone to turn on/off EDGE function
  # Option Values:
  #   TRUE: EGPRS enable
  #   FALSE: EGPRS disable
  # Switch-ability:
  #   Non-switchable

L1_EPSK_TX = TRUE
  # Description:
  #   Turn on/off this compile option may switch the function of EPSK modulation transmitter
  # Option Values:
  #   TRUE: EPSK TX enable
  #   FALSE: EPSK TX disable
  # Switch-ability:
  #   Non-switchable

L1_GPRS = TRUE
  # Description:
  #   Turn on the GPRS support of Layer 1
  #   This is used for internal test
  # Option Values:
  #   TRUE: GPRS enable
  #   FALSE: GPRS disable
  # Switch-ability:
  #   Non-switchable

L1_WCDMA = TRUE
  # Description:
  #   Add an option to enable WCDMA L1 support
  # Option Values:
  #   TRUE: Support UMTS L1
  #   FALSE: Not support UMTS L1
  # Switch-ability:
  #   Non-switchable

L1L2_TX_NEW_ARCH = TRUE
  # Description:
  #   Enable __TX_NEW_ARCHITECTURE__ compile option which is used by UL1 and UL2.
  # Option Values:
  #   TRUE: Enable using __TX_NEW_ARCHITECTURE__ compile option.
  #   FALSE: Disable using __TX_NEW_ARCHITECTURE__ compile option.
  # Switch-ability:
  #   Non-switchable

L2_HSDPA_COPRO = TRUE
  # Description:
  #   Enable using L2 HSDPA Coprocessor
  # Option Values:
  #   TRUE: Enable L2 HSDPA Copro
  #   FALSE: Disable L2 HSDPA Copro
  # Switch-ability:
  #   Non-switchable

L2_HSUPA_COPRO = TRUE
  # Description:
  #   Enable using L2 HSUPA Coprocessor
  # Option Values:
  #   TRUE: Enable L2 HSUPA Copro
  #   FALSE: Disable L2 HSUPA Copro
  # Switch-ability:
  #   Non-switchable

MAX_NUM_OF_NDIS_SUPPORT = 6
  # Description:
  #   support multiple(2~3) NDIS channels
  # Option Values:
  #   3: Configure the maximal number of NDIS channels
  #   2: Configure the maximal number of NDIS channels
  # Switch-ability:
  #   Non-switchable

MCD_CODESET_SHIFT_SUPPORT = NONE
  # Description:
  #   NONE: if Codeset Shift not support
  #   440000: if Codeset shift support by Docomo (Plmn ID: 440000)                                      You can enumerate all suitable support plmn
  # Option Values:
  #   NONE: support codec shift for DOCOMO LAB test
  #   440000: not support codec shift
  # Switch-ability:
  #   Non-switchable

MCD_SUPPORT = TRUE
  # Description:
  #   to enable MediaTek Message Coder Decoder feature for GSM/GPRS/EDGE project
  # Option Values:
  #   TRUE: Enable MCD (default)
  #   FALSE: Disable MCD
  # Switch-ability:
  #   Non-switchable

MODEM_3G_LOGGING = FALSE
  # Description:
  #   Enable 3G modem logging feature
  # Option Values:
  #   TRUE: internal use
  #   FALSE: internal use
  # Switch-ability:
  #   Non-switchable

MULTIPLE_NDIS_SUPPORT = TRUE
  # Description:
  #   support multiple(2~3) NDIS channels
  # Option Values:
  #   TRUE: Support multiple NDIS channels
  #   FALSE: Single NDIS channel is allowed
  # Switch-ability:
  #   Non-switchable

MULTIPLE_TBF = FALSE
  # Description:
  #   REL6 Multiple TBF
  # Option Values:
  #   TRUE: Support multiple TBF
  #   FALSE: Not support multiple TBF
  # Switch-ability:
  #   Non-switchable

PLMN_LIST_PREF_SUPPORT = DEFAULT
  # Description:
  #   PLMN List EM mode setting
  # Option Values:
  #   ON: Let PLMN List mode, accuracy or speed, can be selected by EM mode
  #   OFF: PLMN List mode can not be selected by EM mode
  #   DEFAULT: PLMN list mode can be selected except low cost, L1S project.
  # Switch-ability:
  #   Non-switchable

PPP_TYPE_PDP_DIALUP_SUPPORT = FALSE
  # Description:
  #   The feature is used to create PPP tunnel instead of IP tunnel in PS transfer.
  # Option Values:
  #   TRUE: Enable PPP type PDP context.
  #   FALSE: Disable PPP type PDP context.
  # Switch-ability:
  #   Non-switchable

RRC_PAYLOAD_FOR_3G_UP_SUPPORT = FALSE
  # Description:
  #   Depend on this option to differentiate if UE supports 3G RRC payload type in SUPL POS message or not
  # Option Values:
  #   TRUE: RRC payload can be supported for SUPL
  #   FALSE: RRC payload can not be supported for SUPL
  # Switch-ability:
  #   Non-switchable

SIM_HOT_SWAP = NONE
  # Description:
  #   support SIM hot swap, only MT6253D enable now
  # Option Values:
  #   SIM_SLOT_3: 3 SIM  project, support sim hot swap from SLOT 3
  #   NULL: not support SIM hot swap
  #   SIM_SLOT_4: 4 SIM  project, support sim hot swap from SLOT 4
  #   SIM_SLOT_2: Dual SIM  project, support sim hot swap from SLOT 2
  #   SIM_SLOT_1: single SIM project, support sim hot swap from SLOT 1
  # Switch-ability:
  #   Non-switchable

SMART_PHONE_CORE = ANDROID_MODEM
  # Description:
  #   To distinguish if it is a smart phone MODEM makefile or a smart phone APP makefile. NONE means it is not a smart-phone make file
  # Option Values:
  #   AP: in-house testing load running on application processor
  #   NONE:
  #   WM_MODEM: modem processor software load for Windows Mobile smart phone
  #   MD: in-house testing load running on modem processor
  #   ANDROID_MODEM: modem processor software load for Android smart phone
  # Switch-ability:
  #   Non-switchable

SMART_PHONE_MODEM = FALSE

SP_VIDEO_CALL_SUPPORT = TRUE
  # Description:
  #   Enable Video Call Modem Capability for Smartphone project
  # Option Values:
  #   TRUE: Turn on Video Call Support for Smartphone
  #         (conflict w/ 3G_VIDEO_CALL)
  #   FALSE: Turn off Video Call Support for Smartphone
  # Switch-ability:
  #   Non-switchable

SML_SUPPORT = TRUE
  # Description:
  #   SIM-ME lock support
  # Option Values:
  #   TRUE: Support SIM-ME lock
  #   FALSE: Not support SIM-ME lock
  # Switch-ability:
  #   Non-switchable

SMS_OVER_PS_SUPPORT = TRUE
  # Description:
  #   Wrap the functionality - "SMS over PS" - for switchable
  # Option Values:
  #   TRUE: Enable SMS send over PS
  #   FALSE: Disable SMS send over PS
  # Switch-ability:
  #   Non-switchable

ULCS_ASN_SUPPORT_VERSION = ULCS_ASN_SUPPORT_R99
  # Description:
  #   Depend on this option to differentiate NW IE supported vesions of 3G RRC payload type in SUPL POS message
  # Option Values:
  #   ULCS_ASN_SUPPORT_R99: The ASN codec supports R99 version LCS IEs
  #   ULCS_ASN_SUPPORT_R6: The ASN codec supports R6 version LCS IEs
  # Switch-ability:
  #   Non-switchable

UMTS_RELEASE_SUPPORT = UMTS_R6_SUPPORT
  # Description:
  #   The option is to switch different version of release for 3G modem
  # Option Values:
  #   UMTS_R6_SUPPORT: 3G modem supports Rel6
  #   UMTS_R4_SUPPORT: 3G modem supports Rel4
  #   UMTS_R5_SUPPORT: 3G modem supports Rel5
  # Switch-ability:
  #   Non-switchable

UUS_SUPPORT = FALSE

#[Non-modifiable]# ------------------------------ TD-SCDMA
#[Non-modifiable]# ------------------------------ File system and System service
FS_CHECKDRIVE_SUPPORT = FALSE
  # Description:
  #   Both File System and File Manager support Scan Disk function. System will ask user to do check disk at boot time if the phone was abnormal shutdown last time. Also, user can do that by selecting it from disk menu anytime.
  # Option Values:
  #   TRUE: To enable FS Check Drive
  #   FALSE: To disable FS Check Drive
  # Switch-ability:
  #   [Any] -> [Any]

FS_MOVE_SUPPORT = TRUE

FS_QM_SUPPORT = TRUE
  # Description:
  #   File System will support quota management mechanism on system drive. The mechanism prevents exhausted system drive from single application.
  # Option Values:
  #   TRUE: To enable FS Quota Management
  #   FALSE: To disable FS Quota Management
  # Switch-ability:
  #   [Any] -> [Any]

FS_SORT_MAX_ENTRY = 256
  # Description:
  #   FS supports up to sort up to 20000 files.
  # Option Values:
  #   NA: To specify number of sorting entry
  # Switch-ability:
  #   Non-switchable

FS_SORT_SUPPORT = TRUE
  # Description:
  #   Both File System and File Manager support Sort function. User can browse folders/files with sorted order.
  # Option Values:
  #   TRUE: To enable FS Sorting
  #   FALSE: To disable FS Sorting
  # Switch-ability:
  #   [Any] -> [Any]

FS_TRACE_SUPPORT = FALSE
  # Description:
  #   File System will provide run-time trace information with proper configure. The trace information is very helpful to identify problem source.
  # Option Values:
  #   TRUE: To enable FS debug messange
  #   FALSE: To disable FS debug messange
  # Switch-ability:
  #   [Any] -> [Any]

FS_ASYNC_SUPPORT = FALSE
  # Description:
  #   File system supports handling requests asynchronously, i.e. in FS task.
  # Option Values:
  #   ON: To enable FS Async Interface.
  #   OFF: To disable FS Async Interface.
  # Switch-ability:
  #   [Any] -> [Any]

NVRAM_PSEUDO_MERGE = OFF
  # Description:
  #   NVRAM will combine small files and this will transparent to applications.
  # Option Values:
  #   ON: To enable NVRAM pseudo merge
  #   OFF: To disable NVRAM pseudo merge
  # Switch-ability:
  #   [Any] -> [Any]

KAL_ASSERT_SHRINKING = FALSE

KAL_DEDUG_AND_TRACE_LEVEL = NORMAL_DEBUG_KAL

SECURE_PORT_SUPPORT = FALSE
  # Description:
  #   The feature option define if the sw supports enabling/disabling debug port(such catcher) via a paremater in SECURE_RO or not. TRUE means Support.
  #   It increases the security coverage of our platform.
  # Option Values:
  #   TRUE: To enable debug UART port protection
  #   FALSE: To disable debug UART port protection
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ HW and Project
#[Non-modifiable]# ------------------------------ File system and System service
KEYPAD_DEBUG_TRACE = FALSE
  # Description:
  #   Enable bootup trace with keypad
  # Option Values:
  #   TRUE: Enable
  #   FALSE: Disable
  # Switch-ability:
  #   Non-switchable

RESOURCE_MANAGER = FALSE

SSS_SUPPORT = SSS_LIB
  # Description:
  #   1. bootloader and MAUI share the same library sss.lib
  #   2. Apply this option for USB download related projects
  # Option Values:
  #   SSS_LIB:
  #   SSS_SOURCE:
  # Switch-ability:
  #   Non-switchable

SWDBG_SUPPORT = FALSE

#[Non-modifiable]# ------------------------------ Flash
NAND_SUPPORT = FALSE
  # Description:
  #   To enable NAND flash support, please set NAND_SUPPORT as TRUE.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

SYSTEM_DRIVE_ON_NAND = FALSE
  # Description:
  #   To use NAND flash as system drive,
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   [Any] -> [Any]

NAND_SUPPORT_RAW_DISK = FALSE
  # Description:
  #   NAND Disk can be access without file system format
  # Option Values:
  #   TRUE: Enable NAND RAW disk support
  #   FALSE: Disable NAND RAW disk support. Normal project should be FALSE.
  # Switch-ability:
  #   Non-switchable

ENHANCED_SINGLE_BANK_NOR_FLASH_SUPPORT = FALSE
  # Description:
  #   to enable enhanced single bank NOR flash support.
  # Option Values:
  #   TRUE: Enable Single Bank support
  #   FALSE: Disable Single Bank support
  # Switch-ability:
  #   Non-switchable

SYSDRV_BACKUP_DISK_SUPPORT = NONE
  # Description:
  #   System drive support backup disk option.
  #   Option - RAW, FAT or NONE (MAUI)
  #   Option - TRUE or FALSE (Lipton project)
  #   RAW  : Enable Backup Partition with Raw Disk Format Feature.
  #   FAT/TRUE  : Enable Backup Partition with FAT Format Feature.
  #   NONE/FALSE : Disable Backup Partition for NOR and NAND.
  #   Purpose
  #   (1) Support NVRAM Data Backup Feature.
  #   Usage
  #   (1) Set to RAW/FAT.
  #   Limitation
  #   (1) Currently NOR flash does not support FAT format backup disk.
  #   (2) Currently NAND flash does not support Raw format backup disk.
  #   (3) Must set NOR_SUPPORT_RAW_DISK=TRUE if backup paritition is raw disk format.
  # Option Values:
  #   NONE: System Drive backup to disk is not supported
  #   FAT: System Drive backup to disk is supported, the disk is FAT Format
  #   RAW: System Drive backup to disk is supported, the disk is RAW Format
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Memory card
MSDC_CARD_SUPPORT_TYPE = NONE
  # Description:
  #   to support various memory card type
  # Option Values:
  #   MSDC_SD_MMC: Enable SD/MMC card support
  #   NONE: Disable memory card support
  # Switch-ability:
  #   MSDC_SD_MMC -> NONE

CARD_DOWNLOAD = FALSE
  # Description:
  #   When this option is not NONE, Card Download will be enabled. It will turn on Bootloader and add some LCD and T-Flash dirver in it.
  # Option Values:
  #   NONE: Disable Card Download
  #   FILESYSTEM_ON_CARD: Enable Card DL. The memory card needs to be formated as FAT/FAT32. In other words, the download package can be directly copied into T-flash by Windows. It is the recommanded option for enabling Card DL.
  #   RAW_FORMAT_CARD: Enable Card DL. The image is copied into card via an extra card writer with RAW format. The original content in the card will lost after using the card writer.
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Application
#[Non-modifiable]# ------------------------------ Tools
L1_CATCHER = TRUE
  # Description:
  #   to enable L1 logging.
  # Option Values:
  #   TRUE: Enable
  #   FALSE: Disable
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Application
#[Non-modifiable]# ------------------------------ Tools
M3D_MMI_SUPPORT = FALSE
  # Description:
  #   Add M3D_MMI_SUPPORT for M3D MMI use instead of M3D_SUPPORT
  # Option Values:
  #   FALSE: Disable this feature
  #   TRUE: Enable this feature
  # Switch-ability:
  #   Non-switchable

NDIS_SUPPORT = UPS
  # Description:
  #   to enable NDIS support in modem side
  # Option Values:
  #   NONE: Disable NDIS_SUPPORT
  #   UPS: Enable NDIS over UPS
  #   NDIS: Enable NDIS over CMUX
  # Switch-ability:
  #   Non-switchable

TST_SUPPORT = TRUE
  # Description:
  #   to enable message logging and debug information tracing mechanism.
  # Option Values:
  #   TRUE: Enable
  #   FALSE: Disable
  # Switch-ability:
  #   Non-switchable

TST_LOGACC_SUPPORT = TRUE
  # Description:
  #   to enable the LogAcc HW Support for TST.
  # Option Values:
  #   TRUE: Enable
  #   TRUE: Enable
  #   FALSE: Disable
  #   FALSE: Disable
  # Switch-ability:
  #   Non-switchable

TST_SET_LOG_BUF_SIZ = NONE
  # Description:
  #   This option allow users to set the desired logging buffer size.
  #   With this option, user could have a large logging buffer and reduce the probability of log drop.
  #   It's useful for a feaure testing under QC.
  # Option Values:
  #   NONE: Invalidate this setting
  #   13-23: Set the logging buffer as (2^n) bytes
  # Switch-ability:
  #   Non-switchable

TST_DNT_LOGGING = FALSE
  # Description:
  #   Enable SIB logging (High speed logging interface)
  # Option Values:
  #   TRUE: Enable
  #   FALSE: Disable
  # Switch-ability:
  #   Non-switchable

DT_SUPPORT = FALSE
  # Description:
  #   This option is used to Turn on/off PC tool Phonesuite File manager related functions. If set FALSE, Phonesuite File manager will not available.
  # Option Values:
  #   FLASE: Turn off DT_SUPPORT, In this case PC tool "phonesuite" can use only limited functions(function like file manager is disabled)
  #   TRUE: Turn on DT_SUPPORT, So MediaTek PC tool "Phonesutie" file manager related functions can be used
  # Switch-ability:
  #   Non-switchable

MODIS_ON_VC9 = TRUE

MODIS_FDM = OFF
  # Description:
  #   To enable On-time File System (mcu\fs\fat) on MoDIS.
  # Option Values:
  #   ON: To select On-time FS lib on MoDIS
  #   OFF: To selet MTK propietary FS lib on MoDIS
  # Switch-ability:
  #   [Any] -> [Any]

SPLIT_SYSTEM = FALSE

#[Non-modifiable]# ------------------------------ Java
J2ME_DEFAULT_GAME = DEMO

J2ME_FEATURE_SET = NONE

J2ME_SLIM_MEMORY_SUPPORT = FALSE
  # Description:
  #   To enable J2ME_SLIM_MEMORY_SUPPORT, J2ME_SHARE_MED_EXT_MEM must also be enabled. Once enable J2ME_SLIM_MEMORY_SUPPORT, the global memory used by Java will be shrunk for 210K on QVGA, 6252 and the background running will be supported. Since Java will use MED memory in background running with this compile option, all applications using MED memory can not be concurrent. If we want to integrate any third party application using MED memory, we should
  #   1. test if MED has enough memory, if not, please pop up an error dialog and prevent the user entering Java, or,
  #   2. Allocate the full ASM memory when entering the application. Since Java will also allocate some ASM memory, once the application allocates the whole memory, an out-of-memory dialog will be shown and the user can use it to terminate Java.
  # Option Values:
  #   FALSE: Disable this feature
  #   TRUE: Enable this feature
  #   NO_JSR135_VIDEO: If use the compile option, we'll disable Java video playback, recording, and camera capture.
  # Switch-ability:
  #   Non-switchable

J2ME_SLIM_SCREEN_MEMORY_SUPPORT = FALSE

SUPPORT_JSR_82 = FALSE

SUPPORT_JSR_184 = FALSE

#[Non-modifiable]# ------------------------------ VoIP
TFTP_SUPPORT = FALSE
  # Description:
  #   TFTP transfer protocol support. This feature has been phased out for years.
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

SDP_SUPPORT = FALSE
  # Description:
  #   to enable SDP function which will be used by PoC, VoIP, Video Streaming, etc
  # Option Values:
  #   TRUE: Enable SDP(Session Description Protocol). It is used in streaming features.
  #   FALSE: Disable SDP.
  # Switch-ability:
  #   N/A

SIP_SUPPORT = FALSE
  # Description:
  #   Support SIP service
  # Option Values:
  #   TRUE: Enable SIP(Session Initialization Protocol).
  #   FALSE: Disable SIP.
  # Switch-ability:
  #   N/A

VOIP_SUPPORT = FALSE

#[Non-modifiable]# ------------------------------ Wifi
WIFI_SUPPORT = NONE
  # Description:
  #   General compile option for WiFi features.
  # Option Values:
  #   MT5931: WIFI CHIP vesion
  #   NONE: NO WIFI CHIP
  #   MT5921: WIFI CHIP Version
  # Switch-ability:
  #   Non-switchable

WAPI_SUPPORT = FALSE
  # Description:
  #   for WAPI feature suuport
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

WNDRV_SUPPORT = LIB
  # Description:
  #   When BM builds the code,  the value should be set to SOURCE.
  #   The compiled library should be put in "\mcu\custom\drv\wifi\Project_Name\lib\".
  #   When BM releases the code, the value should be set to LIB. And the
  #   wndrv should be always linked as a library.
  # Option Values:
  #   LIB:
  #   SOURCE:
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Memory card
DUAL_MEMORY_CARD_SUPPORT = NONE
  # Description:
  # Option Values:
  #   N/A
  # Switch-ability:
  #   N/A

#[Non-modifiable]# ------------------------------ Wifi
IPERF_SUPPORT = NONE
  # Description:
  #   Detect compile option conflict for IPERF_SUPPORT.
  #   Iperf (http://dast.nlanr.net/Projects/Iperf/) is an open source software for throughput measurement, especially for Wi-Fi.
  #   Wi-Fi alliance uses it as the benchmark tool for certification program.
  #   To passed
  # Option Values:
  #   QUAD: create 4 iperf tasks.
  #   DUAL: create 2 iperf tasks.
  #   NONE: not open this feature
  #   TRIPLE: create 3 iperf tasks.
  #   SINGLE: create 1 iperf tasks.
  # Switch-ability:
  #   Non-switchable

IPERF_WMM_PS_TEST_SUPPORT = FALSE
  # Description:
  #   Wrap some license request code. When customer need this part and the customer have the authority, this feature could be turn on.
  # Option Values:
  #   TRUE: open this feature
  #   FALSE: not open this feature
  # Switch-ability:
  #   Non-switchable

WIFI_BT_SINGLE_ANTENNA_SUPPORT = FALSE
  # Description:
  #   for SW control the new HW broad design of WIFI/BT single antenna
  # Option Values:
  #   TRUE: Enable BT WIFI single antenna feature
  #   FALSE: Disable BT WIFI single antenna feature
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Operator
OPTR_SPEC = NONE
  # Description:
  #   Operator spec name, version and segment
  # Option Values:
  #   VODAFONE_R15_ENTRY3G: Vodafone R15 Entry3G version
  #   ORANGE_0905_COMMASS: Orange 9.0 COMMASS version
  #   NONE: General version (none operator)
  #   VODAFONE_R12_LIVE: Vodafone R12 Live version
  #   CMCC_TD0200_GENERAL: CMCC TD2.0 General project
  #   UNICOM_0200_GENERAL: China Unicom 2.0 version
  #   ORANGE_0800_MEDIAENTRY: Orange 8.0 MEDIAENTRY version
  #   ORANGE_0605_CAM240: Orange 6.5 CAM240 version
  #   CMCC_0204_SEGC: CMCC 2G spec v2.4 Segment C project
  #   CMCC_FWPBW_GENERAL: CMCC 3G TD Fix Wireless Phone Black&White General project
  #   CMCC_FWPCOLOR_GENERAL: CMCC 3G TD Fix Wireless Phone Color General project
  #   ATT_0402_ADAPT: AT&T 4.2 Adapt Project (Only Modem customized)
  # Switch-ability:
  #   Non-switchable

DM_MO_SUPPORT = NONE
  # Description:
  #   To support DM Management Object
  # Option Values:
  #   NONE: not support lawmo and scomo
  #   SCO: scomo
  #   LAW: lawmo
  #   LAW_SCO: lawmo and scomo
  # Switch-ability:
  #   Non-switchable

MOBILE_VIDEO_SUPPORT = NONE
  # Description:
  #   CMCCTD Key feature, which let user play video by network
  # Option Values:
  #   NONE: Not Define this feature
  #   WONDER: Define this feature
  # Switch-ability:
  #   WONDER -> NONE

#[Non-modifiable]# ------------------------------ TC01
TC01_OAEP_SUPPORT = FALSE
  # Description:
  #   LGE audio effect package for opera.
  #   Currently, it only contains EQ and loudness.
  #   LGE will replace the library with Dolby by themselve.
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   Non-switchable

#[Non-modifiable]# ------------------------------ Others
CUSTOMER_SPECIFIC_FACTORY_DETECTION = NONE
  # Description:
  #   Customer specific factory mode detection mechanism
  #   Some customers may have their own factory mode definition, the mechanism can help to mapping customer specific factory mode to system mode
  # Option Values:
  #   NONE:
  #   CS_FAC_DET_TYPE_1: Use this type of factoty detection algorithm
  # Switch-ability:
  #   Non-switchable

CLASSAB_LOW_VOLTAGE_VOLUME_PROTECTION = FALSE
  # Description:
  #   For MT6253, If the battary is low, reduce class AB volume to prevent power down
  # Option Values:
  #   TRUE:
  #   FALSE:
  # Switch-ability:
  #   FALSE -> TRUE

FAST_DORMANCY_SUPPORT = TRUE
  # Description:
  #   This feature option is used to enable/disable 3G R8 Fast Dormancy feature.
  # Option Values:
  #   TRUE: Enable Fast Dormancy. Available on 3G R5R6 or afterward project. In 3G Gemini 2.0 project, must set to TRUE.
  #   FALSE: Disable Fast Dormancy. In Gemini1.0 project, must set to FALSE and SCRI will be enabled instead.
  # Switch-ability:
  #   Non-switchable

DISABLE_EGSM_CAP = FALSE

2G_RF_CUSTOM_TOOL_SUPPORT = TRUE
  # Description:
  #   2G_RF_CUSTOM_TOOL_SUPPORT set to TRUE is to enable the feature of 2G RF custom setting modified by modem bin update tool and can modify the value by NVRAM editor
  #   For UMTS FDD dual mode projects, if user want to enable this feature,  2G_RF_CUSTOM_TOOL_SUPPORT need to set to TRUE. And customer can not modify it by himself, needs to ask for flavor build (lock build)
  #
  #   2G_RF_CUSTOM_TOOL_SUPPORT feature options are default disabled on 11AW1112MP and 11AW1112_MOTO_ODM_SP (due to it is a patch back feature), but is default enabled on 11AMDW1119SP and 11A (thus default enabled on 11B)
  # Option Values:
  #   TRUE: Enable the feature of 2G/3G RF custom setting and band support setting modified by modem bin update tool and can modify the value by NVRAM editor
  #   FALSE: Disable the feature of 2G/3G RF custom setting and band support setting modified by modem bin update tool and also can not modify the value by NVRAM editor
  # Switch-ability:
  #   Non-switchable

SIM_RECOVERY_ENHANCEMENT = TRUE
  # Description:
  #   Enable continuous SIM card recovery procedure
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

LG_VOICE_ENGINE = FALSE

TDD_RF_CUSTOM_TOOL_SUPPORT = FALSE

IPV6_PDP_SUPPORT = TRUE
  # Description:
  #   Description:
  #   IPv6 PDP Context Support
  #   Option Values:
  #   TRUE: support IPv6 PDP Context, support IPv4v6(3GPP Rel8) PDP Context
  #   FALSE: not support IPv6 PDP Context
  #   Switch-ability:
  #   N/A
  # Option Values:
  #   FALSE: Disable this feature
  #   TRUE: Enable this feature
  # Switch-ability:
  #   Non-switchable

PBCCH_SUPPORT = TRUE
  # Description:
  #   Indicate whether MS supports PBCCH capability
  # Option Values:
  #   TRUE: Enable PBCCH capability
  #   FALSE: Disable PBCCH capability
  # Switch-ability:
  #   Non-switchable

LGE_BT_SYSTEM_SUPPORT = FALSE

WCDMA_PREFERRED = TRUE
  # Description:
  #   enable/disable WCDMA Preferred feature
  # Option Values:
  #   FALSE: Disable this feature
  #   TRUE: Enable this feature
  # Switch-ability:
  #   N/A

GEMINI_VERSION = V2

FDD_RF_CUSTOM_TOOL_SUPPORT = TRUE
  # Description:
  #   FDD_RF_CUSTOM_TOOL_SUPPORT set to TRUE is to enable the feature of 3G RF custom setting and band support setting modified by modem bin update tool and can modify the value by NVRAM editor
  #
  #
  #
  #   For UMTS FDD dual mode projects, if user want to enable this feature, both FDD_RF_CUSTOM_TOOL_SUPPORT  need to set to TRUE. And customer can not modify it by himself, needs to ask for flavor build (lock build)
  #
  #   FDD_RF_CUSTOM_TOOL_SUPPORT  feature options are default disabled on 11AW1112MP and 11AW1112_MOTO_ODM_SP (due to it is a patch back feature), but is default enabled on 11AMDW1119SP and 11A (thus default enabled on 11B)
  # Option Values:
  #   TRUE: Enable the feature of 2G/3G RF custom setting and band support setting modified by modem bin update tool and can modify the value by NVRAM editor
  #   FALSE: Disable the feature of 2G/3G RF custom setting and band support setting modified by modem bin update tool and also can not modify the value by NVRAM editor
  # Switch-ability:
  #   Non-switchable

MCU_DORMANT_MODE = TRUE
  # Description:
  #   MD MCU power off if it enters sleep mode. Need to back up some register to non-sleep memory before MD MCU enters WFI and restore these register after MD MCU leaves WFI. OSTD and system service need this feature option. First chip with this feature is MT6575E1.
  # Option Values:
  #   TRUE: Enable this feature
  #   FALSE: Disable this feature
  # Switch-ability:
  #   Non-switchable

DSPIRDBG_SP = FALSE
  # Description:
  #   To enable FCore usb logging on smartphone. It's needed when DSP log is needed for debug.
  # Option Values:
  #   TRUE: enable FCore usb logging
  #   FALSE: disable FCore usb logging
  # Switch-ability:
  #   Non-switchable

# *************************************************************************
# Release Setting Section
# *************************************************************************
RELEASE_PACKAGE = REL_CR_MMI_hspa # REL_CR_MMI_GPRS, REL_CR_MMI_GSM, REL_CR_L4_GPRS, REL_CR_L4_GSM

RELEASE_$(strip $(INPUT_METHOD)) = SRC	# MTK/SRC, only works when INPUT_METHOD is turning on
RELEASE_INPUT_METHODS_SRC =		# MMI_ZI, MMI_T9, MMI_ITAP,
RELEASE_TYPE = NONE           # NONE, INTERNAL
############################################################

# *************************************************************************
# Consistent Feature Options
# *************************************************************************
COM_DEFS_FOR_MT6577_MODEM_MT6162 = MT6162_RF
COM_DEFS_FOR_MT6577_MODEM_LCM = MT6577_MODEM_LCM
# *************************************************************************
# Include MODEM.mak
# *************************************************************************
include make\MODEM.mak

# *************************************************************************
# Common preprocessor definitions
# *************************************************************************
CUSTOM_OPTION += __FLIGHT_MODE_SUPPORT__ __MANUAL_MODE_NW_SEL__ \
                 __CP_HW_CIPHER_ENABLE__ __CP_HW_INTEGRITY_ENABLE__ \
                 __USIM_SUPPORT__ __USIM_DRV__ __SATC3__ TK6268_FPGA1 \
                 __DISABLE_A5_2__ \
                 MSDC_MMC40_SUPPORT __DRV_MSDC_LAYOUT_DEFECT__ __MINI_LOG_SUPPORT__
CUSTOM_OPTION += __NBR_CELL_INFO__ \
                 __SATCC__ __AT_EPBW_SUPPORT__
CUSTOM_OPTION += __HSPA_DATA_PATH_OPT__

CUSTOM_OPTION += __SMS_OVER_PS_SUPPORT__ __R6_OOS__ __DYNAMIC_HSPA_PREFERENCE__

CUSTOM_OPTION += __HW_PFC_SUPPORT__

CUSTOM_OPTION += MED_V_NOT_PRESENT MED_NOT_PRESENT __CPI_SUPPORT__

CUSTOM_OPTION += __BT_SIM_PROFILE__ __CTA_DUAL_SIM_STANDARD__ __L4C_ROOT__ __SATCE__

CUSTOM_OPTION += __GEMINI_3G_SWITCH__

# *************************************************************************
# Custom Release Component Configuration
# *************************************************************************
include make\$(strip $(RELEASE_PACKAGE)).mak

COM_DEFS_FOR_TINNO77_ICS2_LCM = TINNO77_ICS2_LCM
COM_DEFS_FOR_CUSTOM_MT6162 = MT6162_RF CUSTOM_MT6162

# ************************************************************************* 
# Component trace definition header files for custom release only 
# ************************************************************************* 
# Following trace headers are already existed in custom release 
# You can add new traces with kal_trace() in the following trace headers 
EXISTED_CUS_REL_TRACE_DEFS = 
EXISTED_CUS_REL_TRACE_DEFS += ps\l4\include\l4_trc.h 
EXISTED_CUS_REL_TRACE_DEFS += ps\l4\csm\csm_trc.h 
EXISTED_CUS_REL_TRACE_DEFS += drv\include\drv_trc.h 
EXISTED_CUS_REL_TRACE_DEFS += hal\peripheral\inc\bmt_trc.h 
EXISTED_CUS_REL_TRACE_DEFS += nvram\include\nvram_trc.h 
EXISTED_CUS_REL_TRACE_DEFS += ps\l4\phb\include\phb_trc.h 
EXISTED_CUS_REL_TRACE_DEFS += ps\l4\uem\include\uem_trc.h 
EXISTED_CUS_REL_TRACE_DEFS += hal\audio\src\v2\inc\l1audio_sph_trc.h 
EXISTED_CUS_REL_TRACE_DEFS += hal\audio\src\v2\inc\l1sp_trc.h 
EXISTED_CUS_REL_TRACE_DEFS += ps\sim2\include\sim_trc.h 
EXISTED_CUS_REL_TRACE_DEFS += tst\include\tst_trace.h 
EXISTED_CUS_REL_TRACE_DEFS += init\include\init_trc.h 
 
# Customer can add new trace headers here for new modules 
NEW_CUS_REL_TRACE_DEFS = 
