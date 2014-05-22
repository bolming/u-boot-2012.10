#
# (C) Copyright 2000, 2001, 2002
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# (C) Copyright 2008
# Guennadi Liakhovetki, DENX Software Engineering, <lg@denx.de>
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(BOARD).o

ifndef CONFIG_SPL_BUILD
COBJS-y	:= smdkv210.o
COBJS-$(CONFIG_NAND_S5PV210)	+= s5pv210_nand.o 
endif

ifdef CONFIG_SPL_BUILD
COBJS-y	+= mmc_boot.o init_mem.o
endif

ifndef	CONFIG_SKIP_LOWLEVEL_INIT
SOBJS	:= lowlevel_init.o mem_setup.o init_clock.o
ifdef CONFIG_USE_NAND_BOARD_INIT
SOBJS	+= nand.o
endif
endif

SRCS    := $(SOBJS:.o=.S) $(COBJS-y:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS-y))
SOBJS	:= $(addprefix $(obj),$(SOBJS))

ALL	:=	 $(LIB)

ifdef CONFIG_SPL_BUILD
ALL	+= $(OBJTREE)/tools/mk$(BOARD)spl
endif

all:	$(ALL)

$(LIB):	$(obj).depend $(SOBJS) $(OBJS)
	$(call cmd_link_o_target, $(SOBJS) $(OBJS))

#
ifdef CONFIG_SPL_BUILD
$(OBJTREE)/tools/mk$(BOARD)spl:	tools/mkv210_image.c
	$(HOSTCC) tools/mkv210_image.c -o $(OBJTREE)/tools/mk$(BOARD)spl
endif
#
#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
