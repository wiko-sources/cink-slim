#ifndef __CUST_DISPLAY_H__
#define __CUST_DISPLAY_H__

#define BAR_LEFT            (215)
#define BAR_TOP             (156)
#define BAR_RIGHT           (265)
#define BAR_BOTTOM          (278)

#define BAR_OCCUPIED_COLOR  (0x07E0)    // Green
#define BAR_EMPTY_COLOR     (0xFFFF)    // White
#define BAR_BG_COLOR        (0x0000)    // Black

#define CAPACITY_LEFT                (172) // battery capacity center
#define CAPACITY_TOP                 (330)
#define CAPACITY_RIGHT               (307)
#define CAPACITY_BOTTOM              (546)

#define NUMBER_LEFT                  (178) // number
#define NUMBER_TOP                   (190)
#define NUMBER_RIGHT                 (216)
#define NUMBER_BOTTOM                (244)

#define PERCENT_LEFT                 (254) // percent number_left + 2*number_width
#define PERCENT_TOP                  (190)
#define PERCENT_RIGHT                (302)
#define PERCENT_BOTTOM               (244)

#define TOP_ANIMATION_LEFT           (172) // top animation
#define TOP_ANIMATION_TOP            (100)
#define TOP_ANIMATION_RIGHT          (307)
#define TOP_ANIMATION_BOTTOM         (124)

/* The option of new charging animation */
#define ANIMATION_NEW
#define NO_PERCENT_NEW


#endif // __CUST_DISPLAY_H__
