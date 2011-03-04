#ifndef TELNET_INTERFACE_H
#define TELNET_INTERFACE_H

#include "../select/select.h"

/* Vty */
typedef struct socket_fd connection_t; /* see select.h */

struct  telnet_connection {
	struct llist_head entry;
        connection_t   fd;
        struct vty   *vty;
};
typedef struct  telnet_connection telnet_connection_t;

void telnet_init(int port);

#endif
