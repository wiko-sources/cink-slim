#ifndef CUST_USB_H
#define CUST_USB_H


/*=======================================================================*/
/* USB Control                                                           */	
/*=======================================================================*/
#define CONFIG_USBD_LANG                "0409"
#define USBD_MANUFACTURER               "MediaTek"
#define USBD_PRODUCT_NAME               "MT65xx Preloader"
#define USBD_VENDORID                   (0x0E8D)
#define USBD_PRODUCTID                  (0x2000)

// always defined
#define HAS_USBDL_KEY

#define CFG_USB_HANDSHAKE_TIMEOUT_EN    (1)
#define CFG_USB_ENUM_TIMEOUT_EN         (1)

#endif   /*_CUST_USBDL_FLOW_H*/
