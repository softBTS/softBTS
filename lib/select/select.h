#ifndef _SOCKET_SELECT_H
#define _SOCKET_SELECT_H

#include "../linuxlist/linuxlist.h"

#define SOCKET_FD_READ	 0x0001
#define SOCKET_FD_WRITE	 0x0002
#define SOCKET_FD_EXCEPT 0x0004

struct socket_fd {
	struct llist_head list;
	int fd;
	unsigned int type; 					/* type - > when */
	int (*cb)(struct socket_fd *fd, unsigned int flag ); 	/* flag -> what */
	void *data;
	unsigned int priv_nr;
};
 

int socket_register_fd(struct socket_fd *fd);
void socket_unregister_fd(struct socket_fd *fd);
int socket_select_main(int polling);
#endif /* _SOCKET_SELECT_H */
