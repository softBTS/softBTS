/* select filedescriptor handling, taken from:
 * userspace logging daemon for the iptables ULOG target
 * of the linux 2.4 netfilter subsystem.
 *
 * (C) 2000-2009 by Harald Welte <laforge@gnumonks.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 
 *  as published by the Free Software Foundation
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <fcntl.h>
#include  "select.h"
#include "../linuxlist/linuxlist.h"
#include "../timer/timer.h"

static int maxfd = 0;
static LLIST_HEAD(socket_fds);
static int unregistered_count;

int socket_register_fd(struct socket_fd *fd)
{
	int flags;

	/* make FD nonblocking */
	flags = fcntl(fd->fd, F_GETFL);


	if (flags < 0)
		return flags;
	flags |= O_NONBLOCK;
	flags = fcntl(fd->fd, F_SETFL, flags);

	if (flags < 0)
		return flags;

	/* Register FD */
	if (fd->fd > maxfd)
		maxfd = fd->fd;
	llist_add_tail(&fd->list, &socket_fds);

	return 0;
}

void socket_unregister_fd(struct socket_fd *fd)
{
	unregistered_count++;
	llist_del(&fd->list);
}

int socket_select_main(int polling)
{
	struct socket_fd *ufd, *tmp;
	fd_set readset, writeset, exceptset;
	int work = 0, rc;
	struct timeval no_time = {0, 0};

	FD_ZERO(&readset);
	FD_ZERO(&writeset);
	FD_ZERO(&exceptset);

	/* prepare read and write fdsets */
	llist_for_each_entry(ufd, &socket_fds, list) {
		if (ufd-> type & SOCKET_FD_READ)
			FD_SET(ufd->fd, &readset);

		if (ufd-> type & SOCKET_FD_WRITE)
			FD_SET(ufd->fd, &writeset);

		if (ufd-> type & SOCKET_FD_EXCEPT)
			FD_SET(ufd->fd, &exceptset);

	}



	timer_check();

	if (!polling)
		prepare_timers();
	
	rc = select(maxfd+1, &readset, &writeset, &exceptset, polling ? &no_time : nearest_timer());

	if (rc < 0)
		return 0;

	/* fire timers */
	update_timers();

	/* call registered callback functions */
restart:


	unregistered_count = 0;
	llist_for_each_entry_safe(ufd, tmp, &socket_fds, list) {
		int flags = 0;
               
		if (FD_ISSET(ufd->fd, &readset)) {
			flags |= SOCKET_FD_READ;
			FD_CLR(ufd->fd, &readset);
		
		}

		if (FD_ISSET(ufd->fd, &writeset)) {
			flags |= SOCKET_FD_WRITE;
			FD_CLR(ufd->fd, &writeset);
		}

		if (FD_ISSET(ufd->fd, &exceptset)) {
			flags |= SOCKET_FD_EXCEPT;
			FD_CLR(ufd->fd, &exceptset);
		}

		if (flags) {
			work = 1; 
			ufd->cb(ufd, flags);
		}
		/* ugly, ugly hack. If more than one filedescriptors were
		 * unregistered, they might have been consecutive and
		 * llist_for_each_entry_safe() is no longer safe */
		if (unregistered_count > 1)
			goto restart;
	
	}
	return work;
}
