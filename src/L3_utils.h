
 /*****************************************************************************
 *  L3_utils.h
 *  Layer 3 utilities 
 *
 *  Copyright  2009 -2011 by Fadil Berisha  <fadil.r.berisha@gmail.com>
 *  All Rights Reserved
 ******************************************************************************
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


#ifndef L3_UTILS_H
#define L3_UTILS_H 

struct name {  
        u_int8_t           code;            /*Message code bits         */ 
        char               *msg;            /* Message type name        */

};
typedef struct name name_t;


const char *get_name(const name_t N[], u_int8_t code);
#endif /*EOF*/
