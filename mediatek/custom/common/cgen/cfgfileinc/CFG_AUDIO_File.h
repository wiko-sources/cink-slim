/*******************************************************************************
 *
 * Filename:
 * ---------
 * cfg_audio_file.h
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 * This file is the header of audio customization related function or definition.
 *
 * Author:
 * -------
 * Ning.F
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 * Jun 22 2009 mtk01352
 * [DUMA00007771] Moving modem side customization to AP
 *
 *
 * Apr 29 2009 mtk80306
 * [DUMA00116080] revise the customization of nvram
 * revise nvram customization
 *
 * Mar 21 2009 mtk80306
 * [DUMA00112158] fix the code convention.
 * modify code convention
 *
 * Mar 9 2009 mtk80306
 * [DUMA00111088] nvram customization
 * nvram customization
 *
 *
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef _CFG_AUDIO_FILE_H
#define _CFG_AUDIO_FILE_H

#define CUSTOM_VOLUME_STEP (7)
#define NB_FIR_NUM (45)
#define NB_FIR_INDEX_NUM   (6)
#define SPEECH_COMMON_NUM (12)
#define SPEECH_PARA_MODE_NUM     (8)
#define SPEECH_PARA_NUM                (16)
#define AUDIO_EQ_PARAM_NUM         (8)

#define WB_FIR_NUM (90)
#define WB_FIR_INDEX_NUM   (6)

#define HD_REC_MODE_INDEX_NUM   (30)
#define HD_REC_FIR_INDEX_NUM    (8)
/* audio nvram structure definition*/
typedef enum
{
    VOL_NORMAL   = 0 ,
    VOL_HEADSET      ,
    VOL_HANDFREE     ,
    MAX_VOL_CATE
} volume_category_enum;

typedef enum
{
    VOL_TYPE_TON  = 0 ,
    VOL_TYPE_KEY      ,
    VOL_TYPE_MIC      ,
    VOL_TYPE_FMR      ,
    VOL_TYPE_SPH      ,
    VOL_TYPE_SID	    ,
    VOL_TYPE_MEDIA    ,
    MAX_VOL_TYPE
} volume_type_enum;

#define     NUM_ABF_PARAM 44
#define     NUM_ABFWB_PARAM 76

typedef struct _AUDIO_CUSTOM_EXTRA_PARAM_STRUCT
{
    /* ABF parameters */
    unsigned short ABF_para[NUM_ABF_PARAM + NUM_ABFWB_PARAM];    //with WB
} AUDIO_CUSTOM_EXTRA_PARAM_STRUCT;

#define CFG_FILE_SPEECH_DUAL_MIC_SIZE    sizeof(AUDIO_CUSTOM_EXTRA_PARAM_STRUCT)
#define CFG_FILE_SPEECH_DUAL_MIC_TOTAL   1

typedef struct _AUDIO_CUSTOM_PARAM_STRUCT
{
    /* volume setting */
    unsigned char volume[MAX_VOL_CATE][MAX_VOL_TYPE];
    /* speech enhancement */
    unsigned short speech_common_para[SPEECH_COMMON_NUM];
    unsigned short speech_mode_para[SPEECH_PARA_MODE_NUM][SPEECH_PARA_NUM];
    unsigned short speech_volume_para[4];//in the feature, should extend to [MAX_VOL_CATE][MAX_VOL_TYPE][4]
    /* debug info */
    unsigned short debug_info[16];
    /* speech input FIR */
    short          sph_in_fir[NB_FIR_INDEX_NUM][NB_FIR_NUM];
    /* speech output FIR */
    short          sph_out_fir[NB_FIR_INDEX_NUM][NB_FIR_NUM];
    /* digital gain of DL speech */
    unsigned short Digi_DL_Speech;
    /* digital gain of uplink speech */
    unsigned short Digi_Microphone;
    /* FM record volume*/
    unsigned short FM_Record_Volume;
    /* user mode : normal mode, earphone mode, loud speaker mode */
    unsigned short Bluetooth_Sync_Type;
    unsigned short Bluetooth_Sync_Length;
    unsigned short bt_pcm_in_vol;
    unsigned short bt_pcm_out_vol;
    unsigned short user_mode;
    /* auto VM record setting */
    unsigned short bSupportVM;
    unsigned short bAutoVM;
    // mic bias
    unsigned short uMicbiasVolt;

} AUDIO_CUSTOM_PARAM_STRUCT;

#define CFG_FILE_SPEECH_REC_SIZE        sizeof(AUDIO_CUSTOM_PARAM_STRUCT)
#define CFG_FILE_SPEECH_REC_TOTAL   1

typedef struct _AUDIO_CUSTOM_WB_A2M_PARAM_STRUCT_
{
    /* WB speech enhancement */
    unsigned short speech_mode_wb_para[SPEECH_PARA_MODE_NUM][SPEECH_PARA_NUM];
    /* WB speech input/output FIR */
    short          sph_wb_fir[WB_FIR_INDEX_NUM][WB_FIR_NUM];
    /* in_out flag */
    short          input_out_fir_flag; // 0: input, 1: output
} AUDIO_CUSTOM_WB_A2M_PARAM_STRUCT;

typedef struct _AUDIO_CUSTOM_WB_PARAM_STRUCT
{
    /* WB speech enhancement */
    unsigned short speech_mode_wb_para[SPEECH_PARA_MODE_NUM][SPEECH_PARA_NUM];
    /* WB speech input FIR */
    short          sph_wb_in_fir[WB_FIR_INDEX_NUM][WB_FIR_NUM];
    /* WB speech output FIR */
    short          sph_wb_out_fir[WB_FIR_INDEX_NUM][WB_FIR_NUM];
} AUDIO_CUSTOM_WB_PARAM_STRUCT;

#define CFG_FILE_WB_SPEECH_REC_SIZE        sizeof(AUDIO_CUSTOM_WB_PARAM_STRUCT)
#define CFG_FILE_WB_SPEECH_REC_TOTAL   1

typedef struct _AUDIO_ACF_CUSTOM_PARAM_STRUCT
{
    /* Compensation Filter HSF coeffs       */
    /* BesLoudness also uses this coeffs    */
    unsigned int bes_loudness_hsf_coeff[9][4];

    /* Compensation Filter BPF coeffs       */
    unsigned int bes_loudness_bpf_coeff[4][6][3];
    unsigned int bes_loudness_DRC_Forget_Table[9][2];
    unsigned int bes_loudness_WS_Gain_Max;
    unsigned int bes_loudness_WS_Gain_Min;
    unsigned int bes_loudness_Filter_First;
    char bes_loudness_Gain_Map_In[5];
    char bes_loudness_Gain_Map_Out[5];

} AUDIO_ACF_CUSTOM_PARAM_STRUCT;

#define CFG_FILE_AUDIO_COMPFLT_REC_SIZE        sizeof(AUDIO_ACF_CUSTOM_PARAM_STRUCT)
#define CFG_FILE_AUDIO_COMPFLT_REC_TOTAL   1
#define CFG_FILE_HEADPHONE_COMPFLT_REC_TOTAL   1

typedef struct _AUDIO_EFFECT_CUSTOM_PARAM_STRUCT
{
    //surround parameters
    int bsrd_level;
    unsigned int Distance1;
    unsigned int Distance2;
    int bsrd_band_select;

    //bass parameters
    unsigned int bass_CutoffFreq;
    int bass_IsVB;

    //EQ parameters
    short Normal_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Dance_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Bass_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Classical_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Treble_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Party_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Pop_Gain_dB_level[AUDIO_EQ_PARAM_NUM];
    short Rock_Gain_dB_level[AUDIO_EQ_PARAM_NUM];

    //loudness mode
    int LoudEnhancemode;

    // time stretch
    int Time_TD_FD;
    int Time_TS_Ratio;

} AUDIO_EFFECT_CUSTOM_PARAM_STRUCT;

#define CFG_FILE_AUDIO_EFFECT_REC_SIZE        sizeof(AUDIO_EFFECT_CUSTOM_PARAM_STRUCT)
#define CFG_FILE_AUDIO_EFFECT_REC_TOTAL   1

typedef struct _AUDIO_PARAM_MED_STRUCT
{
    short speech_input_FIR_coeffs[SPEECH_PARA_MODE_NUM][NB_FIR_NUM];
    short speech_output_FIR_coeffs[SPEECH_PARA_MODE_NUM][NB_FIR_INDEX_NUM][NB_FIR_NUM];
    short select_FIR_output_index[SPEECH_PARA_MODE_NUM];
    short select_FIR_intput_index[SPEECH_PARA_MODE_NUM];
    short speech_mode_para[SPEECH_PARA_MODE_NUM][SPEECH_PARA_NUM];
} AUDIO_PARAM_MED_STRUCT;

#define CFG_FILE_AUDIO_PARAM_MED_REC_SIZE        sizeof(AUDIO_PARAM_MED_STRUCT)
#define CFG_FILE_AUDIO_PARAM_MED_REC_TOTAL   1


typedef struct _AUDIO_VOLUME_CUSTOM_STRUCT
{
    unsigned char audiovolume_ring[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_key[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_mic[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_fmr[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_sph[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_sid[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_media[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
    unsigned char audiovolume_matv[MAX_VOL_CATE][CUSTOM_VOLUME_STEP];
} AUDIO_VOLUME_CUSTOM_STRUCT;

#define CFG_FILE_AUDIO_VOLUME_CUSTOM_REC_SIZE        sizeof(AUDIO_VOLUME_CUSTOM_STRUCT)
#define CFG_FILE_AUDIO_VOLUME_CUSTOM_REC_TOTAL   1


#define GAIN_TABLE_SUPPORT
#ifdef GAIN_TABLE_SUPPORT
#define GAIN_TABLE_LEVEL (20)
#else
#define GAIN_TABLE_LEVEL (20)
#endif

#define VOICE_GAIN_TABLE_LEVEL (6)
#define SYSTEM_GAIN_TABLE_LEVEL (7)
#define RING_GAIN_TABLE_LEVEL (7)
#define MUSIC_GAIN_TABLE_LEVEL (13)
#define ALARM_GAIN_TABLE_LEVEL (7)
#define NOTIFICATION_GAIN_TABLE_LEVEL (7)
#define BLUETOOTHSCO_GAIN_TABLE_LEVEL (15)
#define ENFORCEAUDIBLE_GAIN_TABLE_LEVEL (7)
#define DTMF_GAIN_TABLE_LEVEL (15)
#define TTS_GAIN_TABLE_LEVEL (15)
#define FM_GAIN_TABLE_LEVEL (13)
#define MATV_GAIN_TABLE_LEVEL (13)

//speech should equal to voice call level
#define SPEECH_GAIN_TABLE_LEVLE (VOICE_GAIN_TABLE_LEVEL)

enum GAIN_TYPE {
    VOICE_GAIN = 0,
    SYSTEM_GAIN,
    RINGTONE_GAIN,
    MUSIC_GAIN,
    ALARM_GAIN,
    NOTIFICATION_GAIN,
    BLUETOOTH_SCO_GAIN,
    ENFORCEAUDIBLE_GAIN,
    FTMF_GAIN,
    TTS_GAIN,
    FM_GAIN,
    MATV_GAIN,
    MICROPHNE_GAIN,
    SIDETONE_GAIN,
    SPEECH_GAIN,
    MAX_GAIN_TYPE
};

enum VOICE_STREAM_DEVICEGAIN{
    VOICE_HEADSET =0,
    VOICE_SPEAKER ,
    SIP_VOICE_RECEIVER,
    SIP_VOICE_HEADSET,
    SIP_VOICE_SPEAKER,
    MAX_VOICE_STREAM_TYPE
};

enum STREAM_DEVICEGAIN{
    STREAM_HEADSET =0,
    STREAM_SPEAKER,
    MAX_STREAM_TYPE
};

enum RING_DEVICEGAIN{
    RING_HEADSET =0,
    RING_SPEAKER ,
    RING_MODE_HEADSET,
    RING_MODE_SPEAKER,
    MAX_RING_TYPE
};

enum MICROPHONE_DEVICEGAIN{
    IDLE_RECORD_NORMAL =0,
    IDLE_HEADSET ,
    INCALL_NORMAL,
    INCALL_HEADSET,
    INCALL_SPEAKER,
    SIPCALL_NORMAL,
    SIPCALL_HEADSET,
    SIPCALL_SPEAKER,
    Analog_RECORD,
    TTY_DEVICE,
    NUM_OF_MICGAIN = 16
};

enum SIDETONE_DEVICEGAIN{
    SIDETONE_NORMAL =0,
    SIDETONE_HEADSET,
    SIDETONE_SPEAKER,
    NUM_OF_SIDETONEGAIN = 8
};

enum SPEECH_DEVICEGAIN{
    SPEECH_NORMAL =0,
    SPEECH_HEADSET ,
    SPEECH_SPEAKER,
    SPEECH_BT,
    NUM_OF_SPEECHGAIN,
};

typedef struct _AUDIO_GAIN_CONTROL_STRUCT
{
    unsigned char u8Gain_digital;
    unsigned int u32Gain_PGA_Amp;
} AUDIO_GAIN_CONTROL_STRUCT;

typedef struct _STREAM_VOICE_GAIN_CONTROL_STRUCT
{
    AUDIO_GAIN_CONTROL_STRUCT Voice_Gain[MAX_VOICE_STREAM_TYPE][GAIN_TABLE_LEVEL];
    /*
    Headset_Gain[GAIN_TABLE_LEVEL];
    Speaker_Gain[GAIN_TABLE_LEVEL];
    SIP_Receiver_Gain[GAIN_TABLE_LEVEL];
    SIP_Headset_Gain[GAIN_TABLE_LEVEL];
    SIP_Speaker_Gain[GAIN_TABLE_LEVEL];
    */
} STREAM_VOICE_GAIN_CONTROL_STRUCT;

typedef struct _STREAM_RING_GAIN_CONTROL_STRUCT
{
    AUDIO_GAIN_CONTROL_STRUCT Ring_Stream_Gain[MAX_RING_TYPE][GAIN_TABLE_LEVEL];
    /*
    Headset_Gain[GAIN_TABLE_LEVEL];
    Speaker_Gain[GAIN_TABLE_LEVEL];
    RING_Headset_Gain[GAIN_TABLE_LEVEL];
    RING_Sperker_Gain[GAIN_TABLE_LEVEL];
    */
} STREAM_RING_GAIN_CONTROL_STRUCT;

typedef struct _STREAM_GAIN_CONTROL_STRUCT
{
    AUDIO_GAIN_CONTROL_STRUCT Stream_Gain[MAX_STREAM_TYPE][GAIN_TABLE_LEVEL];
    /*
     Headset_Gain[GAIN_TABLE_LEVEL];
     Speaker_Gain[GAIN_TABLE_LEVEL];
    */
} STREAM_GAIN_CONTROL_STRUCT;

typedef struct _STREAM_MICROPHONE_GAIN_CONTROL_STRUCT
{
    unsigned char Mic_Gain[NUM_OF_MICGAIN];
    /*
    unsigned char Idle_Record;                 // here for normal recording gain.
    unsigned char Idle_Record_Headset;
    unsigned char normal;                         // her for speech
    unsigned char Headset;
    unsigned char speaker;
    unsigned char SIP_normal;                 // her for SIP call
    unsigned char SIP_Headset;
    unsigned char SIP_speaker;
    unsigned char Matv_record;
    unsigned char FM_record;
    unsigned char TTY_Mode;
    unsigned char Reserved1;
    unsigned char Reserved2;
    unsigned char Reserved3;
    unsigned char Reserved4;
    unsigned char Reserved5;
    */
} STREAM_MICROPHONE_GAIN_CONTROL_STRUCT;

typedef struct _STREAM_SIDETONE_GAIN_CONTROL_STRUCT
{
    unsigned char SdieTone_Gain[NUM_OF_SIDETONEGAIN];
    /*
    unsigned char normal;                         // here for speech
    unsigned char Headset;
    unsigned char speaker;
    unsigned char SIP_normal;                 // here for SIP call
    unsigned char SIP_Headset;
    unsigned char SIP_speaker;
    unsigned char Reserved1;
    unsigned char Reserved2;
    */
} STREAM_SIDETONE_GAIN_CONTROL_STRUCT;

typedef struct _STREAM_SPEECH_GAIN_CONTROL_STRUCT
{
    AUDIO_GAIN_CONTROL_STRUCT Speech_Gain[NUM_OF_SPEECHGAIN][GAIN_TABLE_LEVEL];
    /*
    Receiver_Gain[GAIN_TABLE_LEVEL];
    Headset_Gain[GAIN_TABLE_LEVEL];
    Speaker_Gain[GAIN_TABLE_LEVEL];
    BT_GAIN[GAIN_TABLE_LEVEL];
    */
} STREAM_SPEECH_GAIN_CONTROL_STRUCT;

typedef struct _AUDIO_GAIN_TABLE_STRUCT
{
    STREAM_VOICE_GAIN_CONTROL_STRUCT Voice_Gain_table;
    STREAM_GAIN_CONTROL_STRUCT System_Gain_table;
    STREAM_RING_GAIN_CONTROL_STRUCT Ring_Gain_table;
    STREAM_GAIN_CONTROL_STRUCT Music_Gain_table;
    STREAM_GAIN_CONTROL_STRUCT Alarm_Gain_table;
    STREAM_GAIN_CONTROL_STRUCT Notification_Gain_table;
    STREAM_GAIN_CONTROL_STRUCT Bluetooth_sco_Gain_table;
    STREAM_GAIN_CONTROL_STRUCT EnforceAudible_table;
    STREAM_GAIN_CONTROL_STRUCT Dtmf_Gain_table;
    STREAM_GAIN_CONTROL_STRUCT Tts_Gain_table;
    STREAM_GAIN_CONTROL_STRUCT Fm_Gain_table;
    STREAM_GAIN_CONTROL_STRUCT Matv_Gain_table;
    STREAM_MICROPHONE_GAIN_CONTROL_STRUCT Microphone_Gain_table;
    STREAM_SIDETONE_GAIN_CONTROL_STRUCT Sidetone_Gain_table;
    STREAM_SPEECH_GAIN_CONTROL_STRUCT Speech_Gain_table;
} AUDIO_GAIN_TABLE_STRUCT;

#define CFG_FILE_AUDIO_GAIN_TABLE_CUSTOM_REC_SIZE        sizeof(AUDIO_GAIN_TABLE_STRUCT)
#define CFG_FILE_AUDIO_GAIN_TABLE_CUSTOM_REC_TOTAL   1


//#if defined(MTK_HD_RECORD_SUPPORT)

#define SPC_MAX_NUM_RECORD_SPH_MODE 30
#define SPC_MAX_NUM_RECORD_INPUT_FIR 8
typedef struct _AUDIO_HD_RECORD_PARAM_STRUCT
{
    /* HD RECORD Mode Num & FIR Num*/
    unsigned short hd_rec_mode_num; // max(hd_rec_fir_num) == 30 ??
    unsigned short hd_rec_fir_num;  // max(hd_rec_fir_num) == 4 ??

    /* HD RECORD Speech Enhancement */
    unsigned short hd_rec_speech_mode_para[SPC_MAX_NUM_RECORD_SPH_MODE][SPEECH_PARA_NUM]; // the contain only have hd_rec_mode_num effective values

    /* HD RECORD FIR */
    short          hd_rec_fir[SPC_MAX_NUM_RECORD_INPUT_FIR][WB_FIR_NUM]; // the contain only have hd_rec_fir_num effective values

    /* HD RECORD FIR Mapping (ex, map[sph_mode] = FIR3) */
    unsigned short hd_rec_map_to_fir_for_ch1[SPC_MAX_NUM_RECORD_SPH_MODE];
    unsigned short hd_rec_map_to_fir_for_ch2[SPC_MAX_NUM_RECORD_SPH_MODE];

    /* HD RECORD Device Mode Mapping (ex, map[sph_mode] = SPH_MODE_NORMAL ) */
    unsigned char hd_rec_map_to_dev_mode[SPC_MAX_NUM_RECORD_SPH_MODE];

    /* HD RECORD Input Source Mapping (ex, map[sph_mode] = BT Earphone mic)*/
    unsigned char hd_rec_map_to_input_src[SPC_MAX_NUM_RECORD_SPH_MODE];

    /* HD RECORD mode is stereo or not (ex, map[sph_mode] = 0(mono), 1(stereo) )*/
    unsigned char hd_rec_map_to_stereo_flag[SPC_MAX_NUM_RECORD_SPH_MODE];

} AUDIO_HD_RECORD_PARAM_STRUCT;


#define CFG_FILE_AUDIO_HD_REC_PAR_SIZE   sizeof(AUDIO_HD_RECORD_PARAM_STRUCT)
#define CFG_FILE_AUDIO_HD_REC_PAR_TOTAL  1



#define MAX_HD_REC_SCENES 10  // max #scene = 10 (10 * 3 = 30 = max modes)

enum HD_REC_DEVICE_SOURCE_T
{
    HD_REC_DEVICE_SOURCE_HANDSET     = 0,
    HD_REC_DEVICE_SOURCE_HEADSET     = 1,
    HD_REC_DEVICE_SOURCE_BT_EARPHONE = 2,
    NUM_HD_REC_DEVICE_SOURCE
};

typedef struct
{
    unsigned char num_voice_rec_scenes;
    unsigned char num_video_rec_scenes;
    unsigned char scene_table[MAX_HD_REC_SCENES][NUM_HD_REC_DEVICE_SOURCE];
} AUDIO_HD_RECORD_SCENE_TABLE_STRUCT;

#define CFG_FILE_AUDIO_HD_REC_SCENE_TABLE_SIZE   sizeof(AUDIO_HD_RECORD_SCENE_TABLE_STRUCT)
#define CFG_FILE_AUDIO_HD_REC_SCENE_TABLE_TOTAL  1
//#endif  //MTK_HD_RECORD_SUPPORT

#endif // _CFG_AUDIO_FILE_H

