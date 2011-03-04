/***************************************************************************
 *            logger.h
 *
 *  Tue Jul 28 22:39:28 2009
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
#ifndef LOGGER_H
#define LOGGER_H 


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define source __FILE__
#define source_function  __FUNCTION__
#define source_line TOSTRING(__LINE__)
#define LOCATION source,source_function,source_line

#define BLACK 		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7

#define ERROR	 RED
#define INFO	 YELLOW
#define WARNING  GREEN
#define NOTICE   WHITE


#define DNM	BLUE
#define DRSL    WHITE
#define DHX     GREEN
 
void logger(const int loglevel, const char *s1, const char *s2, const char *s3, char *format,...);
void debug(const int dtype, char *format,...);
unsigned char *hexdump(unsigned char *buf, int length);

#endif /* LOGGER_H */

