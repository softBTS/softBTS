/***************************************************************************
 *            logger.c
 *
 *  Fri Jul 31 21:48:43 2009
 *  Copyright  2009  Fadil Berisha
 *  <fadil.r.berisha@gmail.com>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <common.h>
#include <softbts.h>
#include <logger.h>

extern bts_directory_t directory;
extern bts_runtime_t   runtime;

char *bts_str_time(void);
char *str_replace_char(gchar * str, gchar old, gchar new);
void print_bits( char ch );

void logger(const int loglevel, const char *s1, const char *s2, const char *s3, char *format,...)

{
	char buff[256] = "", *logfile;
	logfile = &buff[0];
	char msg[1024] = "", *pmsg = &msg[0];;
	FILE *fp;


	/* get the full path of the log file. */
	logfile = g_strjoin("",directory.log_dir,BTS_PATH_SEPARATOR,LOG_FILE,NULL);
    	fp = fopen(logfile,"a");
	if (fp == NULL){	/* Fall back to console output if unable to open file */
		fprintf (stdout,"Unable to open logfile\n");
   		return;
  	}
	sprintf(msg, "\033[31;%dm",30 + loglevel);
	fprintf(fp,"%s",pmsg);

	pmsg = g_strjoin("",bts_str_time()," ",s1,":",s2,":",s3," ",NULL);
	fprintf(fp,"%s",pmsg);

	va_list args;
	va_start (args, format);
	vsprintf(msg, format, args);
	va_end(args);

	fprintf(fp,"%s",msg);
	fclose (fp);

	if(runtime.console == stdout){
	   fprintf (runtime.console,"%s\n", pmsg);
	  }

}

void debug(const int dtype, char *format,...)

{
        char buff[256] = "", *logfile;
        logfile = &buff[0];
        char msg[1024] = "", *pmsg = &msg[0];;
        FILE *fp;


        /* get the full path of the log file. */
        logfile = g_strjoin("",directory.log_dir,BTS_PATH_SEPARATOR,LOG_FILE,NULL);
        fp = fopen(logfile,"a");
        if (fp == NULL){        /* Fall back to console output if unable to open file */
                fprintf (stdout,"Unable to open logfile\n");
                return;
        }
        sprintf(msg, "\033[1;31;%dm",30 + dtype);
        fprintf(fp,"%s",msg);
	fprintf(stdout,"%s",msg);	

        va_list args;
        va_start (args, format);
        vsprintf(msg, format, args);
        va_end(args);

        fprintf(fp,"%s",msg);
	fprintf(stdout,"%s",msg);
        fclose (fp);

        if(runtime.console == stdout){
           fprintf (runtime.console,"%s\n", pmsg);
          }

}

char *str_replace_char(char * str, char old, char new)
{
    char *match;

    g_return_val_if_fail(str != NULL, NULL);
    match = str;
    while ((match = strchr(match, old)))
        *match = new;
    return str;
}

static unsigned char hexbuf[1024]; 
unsigned char *hexdump(unsigned char *buf, int length)
{
int i,rc;
unsigned char *phb;
phb = &hexbuf[0];
	for(i = 0; i < length; i++){
        rc = sprintf(phb,"%02x",*buf);
        buf++;
	phb = phb +rc;
	*phb = ' ';
        phb++;
        }
*phb = '\0';
phb = &hexbuf[0];
return phb;
}

void print_bits( char ch )
{
    unsigned long mask = 0X80 ;
    while( mask ){
        if( (mask & ch) ){
            putchar( '1' ) ;
        }
        else{
            putchar( '0' ) ;
        }

        mask = mask >> 1 ;
    }
}

char *bts_str_time(void)
{
	time_t now;
	struct tm *newtime;
	char *strtime;

	time(&now); 		      /* get time in seconds            */
	newtime = localtime( &now );  /* Convert time to struct tm form */
	strtime = asctime( newtime);  /* Print local time as a string   */
	strtime = str_replace_char(strtime,'\n',' ');
	return  strtime;
}
