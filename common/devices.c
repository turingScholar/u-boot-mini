/*
 * (C) Copyright 2000
 * Paolo Scaffardi, AIRVENT SAM s.p.a - RIMINI(ITALY), arsenio@tin.it
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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

#include <config.h>
#include <common.h>
#include <stdarg.h>
#include <malloc.h>
#include <devices.h>
#include <serial.h>


DECLARE_GLOBAL_DATA_PTR;

list_t devlist = 0;
device_t *stdio_devices[] = { NULL, NULL, NULL };
char *stdio_names[MAX_FILES] = { "stdin", "stdout", "stderr" };


static void drv_system_init (void)
{
	device_t dev;

	memset (&dev, 0, sizeof (dev));
	strcpy (dev.name, "serial");
	
	dev.flags = DEV_FLAGS_OUTPUT | DEV_FLAGS_INPUT | DEV_FLAGS_SYSTEM;
	dev.putc = serial_putc;
	dev.puts = serial_puts;
	dev.getc = serial_getc;
	dev.tstc = serial_tstc;

	device_register (&dev);
}

int device_register (device_t * dev)
{
	ListInsertItem (devlist, dev, LIST_END);
	return 0;
}

int devices_done (void)
{
	ListDispose (devlist);

	return 0;
}

int devices_init (void)
{
	ulong relocation_offset = gd->reloc_off;
	int i;

	/* relocate device name pointers */
	for (i = 0; i < (sizeof (stdio_names) / sizeof (char *)); ++i) 
	{
		stdio_names[i] = (char *) (((ulong) stdio_names[i]) +
						relocation_offset);
	}

	/* Initialize the list */
	devlist = ListCreate (sizeof (device_t));

	if (devlist == NULL) {
		printf("Cannot initialize the list of devices!\n");
		return -1;
	}

	drv_system_init ();

	return (0);
}

