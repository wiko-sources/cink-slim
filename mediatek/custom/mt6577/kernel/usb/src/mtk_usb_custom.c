#include <mach/mtk_musb.h>
#include <mach/mt6577_clock_manager.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <asm/io.h>

#define FRA (48)
#define PARA (28)

bool usb_enable_clock(bool enable) 
{
    #ifndef CONFIG_MT6575T_FPGA
    static int count = 0;
    bool res = TRUE;
    
    if (enable && !count) {
        res = enable_clock(MT65XX_PDN_PERI_USB1, "PERI_USB");
        count++;        
    } else if (!enable) {
        res = disable_clock(MT65XX_PDN_PERI_USB1, "PERI_USB");
        count=0;
    }
    printk("enable(%d), count(%d)\n", enable, count);
    #endif

    return 1;
}

void hs_slew_rate_cal(void){
  
  unsigned long data;
  unsigned long x;
  unsigned char value;
  unsigned long start_time, timeout;
  unsigned int timeout_flag = 0;
  //s1:
  USBPHY_WRITE8(0x12,0x81);
  //s2:
  USBPHY_WRITE8 (0xf00-0x800+0x11,0x01);
  USBPHY_WRITE8 (0xf00-0x800+0x01,0x04);
  USBPHY_SET8 (0xf00-0x800+0x03,0x01);
  //s3:
  start_time = jiffies;
  timeout = jiffies + 3 * HZ;
    
  while(!(USBPHY_READ8(0xf00-0x800+0x10)&0x1)){
    if(time_after(jiffies, timeout)){
        timeout_flag = 1;
        break;
        }
    }
  //s4:
  if(timeout_flag){
    printk("[USBPHY] Slew Rate Calibration: Timeout\n");
    value = 0x4;
    }
  else{
      data = USBPHY_READ32 (0xf00-0x800+0x0c);
      x = ((1024*FRA*PARA)/data);
      value = (unsigned char)(x/1000);
      if((x-value*1000)/100>=5)
        value += 1;
      //printk("[USBPHY]slew calibration:x=%d,value=%d\n",x,value);
    }
  //s5:
  USBPHY_CLR8 (0xf00-0x800+0x03,0x01);//disable frequency meter
  USBPHY_CLR8 (0xf00-0x800+0x11,0x01);//disable free run clock
  USBPHY_WRITE8(0x12,value);
}

void usb_phy_poweron(){
    
    #ifndef CONFIG_MT6575T_FPGA
    usb_enable_clock(true);

    udelay(50);

    USBPHY_CLR8(0x6b, 0x04);  
    USBPHY_CLR8(0x6e, 0x01);
	USBPHY_CLR8(0x1c, 0x80); 
    USBPHY_CLR8(0x02, 0x7f);
    USBPHY_SET8(0x02, 0x09);
    USBPHY_CLR8(0x22, 0x03);
    USBPHY_CLR8(0x6a, 0x04);
    USBPHY_SET8(0x1b, 0x08);

    udelay(800);
    #endif

    printk("usb power on success\n");
    return;
}

void usb_phy_savecurrent(){
    #ifndef CONFIG_MT6575T_FPGA
    USBPHY_CLR8(0x6b, 0x04);  
    USBPHY_CLR8(0x6e, 0x01); 
	
    USBPHY_CLR8(0x6a, 0x04); 

    USBPHY_SET8(0x68, 0xc0);
    USBPHY_CLR8(0x68, 0x30);
	
    USBPHY_SET8(0x68, 0x10);
	
	USBPHY_SET8(0x68, 0x04);
	USBPHY_CLR8(0x69, 0x3c);
	
    USBPHY_SET8(0x6a, 0x3a);   

	USBPHY_CLR8(0x1c, 0x80);
	USBPHY_CLR8(0x1b, 0x08);

    udelay(800);

    USBPHY_SET8(0x63, 0x02); 
    
    udelay(1);

    USBPHY_SET8(0x6a, 0x04); 

    udelay(1);

    usb_enable_clock(false);
    #endif

    printk("usb save current success\n");
    return;
}

void usb_phy_recover(){
    #ifndef CONFIG_MT6575T_FPGA
    usb_enable_clock(true);

    udelay(50);

    USBPHY_CLR8(0x6b, 0x04);  
    USBPHY_CLR8(0x6e, 0x1); 
	
	USBPHY_CLR8(0x6a, 0x04);
	
	USBPHY_CLR8(0x68, 0x40);
	USBPHY_CLR8(0x68, 0x80);
	USBPHY_CLR8(0x68, 0x30); 
	USBPHY_CLR8(0x68, 0x04);
	USBPHY_CLR8(0x69, 0x3c);
	
	USBPHY_CLR8(0x6a, 0x10);
	USBPHY_CLR8(0x6a, 0x20);
	USBPHY_CLR8(0x6a, 0x08);
	USBPHY_CLR8(0x6a, 0x02);
	USBPHY_CLR8(0x6a, 0x80);

	USBPHY_CLR8(0x1c, 0x80);
    USBPHY_SET8(0x1b, 0x08);
    udelay(800);

    hs_slew_rate_cal();
    #endif

    printk("usb recovery success\n");
    return;
}


