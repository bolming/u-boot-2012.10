/*
 * (C) Copyright 2000-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

/*
 * Misc boot support
 */
#include <common.h>
#include <command.h>

#ifdef CONFIG_CMD_XXX

/* Allow ports to override the default behavior */
__attribute__((weak))
int do_xxx (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int     rcode = 0;
	int 	i;

	printf ("do_xxx() argc :%d\n", argc);

	for(i = 0; i < argc; i++){
		printf("arg[%d]: %s\n", i, argv[i]);
	}

	return rcode;
}

/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	xxx, CONFIG_SYS_MAXARGS, 1,do_xxx,
	"xxx cmd",
	"xxx [arg ...]\n	-show all args\n"
);

#endif
