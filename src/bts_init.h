#ifndef BTS_INIT_H
#define BTS_INIT_H 

#define PROTO_SOFTBTS 0xfe
#define PROTO_RSL     0x00
#define PROTO_OML     0xff


enum softbts_id_tags {
        IDTAG_SERNR                = 0x00,
        IDTAG_UNITNAME             = 0x01,
        IDTAG_LOCATION1            = 0x02,
        IDTAG_LOCATION2            = 0x03,
        IDTAG_EQUIPVERS            = 0x04,
        IDTAG_SWVERSION            = 0x05,
        IDTAG_IPADDR               = 0x06,
        IDTAG_MACADDR              = 0x07,
        IDTAG_UNIT                 = 0x08,
};

enum softbts_msg_type {
	MSG_PING	= 0x00,
	MSG_PONG	= 0x01,
	MSG_ID_GET	= 0x04,
	MSG_ID_RESP	= 0x05,
	MSG_ID_ACK	= 0x06,
};

struct  socket_connection {
        struct llist_head list;
        connection_t   socket;
        struct sockaddr_in addr;
};
typedef struct  socket_connection socket_connection_t;




#endif /* BTS_INIT_H */

