
#include <common.h>
#include <softbts.h>
#include <bts_init.h>
#include <trx_init.h>
#include <bts_nm.h>
#include <logger.h>



/* Functions */
static int handle_options(int argc, char** argv);
static void print_version();
static int print_help();
static void set_globals(void);
int kill_background(void);
static void bts_core_set_rlimits(void);
static void signal_handler(int sig);
static void daemonize(void);

/* Vty */
void *tall_bts_ctx = NULL;

/* Daemon */
bts_directory_t	  directory;
bts_log_struct_t  log_struct;
bts_runtime_t     runtime;


/***********************************************************************/
/* OML & RSL							       */
/* Function, Variable and Structure definitions.                       */
/***********************************************************************/
int bts_id_resp = FALSE;
int sw_site_manager = FALSE;

int sw_bts = FALSE;
u_int8_t manager_admin_state = NM_STATE_LOCKED;
u_int8_t manager_opstate = NM_OPSTATE_DISABLED;

u_int8_t bts_admin_state = NM_STATE_LOCKED;
u_int8_t bts_opstate = NM_OPSTATE_DISABLED;

int baseb_transc_sw = FALSE;
u_int8_t baseb_transc_admin_state = NM_STATE_LOCKED;
u_int8_t baseb_transc_opstate = NM_OPSTATE_DISABLED;

u_int8_t radio_carrier_admin_state = NM_STATE_LOCKED;
u_int8_t radio_carrier_opstate = NM_OPSTATE_DISABLED;

u_int8_t channel_admin_state[8] = {NM_STATE_LOCKED};
u_int8_t channel_opstate[8] = {NM_OPSTATE_DISABLED};

extern int oml_connection();
extern int rsl_connection();
extern  socket_connection_t oml;
extern  socket_connection_t rsl;


#if 0
socket_connection_t oml = {
	.socket.type = SOCKET_FD_READ,
	.socket.cb = oml_connection,
	.socket.fd = -1,
};

socket_connection_t rsl = {
        .socket.type = SOCKET_FD_READ,
        .socket.cb = rsl_connection,
	.socket.fd = -1,
};

#endif

//char   msg[BUFFER_LENGTH], *pmsg = &msg[0];

u_int8_t channel;
char server[40];



int main(int argc, char** argv)

{
/***********************************************************************/
/* Variable and structure definitions.                                 */
/***********************************************************************/


	tall_bts_ctx = talloc_named_const(NULL, 1, "softBTS");

	handle_options(argc, argv);
	set_globals();
	bts_core_set_rlimits();
	daemonize();
	/* Initialize */
	srand(time(NULL));
	telnet_init(TELNET_PORT);
	
	
	socket_init(&oml.addr, &oml.socket, BSC_OML_PORT);
	socket_init(&rsl.addr, &rsl.socket, BSC_RSL_PORT);	
	trx_socket_init(TRX_PORT);

	while(1){
		socket_select_main(0);

	}
}



static int handle_options(int argc, char** argv)
{

        int c;
	char *cfg = NULL;
	

/* Vars for optargs */
	int option_index = 0;
         static struct option options[] = {
                 {"help", 0, 0, 'h'},
                 {"stop", 0, 0, 's'},
                 {"verbose", 0, 0, 'v'},
                 {"version", 0, 0, 'V'},
                 {"address", 1, 0, 'a'},
                 {"debug", 1, 0, 'd'},
                 {"loglevel", 1, 0, 'l'},
                 {"quiet", 0, 0, 'q'},
                 {0, 0, 0, 0}
         };


	if (argc ==1) {
        fprintf(stderr, 
        "This program needs arguments.\n \
         \tYou have to specify the IP address.\n \
	 \tUse -h  for more information\n");
        exit(EXIT_SUCCESS);
        }
	for(;;){
	c = getopt_long(argc, argv, ":sq?hVc:a:", options, &option_index);
		if (c == -1){
			break;
		}	

                switch(c) {
		case 's':
			kill_background();
			exit(EXIT_SUCCESS);
                case 'h':
		case '?':
                        print_help();
                        exit(EXIT_SUCCESS);
                case 'c':
                        strcpy(cfg,optarg);
                        break;
		case 'a':
			strcpy(server,optarg);
			break;
                case 'V':
                        print_version();
                        exit(EXIT_SUCCESS);
		case 'v': /* verbose flag */
			break;
		default:
			printf ("You specified a parameter I don't "
					"know about.\n");
			c =0 ;
	 	}
	};
	if(strlen(server) == 0){
		printf("\n Please specify IP Address ........");
		return print_help();
	}
	return 0;
}

static void print_version()
{
const char *copyright =
	"Copyright (C) 2009-2010 by Fadil R. Berisha";
const char *license = 
	"License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>\n";

	printf("%s-%s\n", PACKAGE,VERSION);
    	printf("%s\n", copyright);
	printf("%s\n", license);
}

static int print_help()
{
	printf("\n\n%s - %s \n\n", PACKAGE, VERSION);
	printf("%s [-h] [-V] [-v] [-c FILE]  -a IP\n", PACKAGE); 
	printf("Those are the optional arguments [] you can pass to BTS\n");
	printf(" -h 	        print this help and exit\n");
	printf(" -s 		stop proccess  \n");
	printf(" -v		set verbose flag \n");
	printf(" -V		print version and exit \n");
	printf(" -c 	FILE	set configure directories\n");
	printf(" -a     ADDRESS specify an IP Adress of BSC\n");

	return 0;
}

static void set_globals(void)
{
#define BUFSIZE 1024
char base_dir[1024] = BTS_PREFIX_DIR;

	if (!directory.base_dir && (directory.base_dir = (char *) malloc(BUFSIZE))) {
		g_snprintf(directory.base_dir, BUFSIZE, "%s", base_dir);
	}

	if (!directory.mod_dir && (directory.mod_dir = (char *) malloc(BUFSIZE))) {
#ifdef BTS_MOD_DIR
		g_snprintf(directory.mod_dir, BUFSIZE, "%s", BTS_MOD_DIR);
#else
		g_snprintf(directory.mod_dir, BUFSIZE, "%s%smod", base_dir, BTS_PATH_SEPARATOR);
#endif
	}

	if (!directory.conf_dir && (directory.conf_dir = (char *) malloc(BUFSIZE))) {
#ifdef BTS_CONF_DIR
		g_snprintf(directory.conf_dir, BUFSIZE, "%s", BTS_CONF_DIR);
#else
		g_snprintf(directory.conf_dir, BUFSIZE, "%s%sconf", base_dir, BTS_PATH_SEPARATOR);
#endif
	}

	if (!directory.log_dir && (directory.log_dir = (char *) malloc(BUFSIZE))) {
#ifdef BTS_LOG_DIR
		g_snprintf(directory.log_dir, BUFSIZE, "%s", BTS_LOG_DIR);
#else
		g_snprintf(directory.log_dir, BUFSIZE, "%s%slog", base_dir, BTS_PATH_SEPARATOR);
#endif
	}



	if (!directory.temp_dir && (directory.temp_dir = (char *) malloc(BUFSIZE))) {
#ifdef BTS_TEMP_DIR
		g_snprintf(directory.temp_dir, BUFSIZE, "%s", BTS_TEMP_DIR);
#else
		g_snprintf(directory.temp_dir, BUFSIZE, "%s", "/tmp/");
#endif
	}

	g_assert(directory.base_dir);
	g_assert(directory.mod_dir);
	g_assert(directory.conf_dir);
	g_assert(directory.log_dir);
	g_assert(directory.temp_dir);

}

/* kill a bts process running in background mode */
int kill_background(void)
{
	FILE *fp;				/* FILE handle to open the pid file */
	int fd;					/* file descriptor */
	char *lockfile;			     	/* full path of the PID file */
	pid_t pid = 0;				/* pid from the pid file */
	int ret;

	/* duhet te egzistoj ne rast se thirret bts kur processi eshte i aktivizuem */
	set_globals();

	/* get the full path of the lockfile lockfile. */
	lockfile = g_strjoin(NULL,directory.log_dir,BTS_PATH_SEPARATOR,LOCK_FILE,NULL);

	/* open the lock file */
	if ((fp = fopen(lockfile, "r")) == 0) {
		/* lock file does not exist */
		fprintf(stderr, "Unable to open pid file %s errorno  %d - %s\n",lockfile, errno, strerror(errno));
		return 1;
	}

	/* pull the pid from the file */
	if (fscanf(fp, "%d", (int *)(intptr_t)&pid)!=1) { 
	   fprintf(stderr,"Unable to get the pid!\n");
	}

	/* if we have a valid pid */
	if (pid > 0) {
		/* find file descriptor, unlock and remove file*/
		fd = fileno(fp);
		ret = lockf(fd,F_ULOCK ,0);
		if ( ret == -1){
			fprintf(stderr, "Unable to lock file %s errorno  %d - %s\n",lockfile, errno, strerror(errno));
		}
		if( remove(lockfile ) == -1 ){
        	  	fprintf(stderr, "Error deleting lock file%s\n",lockfile );
		}
		/*  send the signal to kill process */
		kill(pid, SIGTERM);

	}
	else {
		fprintf(stderr,"No valid pid found\n");
	}
	/* close the file handle to the lock file */
	fclose(fp);
	return 0;
}

void bts_core_set_rlimits(void)
 {
#ifdef HAVE_SETRLIMIT
         struct rlimit rlp;
         memset(&rlp, 0, sizeof(rlp));
         rlp.rlim_cur = BTS_THREAD_STACKSIZE;
         rlp.rlim_max = BTS_SYSTEM_THREAD_STACKSIZE;
         setrlimit(RLIMIT_STACK, &rlp);

         memset(&rlp, 0, sizeof(rlp));
         rlp.rlim_cur = 999999;
         rlp.rlim_max = 999999;
         setrlimit(RLIMIT_NOFILE, &rlp);
 
         memset(&rlp, 0, sizeof(rlp));
         rlp.rlim_cur = RLIM_INFINITY;
         rlp.rlim_max = RLIM_INFINITY;
 
         setrlimit(RLIMIT_CPU, &rlp);
         setrlimit(RLIMIT_DATA, &rlp);
         setrlimit(RLIMIT_FSIZE, &rlp);
#ifdef RLIMIT_NPROC
	 setrlimit(RLIMIT_NPROC, &rlp);
 #endif
#ifdef RLIMIT_RTPRIO
         setrlimit(RLIMIT_RTPRIO, &rlp);
#endif
 
#if !defined(__OpenBSD__) && !defined(__NetBSD__)
         setrlimit(RLIMIT_AS, &rlp);
#endif
 #endif
         return;
}

void signal_handler(int sig)

{
	switch(sig) {

	case SIGHUP:
		logger(INFO,LOCATION,"hangup signal catched\n");
		break;

	case SIGTERM:
		logger(INFO,LOCATION,"kill signal catched\n");
		exit(EXIT_SUCCESS);
		break;
	default:
		break;
	}
}

void daemonize(void)
{
	int fd;
	pid_t pid,  sid;
	char str[10];
	char *lockfile;

	pid = fork();	 /* Fork the parent process */
	if (pid < 0) {
           fprintf(stderr, "Unable to get in background, code = %d - %s\n", errno, strerror(errno));
	   exit(EXIT_FAILURE);
	   }
	if (pid > 0) { /* If we got a good PID, then we can exit the parent process. */
 	    exit(EXIT_SUCCESS);
    	   }

	/* At this point we are executing as the child process */
	/* Create a new session - obtain a new process group,
      	   making the process a group leader. It also detaches us from our
           controlling terminal  */

	sid = setsid();
	if (sid < 0) {
		fprintf(stderr, "Unable to create a new session (setsid)! %d - %s\n", errno, strerror(errno));
		exit(EXIT_SUCCESS);
	 }

	/* a fork2() after a sucessful call to setsid() ensure the  daemon and all it descendants 
	 (unless they create a new session for themselves) can NEVER attain  a controlling terminal. */

	pid = fork();
	switch (pid) {
	         case  0:
			break;
 		 case -1:
			fprintf(stderr, "Unable to get in background (fork2), code = ! %d - %s\n", errno, strerror(errno));
			exit(EXIT_FAILURE);
			break;
		default:
			exit(EXIT_SUCCESS);
		}

/* Change the file mode mask */
        umask(0);

/* Change the current working directory.  This prevents the current directory from being locked; */

        if ((chdir(directory.base_dir)) < 0) {
           logger(ERROR,LOCATION, "BTS unable to change directory to runing dir\n" );
            exit(EXIT_FAILURE);
        }

/* Open lock file */
		lockfile = g_strjoin("",directory.log_dir,BTS_PATH_SEPARATOR,LOCK_FILE,NULL);
	fd = open(lockfile, O_RDWR | O_CREAT,0644);

	if (fd < 0) {
		logger(ERROR,LOCATION, "BTS unable to open lock file\n");
		exit(EXIT_FAILURE); /* can not open */
	}

	if ( lockf(fd,F_TLOCK,0) < 0){ /* F_TLOCK - Test and lock a section for exclusive use */
		logger(INFO,LOCATION, "Unable to lock file, BTS already running ...\n");
		exit(EXIT_SUCCESS);			/* can not lock */
	}

/* Get pid,  record PID in lockfile */
	ssize_t retval;
	sprintf(str,"%d",getpid());
	retval = write(fd,str,strlen(str));	/* record pid to lockfile */

	if( retval  == strlen(str)){ 
	   logger(INFO,LOCATION,"pid %s recorded in lockfile\n",str);
	   }


	/*Standart I/O descriptors should be opened and connect them to a I/O device /dev/null */
/*
	fd = open("/dev/null", O_RDONLY);
	if (fd != 0) {
		dup2(fd, 0);
		close(fd);
		}

	 fd = open("/dev/null", O_WRONLY);
		if (fd != 1) {
 			dup2(fd, 1);
			close(fd);
		}
	fd = open("/dev/null", O_WRONLY);
		if (fd != 2) {
 			dup2(fd, 2);
			close(fd);
 		}
*/
	signal(SIGHUP,signal_handler);  /* catch hangup signal */
	signal(SIGTERM,signal_handler); /* catch kill signal */
}


