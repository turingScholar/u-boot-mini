/*
 * URB OHCI HCD (Host Controller Driver) for USB.
 *
 * usb-ohci.h
 */

#define S3C6410_USB_HOST_BASE	0x74300000



typedef struct 
{
	volatile uint32_t HcRevision;
	volatile uint32_t HcControl;
	volatile uint32_t HcCommandStatus;
	volatile uint32_t HcInterruptStatus;
	volatile uint32_t HcInterruptEnable;
	volatile uint32_t HcInterruptDisable;
	volatile uint32_t HcHCCA;
	volatile uint32_t HcPeriodCurrentED;
	volatile uint32_t HcControlHeadED;
	volatile uint32_t HcControlCurrentED;
	volatile uint32_t HcBulkHeadED;
	volatile uint32_t HcBulkCurrentED;
	volatile uint32_t HcDoneHead;
	volatile uint32_t HcFmInterval;
	volatile uint32_t HcFmRemaining;
	volatile uint32_t HcFmNumber;
	volatile uint32_t HcPeriodicStart;
	volatile uint32_t HcLSThreshold;
	volatile uint32_t HcRhDescriptorA;
	volatile uint32_t HcRhDescriptorB;
	volatile uint32_t HcRhStatus;
	volatile uint32_t HcRhPortStatus1;
	volatile uint32_t HcRhPortStatus2;
	
}S3C6410_USB_OHCI ;


struct ohci_hcca 
{
	uint32_t		HccaInterruptTable[32];
	uint16_t		HccaFrameNumber;
	uint16_t		HccaPad1;
	uint32_t		HccaDoneHead;
	uint8_t		reserved[116];
} __attribute((aligned(256)));



struct ed
{
	uint32_t		hwINFO;
	uint32_t		hwTailP;
	uint32_t		hwHeadP;
	uint32_t		hwNextED;

} __attribute((aligned(16)));
typedef struct ed	ed_t;



struct td 
{
	uint32_t		hwINFO;
  	uint32_t		hwCBP;	
  	uint32_t		hwNextTD;
  	uint32_t		hwBE;

} __attribute((aligned(32)));
typedef struct td td_t;


static inline S3C6410_USB_OHCI* S3C6410_GetBase_OHCI(void)
{
	return (S3C6410_USB_OHCI* const)S3C6410_USB_HOST_BASE;
}


