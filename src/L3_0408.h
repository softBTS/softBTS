
 /*****************************************************************************
 *            L3_0408.h
 *  Layer 3 specification  GSM  04.08 
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


#ifndef L3_0408_H
#define L3_0408_H 
#include <L3_utils.h>


/* 10.5.1.1 Cell Identity */
struct cell_ident {					  /* Length 2 octet   */
	u_int8_t   ci1;					  /* CI cell identity */
        u_int8_t   ci2;
} __attribute__ ((packed));	
typedef struct cell_ident cell_identity_t;


/* 10.5.1.3 Location Area Identification */
/* The LAI is used for paging, to indicate to the MSC in which
   Location Area (LA) the MS is currently situated and also for
   location updating of mobile subscribers.
   LAI = MCC + MNC + LAC
   MCC = Mobile Country Code, same as IMSI MCC
   MNC = Mobile Network Code, same as IMSI MNC
   LAC = Location Area Code, the length of LAC is 16 bits,
   enabling 65,536 different location areas to be defined in one PLMN.
*/
struct location_area_id {			   /* Length 5 octet 	      */
        u_int8_t mcc1:4,			   /* MCC 3 digit BCD coded   */
                 mcc2:4;
        u_int8_t mcc3:4,
        	 mnc3:4;		   	   /* MNC 2&3  digit BCD coded*/
        u_int8_t mnc1:4,
        	 mnc2:4; 		 
        u_int8_t lac;			  	   /* LAC  Location Area Code */
        u_int8_t lac_cont;
} __attribute__ ((packed));
typedef struct location_area_id lai_t;


/* 10.5.2.11 Control Channel Description */
struct control_channel_desc {				/* Length 3 octet     */
        u_int8_t      ccch_conf :3,			/* CCCH_CONF	      */
                 bs_ag_blks_res :3,			/* BS_AG_BLKS_RES     */
                            att :1,     	        /* ATT		      */
                          spare :1;
        u_int8_t    bs_pa_mfrms :3,			/* BS_PA_MFRMS        */ 
                         spare2 :5;         
        u_int8_t             t3212;         		/* T3212 timeout      */
} __attribute__ ((packed));
typedef struct  control_channel_desc control_chanel_description_t;

/* Table 10.5.33/GSM 04.08 */
static const name_t ATT[] = {
{0, "MSs in the cell are not allowed to apply IMSI attach and detach procedure"},
{1, "MSs in the cell shall apply IMSI attach and detach procedure"},
{0, NULL}
};

static const name_t CCCH_CONF[] = {
{0, "1 basic physical channel used for CCCH, not combined with SDCCHs"},
{1, "1 basic physical channel used for CCCH, combined with SDCCHs"},
{2, "2 basic physical channel used for CCCH, not combined with SDCCHs"},
{3, " 3 basic physical channel used for CCCH, not combined with SDCCHs"},
{4, " basic physical channels used for CCCH, not combined with SDCCHs"},
{0, NULL}
};

static const name_t BS_PA_MFRMS[] = {
{0x00, "2 multiframes period for transm. PAGING REQUEST to the same paging subgroup"},
{0x01, "3 multiframes period for transm. PAGING REQUEST to the same paging subgroup"},
{0x02, "4 multiframes period for transm. PAGING REQUEST to the same paging subgroup"},
{0x03, "5 multiframes period for transm. PAGING REQUEST to the same paging subgroup"},
{0x04, "6 multiframes period for transm. PAGING REQUEST to the same paging subgroup"},
{0x05, "7 multiframes period for transm. PAGING REQUEST to the same paging subgroup"},
{0x06, "8 multiframes period for transm. PAGING REQUEST to the same paging subgroup"},
{0x0f, "9 multiframes period for transm. PAGING REQUEST to the same paging subgroup"},
{0, NULL}
};

/* 10.5.2.3 Cell Options BCCH */
struct cell_opt {				/* Length 1 octet             */
        u_int8_t radio_link_timeout:4,          /* RADIO_LINK_TIMEOUT GSM-5.08*/ 
                                dtx:2,		/* DTX 0-2 uplink discontinous*/ 
                               pwrc:1,          /* PWRC 0-1 GSM-5.08          */
                              spare:1;
} __attribute__ ((packed));
typedef struct  cell_opt cell_options_t;

static const name_t DTX[] = {
{0, "The MSs may use uplink discontinuous transmission"},
{1, "The MSs shall use uplink discontinuous transmission"},
{2, "The MS shall not use uplink discontinuous transmission"},
{0, NULL}
};



/* 10.5.2.4 Cell Selection Parameters */
struct cell_selection_par {				/* Length 2 octet     */
        u_int8_t ms_txpwr_max_ccch:5,   /* MS-TXPWR-MAX-CCCH        GSM 05.08 */
                   cell_resel_hyst:3;   /* CELL-RESELECT-HYSTERESIS GSM 05.08 */
        u_int8_t     rxlev_acc_min:6,   /* RXLEV-ACCESS-MIN         GSM 05.08 */
                              neci:1,   /* NECI 0-1 helf rate suport          */
                               acs:1;   /* ACS  0-1 			      */	
} __attribute__ ((packed));
typedef struct cell_selection_par  cell_selection_parameters_t;




/* 10.5.2.29 RACH control parameters */
/* To prevent collision in air interface the parametrs Tx-integer and Max trans
   are send in BCCH chanell.
   Tx-integer  parameter is used for calculation of the time interval between
               transmision of random request. 
   Max-retrans parameter represents maximum number of access attempts if no AGCH
               answer from network.    
All mobile stations with an inserted SIM are members of one out of 10 access 
classes numbered 0 to 9. The access class number is stored in the SIM. 
In addition, mobile stations may be members of one or more out of 5 special 
access classes (access classes 11 to 15) 
The parameters are sent as a bit-map with EC indicating whether emergency calls 
are allowed in this cell.
Bit position 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
Access class(15 14 13 12 11)10(9 8 7 6 5 4 3 2 1 0)
1 barred            1                        1 

In this example access classes 2 and 13 are not allowed to use the cell. Access
classes 0–9 are the normal subscriber classes, 11–15 are reserved for emergency,
security and network services.
*/
struct rach_control_par {        	/*Length 3 octet 		      */	
        u_int8_t               re :1,	/* RE 0-1 Call reeastablishment       */
                  cell_bar_access :1,   /* CELL-BAR-ACCESS   GSM 03.22        */
                       tx_integer :4,   /* TX-INTEGER	         	      */	
                      max_retrans :2;   /* MAX RETRANS 	     		      */
        u_int8_t             ac8_9:2,   /* ACCESS CONTROL CLASS 0..9, 11..15  */
        			ec:1,   /* EMERGENCY CALL */
        		   ac15_11:5;     
        u_int8_t             ac7_0:8;
} __attribute__ ((packed));
typedef struct rach_control_par  rach_control_parameters_t;



/* 10.5.2.34 SI 3 Rest Octets */
/* The SI 3 Rest Octets information element is a type 5 information element 
with 5  octets length. */
struct si3_rest {
	u_int8_t	data[4];
} __attribute__((packed));
typedef struct si3_rest si3_rest_t;

struct si3_selection_params {
        u_int16_t penalty_time:5,
                     temp_offs:3,
                cell_resel_off:6,
                           cbq:1,
                       present:1;
} __attribute__((packed));
typedef struct si3_selection_params si3_selection_params_t;

struct si3_power_offset {
      u_int8_t        spare:5,
	       power_offset:2,
                    present:1;
}__attribute__((packed));
typedef struct si3_power_offset si3_power_offset_t;


static const name_t REST_OCTETS_SI3[] = {
{1,"Selection Parameters:"		},
{2,"Power Offset:"			},
{3,"System Information 2nd Indicator:"	},
{4,"Early Classmark Sending Control:"	},
{5,"Scheduling if and where:"		},
{6,"GPRS indicator:"			},
{0, NULL}
};

static const name_t STATE[] = {
{TRUE , "HIGH present"    },
{FALSE, "LOW not present" },
{0, NULL}
};

/* 10.5.2.19 L2 Pseudo Length */
/* The L2 Pseudo Length information element indicates the number of octets 
following it in the message, the total number of octets (excluding the Rest 
Octets) for which T, V, TV, LV, or TLV formatting is used.
   NOTE: For any of the SYSTEM INFORMATION messages sent on the BCCH, 
a mobile station should ignore the contents of the L2 Pseudo Length value 
contained in the L2 Pseudo Length information element.
*/ 
struct L2_plength {				  /* See NOTE 	              */
	u_int8_t	   fb:2,                  /* Predetermined value 0x01 */	
 	                   plv:6;	          /* L2 pseudo length value   */
}__attribute__((packed));
typedef struct L2_plength L2_pseudo_length_t;


/* PD Protocol discriminator & TI transaction identifier or 
   Skip Indicator -  Figure 10.1/GSM 04.08: General message organization */
struct PD_trans { 			                    /* Length 1 octet */
      u_int8_t 	        discr:4,                    /* Protocol discriminator */
	                trans:4;                    /* transaction identifier */
			 		            /* defined in GSM 04.07   */
}__attribute__((packed));
typedef struct PD_trans PD_trans_t;

struct PD_skip { 			             /* Length 1 octet        */
      u_int8_t	        discr:4,                     /* Protocol discriminator*/
		        skip:4;                      /* skip indicator        */
}__attribute__((packed));
typedef struct PD_skip PD_skip_t;

enum PD {		        /* The Protocol Discriminator (PD)   	*/
	PD_CC = 0x03, 	        /* Call Control related to SS Messages 	*/
	PD_MM = 0x05, 	        /* Mobility Managament messages    	*/
	PD_RR = 0x06, 	        /* Radio Resourse Messages             	*/
        PD_MM_GPRS = 0x08, 	/* Mobility Managament GPRS Messages   	*/ 
	PD_SM = 0x0a,	        /* Session Managament Messages         	*/
};
#define SKIP 0x00	/*Message received with 0000 not to be ignored  */

/* 9.1.35 System information type 3 */
/* This message is sent on the BCCH by the network giving information of 
   control on the RACH, the location area identification, the cell identity etc 
   For any of the SYSTEM INFORMATION messages sent on the BCCH, a mobile station 
   should ignore the contents of the L2 Pseudo Length value.
   Pseduo length value is 18 octet (excluding pseduo length value and rest oct.) 	
*/
struct system_information_type3 {
	L2_pseudo_length_t             L2;                       /*  1 octet  */
	PD_skip_t                      DP;                       /*  1 octet  */ 
	u_int8_t                       msg_type;                 /*   1 octet */
	cell_identity_t		       cell_id;     	         /*   2 octet */
	lai_t			       lai;   			 /*   5 octet */
	control_chanel_description_t   cch_descr;                /*   3 octet */
	cell_options_t		       cell_options;  	 	 /*   1 octet */
	cell_selection_parameters_t    csp;     	         /*   2 octet */
	rach_control_parameters_t      rach_control; 	         /*   3 octet */
	si3_rest_t		       si3;    	                 /*   4 octet */
} __attribute__ ((packed));
typedef struct system_information_type3 system_information_type3_t;

/* 0.5.2.22 Neighbour Cells Description 
   The purpose of the Neighbour Cells Description information element is to 
   provide the absolute radio frequency channel numbers of the BCCH carriers 
   to be monitored by the mobile stations in the cell.*/   
struct neighbour_cell_descr {
	u_int8_t	           ch_nr_124_121:4,
			                  ba_ind:1,
			                 ext_ind:1,
			           ch_nr_128_127:2;
	u_int8_t	          ch_nr_120_00[15];
} __attribute__ ((packed));
typedef struct neighbour_cell_descr neighbour_cell_description_t;

/* 10.5.2.27 NCC Permitted
    The purpose of the NCC Permitted information element is to provide a 
    definition of the allowed NCCs on the BCCH carriers to be reported in 
    the MEASUREMENT REPORT  message by the mobile stations in the cell
*/

/* 9.1.32 System information type 2
   This message is sent on the BCCH by the network to all mobile stations 
   within the cell giving information of control of the RACH and of the BCCH 
   allocation in the neighbour cells. */
struct system_information_type2 {
	L2_pseudo_length_t             L2;                        /*  1 octet */
	PD_skip_t                      DP;                        /*  1 octet */
	u_int8_t                       msg_type;                  /*  1 octet */
	neighbour_cell_description_t   ncd;                       /* 16 octet */
	u_int8_t 		       ncc_permitted;		  /*  1 octed */
	rach_control_parameters_t      rach_control;              /*  3 octet */
} __attribute__ ((packed));
typedef struct system_information_type2 system_information_type2_t;



/* 10.5.2.2 Cell Description  */
/* BCC = Base Station Color Code (3 bits), identifies the Base
   Station to help distinguish between BTS using the same BCCH
   frequencies.(The BCC is also used to identify the TSC (Training Sequence Code) 
   to be used when reading the BCCH.)
   NCC = Network Color Code (3 bits), identifies the PLMN. Note
   that it does not uniquely identify the operator. NCC is primarily
   used to distinguish between operators on each side of a border.
*/
struct cell_desc {                                 /* Length 2 octet          */
        u_int8_t          bcc:3,                   /* BCC BS colour code  0 7 */
                          ncc:3,                   /* NCC PLMN color code 0 7 */ 
                     arfcn_hi:2;                   /* ARFCN            0 1023 */
        u_int8_t       arfcn_lo;
} __attribute__((packed));
typedef struct cell_desc  cell_description_t;

/* 10.5.2.1b Cell Channel Description */
struct cell_channel_descr {
	u_int8_t bit_121:1,         /* bit121                                 */
		 format_id_low:3,   /* bit124, bit123, bit122                 */
		 spare:2,           /* bit126, bit125                         */ 
		 format_id_high:2;  /* bit128, bit127                         */
	u_int8_t bits[15];          /* bit120, bit119, ...              bit113*/
				    /* .......                                */  
                                    /* bit008  bit007  ...      bit002  bit001*/    
} __attribute__((packed));
typedef struct cell_channel_descr cell_channel_descr_t;

/* 10.5.2.1b.7 Variable bit map format - Cell Channel Description
FIXME: Add other formats*/
struct cell_var_bit {
	u_int8_t	orig_arfcn_hi:1,   /* bit121                          */
		 	format_id_low:3,   /* bit124, bit123, bit122          */
		 	spare:2,           /* bit126, bit125                  */ 
		 	format_id_high:2;  /* bit128, bit127                  */
	u_int8_t	orig_arfcn_mid;
	u_int8_t	rrfcn_1_7:7,
		        orig_arfcn_lo:1;
	u_int8_t        rrfcn_8_111[13];
} __attribute__ ((packed));
typedef struct cell_var_bit cell_var_bit_t;

/* 10.5.2.32 SI 1 Rest Octets -> NCH Position on the CCCH
   The SI 1 Rest Octets information element contains the position about the 
   NCH - Notification Channel and 3 spare bits. 
   The value of NCH is number of 1st block of the CCCH which is used for the
   first NCH block. This value determined also number of blocks used for the 
   Notification Channel NCH.
   NCH is used to infor MS for incoming group and broadcast calls.
   The following coding applies if 1 or more basic physical channels are used 
   for CCCH, not combined with SDCCHs.
   System Information Type 1 need only be sent if frequency hopping is
   in use or when the NCH is present in a cell. 
*/


/* 9.1.32 System information type 1
   Send when TC = 0, TC = (FN DIV 51) mod (8))
   GSM 5.02 p.27 System Information Type 1 need only be sent if frequency 
   hopping is in use or when the NCH is present in a cell. If the MS finds 
   another message when TC = 0, it can assume that System Information Type 1 
   is not in use.
 */

struct system_information_type1 {
	L2_pseudo_length_t             L2;                        /*  1 octet */
	PD_skip_t                      DP;                        /*  1 octet */
	u_int8_t                       msg_type;                  /*  1 octet */
	cell_channel_descr_t           chd;                        /* 16 oct   */
	rach_control_parameters_t      rach_control;              /*  3 octet */
	u_int8_t	       	       si1;                       /*  1 octet */
} __attribute__ ((packed));
typedef struct system_information_type1 system_information_type1_t;

/* Table 10.4 GSM 04.08 */
static const name_t RR[] = {
 {0x3c,    "RR_INIT_REQ"		},
 {0x3b,    "RR_ADD_ASS"			},
 {0x3f,    "RR_IMM_ASS"			},
 {0x39,    "RR_IMM_ASS_EXT"		},
 {0x3a,    "RR_IMM_ASS_REJ"		},
 {0x35,    "RR_CIPH_M_CMD"		},
 {0x32,    "RR_CIPH_M_COMPL"		},
 {0x30,    "RR_CFG_CHG_CMD"		},
 {0x31,    "RR_CFG_CHG_ACK"		},
 {0x33,    "RR_CFG_CHG_REJ"		},
 {0x2e,    "RR_ASS_CMD"			},
 {0x29,    "RR_ASS_COMPL"		},
 {0x2f,    "RR_ASS_FAIL"		},
 {0x2b,    "RR_HANDO_CMD"		},
 {0x2c,    "RR_HANDO_COMPL"		},
 {0x28,    "RR_HANDO_FAIL"		},
 {0x2d,    "RR_HANDO_INFO"		},
 {0x08,    "RR_CELL_CHG_ORDER"		},
 {0x23,    "RR_PDCH_ASS_CMD"		},
 {0x0d,    "RR_CHAN_REL"		},
 {0x0a,    "RR_PART_REL"		},
 {0x0f,    "RR_PART_REL_COMP"		},
 {0x21,    "RR_PAG_REQ_1"		},
 {0x22,    "RR_PAG_REQ_2"		},
 {0x24,    "RR_PAG_REQ_3"		},
 {0x27,    "RR_PAG_RESP"		},
 {0x20,    "RR_NOTIF_NCH"		},
 {0x25,    "RR_NOTIF_FACCH"		},
 {0x26,    "RR_NOTIF_RESP"		},
 {0x18,    "RR_SYSINFO_8"		},
 {0x19,    "RR_SYSINFO_1"		},
 {0x1a,    "RR_SYSINFO_2"		},
 {0x1b,    "RR_SYSINFO_3"		},
 {0x1c,    "RR_SYSINFO_4"		},
 {0x1d,    "RR_SYSINFO_5"		},
 {0x1e,    "RR_SYSINFO_6"		},
 {0x1f,    "RR_SYSINFO_7"		},
 {0x02,    "RR_SYSINFO_2bis"		},
 {0x03,    "RR_SYSINFO_2ter"		},
 {0x05,    "RR_SYSINFO_5bis"		},
 {0x06,    "RR_SYSINFO_5ter"		},
 {0x04,    "RR_SYSINFO_9"		},
 {0x00,    "RR_SYSINFO_13"		},
 {0x3d,    "RR_SYSINFO_16"		},
 {0x3e,    "RR_SYSINFO_17"		},
 {0x10,    "RR_CHAN_MODE_MODIF"		},
 {0x12,    "RR_STATUS"			},
 {0x17,    "RR_CHAN_MODE_MODIF_ACK"	},
 {0x14,    "RR_FREQ_REDEF"		},
 {0x15,    "RR_MEAS_REP"		},
 {0x16,    "RR_CLSM_CHG"		},
 {0x13,    "RR_CLSM_ENQ"		},
 {0x36,    "RR_EXT_MEAS_REP"		},
 {0x37,    "RR_EXT_MEAS_REP_ORD"	},
 {0x34,    "RR_GPRS_SUSP_REQ"		},
 {0x08,    "RR_VGCS_UPL_GRANT"		},
 {0x0e,    "RR_UPLINK_RELEASE"		},
 {0x0c,    "RR_UPLINK_FREE"		},
 {0x2a,    "RR_UPLINK_BUSY"		},
 {0x11,    "RR_TALKER_IND"		},
 {0x38,    "RR_APP_INFO"		},
 {0x00,NULL                         	}
};

/* Table 10.2 GSM 04.08 */
static const name_t MM[] = {
 {0x01,     "MM_IMSI_DETACH_IND" },
 {0x02,     "MM_LOC_UPD_ACCEPT"  },
 {0x04,     "MM_LOC_UPD_REJECT"  },
 {0x08,     "MM_LOC_UPD_REQUEST" },
 {0x11,     "MM_AUTH_REJ"	 },
 {0x12,     "MM_AUTH_REQ"        },
 {0x14,     "MM_AUTH_RESP"	 },
 {0x18,     "MM_ID_REQ"	   	 },
 {0x19,     "MM_ID_RESP"	 },
 {0x1a,     "MM_TMSI_REALL_CMD"  },
 {0x1b,     "MM_TMSI_REALL_COMPL"},
 {0x21,     "MM_CM_SERV_ACC" 	 },
 {0x22,     "MM_CM_SERV_REJ"     },
 {0x23,     "MM_CM_SERV_ABORT"   },
 {0x24,     "MM_CM_SERV_REQ"     },
 {0x25,     "MM_CM_SERV_PROMPT"  },
 {0x28,     "MM_CM_REEST_REQ"    },
 {0x29,     "MM_ABORT"           },
 {0x30,     "MM_NULL"            },
 {0x31,     "MM_STATUS"          },
 {0x32,     "MM_INFO"            },
 {0x00, NULL                     }
};

static const name_t NCH[] = {
    { 0, "No of blocks = 1 and Number of first block = 0"},
    { 1, "No of blocks = 1 and Number of first block = 1"},
    { 2, "No of blocks = 1 and Number of first block = 2"},
    { 3, "No of blocks = 1 and Number of first block = 3"},
    { 4, "No of blocks = 1 and Number of first block = 4"},
    { 5, "No of blocks = 1 and Number of first block = 5"},
    { 6, "No of blocks = 1 and Number of first block = 6"},
    { 7, "No of blocks = 1 and Number of first block = 0"},
    { 8, "No of blocks = 2 and Number of first block = 1"},
    { 9, "No of blocks = 2 and Number of first block = 2"},
    {10, "No of blocks = 2 and Number of first block = 3"},
    {11, "No of blocks = 2 and Number of first block = 4"},
    {12, "No of blocks = 2 and Number of first block = 5"},
    {13, "No of blocks = 3 and Number of first block = 0"},
    {14, "No of blocks = 3 and Number of first block = 1"},
    {15, "No of blocks = 3 and Number of first block = 2"},
    {16, "No of blocks = 3 and Number of first block = 3"},
    {17, "No of blocks = 3 and Number of first block = 4"},
    {18, "No of blocks = 4 and Number of first block = 0"},
    {19, "No of blocks = 4 and Number of first block = 1"},
    {20, "No of blocks = 4 and Number of first block = 2"},
    {21, "No of blocks = 4 and Number of first block = 3"},
    {22, "No of blocks = 5 and Number of first block = 0"},
    {23, "No of blocks = 5 and Number of first block = 1"},
    {24, "No of blocks = 5 and Number of first block = 2"},
    {25, "No of blocks = 6 and Number of first block = 0"},
    {26, "No of blocks = 6 and Number of first block = 1"},
    {27, "No of blocks = 7 and Number of first block = 0"},
    {28, "Reserved"},
    {29, "Reserved"},
    {30, "Reserved"},
    {31, "Reserved"},
    { 0, NULL }
};

#endif /*EOF*/
