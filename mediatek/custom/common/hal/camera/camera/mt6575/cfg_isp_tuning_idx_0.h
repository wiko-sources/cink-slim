#ifndef _CFG_ISP_TUNING_IDX_0_H_
#define _CFG_ISP_TUNING_IDX_0_H_


/*******************************************************************************
*
*******************************************************************************/
#define Define_ESensorRole    ESensorRole_Main


/*************************************************************************************************
* On-line Preview
*************************************************************************************************/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Auto SCENE (SCENE_MODE_OFF)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_OFF, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_OFF, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_OFF, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_OFF, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_OFF, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Normal SCENE (SCENE_MODE_NORMAL)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_NORMAL, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_NORMAL, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_NORMAL, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_NORMAL, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_NORMAL, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Action SCENE (SCENE_MODE_ACTION)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_ACTION, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_ACTION, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_ACTION, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_ACTION, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_ACTION, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Portrait SCENE (SCENE_MODE_PORTRAIT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_PORTRAIT, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_PORTRAIT, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_PORTRAIT, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_PORTRAIT, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_PORTRAIT, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Landscape SCENE (SCENE_MODE_LANDSCAPE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_LANDSCAPE, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_LANDSCAPE, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_LANDSCAPE, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_LANDSCAPE, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_LANDSCAPE, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Night SCENE (SCENE_MODE_NIGHTSCENE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_NIGHTSCENE, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_NIGHTSCENE, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_NIGHTSCENE, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_NIGHTSCENE, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_NIGHTSCENE, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Night Portrait SCENE (SCENE_MODE_NIGHTPORTRAIT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Theatre SCENE (SCENE_MODE_THEATRE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_THEATRE, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_THEATRE, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_THEATRE, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_THEATRE, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_THEATRE, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Beach SCENE (SCENE_MODE_BEACH)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_BEACH, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_BEACH, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_BEACH, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_BEACH, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_BEACH, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Snow SCENE (SCENE_MODE_SNOW)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_SNOW, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_SNOW, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_SNOW, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_SNOW, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_SNOW, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sunset SCENE (SCENE_MODE_SUNSET)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_SUNSET, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_SUNSET, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_SUNSET, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_SUNSET, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_SUNSET, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Steady Photo SCENE (SCENE_MODE_STEADYPHOTO)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_STEADYPHOTO, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_STEADYPHOTO, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_STEADYPHOTO, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_STEADYPHOTO, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_STEADYPHOTO, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Fireworks SCENE (SCENE_MODE_FIREWORKS)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_FIREWORKS, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_FIREWORKS, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_FIREWORKS, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_FIREWORKS, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_FIREWORKS, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sports SCENE (SCENE_MODE_SPORTS)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_SPORTS, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_SPORTS, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_SPORTS, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_SPORTS, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_SPORTS, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Party SCENE (SCENE_MODE_PARTY)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_PARTY, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_PARTY, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_PARTY, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_PARTY, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_PARTY, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Candlelight SCENE (SCENE_MODE_CANDLELIGHT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Preview( SCENE_MODE_CANDLELIGHT, eIDX_ISO_100 )
IDX_SET( DM:0, DP:0, NR1:0, NR2:0, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:0);
// ISO 200
IDX_MODE_Online_Preview( SCENE_MODE_CANDLELIGHT, eIDX_ISO_200 )
IDX_SET( DM:0, DP:0, NR1:1, NR2:1, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:1);
// ISO 400
IDX_MODE_Online_Preview( SCENE_MODE_CANDLELIGHT, eIDX_ISO_400 )
IDX_SET( DM:0, DP:0, NR1:2, NR2:2, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:2);
// ISO 800
IDX_MODE_Online_Preview( SCENE_MODE_CANDLELIGHT, eIDX_ISO_800 )
IDX_SET( DM:1, DP:0, NR1:3, NR2:3, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:3);
// ISO 1600
IDX_MODE_Online_Preview( SCENE_MODE_CANDLELIGHT, eIDX_ISO_1600 )
IDX_SET( DM:2, DP:0, NR1:4, NR2:4, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:4);
//

/*************************************************************************************************
* On-line Capture
*************************************************************************************************/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Auto SCENE (SCENE_MODE_OFF)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_OFF, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_OFF, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_OFF, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_OFF, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_OFF, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Normal SCENE (SCENE_MODE_NORMAL)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_NORMAL, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_NORMAL, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_NORMAL, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_NORMAL, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_NORMAL, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Action SCENE (SCENE_MODE_ACTION)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_ACTION, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_ACTION, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_ACTION, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_ACTION, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_ACTION, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Portrait SCENE (SCENE_MODE_PORTRAIT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_PORTRAIT, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_PORTRAIT, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_PORTRAIT, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_PORTRAIT, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_PORTRAIT, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Landscape SCENE (SCENE_MODE_LANDSCAPE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_LANDSCAPE, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_LANDSCAPE, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_LANDSCAPE, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_LANDSCAPE, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_LANDSCAPE, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Night SCENE (SCENE_MODE_NIGHTSCENE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_NIGHTSCENE, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_NIGHTSCENE, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_NIGHTSCENE, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_NIGHTSCENE, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_NIGHTSCENE, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Night Portrait SCENE (SCENE_MODE_NIGHTPORTRAIT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Theatre SCENE (SCENE_MODE_THEATRE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_THEATRE, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_THEATRE, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_THEATRE, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_THEATRE, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_THEATRE, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Beach SCENE (SCENE_MODE_BEACH)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_BEACH, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_BEACH, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_BEACH, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_BEACH, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_BEACH, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Snow SCENE (SCENE_MODE_SNOW)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_SNOW, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_SNOW, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_SNOW, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_SNOW, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_SNOW, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sunset SCENE (SCENE_MODE_SUNSET)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_SUNSET, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_SUNSET, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_SUNSET, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_SUNSET, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_SUNSET, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Steady Photo SCENE (SCENE_MODE_STEADYPHOTO)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_STEADYPHOTO, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_STEADYPHOTO, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_STEADYPHOTO, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_STEADYPHOTO, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_STEADYPHOTO, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Fireworks SCENE (SCENE_MODE_FIREWORKS)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_FIREWORKS, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_FIREWORKS, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_FIREWORKS, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_FIREWORKS, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_FIREWORKS, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sports SCENE (SCENE_MODE_SPORTS)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_SPORTS, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_SPORTS, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_SPORTS, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_SPORTS, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_SPORTS, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Party SCENE (SCENE_MODE_PARTY)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_PARTY, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_PARTY, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_PARTY, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_PARTY, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_PARTY, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Candlelight SCENE (SCENE_MODE_CANDLELIGHT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Online_Capture( SCENE_MODE_CANDLELIGHT, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:5, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Online_Capture( SCENE_MODE_CANDLELIGHT, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:6, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Online_Capture( SCENE_MODE_CANDLELIGHT, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:7, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Online_Capture( SCENE_MODE_CANDLELIGHT, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:8, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Online_Capture( SCENE_MODE_CANDLELIGHT, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:9, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);


/*************************************************************************************************
* Off-line Capture Pass1
*************************************************************************************************/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Auto SCENE (SCENE_MODE_OFF)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_OFF, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_OFF, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_OFF, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_OFF, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_OFF, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Normal SCENE (SCENE_MODE_NORMAL)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NORMAL, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NORMAL, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NORMAL, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NORMAL, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NORMAL, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Action SCENE (SCENE_MODE_ACTION)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_ACTION, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_ACTION, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_ACTION, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_ACTION, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_ACTION, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Portrait SCENE (SCENE_MODE_PORTRAIT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_PORTRAIT, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_PORTRAIT, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_PORTRAIT, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_PORTRAIT, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_PORTRAIT, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Landscape SCENE (SCENE_MODE_LANDSCAPE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_LANDSCAPE, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_LANDSCAPE, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_LANDSCAPE, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_LANDSCAPE, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_LANDSCAPE, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Night SCENE (SCENE_MODE_NIGHTSCENE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NIGHTSCENE, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NIGHTSCENE, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NIGHTSCENE, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NIGHTSCENE, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NIGHTSCENE, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Night Portrait SCENE (SCENE_MODE_NIGHTPORTRAIT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Theatre SCENE (SCENE_MODE_THEATRE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_THEATRE, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_THEATRE, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_THEATRE, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_THEATRE, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_THEATRE, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Beach SCENE (SCENE_MODE_BEACH)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_BEACH, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_BEACH, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_BEACH, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_BEACH, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_BEACH, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Snow SCENE (SCENE_MODE_SNOW)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SNOW, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SNOW, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SNOW, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SNOW, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SNOW, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sunset SCENE (SCENE_MODE_SUNSET)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SUNSET, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SUNSET, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SUNSET, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SUNSET, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SUNSET, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Steady Photo SCENE (SCENE_MODE_STEADYPHOTO)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_STEADYPHOTO, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_STEADYPHOTO, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_STEADYPHOTO, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_STEADYPHOTO, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_STEADYPHOTO, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Fireworks SCENE (SCENE_MODE_FIREWORKS)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_FIREWORKS, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_FIREWORKS, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_FIREWORKS, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_FIREWORKS, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_FIREWORKS, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sports SCENE (SCENE_MODE_SPORTS)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SPORTS, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SPORTS, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SPORTS, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SPORTS, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_SPORTS, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Party SCENE (SCENE_MODE_PARTY)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_PARTY, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_PARTY, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_PARTY, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_PARTY, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_PARTY, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Candlelight SCENE (SCENE_MODE_CANDLELIGHT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_CANDLELIGHT, eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_CANDLELIGHT, eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_CANDLELIGHT, eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_CANDLELIGHT, eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass1( SCENE_MODE_CANDLELIGHT, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);


/*************************************************************************************************
* Off-line Capture Pass2
*************************************************************************************************/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Auto SCENE (SCENE_MODE_OFF)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_OFF, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_OFF, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_OFF, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_OFF, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_OFF, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Normal SCENE (SCENE_MODE_NORMAL)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NORMAL, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NORMAL, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NORMAL, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NORMAL, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NORMAL, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Action SCENE (SCENE_MODE_ACTION)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_ACTION, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_ACTION, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_ACTION, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_ACTION, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_ACTION, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Portrait SCENE (SCENE_MODE_PORTRAIT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_PORTRAIT, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_PORTRAIT, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_PORTRAIT, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_PORTRAIT, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_PORTRAIT, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Landscape SCENE (SCENE_MODE_LANDSCAPE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_LANDSCAPE, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_LANDSCAPE, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_LANDSCAPE, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_LANDSCAPE, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_LANDSCAPE, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Night SCENE (SCENE_MODE_NIGHTSCENE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NIGHTSCENE, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NIGHTSCENE, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NIGHTSCENE, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NIGHTSCENE, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NIGHTSCENE, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Night Portrait SCENE (SCENE_MODE_NIGHTPORTRAIT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_NIGHTPORTRAIT, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Theatre SCENE (SCENE_MODE_THEATRE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_THEATRE, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_THEATRE, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_THEATRE, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_THEATRE, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_THEATRE, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Beach SCENE (SCENE_MODE_BEACH)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_BEACH, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_BEACH, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_BEACH, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_BEACH, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_BEACH, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Snow SCENE (SCENE_MODE_SNOW)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SNOW, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SNOW, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SNOW, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SNOW, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SNOW, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sunset SCENE (SCENE_MODE_SUNSET)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SUNSET, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SUNSET, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SUNSET, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SUNSET, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SUNSET, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Steady Photo SCENE (SCENE_MODE_STEADYPHOTO)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_STEADYPHOTO, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_STEADYPHOTO, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_STEADYPHOTO, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_STEADYPHOTO, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_STEADYPHOTO, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Fireworks SCENE (SCENE_MODE_FIREWORKS)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_FIREWORKS, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_FIREWORKS, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_FIREWORKS, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_FIREWORKS, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_FIREWORKS, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sports SCENE (SCENE_MODE_SPORTS)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SPORTS, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SPORTS, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SPORTS, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SPORTS, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_SPORTS, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Party SCENE (SCENE_MODE_PARTY)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_PARTY, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_PARTY, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_PARTY, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_PARTY, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_PARTY, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Candlelight SCENE (SCENE_MODE_CANDLELIGHT)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ISO 100
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_CANDLELIGHT, eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_CANDLELIGHT, eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_CANDLELIGHT, eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_CANDLELIGHT, eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_Offline_Capture_Pass2( SCENE_MODE_CANDLELIGHT, eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);


/*************************************************************************************************
* HDR Pass1 Single Frame
*************************************************************************************************/
// ISO 100
IDX_MODE_HDR_Cap_Pass1_SF( eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_HDR_Cap_Pass1_SF( eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_HDR_Cap_Pass1_SF( eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_HDR_Cap_Pass1_SF( eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_HDR_Cap_Pass1_SF( eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

/*************************************************************************************************
* HDR Pass1 Multi Frame Stage1
*************************************************************************************************/
// ISO 100
IDX_MODE_HDR_Cap_Pass1_MF1( eIDX_ISO_100 )
IDX_SET( DM:3, DP:0, NR1:5, NR2:15, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_HDR_Cap_Pass1_MF1( eIDX_ISO_200 )
IDX_SET( DM:3, DP:0, NR1:6, NR2:15, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_HDR_Cap_Pass1_MF1( eIDX_ISO_400 )
IDX_SET( DM:4, DP:0, NR1:7, NR2:15, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_HDR_Cap_Pass1_MF1( eIDX_ISO_800 )
IDX_SET( DM:5, DP:0, NR1:8, NR2:15, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_HDR_Cap_Pass1_MF1( eIDX_ISO_1600 )
IDX_SET( DM:6, DP:0, NR1:9, NR2:15, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);

/*************************************************************************************************
* HDR Pass2
*************************************************************************************************/
// ISO 100
IDX_MODE_HDR_Cap_Pass2( eIDX_ISO_100 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:10, Saturation:0, Contrast:4, Hue:1, Gamma:0, EE:5);
// ISO 200
IDX_MODE_HDR_Cap_Pass2( eIDX_ISO_200 )
IDX_SET( DM:3, DP:3, NR1:10, NR2:11, Saturation:1, Contrast:4, Hue:1, Gamma:0, EE:6);
// ISO 400
IDX_MODE_HDR_Cap_Pass2( eIDX_ISO_400 )
IDX_SET( DM:4, DP:3, NR1:10, NR2:12, Saturation:2, Contrast:4, Hue:1, Gamma:0, EE:7);
// ISO 800
IDX_MODE_HDR_Cap_Pass2( eIDX_ISO_800 )
IDX_SET( DM:5, DP:3, NR1:10, NR2:13, Saturation:3, Contrast:4, Hue:1, Gamma:0, EE:8);
// ISO 1600
IDX_MODE_HDR_Cap_Pass2( eIDX_ISO_1600 )
IDX_SET( DM:6, DP:3, NR1:10, NR2:14, Saturation:4, Contrast:4, Hue:1, Gamma:0, EE:9);


/*******************************************************************************
*
*******************************************************************************/
#undef  Define_ESensorRole
//
#endif //  _CFG_ISP_TUNING_IDX_0_H_

