#ifndef __MT6577_DRAMC_H__
#define __MT6577_DRAMC_H__

typedef enum
{
    /* LPDDR allowable clock range -- begin */
    LPDDR_DRAM_CLOCK_156 = 156,
    LPDDR_DRAM_CLOCK_161 = 161,
    LPDDR_DRAM_CLOCK_166 = 166,
    LPDDR_DRAM_CLOCK_171 = 171,
    LPDDR_DRAM_CLOCK_176 = 176,
    LPDDR_DRAM_CLOCK_182 = 182,
    LPDDR_DRAM_CLOCK_187 = 187,
    LPDDR_DRAM_CLOCK_192 = 192, /* LPDDR default clock */
    /* LPDDR allowable clock range -- end */

    /* LPDDR2 allowable clock range -- begin */
    LPDDR2_DRAM_CLOCK_195 = 195,
    LPDDR2_DRAM_CLOCK_201 = 201,
    LPDDR2_DRAM_CLOCK_208 = 208,
    LPDDR2_DRAM_CLOCK_214 = 214,
    LPDDR2_DRAM_CLOCK_221 = 221,
    LPDDR2_DRAM_CLOCK_227 = 227,
    LPDDR2_DRAM_CLOCK_234 = 234,
    LPDDR2_DRAM_CLOCK_240 = 240,
    LPDDR2_DRAM_CLOCK_247 = 247,
    LPDDR2_DRAM_CLOCK_253 = 254,
    LPDDR2_DRAM_CLOCK_260 = 260, /* LPDDR2 default clock */
    /* LPDDR2 allowable clock range -- end */
}
EN_DRAM_CLOCK;

int get_dram_refresh_clock(void);
int get_dram_default_refresh_clock(void);

/*
 * refresh_interval is proportional to 1/refresh_clock
 */
int set_dram_refresh_clock(int clock);

int get_dram_clock(void);
int get_dram_default_clock(void);
int set_dram_clock(EN_DRAM_CLOCK clock);


#endif
