#define ACCDET_EINT

#ifdef ACCDET_EINT
#define ACCDET_DELAY_ENABLE_TIME 500 //delay 500ms to enable accdet after EINT 
//#define ACCDET_EINT_HIGH_ACTIVE //defaule low active if not define ACCDET_EINT_HIGH_ACTIVE
#endif

struct tv_mode_settings{
    int start_line0;	//range: 1~19
    int end_line0;	//range: 1~19
    int start_line1;	//range: 263~285(NTSC), 310~336(PAL)
    int end_line1;	//range: 263~285(NTSC), 310~336(PAL)
    int pre_line;
    int start_pixel;	//range: 112~850
    int end_pixel;	//range: 112~850
    int fall_delay;
    int rise_delay;
    int div_rate;	//pwm div in tv-out mode 
    int debounce;	//tv-out debounce
};


struct headset_mode_settings{
    int pwm_width;	//pwm frequence
    int pwm_thresh;	//pwm duty 
    int fall_delay;	//falling stable time
    int rise_delay;	//rising stable time
    int debounce0;	//hook switch or double check debounce
    int debounce1;	//mic bias debounce
    int debounce3;	//plug out debounce
};


//remote button customization: long press time
int long_press_time = 2000;


//headset mode register settings(for MT6575)
struct headset_mode_settings cust_headset_settings = {
	0x1900, 0x140, 1, 0x12c, 0x3000, 0x3000, 0x400
};



#ifdef MTK_TVOUT_SUPPORT
#define TV_OUT_SUPPORT
#endif


#ifdef TV_OUT_SUPPORT

//tv mode register settings (for MT6575)
struct tv_mode_settings cust_tv_settings[2]={
        {9, 19, 273, 284, 5, 112, 850, 2, 0x160, 0x20, 0x6}, //NTSC mode
        {9, 19, 320, 334, 5, 112, 850, 2, 0x160, 0x20, 0x6}, //PAL mode
};


#endif
