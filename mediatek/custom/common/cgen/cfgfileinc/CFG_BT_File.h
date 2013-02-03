#ifndef _CFG_BT_FILE_H
#define _CFG_BT_FILE_H


// the record structure define of bt nvram file
#ifdef MTK_COMBO_SUPPORT
typedef struct
{
    unsigned char addr[6];            // BT address
    unsigned char Voice[2];           // Voice setting for SCO connection
    unsigned char Codec[4];           // PCM codec setting
    unsigned char Radio[6];           // RF configuration
    unsigned char Sleep[7];           // Sleep mode configuration
    unsigned char BtFTR[2];           // Other feature setting
    unsigned char TxPWOffset[3];      // TX power channel offset compensation
} ap_nvram_btradio_mt6610_struct;
#else
// MT661x is phased out, current for MT662x
typedef struct
{
    unsigned char addr[6];                   /*0*/
    unsigned char CapId[1];                  /*6*/
    unsigned char LinkKeyType[1];            /*7*/
    unsigned char UintKey[16];               /*8*/
    unsigned char Encryption[3];             /*24*/
    unsigned char PinCodeType[1];            /*27*/
    unsigned char Voice[2];                  /*28*/
    unsigned char Codec[4];                  /*30*/
    unsigned char Radio[6];                  /*34*/
    unsigned char Sleep[7];                  /*40*/
    unsigned char BtFTR[2];                  /*47*/
    unsigned char TxPWOffset[3];             /*49*/
    unsigned char ECLK_SEL[1];               /*52*/
    unsigned char Reserved1[1];              /*53*/
    unsigned char Reserved2[2];              /*54*/
    unsigned char Reserved3[4];              /*56*/
    unsigned char Reserved4[4];              /*60*/
    unsigned char Reserved5[16];             /*64*/
    unsigned char Reserved6[16];             /*80*/
} ap_nvram_btradio_mt6610_struct;
#endif

//the record size and number of bt nvram file
#define CFG_FILE_BT_ADDR_REC_SIZE    sizeof(ap_nvram_btradio_mt6610_struct)
#define CFG_FILE_BT_ADDR_REC_TOTAL   1

#endif


