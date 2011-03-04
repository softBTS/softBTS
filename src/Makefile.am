INCLUDES = $(all_includes) -I$(top_srcdir)/include
AM_CFLAGS=-Wall  -Wno-pointer-sign

sbin_PROGRAMS = softbts
softbts_INCLUDES = softbts.h common.h\
		   logger.h\
		   bts_init.h\
		   bts_nm.h\
                   trx_init.h\
 	           abis_rsl.h\
                   L3_0408.h\
		   L3_utils.h\
		   abis_utils.h  
 
softbts_SOURCES = softbts.c\
		  logger.c \
		  bts_init.c\
		  bts_nm.c \
		  trx_init.c\
                  abis_rsl.c\
                  L3_utils.c\
                  abis_utils.c
                  
		          
	         



softbts_LDADD = ../lib/select/libselect.a \
		../lib/talloc/libtalloc.a \
		../lib/timer/libtimer.a \
		../lib/vty/libvty.a \
		../lib/linuxlist/liblinuxlist.a 


