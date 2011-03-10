#include <common.h>
#include <softbts.h>
#include <bts_init.h>
#include <bts_nm.h>
#include <logger.h>


extern char bsc[40];
int oml_connection(connection_t *socket, int flag);
int rsl_connection(connection_t *socket, int flag);
void socket_init( struct sockaddr_in *bsc_addr, connection_t *sock, int port);

socket_connection_t oml = {
        .socket.type = SOCKET_FD_READ,
        .socket.cb = (int *)oml_connection,
        .socket.fd = -1,
};

socket_connection_t rsl = {
        .socket.type = SOCKET_FD_READ,
        .socket.cb = (int *)rsl_connection,
        .socket.fd = -1,
};

char   msg[BUFFER_LENGTH], *pmsg = &msg[0];


void socket_init( struct sockaddr_in *bsc_addr, connection_t *sock, int port)
{

	int rc, on =1;
	socklen_t len;

	len = sizeof(struct sockaddr_in);

	sock->fd = socket(AF_INET, SOCK_STREAM, 0);
	if ( sock->fd < 0) {
		logger(ERROR,LOCATION, "socket() failed\n");
		exit(EXIT_FAILURE);
	}
	/*  set SO_REUSEADDR on a socket to true: */
        setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));


	memset(bsc_addr, 0, sizeof(bsc_addr));
      	bsc_addr->sin_family      = AF_INET;
      	bsc_addr->sin_port        = htons(port);
     	bsc_addr->sin_addr.s_addr = inet_addr(bsc);


      	rc = connect(sock->fd, (struct sockaddr *)bsc_addr, len);
      	if (rc < 0){
         logger(ERROR,LOCATION,"connect() failed, check is BSC runnimg in this network? \
         or wrong IP address of BSC \n");
         kill_background();
      	}
      	 else {
	 logger(INFO,LOCATION,"connect() successful connection to new socket %d  link %s:%d\n",sock->fd, bsc, port);
      	 }

    	rc = socket_register_fd(sock);
	if (rc != 0) {
	logger(ERROR,LOCATION,"Unable to register socket = %d\n",sock->fd);
	kill_background();
	}
}

int oml_connection(connection_t *socket, int flag)
{

	int rc;
	if(socket->type & SOCKET_FD_READ){
	rc = receive_msg( pmsg, socket->fd);

	}
return 0;
}

int rsl_connection(connection_t *socket, int flag)
{
	int rc;
	if(socket->type & SOCKET_FD_READ){
        rc = receive_msg( pmsg, socket->fd);
        }
return 0;
}

