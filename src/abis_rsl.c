 /*****************************************************************************
 *            abis_rsl.c
 *  GSM Radio Signalling Link messages on the A-bis interface GSM  08.58 
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
#include <common.h>
#include <softbts.h>
#include <bts_init.h>
#include <bts_nm.h>
#include <logger.h>
#include <abis_rsl.h>
#include <abis_utils.h>
#include <L3_0408.h>
#include <L3_utils.h>

void rsl_proto_msg(char *msg, int sd);
static void rsl_mg_cch(char *msg);

int is_bit_set(u_int8_t ch, u_int8_t offset);
int is_bit_H(u_int8_t ch, u_int8_t offset);
void rest_octets_si3(u_int8_t *si);

void rsl_msg(char *msg, int sd)
{
 
        rsl_hdr_t  *hdr;
        hdr = (rsl_hdr_t *) msg +3;


        switch (hdr->discr_T) {
	        case TRANSPARENT:
                printf("RSL_MT_TRANSP %x MESSAGE GR %x\n",hdr->discr_T  ,hdr->discr_msg_group);
                break;
        	case NON_TRANSPARENT:
                     switch(hdr->discr_msg_group){
                           case ABIS_MG_RES: 
		           break;
                           case ABIS_MG_RLL:
                           break;
                           case ABIS_MG_DCH:
                           break;
                           case ABIS_MG_CCH:
                                rsl_mg_cch(msg); 
                           break;
                           case ABIS_MG_TRX:
                           printf("RSL_MT_NONTRANSP %x MESSAGE GR ABIS_MG_TRX %x \n",hdr->discr_T ,hdr->discr_msg_group);
                           break;
                           case ABIS_MG_LOC:
                           default:
                           break;
                     } 
                break;

	default:
	printf("RSL unsuported message %x\n",hdr->discr_T);
	break;
	}
}

/* RSL: Message Group - Common Channel */ 
static void rsl_mg_cch(char *msg)
{
       rsl_hdr_t  *hdr;
       hdr = (rsl_hdr_t *) msg +3;       /* Skip softBTS specific 1st 3 byte */ 
       rsl_cch_t *cch;
       cch = (rsl_cch_t *) &hdr->data[0];
       
	      switch(cch->msg_type){
              case BCCH_INFO:
		 printf("\b");
       		 rsl_sys_info_t *sys_info;
       		 sys_info =(rsl_sys_info_t *)&cch->data[0];
       		 rsl_full_BCCH_t *bcch;
       		 bcch =(rsl_full_BCCH_t *)&sys_info->data[0];
       		 rsl_start_time_t *start_time;
       		 start_time = (rsl_start_time_t *)&bcch->data[0];
	         debug(DRSL,"Transparency: T_bit(%x) Message Group Discriminator: %s (%x)\n"\
                          ,hdr->discr_T, get_name(MG,hdr->discr_msg_group),hdr->discr_msg_group);
		debug(DRSL,"Message type: %s (%x)  %s(%x)  TN & C Bits:  %x & %s(%x)\n"\
                          ,get_name(MT_CCH,cch->msg_type),cch->msg_type, get_name(EI,cch->ei),cch->ei,cch->chan_TN,get_name(CH,cch->chan_nr),cch->chan_nr);
                 debug(DRSL,"System Info Type: EI(%x) %s(%x)\n",sys_info->ei,get_name(EI,sys_info->ei),sys_info->type);
#if 0
	         u_int32_t x,y;
                 x =(int )&msg[1] + msg[1] +1;
                 y = (int )&bcch->length + bcch->length; 
#endif
		 debug(DRSL,"Full BCCH Info EI: %s(%x)   Length = %x \nL3 = %s\n" 
                      ,get_name(EI,bcch->ei),bcch->ei,bcch->length, hexdump(bcch->L3_msg,bcch->length));
                 switch(sys_info->type){
		      case SYSTEM_INFORMATION_TYPE_1:
			      printf("\b");	
  	     	              system_information_type1_t *sys1;
                              sys1 = (system_information_type1_t *)&bcch->L3_msg;
			      print_bits(sys1->L2);
		              debug(DRSL," Pseudo Length: fixed bits = %02x L2 Pseudo Length Value = %x\n",sys1->L2.fb,sys1->L2.plv);
			      print_bits(sys1->DP);
			      debug(DRSL," Proto  Discr: SKIP (%x) DISCR (%x) \n",sys1->DP.skip, sys1->DP.discr);
			      print_bits(sys1->msg_type);  
  	                      debug(DRSL," Message Type: %s(%x)\n",get_name(RR,sys1->msg_type),sys1->msg_type);
                              print_bits(sys1->chd); 
   	                      debug(DRSL," Cell Channel Description: Format_high %x format_low %x\n",sys1->chd.format_id_high,sys1->chd.format_id_low);
			      /* FORMAT-ID:10111 */
			      if((sys1->chd.format_id_high == 2) && (sys1->chd.format_id_low == 7)){
    	 	                debug(DRSL,"\t Detected vairiable bit map FORMAT-ID:10xx111x\n");
  	 	                cell_var_bit_t *bits;
         	                bits = (cell_var_bit_t *)&sys1->chd;
                                int arfcn =  (bits->orig_arfcn_hi<<9)|(bits->orig_arfcn_mid<<1)|bits->orig_arfcn_lo;
			        debug(DRSL,"\t List of ARFCNs: ORIG-ARFCN high %x midd %02x low %x ARFCN = %d dec\n\t %s\n",\
                                     bits->orig_arfcn_hi, bits->orig_arfcn_mid, bits->orig_arfcn_lo,arfcn,hexdump(bits->rrfcn_8_111,13)); 
			      }
			      print_bits(sys1->rach_control);	 
                              debug(DRSL," RACH Control: CALL-RE-ESTABLISHMENT (%x); CELL-BAR-ACCESS (%x); TX-INTEGER (%x); MAX-RETRANS (%x); \n",
                                     sys1->rach_control.re, sys1->rach_control.cell_bar_access,\
                                     sys1->rach_control.tx_integer,sys1->rach_control.max_retrans);
                              print_bits(sys1->rach_control.ac8_9|sys1->rach_control.ec<<2|sys1->rach_control.ac15_11<<3);
                              debug(DRSL," EMERGENCY-CALL(%x) ACCESS-CONTROL-CLASS [15-11] (%x) [9-8] (%x) [7-0] (%x)\n",\
                                    sys1->rach_control.ec,sys1->rach_control.ac15_11,sys1->rach_control.ac15_11,sys1->rach_control.ac7_0);
                              print_bits( sys1->si1);
                              debug(DRSL," Rest Octet: NCH Value (%x)  %s\n",0x1f & sys1->si1, get_name(NCH, 0x1f & sys1->si1));
		    break;
		    case SYSTEM_INFORMATION_TYPE_2:
  			      printf("\b");	
  	     	              system_information_type2_t *sys2;
                              sys2 = (system_information_type2_t *)&bcch->L3_msg;
			      print_bits(sys2->L2);
		              debug(DRSL," Pseudo Length: fixed bits = %02x L2 Pseudo Length Value = %x\n",sys2->L2.fb,sys2->L2.plv);
			      print_bits(sys2->DP);
			      debug(DRSL," Proto  Discr: SKIP (%x) DISCR (%x) \n",sys2->DP.skip, sys2->DP.discr);
			      print_bits(sys2->msg_type);  
  	                      debug(DRSL," Message Type:  %s(%x)\n",get_name(RR,sys2->msg_type),sys2->msg_type);
                              print_bits(sys2->ncd); 
                              debug(DRSL," Neighbour Cell Channel Description: BA-IND (%x) EXT-IND (%x) \n",sys2->ncd.ba_ind, sys2->ncd.ext_ind);
			      debug(DRSL,"\t List of ARFCN channels to monitor: [128 127] (%x) [124 121] (%x) [120 0] %s\n", 
                                   sys2->ncd.ch_nr_128_127,sys2->ncd.ch_nr_124_121,hexdump(sys2->ncd.ch_nr_120_00,15));
                              print_bits(sys2->ncc_permitted);
                              debug(DRSL," NCC Permited: (%x)\n",sys2->ncc_permitted);
                              print_bits(sys2->rach_control);	 
                              debug(DRSL," RACH Control: CALL-RE-ESTABLISHMENT (%x); CELL-BAR-ACCESS (%x); TX-INTEGER (%x); MAX-RETRANS (%x); \n",
                                     sys2->rach_control.re, sys2->rach_control.cell_bar_access,\
                                     sys2->rach_control.tx_integer,sys2->rach_control.max_retrans);
                              print_bits(sys2->rach_control.ac8_9|sys2->rach_control.ec<<2|sys2->rach_control.ac15_11<<3);
                              debug(DRSL," EMERGENCY CALL(%x) ACCESS-CONTROL-CLASS [15-11] (%x) [9-8] (%x) [7-0] (%x)\n",\
                                    sys2->rach_control.ec,sys2->rach_control.ac15_11,sys2->rach_control.ac15_11,sys2->rach_control.ac7_0);
                              
		    break;
		case SYSTEM_INFORMATION_TYPE_3:
  			      printf("\b");	
  	     	              system_information_type3_t *sys3;
                              sys3 = (system_information_type3_t *)&bcch->L3_msg;
			      print_bits(sys3->L2);
		              debug(DRSL," Pseudo Length: fixed bits = %02x L2 Pseudo Length Value = %x\n",sys3->L2.fb,sys3->L2.plv);
			      print_bits(sys3->DP);
                              debug(DRSL," Proto  Discr: SKIP (%x) DISCR (%x) \n",sys3->DP.skip, sys3->DP.discr);
			      print_bits(sys3->msg_type);  
  	                      debug(DRSL," Message Type:  %s(%x)\n",get_name(RR,sys3->msg_type),sys3->msg_type);
			      print_bits(sys3->cell_id.ci1);
			      debug(DRSL," Cell ID: CI(%x)\n",sys3->cell_id.ci1);
                              print_bits(sys3->cell_id.ci2);
			      debug(DRSL," Cell ID: CI-CONT(%x)\n", sys3->cell_id.ci2);
			      debug(DRSL,"\t Mobile Country Code MCC: %x %x %x\n",sys3->lai.mcc1, sys3->lai.mcc2, sys3->lai.mcc3);
			      if(sys3->lai.mnc3 != 0x0f){
			        debug(DRSL,"\t Mobile Network Code MNC: %x %x %x\n",sys3->lai.mnc1, sys3->lai.mnc2, sys3->lai.mnc3);	 				      }
                              else{
                                   debug(DRSL,"\t Used two digit Mobile Network Code MNC: %x %x \n",sys3->lai.mnc1, sys3->lai.mnc2);
                              }
			      print_bits(sys3->lai.lac);
			      debug(DRSL," Location Area Code: LAC(%x)\n",sys3->lai.lac);
			      print_bits(sys3->lai.lac_cont);
			      debug(DRSL," Location Area Code: LAC-CONT(%x)\n",sys3->lai.lac_cont);
			      print_bits(sys3->cch_descr);
                              debug(DRSL," Control Channel Description: ATT(%x) %s\n",sys3->cch_descr.att,get_name(ATT,sys3->cch_descr.att));
                              debug(DRSL,"\t CCCH-CONF(%x) %s\n",sys3->cch_descr.ccch_conf,get_name(CCCH_CONF,sys3->cch_descr.ccch_conf));
                              debug(DRSL,"\t BS-AG-BLKS-RES(%x) Number of blocks reserved for access grant \n",sys3->cch_descr.bs_ag_blks_res);
                              print_bits(sys3->cch_descr.bs_pa_mfrms);
                              debug(DRSL," BS_PA_MFRMS(%x) %s\n", sys3->cch_descr.bs_pa_mfrms, get_name(BS_PA_MFRMS,sys3->cch_descr.bs_pa_mfrms));
			      print_bits(sys3->cch_descr.t3212); 
 			      debug(DRSL," T3212(%x) \n",sys3->cch_descr.t3212);
			      print_bits(sys3->cell_options);
			      debug(DRSL," Cell Options (BCCH): PWRC(%x) DTX(%x) %s\n", 
			           sys3->cell_options.pwrc, sys3->cell_options.dtx, get_name(DTX,sys3->cell_options.dtx));
                              debug(DRSL,"\t RADIO-LINK-TIMEOUT(%x) %d \n",sys3->cell_options.radio_link_timeout,4*sys3->cell_options.radio_link_timeout+4);
 			      print_bits(sys3->csp);
                              debug(DRSL," Cell Selection Parameters: MS-TXPWR-MAX-CCCH(%x) CELL-RESELECT-HYSTERESIS(%x)"
                                   " RXLEV-ACCESS-MIN(%x) NECI(%x) ACS(%x)\n",sys3->csp.ms_txpwr_max_ccch,
                                   sys3->csp.cell_resel_hyst,sys3->csp.rxlev_acc_min, sys3->csp.neci, sys3->csp.acs); 
			       print_bits(sys3->rach_control);	 
                              debug(DRSL," RACH Control: CALL-RE_ESTABLISHMENT (%x); CELL-BAR_ACCESS (%x); TX-INTEGER (%x); MAX-RETRANS (%x); \n",
                                     sys3->rach_control.re, sys3->rach_control.cell_bar_access,\
                                     sys3->rach_control.tx_integer,sys3->rach_control.max_retrans);
                              print_bits(sys3->rach_control.ac8_9|sys3->rach_control.ec<<2|sys3->rach_control.ac15_11<<3);
                              debug(DRSL," EMERGENCY-CALL(%x) ACCESS-CONTROL-CLASS [15-11] (%x) [9-8] (%x) [7-0] (%x)\n",\
                                    sys3->rach_control.ec,sys3->rach_control.ac15_11,sys3->rach_control.ac15_11,sys3->rach_control.ac7_0);
			   //   print_bits(sys3->si3.si0);
			   //   debug(DRSL," SI3 Rest Octets:\n");
			      rest_octets_si3(&sys3->si3.data[0]);
                              
                              
		 break;	
                 default:
		 break;
                 }  	 
              break;
              default:
	      break;
	      }		  	
};


//int is_bit_set(u_int8_t *ch, u_int8_t offset)
//{
//   u_int8_t  bit_mask, value;
//   bit_mask = 0x80 >> (offset & 0x07);
//   value = *ch & bit_mask;

  // printf("ch %x value %x\n",*ch, value);
//   if(value){
//     return TRUE;
//   }
//    return FALSE;
//}

/* numbering from left to right bit 1 2 3 4 5 6 7 8 */
int is_bit_set(u_int8_t ch, u_int8_t offset)
{

  u_int8_t  bit_mask, value;
  bit_mask = 0x80 >> (offset & 0x07);
  value = ch & bit_mask;
if(value){
 return TRUE;
 }
 return FALSE;
}


/* testing bit related to pading byte 0x2b
   numbering from left to right bit 1 2 3 4 5 6 7 8 */

int is_bit_H(u_int8_t ch, u_int8_t offset)
{
  u_int8_t  bit_mask, value;
  bit_mask = 0x80 >> (offset & 0x07);
  value = (ch & bit_mask) ^ (0x2b & bit_mask);
  if(value){
 return TRUE;
 }
 return FALSE;
}

#define PADDING_BYTE 0x2B
/* Rest octets SI3 indication paramters 
   spi - selection parameters
   poi - optional power offset parameters
   sii - system information 2nd indicator
   csi - early classmark sending control
   sci - scheduling if and where
   gpi - GPRS
   gci - 3G early classmark sending restrictions
*/
void rest_octets_si3(u_int8_t *si3)
{
u_int8_t spi, poi, sii, csi, sci, gpi, gci, bit, octet;
u_int8_t s = *si3 ^ PADDING_BYTE, *ptrs = &s;
u_int8_t info, octets, next_test_bit ;
print_bits(*si3);
debug(DRSL," SI3 Rest Octets:\n");
/* Optional Selection Parameters */
si3_selection_params_t *sel;
si3_power_offset_t *poii;
sel = (si3_selection_params_t *)si3;

octet = 0;
bit = spi = 0;
printf("spi %x, octet %x, bit %x\n",spi,octet,bit);
 if(is_bit_H(si3[octet],bit)){
   poi = spi +16;
   }
   else{
   poi = spi +1;
   }
   octet = poi/8;	  
   bit = poi - 8*octet +1;
   printf("poi %x, octet %x, bit %x\n",poi,octet,bit);
 if(is_bit_H(si3[octet],bit)){
   sii = poi +3;
   }
   else{
   sii = poi +1;
   }
   octet = sii/8;	  
   bit = sii - 8*octet +1;
   printf("sii %x, octet %x, bit %x\n",sii,octet,bit);
   if(is_bit_H(si3[octet],bit)){
   csi = sii + 1;
   }
   else{
   csi = sii + 1;
   }
   octet = csi/8;	  
   bit = csi - 8*octet +1;
   printf("csi %x, octet %x, bit %x\n",csi,octet,bit);
   if(is_bit_H(si3[octet],bit)){
   sci = csi +1;
   }
   else{
   sci = csi +1;
   }
   octet = sci/8;	  
   bit = sci - 8*octet +1;
   printf("sci %x, octet %x, bit %x\n",sci,octet,bit);
   if(is_bit_H(si3[octet],bit)){
   

        if(is_bit_H(sel->present,4)){
	   print_bits(*sel); 
           debug(DRSL," %s %s\n",get_name(REST_OCTETS_SI3,1), get_name(STATE,sel->present));
           debug(DRSL,"CBQ(%x) CELL_RESELECT_OFFSET(%x) TEMPORARY_OFFSET(%x) PENALTY_TIME(%x)\n",
           sel->cbq, sel->cell_resel_off, sel->temp_offs, sel->penalty_time);
           poi = (si3_power_offset_t *)&si3[2]; 
           /* power offset present bit is in 1st bit of 3 octet */
	   next_test_bit =17; 
	 }
         else{
         debug(DRSL,"\t %s %s\n",get_name(REST_OCTETS_SI3,1), get_name(STATE,sel->present));
	 /* power offset present bit is in 2nd bit of 1 octet*/
	 next_test_bit = 2;
	 }
 /* Power Offset */
        if(octets == 1){
	info = si3[0] <<  1;
        poii = (si3_power_offset_t *) &info;
        print_bits(*poii);
        }
	if(poii->present){
        debug(DRSL,"\t %s %s\n",get_name(REST_OCTETS_SI3,2), get_name(STATE,poii->present));
	debug(DRSL,"POWER_OFFSET(%x)\n",poii->power_offset);
        /* octet po: left to right bit 2 and 3 are power offset,  bit 4,5 and 6 next info bits*/
        }
        else{
	debug(DRSL,"\t %s %s\n",get_name(REST_OCTETS_SI3,2), get_name(STATE,poii->present));
        } 
	
}
/*
	for( i=0; i< 6; i++){
	bit = i; 
	ret = is_bit_set(s,bit);
	debug(DRSL,"\t %s %s\n",get_name(REST_OCTETS_SI3,bit), get_name(STATE,ret));
	}
*/



