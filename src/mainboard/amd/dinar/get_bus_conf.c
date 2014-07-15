/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <cpu/amd/amdfam15.h>
#include "agesawrapper.h"


/* Global variables for MB layouts and these will be shared by irqtable mptable
 * and acpi_tables busnum is default.
 */
u8 bus_isa;
u8 bus_sb700[2];
u8 bus_rd890[14];

/*
 * Here you only need to set value in pci1234 for HT-IO that could be installed or not
 * You may need to preset pci1234 for HTIO board,
 * please refer to src/northbridge/amd/amdk8/get_sblk_pci1234.c for detail
 */
u32 pci1234x[] = {
	0x0000ff0,
};

/*
 * HT Chain device num, actually it is unit id base of every ht device in chain,
 * assume every chain only have 4 ht device at most
 */
u32 hcdnx[] = {
	0x20202020,
};

u32 sbdn_sb700;
u32 sbdn_rd890;

void get_bus_conf(void)
{
	device_t dev;
	int i;

	printk(BIOS_DEBUG, "Mainboard - Get_bus_conf.c - get_bus_conf - Start.\n");

	sbdn_sb700 = 0;

	for (i = 0; i < ARRAY_SIZE(bus_sb700); i++) {
		bus_sb700[i] = 0;
	}
	for (i = 0; i < ARRAY_SIZE(bus_rd890); i++) {
		bus_rd890[i] = 0;
	}


	bus_rd890[0] = (pci1234x[0] >> 16) & 0xff;
	bus_sb700[0] = bus_rd890[0];

	/* sb700 */
	dev = dev_find_slot(bus_sb700[0], PCI_DEVFN(sbdn_sb700 + 0x14, 4));



	if (dev) {
		bus_sb700[1] = pci_read_config8(dev, PCI_SECONDARY_BUS);

		bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
		bus_isa++;
	}

	/* rd890 */
	for (i = 1; i < ARRAY_SIZE(bus_rd890); i++) {
		dev = dev_find_slot(bus_rd890[0], PCI_DEVFN(sbdn_rd890 + i, 0));
		if (dev) {
			bus_rd890[i] = pci_read_config8(dev, PCI_SECONDARY_BUS);
			if(255 != bus_rd890[i]) {
				bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
				bus_isa++;
			}
		}
	}


	/* I/O APICs:   APIC ID Version State   Address */
	bus_isa = 10;

	printk(BIOS_DEBUG, "Mainboard - Get_bus_conf.c - get_bus_conf - End.\n");
}
