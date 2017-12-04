#include <common.h>
#include <command.h>
#include <s3c6410.h>
#include "usb_ohci.h"



int usb_lowlevel_init(void)
{	
	CLK_SRC_REG &= 0x9b;
	CLK_SRC_REG |= 0x04;
	
	CLK_DIV1_REG &= 0xff0fffff;
	
	SCLK_GATE_REG |= 0x40000000;
	
	return 0;
}


int usb_init(void)
{
	S3C6410_USB_OHCI *const ohci_regs = S3C6410_GetBase_OHCI();
	uint32_t		tmp;
	
	//usb_lowlevel_init();
	
	tmp = ohci_regs->HcRevision;
	printf("HcRevision: %02x\n", tmp);
	if( (tmp & 0xff) != 0x10 )
	{
		printf("HcRevision error: 0x%02x\n", tmp);
		return -1;
	}
	
	
	return 0;
	
}

#define NF8_ReadPage(a,b,c) (((int(*)(uint, uint, uchar*))(*((uint *)0x0C004000)))(a,b,c))
#define NF8_ReadPage_Adv(a,b,c) (((int(*)(uint, uint, uchar*))(*((uint *)0x0C004004)))(a,b,c)) 


#define sd_readl(x)		readl(ELFIN_HSMMC_BASE + (x))
#define sd_readw(x)		readw(ELFIN_HSMMC_BASE + (x))
#define sd_readb(x)		readb(ELFIN_HSMMC_BASE + (x))


#define sd_writel(v,x)		writel((v),ELFIN_HSMMC_BASE + (x))
#define sd_writew(v,x)	writew((v),ELFIN_HSMMC_BASE + (x))
#define sd_writeb(v,x)		writeb((v),ELFIN_HSMMC_BASE + (x))


int do_test(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	uint	reg;
	S3C64XX_GPIO *gpio = S3C64XX_GetBase_GPIO();

	reg = gpio->GPGCON & 0xf0000000;
	gpio->GPGCON = reg | 0x02222222;

	reg = gpio->GPGPUD & 0xfffff000;
	gpio->GPGPUD = reg;

	sd_writeb(1, SW_RST_OFFSET);
	
	return 0;
}



U_BOOT_CMD(
		   test,	CFG_MAXARGS,	1,	do_test,
		   "test    - test command\n",
		   "[args..]\n"
		   "    - test functionality\n"
		   );
