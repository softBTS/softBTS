/***************************************************************************
 *            softbts.h
 *
 *  Tue Jul 28 22:39:28 2009
 *  Copyright  2009  Fadil Berisha
 *  <fadil.r.berisha@gmail.com>
 ****************************************************************************/

#ifndef SOFT_BTS_H
#define SOFT_BTS_H 

#define BTS_DECLARE_DATA
#define BTS_PATH_SEPARATOR "/"
#define BTS_PREFIX_DIR     "."

/* lock filename: Stores the process id of the bts process - PID */
#define LOCK_FILE	"bts.lock"
#define LOG_FILE	"bts.log"
#define CONF_FILE       "bts.conf"

/**************************************************************************/
/* Constants used by this program                                         */
/**************************************************************************/
#define BSC_OML_PORT        3002
#define BSC_RSL_PORT        3003
#define TRX_PORT            3004

#define BUFFER_LENGTH    255
#define TELNET_PORT     4243


typedef int64_t bts_time_t; 	/* microseconds since 00:00:00 january 1, 1970 UTC, time_t
				   time in seconds. <sys/time.h> */
typedef int64_t bts_interval_time_t;

struct bts_directory {
	char *base_dir;
	char *mod_dir;
	char *conf_dir;
	char *log_dir;
	char *temp_dir;
};
typedef  struct bts_directory bts_directory_t;

struct bts_log_struct {
	char *message;						/* The complete log message */
	char file[80];						/* The file where the message originated */
	uint32_t line;						/* The line number where the message originated */
	char func[80];						/* The function where the message originated */
	bts_time_t timestamp;				        /* The time when the log line was sent */
	char *content;						/*  A pointer to where the actual content of the 
 								    message starts (skipping past the preformatted portion) */
	const char *userdata;		
};
typedef struct bts_log_struct bts_log_struct_t;

struct  bts_runtime {
	bts_time_t 		initiated;
	bts_time_t 		reference;
	bts_time_t 		offset;
	FILE 		        *console;
	uint8_t 		running;
	uint32_t		flags;
	
};
typedef struct bts_runtime bts_runtime_t;


/* Non staic Functions */
int kill_background(void);



#endif /* SOFT_BTS_H */
