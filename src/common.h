
#ifndef SOFTBTS_COMMON_H
#define SOFTBTS_COMMON_H 1

#if HAVE_CONFIG
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>
#include <fcntl.h>
#include <libgen.h> /* basename() */
#include <glib.h>

#include <time.h>       /* struct tm */
#include <stdarg.h> 


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "../lib/talloc/talloc.h"
#include "../lib/linuxlist/linuxlist.h"
#include "../lib/select/select.h"
#include "../lib/timer/timer.h"
#include "../lib/vty/vty.h"
#include "../lib/vty/version.h"
#include "../lib/vty/telnet_interface.h"

#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS  0
#  define EXIT_FAILURE  1
#endif



#endif /* SOFTBTS_COMMON_H */


