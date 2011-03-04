 /*****************************************************************************
 *            abis_rsl.h
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



#ifndef ABIS_RSL_H
#define ABIS_RSL_H
#include <L3_utils.h>

struct  rsl_hdr {
        u_int8_t    discr_T:1,           /* 9.1 T=1 transparent message   */
		    discr_msg_group:7;   /* Message Group                 */ 
	u_int8_t    data[0];
} __attribute__ ((packed));
typedef struct  rsl_hdr rsl_hdr_t;

enum message_transparency {
		TRANSPARENT = 0x01,
	    NON_TRANSPARENT = 0x00,
};

enum  	message_group {
	 	ABIS_MG_RES    = 0x00,	/* Reserved */
		ABIS_MG_RLL    = 0x01,	/* Radio Link Layer Message */
		ABIS_MG_DCH    = 0x04,	/* Dedicated Channel Management messages */
		ABIS_MG_CCH    = 0x06,	/* Common Channel Management messages */
		ABIS_MG_TRX    = 0x08,	/* TRX Management messages */
		ABIS_MG_LOC    = 0x10,	/* Location Services messages */
};
static const name_t MG[] = {
                {0x00,	" Reserved"},
		{0x01,	" Radio Link Layer Message"},
		{0x04,	" DCH Management Message"},
		{0x06,	" Common CHannel Management Message"},
		{0x08,	" TRX Management messages"},
		{0x10,	" Location Services message"},
                {0x00,NULL}            
};

/* 8.3 General format of Radio Link Layer Management structure rsl_hdr+rsl_rll */
struct  rsl_rll { 
        u_int8_t    msg_type;	        /* 9.2   Message Type                 */
        u_int8_t    chan_ie;	        /* 9.3.1 Element Identifier           */
        u_int8_t    chan_TN:3,          /* TN and C-bits (chan_nr)            */
		    chan_nr:5;	
        u_int8_t    linkid_ie;          /* 9.3.2 Link Identifier, ie          */
        u_int8_t    linkid_SAPI:3,      /* chan.type, NA, priority and SAPI   */
	            linkid_priority:2,
		    linkid_NA:1,     
		    linkid_ch_type:2; 
        u_int8_t    data[0];
} __attribute__ ((packed));
typedef struct rsl_rll rsl_rll_t;

enum	message_type_rll {	   
	/* Radio Link Layer Management messages*/
	RSL_MT_DATA_REQ  = 0x01,		/* Data request */
	RSL_MT_DATA_IND  = 0x02,		/* Data indication */
	RSL_MT_ERROR_IND = 0x03,		/* Error Indicator */
	RSL_MT_EST_REQ   = 0x04,		/* Establish  Request */
	RSL_MT_EST_CONF  = 0x05,		/* Establish Confirmed */
	RSL_MT_EST_IND   = 0x06,		/* Establish Indication */
	RSL_MT_REL_REQ   = 0x07,		/* Release Request */
	RSL_MT_REL_CONF  = 0x08,		/* Release Confirm */
	RSL_MT_REL_IND   = 0x09,		/* Release Indication */
	RSL_MT_UNIT_DATA_REQ   = 0x0a,		/* Unit Data Request */
	RSL_MT_UNIT_DATA_IND   = 0x0b,		/* Unit Data Indication */
};



/* 8.4 General format of Dedicated Channel Management structure rsl_hdr+rsl_dch*/
struct  rsl_dch { 
        u_int8_t    msg_type;	        /* 9.2   Message Type                 */
        u_int8_t    chan_ie;	        /* 9.3.1 Element Identifier           */
        u_int8_t    chan_TN:3,          /* TN and C-bits (chan_nr)            */
		    chan_nr:5;	
        u_int8_t    data[0];
} __attribute__ ((packed));
typedef struct rsl_dch rsl_dch_t;

enum	message_type_dch{	   
	/* Dedicated Channel Management messages */
	RSL_MT_CHAN_ACTIV        = 0x21,	/* Channel Activation */
	RSL_MT_CHAN_ACTIV_ACK    = 0x22,	/* Channel Activation acknowledge */
	RSL_MT_CHAN_ACTIV_N      = 0x23,	/* Chanel Activation Negative ACK */
	RSL_MT_CONN_FAIL         = 0x24,	/* Connection Failure */
	RSL_MT_DEACTIVATE_SACCH  = 0x25,	/* Deactivate SACCH */
	RSL_MT_ENCR_CMD          = 0x26,	/* Encryption command */
	RSL_MT_HANDO_DET         = 0x27, 	/* Handover Detection */
	RSL_MT_MEAS_RES          = 0x28,	/*  Measurement Results */
	RSL_MT_MODE_MODIFY_REQ   = 0x29,	/* Mode modify Request */
	RSL_MT_MODE_MODIFY_ACK   = 0x2a,	/* Mode Modify Acknowledge */
        RSL_MT_MODE_MODIFY_N_ACK = 0x2b,        /* Mode  Modify Negative Acknowledge */
	RSL_MT_PHY_CONTEXT_REQ   = 0x2c,	/* Physical Context Requst */
	RSL_MT_PHY_CONTEXT_CONF  = 0x2d,	/* Physical Context Confirm */
	RSL_MT_RF_CHAN_REL       = 0x2e,	/* RF Channel Release */
	RSL_MT_MS_POWER_CONTROL  = 0x2f,	/* MS Power Control */
	RSL_MT_BS_POWER_CONTROL  = 0x30,	/* BS Power Control */
	RSL_MT_PREPROC_CONFIG    = 0x31,	/* Pre Proccess Configure */
	RSL_MT_PREPROC_MEAS_RES  = 0x32,	/* Pre Proccess Measurement Result */
	RSL_MT_RF_CHAN_REL_ACK   = 0x33,	/* RF Channel Relese Acknowledge */
	RSL_MT_SACCH_INFO_MODIFY = 0x34,
	RSL_MT_TALKER_DET        = 0x35,	/* Talker Detection */
	RSL_MT_LISTENER_DET      = 0x36,	/* Listener Detection */
	RSL_MT_REMOTE_CODEC_CONF = 0x37,	/* Remote CODEC Configure */
	RSL_MT_R_T_D_REP	 = 0x38,	/* Rond Trip Delay Report */
	RSL_MT_PRE_HANDO_NOTIF   = 0x39,	/* Pre-Handover Notification */
	RSL_MT_M_R_CODEC_MOD_REQ = 0x3a,	/* Multirate CODEC Modification Request */
	RSL_MT_M_R_CODEC_MOD_ACK = 0x3b,	/* Multirate CODEC Modification Acknowledge */
        RSL_MT_M_R_CODEC_MOD_N_ACK= 0x3c,	/* Multirate CODEC Modification Negative Acknowledge */
	RSL_MT_M_R_CODEC_MOD_PER = 0x3d,	/* Multirate CODEC Modification Performed */
	RSL_MT_TFO_REP		 = 0x3e,	/* TFO Report */
	RSL_MT_TFO_MOD_REQ       = 0x3f,	/* TFO Modification Reques */

};


enum	message_type_li{	   
	/* Location Services messages */
	RSL_MT_LI                 = 0x41,        /* Location Information */
};
/*=============================================================================*/

/* 8.5 General format of Common Channel Management structure rsl_hdr+rsl_cch  */
struct  rsl_cch { 
        u_int8_t    msg_type;	        /* 9.2   Message Type                 */
        u_int8_t    ei;	 	       /* 9.3.1 Element Identifier           */
        u_int8_t    chan_TN:3,          /* TN and C-bits                      */
		    chan_nr:5;	
        u_int8_t    data[0];
} __attribute__ ((packed));
typedef struct rsl_cch rsl_cch_t;

enum	message_type_cch {	   
 	/* Common Channel Management/TRX Management messages*/
	BCCH_INFO       = 0x11,		/* BCCH Information */
	CCCH_LOAD_IND   = 0x12,		/* CCCH Load Indication */
	CHAN_REQ	= 0x13,		/* Channel Required */
	DELETE_IND      = 0x14,		/* Delete Indication */
       	PAGING_CMD      = 0x15,		/* Paging command */
        IMMEDIATE_ASSIGN_COMMAND  = 0x16,/* Immediate Assign command */
	SMS_BC_REQ      = 0x17,		/* SMS Broadcast Request */
	RF_RES_IND      = 0x19,		/* RF Resource Indication */
	SACCH_FILL      = 0x1a,		/* SACCH Filling */
	OVERLOAD        = 0x1b,	        /* Overload */
	ERROR_REPORT    = 0x1c,		/* Error Report */
	SMS_BC_CMD      = 0x1d,		/* SMS Broadcast command */
	CBCH_LOAD_IND   = 0x1e,		/* CBCH Load Indicator */
	NOT_CMD         = 0x1f,		/* notification command */
};

/* 9.2 MESSAGE TYPE - Common Channel Management/TRX Management messages*/
static const name_t MT_CCH[] = {	   
	{0x11,		"BCCH Information"},
	{0x12,		"CCCH Load Indication"},
	{0x13,		"Channel Required"},
	{0x14,		"Delete Indication"},
       	{0x15,		"Paging Command"},
        {0x16,          "Immediate Assign Command"},
	{0x17,		"SMS Broadcast Request"},
	{0x19,		"RF Resource Indication"},
	{0x1a,		"SACCH Filling"},
	{0x1b,	        "Overload"},
	{0x1c,		"Error Report"},
	{0x1d,		"SMS Broadcast Command"},
	{0x1e,		"CBCH Load Indicator"},
	{0x1f,		"Notification Command "},
        {0x00,          NULL} 
};



/* 9.3.30 System Info Type */
struct  rsl_sys_info {
	u_int8_t ei;       /* Element identifier */
	u_int8_t type;
	u_int8_t data[0];
} __attribute__ ((packed));
typedef struct rsl_sys_info rsl_sys_info_t;

/* 9.3.39 Full BCCH Information */
struct rsl_full_BCCH {
	u_int8_t ei;
	u_int8_t length;
	u_int8_t L3_msg[23];
	u_int8_t data[0];
} __attribute__ ((packed));
typedef struct rsl_full_BCCH rsl_full_BCCH_t;

/* 9.3.23 Starting Time */
struct rsl_start_time {
	u_int8_t ei;
	u_int8_t T3_H:3,
	         T1p:5;
	u_int8_t T2:5,
                 T3_L:3;
} __attribute__ ((packed));
typedef struct rsl_start_time rsl_start_time_t;



/* GSM-8.58 9.3.1 C-bits */
/*
C5 C4 C3 C2 C1
0   0 0   0 1  Bm + ACCH's
0   0 0   1 T  Lm + ACCH's
0   0 1  T  T  SDCCH/4 + ACCH
0   1 T  T  T  SDCCH/8 + ACCH
1   0 0   0 0  BCCH
1   0 0   0 1  Uplink CCCH (RACH)
1   0 0   1 0  Downlink CCCH (PCH + AGCH)
*/

#define RSL_CHAN_Bm_ACCHs       0x01    
#define RSL_CHAN_Lm_ACCHs       0x02    /*0x02 .. 0x03 */
#define RSL_CHAN_SDCCH4_ACCH    0x04    /*0x04 .. 0x07 */
#define RSL_CHAN_SDCCH8_ACCH    0x08    /*0x08 .. 0x0f */
#define RSL_CHAN_BCCH           0x10    
#define RSL_CHAN_RACH           0x11    
#define RSL_CHAN_PCH_AGCH       0x12   

static const name_t CH[] = {
           {0x01,"Bm_ACCHs"       },    
	   {0x02,"Lm_ACCHs"       },    /*0x02 .. 0x03 */
	   {0x03,"Lm_ACCHs"       },
 	   {0x04,"SDCCH4_ACCH"    },    /*0x04 .. 0x07 */
           {0x05,"SDCCH4_ACCH"    },
           {0x06,"SDCCH4_ACCH"    },
           {0x07,"SDCCH4_ACCH"    },
           {0x08,"SDCCH8_ACCH"    },    /*0x08 .. 0x0f */
	   {0x09,"SDCCH8_ACCH"    },
           {0x0a,"SDCCH8_ACCH"    },
           {0x0b,"SDCCH8_ACCH"    },
           {0x0c,"SDCCH8_ACCH"    },
           {0x0d,"SDCCH8_ACCH"    },
           {0x0e,"SDCCH8_ACCH"    },
           {0x0f,"SDCCH8_ACCH"    },
           {0x10,"BCCH"           },    
           {0x11,"RACH"           },    
           {0x12,"PCH_AGCH"       },
           {0x00,NULL             }   
};

/* GSM 08.58  9.3 Element Identifier*/
static const name_t EI[] = {
        {0x01, "CHAN_NR"                },
        {0x02,"LINK_IDENT"		},
        {0x03,"ACT_TYPE"		},
        {0x04,"BS_POWER"		},
        {0x05,"CHAN_IDENT"		},
        {0x06,"CHAN_MODE"		},
        {0x07,"ENCR_INFO"		},
        {0x08,"FRAME_NUMBER"	        },
        {0x09,"HANDO_REF"     	        },
        {0x0a,"L1_INFO"		        },
        {0x0b,"L3_INFO"		        },
        {0x0c,"MS_IDENTITY"		},
        {0x0d,"MS_POWER"		},
        {0x0e,"PAGING_GROUP"	        },
        {0x0f,"PAGING_LOAD"		},
        {0x10,"PYHS_CONTEXT"            },
        {0x11,"ACCESS_DELAY"	        },
        {0x12,"RACH_LOAD"		},
        {0x13,"REQ_REFERENCE"	        },
        {0x14,"RELEASE_MODE"	        },
        {0x15,"RESOURCE_INFO"	        },
        {0x16,"RLM_CAUSE"		},
        {0x17,"STARTNG_TIME"	        },
        {0x18,"TIMING_ADVANCE"	        },
        {0x19,"UPLINK_MEAS"		},
        {0x1a,"CAUSE"		        },
        {0x1b,"MEAS_RES_NR"		},
        {0x1c,"MSG_ID"		        },
        /* reserved */
        {0x1e,"SYSINFO_TYPE"            },
        {0x1f,"MS_POWER_PARAM"	        },
        {0x20,"BS_POWER_PARAM"	        },
        {0x21,"PREPROC_PARAM"	        },
        {0x22,"PREPROC_MEAS"	        },
        {0x23,"IMM_ASS_INFO"            },    
        {0x24,"SMSCB_INFO"              },
        {0x25,"MS_TIMING_OFFSET"	},
        {0x26,"ERR_MSG"		        },
        {0x27,"FULL_BCCH_INFO"	        },
        {0x28,"CHAN_NEEDED"		},
        {0x29,"CB_CMD_TYPE"		},
        {0x2a,"SMSCB_MSG"		},
        {0x2b,"FULL_IMM_ASS_INFO"	},
        {0x2c,"SACCH_INFO"		},
        {0x2d,"CBCH_LOAD_INFO"	        },
        {0x2e,"SMSCB_CHAN_INDICATOR"    },
        {0x2f,"GROUP_CALL_REF"	        },
        {0x30,"CHAN_DESC"		},
        {0x31,"NCH_DRX_INFO"	        },
        {0x32,"CMD_INDICATOR"	        },
        {0x33,"EMLPP_PRIO"		},
        {0x34,"UIC"			},
        {0x35,"MAIN_CHAN_REF"	        },
        {0x36,"MR_CONFIG"		},
        {0x37,"MR_CONTROL"		},
        {0x38,"SUP_CODEC_TYPES"	        },
        {0x39,"CODEC_CONFIG"	        },
        {0x3a,"RTD"			},
        {0x3b,"TFO_STATUS"		},
        {0x3c,"LLP_APDU"		},
	{0x00,NULL		        }
};




enum sys_info_type{
 	 SYSTEM_INFORMATION_TYPE_8 = 0x08,
 	 SYSTEM_INFORMATION_TYPE_1 = 0x01,
 	 SYSTEM_INFORMATION_TYPE_2 = 0x02,
 	 SYSTEM_INFORMATION_TYPE_3 = 0x03,
 	 SYSTEM_INFORMATION_TYPE_4 = 0x04,
 	 SYSTEM_INFORMATION_TYPE_5 = 0x05,
 	 SYSTEM_INFORMATION_TYPE_6 = 0x06,
 	 SYSTEM_INFORMATION_TYPE_7 = 0x07,
};
#endif /* ABIS_RSL_H */


