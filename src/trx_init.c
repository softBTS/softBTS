#include <common.h>
#include <softbts.h>
#include <bts_init.h>
#include <bts_nm.h>
#include <logger.h>

#define MAX_TRX 4
int backlog = 0;


static int trx_cb(connection_t *trx, unsigned int type);
static void print_message(int sd);
static int accept_connection(socket_connection_t *connection, int flag);
static int find_index(int sd);
static int find_empty_record(void);


static const u_int8_t id_req[] = {0, 11, PROTO_SOFTBTS, MSG_ID_GET, 
				   0x00, IDTAG_UNIT,
				   0x00, IDTAG_UNITNAME,
				   0x00, IDTAG_SWVERSION,
				   0x00, IDTAG_MACADDR,
				   0x00, IDTAG_IPADDR,
				  };


socket_connection_t trx[MAX_TRX] = {
{       .socket.type = SOCKET_FD_READ,
        .socket.cb = trx_cb,
        .socket.fd = -1,},

{       .socket.type = SOCKET_FD_READ,
        .socket.cb = trx_cb,
        .socket.fd = -1,},

{       .socket.type = SOCKET_FD_READ,
        .socket.cb = trx_cb,
        .socket.fd = -1,},

{       .socket.type = SOCKET_FD_READ,
        .socket.cb = trx_cb,
        .socket.fd = -1,},
};



char   msg_trx[BUFFER_LENGTH], *pmsg_trx =&msg_trx[0];

connection_t trx_socket = {
        .type = SOCKET_FD_READ,
        .cb = accept_connection,
        .fd = -1,
};


void trx_socket_init(int port)
{
        struct sockaddr_in addr;
	int rc;
	int sd; 	/* socket file deskriptor for our listening  socket */
        int on = 1; 	/* Used, so we can re-bind to our port  while \
		           previus connection is still in TIME_WAIT state */

        sd = socket(PF_INET, SOCK_STREAM, 0);
        if (sd < 0) {
                logger(INFO,LOCATION,"socket() creation failed %s\n",strerror(errno));
                return;
        }

        setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(port);

        rc = bind(sd, (struct sockaddr*)&addr, sizeof(addr));
	if(rc < 0) {
                logger(INFO,LOCATION, "bind() failed: %s\n",strerror(errno));
		close(sd);
                return;
        }
	rc = listen(sd, backlog);

        if (rc < 0) {
                logger(INFO,LOCATION,"listen() failed %s\n",strerror(errno));
                return;
        }
	trx_socket.fd = sd;
        socket_register_fd(&trx_socket);
return;
}


static int accept_connection(socket_connection_t *connection, int flag)
{

        int i, sd, rc;
	i = find_empty_record();
	if( i < 0){
	  sd = accept(trx_socket.fd,NULL,NULL);
          if( sd < 0){
                logger(INFO,LOCATION,"accept() failed >> %s\n",strerror(errno));
                return sd;
          }
	  print_message(sd);
	  close(sd);
	  goto end;
	}

	connection = &trx[i];
	sd = accept(trx_socket.fd,NULL,NULL);
        if( sd < 0){
                logger(INFO,LOCATION,"accept() failed >> %s\n",strerror(errno));
                return sd;
        };

	connection->socket.data = connection;
	connection->socket.fd = sd;
	connection->socket.type = SOCKET_FD_READ;
	connection->socket.cb = trx_cb;

	rc = socket_register_fd(&connection->socket);
	 if (rc < 0){
	   	logger(ERROR,LOCATION,"Could not regisr socket deskriptor >> %s\n",strerror(errno));
		close(sd);
		return rc;
	 }

	rc = write (sd, id_req, sizeof(id_req));
	 if (rc < 0){
               	logger(ERROR,LOCATION,"write() failed >> %s\n",strerror(errno));
                close(sd);
                return rc;
         }
end:
	return 0;

}


static int trx_cb(connection_t *socket, unsigned int flag)
{
        int rc, i;
        socket_connection_t *connection;

	i = find_index(socket->fd);
	connection = &trx[i];
/* flag e vendos funkcioni select() kur thirret trx_cb */
	if(flag == SOCKET_FD_READ){
		rc = receive_msg( pmsg_trx, connection->socket.fd);
		if (rc == 0){
	   		   close(connection->socket.fd);
	   		   connection->socket.fd = -1;
           		   socket_unregister_fd(&connection->socket);
	   		   logger(INFO,LOCATION,"TRX[%d] lost connection >> %s\n",i,strerror(errno));
          	}
        }
return rc;
}

static int find_index(int sd)
{
	int i;
	socket_connection_t *conn;

	for(i = 0; i < MAX_TRX; i++){
 	   	conn = &trx[i];
		if (conn->socket.fd == sd){
	        return i;
		}
	}
return -1;
}


static int find_empty_record(void)
{
	int i;
        socket_connection_t *conn;
        for(i = 0; i < MAX_TRX; i++){
                conn = &trx[i];
                if (conn->socket.fd == -1){
                return i;
                }
        }
return -1;
}


static void print_message(int sd)
{
        int ret;
        static char *msg =
                "\nThis BTS can serve only 4 TRX";
        ret = write(sd, msg, strlen(msg));
}



