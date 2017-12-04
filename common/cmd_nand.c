#include <common.h>
#include <command.h>
#include <s3c6410.h>
#include <nand.h>



void nand_reset(void)
{
	NAND_ENABLE_CE();
	
	NAND_CMD(NAND_CMD_RESET);
	NAND_TRANS_RNB();
	
	NAND_DISABLE_CE();
}


void nand_init(void)
{
	MEM_SYS_CFG_REG &= ~(1<<1);


	NFCONF_REG &= ~((1<<30) | (7<<12) | (7<<8) | (7<<4));
	NFCONF_REG |= ((TACLS<<12) | (TWRPH0<<8) | (TWRPH1<<4));

	NFCONT_REG |= 1;

	nand_reset();	
}

int nand_read_id(void)
{
	unsigned char ch[16];
	int i, tag;

	
	NAND_ENABLE_CE();

	NAND_CMD(NAND_CMD_READ_ID);
	NAND_ADDR(0);
	
	NAND_TRANS_RNB();

	tag = 1;
	ch[0] = NAND_GET_BYTE();
	for(i=1; i<16; ++i)
	{
		ch[i] = NAND_GET_BYTE();
		if( ch[i] == ch[0] )
		{
			tag = i;
			break;
		}
	}
	
	NAND_DISABLE_CE();

	printf("\nRead ID: ");
	for(i=0; i<tag; ++i)
		printf("%x ", ch[i]);
	printf("\n");

	return 0;
}

int nand_read(void)
{
	int		i;
	uint		addr;
	uchar	*buffer;

	addr		= 0x200000 >> 12;
	buffer	= 0x50000000;

	NAND_ENABLE_CE();

	NAND_CMD(NAND_CMD_READ);
	
	NAND_ADDR(0);
	NAND_ADDR(0);
	NAND_ADDR( addr & 0xff );
	NAND_ADDR( (addr >>8) & 0xff );
	NAND_ADDR( (addr >>16) & 0xff );

	NAND_CMD(NAND_CMD_READ_START);
	
	NAND_TRANS_RNB();

	for(i=0; i<2048; ++i)
	{
		buffer[i] = NAND_GET_BYTE();
	}

	for(i=0; i<2048; ++i)
	{
		printf("%x ", buffer[i]);
		
		if(i%16==15)
			printf("\n");
	}
	

	NAND_DISABLE_CE();

	
	return 0;
}

int do_nand(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	nand_init();
	nand_read_id();
	nand_reset();
	nand_read();
	
	return 0;	
}


U_BOOT_CMD
	(nand, 5, 1, do_nand,
	"nand    - NAND sub-system\n",
	"info             - show available NAND devices\n"
	"nand device [dev]     - show or set current device\n"
	"nand read[.jffs2]     - addr off|partition size\n"
	"nand write[.jffs2]    - addr off|partiton size - read/write `size' bytes starting\n"
	"    at offset `off' to/from memory address `addr'\n"
 	"nand erase [clean] [off size] - erase `size' bytes from\n"
	"    offset `off' (entire device if not specified)\n"
	"nand bad - show bad blocks\n"
	"nand dump[.oob] off - dump page\n"
	"nand scrub - really clean NAND erasing bad blocks (UNSAFE)\n"
	"nand markbad off - mark bad block at offset (UNSAFE)\n"
	"nand biterr off - make a bit error at offset (UNSAFE)\n"
	"nand lock [tight] [status] - bring nand to lock state or display locked pages\n"
	"nand unlock [offset] [size] - unlock section\n");

