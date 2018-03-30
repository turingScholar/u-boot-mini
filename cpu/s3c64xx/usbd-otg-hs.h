/*
 * cpu/s3c64xx/usbd-otg-hs.h
 *
 *  $Id: usbd-otg-hs.h,v 1.1 2007/04/17 02:01:33 dasan Exp $
 *
 * (C) Copyright 2007
 * Byungjae Lee, Samsung Erectronics, bjlee@samsung.com.
 *	- only support for S3C6400
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __S3C_OTG_HS_H__
#define __S3C_OTG_HS_H__

#include <asm/byteorder.h>
#include <asm/hardware.h>
//#include <asm/io.h>


#define readl(a) (*((vu_long *)(a)))
#define writel(a, b) (*((vu_long *)(b)) = ((vu_long)a))


/*
 * USB2.0 HS OTG (Chapter 26)
 */
#define USBOTG_LINK_BASE		(0x7C000000)
#define USBOTG_PHY_BASE		(0x7C100000)

/* Core Global Registers */
#define S3C_OTG_GOTGCTL		(USBOTG_LINK_BASE + 0x000)	/* OTG Control & Status */
#define S3C_OTG_GOTGINT		(USBOTG_LINK_BASE + 0x004)	/* OTG Interrupt */
#define S3C_OTG_GAHBCFG		(USBOTG_LINK_BASE + 0x008)	/* Core AHB Configuration */
#define S3C_OTG_GUSBCFG		(USBOTG_LINK_BASE + 0x00C)	/* Core USB Configuration */
#define S3C_OTG_GRSTCTL		(USBOTG_LINK_BASE + 0x010)	/* Core Reset */
#define S3C_OTG_GINTSTS		(USBOTG_LINK_BASE + 0x014)	/* Core Interrupt */
#define S3C_OTG_GINTMSK		(USBOTG_LINK_BASE + 0x018)	/* Core Interrupt Mask */
#define S3C_OTG_GRXSTSR		(USBOTG_LINK_BASE + 0x01C)	/* Receive Status Debug Read/Status Read */
#define S3C_OTG_GRXSTSP		(USBOTG_LINK_BASE + 0x020)	/* Receive Status Debug Pop/Status Pop */
#define S3C_OTG_GRXFSIZ		(USBOTG_LINK_BASE + 0x024)	/* Receive FIFO Size */
#define S3C_OTG_GNPTXFSIZ	(USBOTG_LINK_BASE + 0x028)	/* Non-Periodic Transmit FIFO Size */
#define S3C_OTG_GNPTXSTS	(USBOTG_LINK_BASE + 0x02C)	/* Non-Periodic Transmit FIFO/Queue Status */

#define S3C_OTG_HPTXFSIZ	(USBOTG_LINK_BASE + 0x100)	/* Host Periodic Transmit FIFO Size */
#define S3C_OTG_DPTXFSIZ1	(USBOTG_LINK_BASE + 0x104)	/* Device Periodic Transmit FIFO-1 Size */
#define S3C_OTG_DPTXFSIZ2	(USBOTG_LINK_BASE + 0x108)	/* Device Periodic Transmit FIFO-2 Size */
#define S3C_OTG_DPTXFSIZ3	(USBOTG_LINK_BASE + 0x10C)	/* Device Periodic Transmit FIFO-3 Size */
#define S3C_OTG_DPTXFSIZ4	(USBOTG_LINK_BASE + 0x110)	/* Device Periodic Transmit FIFO-4 Size */
#define S3C_OTG_DPTXFSIZ5	(USBOTG_LINK_BASE + 0x114)	/* Device Periodic Transmit FIFO-5 Size */
#define S3C_OTG_DPTXFSIZ6	(USBOTG_LINK_BASE + 0x118)	/* Device Periodic Transmit FIFO-6 Size */
#define S3C_OTG_DPTXFSIZ7	(USBOTG_LINK_BASE + 0x11C)	/* Device Periodic Transmit FIFO-7 Size */
#define S3C_OTG_DPTXFSIZ8	(USBOTG_LINK_BASE + 0x120)	/* Device Periodic Transmit FIFO-8 Size */
#define S3C_OTG_DPTXFSIZ9	(USBOTG_LINK_BASE + 0x124)	/* Device Periodic Transmit FIFO-9 Size */
#define S3C_OTG_DPTXFSIZ10	(USBOTG_LINK_BASE + 0x128)	/* Device Periodic Transmit FIFO-10 Size */
#define S3C_OTG_DPTXFSIZ11	(USBOTG_LINK_BASE + 0x12C)	/* Device Periodic Transmit FIFO-11 Size */
#define S3C_OTG_DPTXFSIZ12	(USBOTG_LINK_BASE + 0x130)	/* Device Periodic Transmit FIFO-12 Size */
#define S3C_OTG_DPTXFSIZ13	(USBOTG_LINK_BASE + 0x134)	/* Device Periodic Transmit FIFO-13 Size */
#define S3C_OTG_DPTXFSIZ14	(USBOTG_LINK_BASE + 0x138)	/* Device Periodic Transmit FIFO-14 Size */
#define S3C_OTG_DPTXFSIZ15	(USBOTG_LINK_BASE + 0x13C)	/* Device Periodic Transmit FIFO-15 Size */
	
/* Host Global Registers */
#define S3C_OTG_HCFG		(USBOTG_LINK_BASE + 0x400)	/* Host Configuration */
#define S3C_OTG_HFIR		(USBOTG_LINK_BASE + 0x404)	/* Host Frame Interval */
#define S3C_OTG_HFNUM		(USBOTG_LINK_BASE + 0x408)	/* Host Frame Number/Frame Time Remaining */
#define S3C_OTG_HPTXSTS		(USBOTG_LINK_BASE + 0x410)	/* Host Periodic Transmit FIFO/Queue Status */
#define S3C_OTG_HAINT		(USBOTG_LINK_BASE + 0x414)	/* Host All Channels Interrupt */
#define S3C_OTG_HAINTMSK	(USBOTG_LINK_BASE + 0x418)	/* Host All Channels Interrupt Mask */

/* Host Port Control & Status Registers */
#define S3C_OTG_HPRT		(USBOTG_LINK_BASE + 0x440)	/* Host Port Control & Status */

/* Host Channel-Specific Registers */
#define S3C_OTG_HCCHAR0		(USBOTG_LINK_BASE + 0x500)	/* Host Channel-0 Characteristics */
#define S3C_OTG_HCSPLT0		(USBOTG_LINK_BASE + 0x504)	/* Host Channel-0 Split Control */
#define S3C_OTG_HCINT0		(USBOTG_LINK_BASE + 0x508)	/* Host Channel-0 Interrupt */
#define S3C_OTG_HCINTMSK0	(USBOTG_LINK_BASE + 0x50C)	/* Host Channel-0 Interrupt Mask */
#define S3C_OTG_HCTSIZ0		(USBOTG_LINK_BASE + 0x510)	/* Host Channel-0 Transfer Size */
#define S3C_OTG_HCDMA0		(USBOTG_LINK_BASE + 0x514)	/* Host Channel-0 DMA Address */

	
/* Device Global Registers */
#define S3C_OTG_DCFG		(USBOTG_LINK_BASE + 0x800)	/* Device Configuration */
#define S3C_OTG_DCTL		(USBOTG_LINK_BASE + 0x804)	/* Device Control */
#define S3C_OTG_DSTS		(USBOTG_LINK_BASE + 0x808)	/* Device Status */
#define S3C_OTG_DIEPMSK 	(USBOTG_LINK_BASE + 0x810)	/* Device IN Endpoint Common Interrupt Mask */
#define S3C_OTG_DOEPMSK 	(USBOTG_LINK_BASE + 0x814)	/* Device OUT Endpoint Common Interrupt Mask */
#define S3C_OTG_DAINT		(USBOTG_LINK_BASE + 0x818)	/* Device All Endpoints Interrupt */
#define S3C_OTG_DAINTMSK	(USBOTG_LINK_BASE + 0x81C)	/* Device All Endpoints Interrupt Mask */
#define S3C_OTG_DTKNQR1 	(USBOTG_LINK_BASE + 0x820)	/* Device IN Token Sequence Learning Queue Read 1 */
#define S3C_OTG_DTKNQR2 	(USBOTG_LINK_BASE + 0x824)	/* Device IN Token Sequence Learning Queue Read 2 */
#define S3C_OTG_DVBUSDIS	(USBOTG_LINK_BASE + 0x828)	/* Device VBUS Discharge Time */
#define S3C_OTG_DVBUSPULSE	(USBOTG_LINK_BASE + 0x82C)	/* Device VBUS Pulsing Time */
#define S3C_OTG_DTKNQR3 	(USBOTG_LINK_BASE + 0x830)	/* Device IN Token Sequence Learning Queue Read 3 */
#define S3C_OTG_DTKNQR4 	(USBOTG_LINK_BASE + 0x834)	/* Device IN Token Sequence Learning Queue Read 4 */
	
/* Device Logical IN Endpoint-Specific Registers */
#define S3C_OTG_DIEPCTL0	(USBOTG_LINK_BASE + 0x900)	/* Device IN Endpoint 0 Control */
#define S3C_OTG_DIEPINT0	(USBOTG_LINK_BASE + 0x908)	/* Device IN Endpoint 0 Interrupt */
#define S3C_OTG_DIEPTSIZ0	(USBOTG_LINK_BASE + 0x910)	/* Device IN Endpoint 0 Transfer Size */
#define S3C_OTG_DIEPDMA0	(USBOTG_LINK_BASE + 0x914)	/* Device IN Endpoint 0 DMA Address */

/* Device Logical OUT Endpoint-Specific Registers */
#define S3C_OTG_DOEPCTL0	(USBOTG_LINK_BASE + 0xB00)	/* Device OUT Endpoint 0 Control */
#define S3C_OTG_DOEPINT0	(USBOTG_LINK_BASE + 0xB08)	/* Device OUT Endpoint 0 Interrupt */
#define S3C_OTG_DOEPTSIZ0	(USBOTG_LINK_BASE + 0xB10)	/* Device OUT Endpoint 0 Transfer Size */
#define S3C_OTG_DOEPDMA0	(USBOTG_LINK_BASE + 0xB14)	/* Device OUT Endpoint 0 DMA Address */
	
/* Power & clock gating registers */
#define S3C_OTG_PCGCCTRL	(USBOTG_LINK_BASE + 0xE00)

/* Endpoint FIFO address */
#define S3C_OTG_EP0_FIFO	(USBOTG_LINK_BASE + 0x1000)



/* OTG PHY CORE REGISTERS */
#define S3C_OTG_PHYPWR		(USBOTG_PHY_BASE+0x00)
#define S3C_OTG_PHYCTRL		(USBOTG_PHY_BASE+0x04)
#define S3C_OTG_RSTCON		(USBOTG_PHY_BASE+0x08)





#if 1
#define S3C_USBD_DETECT_IRQ()	(readl(0x7C000000 + 0x014) & 0x800c3810)
#define S3C_USBD_CLEAR_IRQ()	do { \
					writel(BIT_ALLMSK, (0x7C000000+0x014)); \
				} while (0)
#else
#define S3C_USBD_DETECT_IRQ()	(readl(0x71300008) & (1<<26))
#define S3C_USBD_CLEAR_IRQ()	do { \
					writel((1<<26), 0x71300008); \
				} while (0)
#endif

#define CONTROL_EP		0
#define BULK_IN_EP		1
#define BULK_OUT_EP		2

#define FS_CTRL_PKT_SIZE	8
#define FS_BULK_PKT_SIZE	64

#define HS_CTRL_PKT_SIZE	64
#define HS_BULK_PKT_SIZE	512

#define RX_FIFO_SIZE		512
#define NPTX_FIFO_START_ADDR	RX_FIFO_SIZE
#define NPTX_FIFO_SIZE		512
#define PTX_FIFO_SIZE		512

// string descriptor
#define LANGID_US_L		(0x09)
#define LANGID_US_H		(0x04)

// Feature Selectors
#define EP_STALL		0
#define DEVICE_REMOTE_WAKEUP	1
#define TEST_MODE		2

/* Test Mode Selector*/
#define TEST_J			1
#define TEST_K			2
#define TEST_SE0_NAK		3
#define TEST_PACKET		4
#define TEST_FORCE_ENABLE	5

#define S3C_OTG_DIEPCTL_IN	(S3C_OTG_DIEPCTL0 + 0x20*BULK_IN_EP)
#define S3C_OTG_DIEPINT_IN	(S3C_OTG_DIEPINT0 + 0x20*BULK_IN_EP)
#define S3C_OTG_DIEPTSIZ_IN	(S3C_OTG_DIEPTSIZ0 + 0x20*BULK_IN_EP)
#define S3C_OTG_DIEPDMA_IN	(S3C_OTG_DIEPDMA0 + 0x20*BULK_IN_EP)
#define S3C_OTG_DOEPCTL_OUT	(S3C_OTG_DOEPCTL0 + 0x20*BULK_OUT_EP)
#define S3C_OTG_DOEPINT_OUT	(S3C_OTG_DOEPINT0 + 0x20*BULK_OUT_EP)
#define S3C_OTG_DOEPTSIZ_OUT	(S3C_OTG_DOEPTSIZ0 + 0x20*BULK_OUT_EP)
#define S3C_OTG_DOEPDMA_OUT	(S3C_OTG_DOEPDMA0 + 0x20*BULK_OUT_EP)
#define S3C_OTG_IN_FIFO		(S3C_OTG_EP0_FIFO + 0x1000*BULK_IN_EP)
#define S3C_OTG_OUT_FIFO	(S3C_OTG_EP0_FIFO + 0x1000*BULK_OUT_EP)


typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bcdUSBL;
	uint8_t bcdUSBH;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	uint8_t idVendorL;
	uint8_t idVendorH;
	uint8_t idProductL;
	uint8_t idProductH;
	uint8_t bcdDeviceL;
	uint8_t bcdDeviceH;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
} __attribute__ ((packed)) device_desc_t;

typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t wTotalLengthL;
	uint8_t wTotalLengthH;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	uint8_t bmAttributes;
	uint8_t maxPower;
} __attribute__ ((packed)) config_desc_t;

typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
} __attribute__ ((packed)) intf_desc_t;

typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	uint8_t wMaxPacketSizeL;
	uint8_t wMaxPacketSizeH;
	uint8_t bInterval;
} __attribute__ ((packed)) ep_desc_t;

typedef struct
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint8_t wValue_L;
	uint8_t wValue_H;
	uint8_t wIndex_L;
	uint8_t wIndex_H;
	uint8_t wLength_L;
	uint8_t wLength_H;
} __attribute__ ((packed)) device_req_t;

typedef struct
{
	device_desc_t dev;
	config_desc_t config;
	intf_desc_t intf;
	ep_desc_t ep1;
	ep_desc_t ep2;
	ep_desc_t ep3;
	ep_desc_t ep4;
} __attribute__ ((packed)) descriptors_t;

typedef struct
{
	uint8_t Device;
	uint8_t Interface;
	uint8_t ep_ctrl;
	uint8_t ep_in;
	uint8_t ep_out;
} __attribute__ ((packed)) get_status_t;

typedef struct
{
	uint8_t AlternateSetting;
} __attribute__ ((packed)) get_intf_t;


typedef enum
{
	USB_CPU, USB_DMA
} USB_OPMODE;

typedef enum
{
	USB_HIGH, USB_FULL, USB_LOW
} USB_SPEED;

typedef enum
{
	EP_TYPE_CONTROL, EP_TYPE_ISOCHRONOUS, EP_TYPE_BULK, EP_TYPE_INTERRUPT
} EP_TYPE;


typedef struct
{
	descriptors_t desc;
	device_req_t dev_req;

	uint32_t  ep0_state;
	uint32_t  ep0_substate;
	USB_OPMODE op_mode;
	USB_SPEED speed;
	uint32_t  ctrl_max_pktsize;
	uint32_t  bulkin_max_pktsize;
	uint32_t  bulkout_max_pktsize;
	uint32_t  dn_addr;
	uint32_t  dn_filesize;
	uint32_t  up_addr;
	uint32_t  up_size;
	uint8_t*  dn_ptr;
	uint8_t*  up_ptr;
	uint32_t  set_config;
	uint32_t  req_length;
} __attribute__ ((packed)) otg_dev_t;

#if 0
typedef struct
{
	uint8_t ConfigurationValue;
} __attribute__ ((packed)) config_set_t;
#endif

// SPEC1.1

// Standard bmRequestType (direction)
enum DEV_REQUEST_DIRECTION
{
	HOST_TO_DEVICE				= 0x00,
	DEVICE_TO_HOST				= 0x80
};

// Standard bmRequestType (Type)
enum DEV_REQUEST_TYPE
{
	STANDARD_TYPE			= 0x00,
	CLASS_TYPE			= 0x20,
	VENDOR_TYPE			= 0x40,
	RESERVED_TYPE			= 0x60
};

// Standard bmRequestType (Recipient)
enum DEV_REQUEST_RECIPIENT
{
	DEVICE_RECIPIENT		= 0,
	INTERFACE_RECIPIENT		= 1,
	ENDPOINT_RECIPIENT		= 2,
	OTHER_RECIPIENT			= 3
};

// Descriptor types
enum DESCRIPTOR_TYPE
{
	DEVICE_DESCRIPTOR		= 1,
	CONFIGURATION_DESCRIPTOR	= 2,
	STRING_DESCRIPTOR		= 3,
	INTERFACE_DESCRIPTOR		= 4,
	ENDPOINT_DESCRIPTOR		= 5,
	DEVICE_QUALIFIER		= 6,
	OTHER_SPEED_CONFIGURATION	= 7,
	INTERFACE_POWER			= 8
};

// configuration descriptor: bmAttributes
enum CONFIG_ATTRIBUTES
{
	CONF_ATTR_DEFAULT		= 0x80,
	CONF_ATTR_REMOTE_WAKEUP 	= 0x20,
	CONF_ATTR_SELFPOWERED		= 0x40
};

// endpoint descriptor
enum ENDPOINT_ATTRIBUTES
{
	EP_ADDR_IN			= 0x80,
	EP_ADDR_OUT			= 0x00,

	EP_ATTR_CONTROL			= 0x0,
	EP_ATTR_ISOCHRONOUS		= 0x1,
	EP_ATTR_BULK			= 0x2,
	EP_ATTR_INTERRUPT		= 0x3
};

// Standard bRequest codes
enum STANDARD_REQUEST_CODE
{
	STANDARD_GET_STATUS		= 0,
	STANDARD_CLEAR_FEATURE		= 1,
	STANDARD_RESERVED_1		= 2,
	STANDARD_SET_FEATURE		= 3,
	STANDARD_RESERVED_2		= 4,
	STANDARD_SET_ADDRESS		= 5,
	STANDARD_GET_DESCRIPTOR		= 6,
	STANDARD_SET_DESCRIPTOR		= 7,
	STANDARD_GET_CONFIGURATION	= 8,
	STANDARD_SET_CONFIGURATION	= 9,
	STANDARD_GET_INTERFACE		= 10,
	STANDARD_SET_INTERFACE		= 11,
	STANDARD_SYNCH_FRAME		= 12
};

int s3c_usbctl_init(void);
int s3c_usbc_activate (void);
int s3c_usb_stop( void );
void s3c_udc_int_hndlr(void);

/* in usbd-otg-hs.c */
extern unsigned int s3c_usbd_dn_addr;
extern unsigned int s3c_usbd_dn_cnt;
extern int DNW;
extern int s3c_got_header;
extern int s3c_receive_done;

#endif
