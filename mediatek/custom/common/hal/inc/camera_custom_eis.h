#ifndef _EIS_CONFIG_H
#define _EIS_CONFIG_H

    typedef enum
    {
        CUSTOMER_EIS_SENSI_LEVEL_HIGH = 0,
        CUSTOMER_EIS_SENSI_LEVEL_NORMAL = 1,
    
    } CUSTOMER_EIS_SENSI;

	CUSTOMER_EIS_SENSI get_EIS_Sensitivity();
	
#endif /* _EIS_CONFIG_H */

