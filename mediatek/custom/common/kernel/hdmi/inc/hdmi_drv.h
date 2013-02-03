#ifndef __HDMI_DRV_H__
#define __HDMI_DRV_H__

#ifndef ARY_SIZE
#define ARY_SIZE(x) (sizeof((x)) / sizeof((x[0])))
#endif

typedef enum
{
    HDMI_POLARITY_RISING  = 0,
    HDMI_POLARITY_FALLING = 1
}HDMI_POLARITY;

typedef enum
{
    HDMI_CLOCK_PHASE_0  = 0,
    HDMI_CLOCK_PHASE_90 = 1
}HDMI_CLOCK_PHASE;

typedef enum
{
    HDMI_COLOR_ORDER_RGB = 0,
    HDMI_COLOR_ORDER_BGR = 1
}HDMI_COLOR_ORDER;

typedef enum
{    
   IO_DRIVING_CURRENT_8MA       = (1 << 0),
   IO_DRIVING_CURRENT_4MA       = (1 << 1),
   IO_DRIVING_CURRENT_2MA       = (1 << 2),
   IO_DRIVING_CURRENT_SLEW_CNTL = (1 << 3),
}IO_DRIVING_CURRENT;

typedef enum
{
	HDMI_VIDEO_720x480p_60Hz = 0,
	HDMI_VIDEO_1280x720p_60Hz,
	HDMI_VIDEO_1920x1080p_30Hz,
	HDMI_VIDEO_RESOLUTION_NUM
}HDMI_VIDEO_RESOLUTION;

typedef enum
{
    HDMI_VIN_FORMAT_RGB565,
    HDMI_VIN_FORMAT_RGB666,
    HDMI_VIN_FORMAT_RGB888,
} HDMI_VIDEO_INPUT_FORMAT;

typedef enum
{
	HDMI_VOUT_FORMAT_RGB888,
	HDMI_VOUT_FORMAT_YUV422,
	HDMI_VOUT_FORMAT_YUV444,
}HDMI_VIDEO_OUTPUT_FORMAT;

typedef enum
{
	HDMI_AUDIO_PCM_16bit_48000,
	HDMI_AUDIO_PCM_16bit_44100,
	HDMI_AUDIO_PCM_16bit_32000,
	HDMI_AUDIO_SOURCE_STREAM,
}HDMI_AUDIO_FORMAT; 

typedef struct
{
	HDMI_VIDEO_RESOLUTION		vformat;
	HDMI_VIDEO_INPUT_FORMAT 	vin;
	HDMI_VIDEO_OUTPUT_FORMAT 	vout;
	HDMI_AUDIO_FORMAT			aformat;
}HDMI_CONFIG;

typedef enum{
    HDMI_OUTPUT_MODE_LCD_MIRROR,
    HDMI_OUTPUT_MODE_VIDEO_MODE,
    HDMI_OUTPUT_MODE_DPI_BYPASS
}HDMI_OUTPUT_MODE;

typedef struct
{
	unsigned int width;
	unsigned int height;

	HDMI_CONFIG init_config;

    /* polarity parameters */
    HDMI_POLARITY clk_pol;
    HDMI_POLARITY de_pol;
    HDMI_POLARITY vsync_pol;
    HDMI_POLARITY hsync_pol;

    /* timing parameters */
    unsigned int hsync_pulse_width;
    unsigned int hsync_back_porch;
    unsigned int hsync_front_porch;
    unsigned int vsync_pulse_width;
    unsigned int vsync_back_porch;
    unsigned int vsync_front_porch;
    
    /* output format parameters */
    HDMI_COLOR_ORDER rgb_order;

    /* intermediate buffers parameters */
    unsigned int intermediat_buffer_num; // 2..3

    /* iopad parameters */
    IO_DRIVING_CURRENT io_driving_current;
    HDMI_OUTPUT_MODE    output_mode;

    int is_force_awake;
    int is_force_landscape;
}HDMI_PARAMS;

typedef enum{
	HDMI_STATE_NO_DEVICE,
	HDMI_STATE_ACTIVE
}HDMI_STATE;

// ---------------------------------------------------------------------------

typedef struct
{
    void (*set_reset_pin)(unsigned int value);
    int  (*set_gpio_out)(unsigned int gpio, unsigned int value);
    void (*udelay)(unsigned int us);
    void (*mdelay)(unsigned int ms);
    void (*wait_transfer_done)(void);
	void (*state_callback)(HDMI_STATE state);
}HDMI_UTIL_FUNCS;


typedef struct
{
    void (*set_util_funcs)(const HDMI_UTIL_FUNCS *util);
    void (*get_params)(HDMI_PARAMS *params);

    int (*init)(void);
    int (*enter)(void);
    int (*exit)(void);
    void (*suspend)(void);
    void (*resume)(void);
	int  (*audio_config)(HDMI_AUDIO_FORMAT aformat);
	int  (*video_config)(HDMI_VIDEO_RESOLUTION vformat, HDMI_VIDEO_INPUT_FORMAT vin, HDMI_VIDEO_OUTPUT_FORMAT vou);
	int  (*video_enable)(bool enable);
	int  (*audio_enable)(bool enable);
	int	 (*irq_enable)(bool enable);
	void (*power_on)(void);
	void (*power_off)(void);
    HDMI_STATE (*get_state)(void);
    void (*set_mode)(unsigned char ucMode);
    void (*dump)(void);
    void (*read)(unsigned char u8Reg);
    void (*write)(unsigned char u8Reg, unsigned char u8Data);
    void (*log_enable)(bool enable);
} HDMI_DRIVER;


// ---------------------------------------------------------------------------
//  HDMI Driver Functions
// ---------------------------------------------------------------------------

const HDMI_DRIVER* HDMI_GetDriver(void);

#endif // __HDMI_DRV_H__
