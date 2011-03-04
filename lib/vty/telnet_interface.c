#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "telnet_interface.h"
#include "vty.h"
#include "buffer.h"
#include "../talloc/talloc.h"
#include "../select/select.h"



void subscr_put() {abort();}
static int telnet_new_connection(connection_t  *socket, int flag );
static int client_data(connection_t *fd, unsigned int tye);
static void print_message(int fd);
void telnet_init(int port);


static  LLIST_HEAD(active_connections);
static void *tall_telnet_ctx;

connection_t server_socket = {
	.type = SOCKET_FD_READ,
	.cb = (int *)telnet_new_connection,
};



/* callback from VTY code */
void vty_event(enum event  event, int sock, struct vty *vty)
{
	telnet_connection_t *connection = vty->priv;
	connection_t        *socket = &connection->fd;

       
        if (vty->type != VTY_TERM)
                return;

        switch (event) {
        case VTY_READ:
		 socket -> type  |= SOCKET_FD_READ; 
                break;

        case VTY_WRITE:
                 socket -> type |= SOCKET_FD_WRITE;
                break;

        case VTY_CLOSED:
                /* vty layer is about to free() vty */
		connection->vty =NULL;
                telnet_close_client(socket);
                break;

        default:
                break;
        }
}

void bts_vty_init(void)
{
        cmd_init(1);
        vty_init();
};

int telnet_close_client(connection_t *telnet_fd) {
       telnet_connection_t *conn = (telnet_connection_t *)telnet_fd->data;

        close(telnet_fd->fd);
        socket_unregister_fd(telnet_fd);

	llist_del(&conn->entry);
        talloc_free(conn);

        return 0;
}

void telnet_init(int port) 
{
        struct sockaddr_in sock_addr;
        int sd, on = 1;

        tall_telnet_ctx = talloc_named_const(NULL, 1, "telnet-connection");

        bts_vty_init();
        sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (sd < 0) {
                printf("Telnet interface: socket() creation failed\n");
                return;
        }

        setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        memset(&sock_addr, 0, sizeof(sock_addr));

        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = htons(port);
        sock_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        if (bind(sd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0) {
                printf( "Telnet interface: bind() failed\n");
                return;
        }

        if (listen(sd, 0) < 0) {
   		printf("Telnet interface failed to listen\n");
                return;
        }

        server_socket.fd = sd;
        socket_register_fd(&server_socket);
}


static int telnet_new_connection(connection_t  *socket, int flag )
{
	telnet_connection_t *connection;
	struct sockaddr_in  sockaddr;
	socklen_t len;
	int sd;

	len = sizeof(sockaddr);
	sd = accept(socket->fd, (struct sockaddr*)&sockaddr, &len);

	if( sd < 0){
		printf("Telnet interface: accept() failed\n");
		return -1;
	};
	connection = talloc_zero(tall_telnet_ctx, telnet_connection_t);
//	connection->network = (struct gsm_network *)fd->data;
        connection->fd.data = connection;
        connection->fd.fd = sd;
        connection->fd.type = SOCKET_FD_READ;
        connection->fd.cb = client_data;
        socket_register_fd(&connection->fd);
        llist_add_tail(&connection->entry, &active_connections);

	connection->vty = vty_create(sd, connection);
	if (!connection->vty) {
                printf("LOGL_ERROR, couldn't create VTY\n");
                return -1;
        }
	print_message(sd);
	return 0;

}

static int client_data(connection_t *fd, unsigned int type)
{
        telnet_connection_t *conn = fd->data;
        int rc = 0;


        if (type & SOCKET_FD_READ) {
                conn->fd.type &= ~SOCKET_FD_READ;
                rc = vty_read(conn->vty);
        }

        /* vty might have been closed from vithin vty_read() */
        if (!conn->vty)
                return rc;

        if (type & SOCKET_FD_WRITE) {
                rc = buffer_flush_all(conn->vty->obuf, fd->fd);
                if (rc == BUFFER_EMPTY)
                        conn->fd.type &= ~SOCKET_FD_WRITE;
        }

        return rc;
}


static void print_message(int fd)
{
        int ret;
        static char *msg =
                "\nType \"help\" to get a short introduction.\n";
        ret = write(fd, msg, strlen(msg));
}

