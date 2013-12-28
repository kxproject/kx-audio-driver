// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
// All rights reserved

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef _SURROUNDER2_DSP_TAGS_H_
#define _SURROUNDER2_DSP_TAGS_H_

enum {
	macs,
	macsn,
	macw,
	macwn,
	macints,
	macintw,
	acc3,
	macmv,
	andxor, 
	tstneg, 
	limit,
	limitn, 
	_log,
	_exp,
	interp, 
	skip,
};

#define SURROUNDER2_CODE_SIZE   24

const word sc_code_sp_mode[][SURROUNDER2_CODE_SIZE][speaker_modes_count] =
{
	{ // 2.1 mode
	{macsn, R_FL, R_LB, R_VWL, 0x2040u      },
	{macsn, R_FR, R_RB, R_VWR, 0x2040u      },
	{acc3,  R_W, R_IW, R_VWL, R_VWR         },
	{macs,  0x2040u, R_VWL, R_WFZL, R_WFB   },
	{macmv, R_WFZL, R_VWL, R_VWL, R_WFY     },
	{macs,  R_VWL, 0x2056u, R_LB, R_WFA     },
	{macs,  0x2040u, R_VWR, R_WFZR, R_WFB   },
	{macmv, R_WFZR, R_VWR, R_VWR, R_WFY     },
	{macs,  R_VWR, 0x2056u, R_RB, R_WFA     },
	{macs,  R_LB, R_IL, R_ISL, R_SK1        },
	{macsn, R_LB, R_LB, R_ISR, R_SK2        },
	{macs,  R_LB, R_LB, R_IC, R_CK          },
	{macs,  R_LB, R_LB, R_IFL, 0x204Fu      },
	{macs,  R_SL, 0x2040u, 0x2040u, 0x2040u },
	{macs,  R_RB, R_IR, R_ISR, R_SK1        },
	{macsn, R_RB, R_RB, R_ISL, R_SK2        },
	{macs,  R_RB, R_RB, R_IC, R_CK          },
	{macs,  R_RB, R_RB, R_IFR, 0x204Fu      },
	{macs,  R_SR, 0x2040u, 0x2040u, 0x2040u },
	{macs,  R_C, 0x2040u, 0x2040u, 0x2040u  },

	{macs,  0x2040u,0x2040u,0x2040u,0x2040u }, // nop
	{macs,  0x2040u,0x2040u,0x2040u,0x2040u }, // nop

	// zero sCenter 1/2 
	{macs,  R_SCNTR1,0x2040u,0x2040u,0x2040u},
	{macs,  R_SCNTR2,0x2040u,0x2040u,0x2040u}
	// W,E,S are NOT USED
	},

	{ // 4.1 mode
	{macsn, R_LB, R_IL, R_VWL, 0x2040u      },
	{macsn, R_RB, R_IR, R_VWR, 0x2040u      },
	{acc3,  R_W, R_IW, R_VWL, R_VWR         },
	{macs,  0x2040u, R_VWL, R_WFZL, R_WFB   },
	{macmv, R_WFZL, R_VWL, R_VWL, R_WFY     },
	{macs,  R_VWL, 0x2056u, R_IL, R_WFA     },
	{macs,  0x2040u, R_VWR, R_WFZR, R_WFB   },
	{macmv, R_WFZR, R_VWR, R_VWR, R_WFY     },
	{macs,  R_VWR, 0x2056u, R_IR, R_WFA     },
	{macs,  R_C, 0x2040u, R_LB, R_SK1       },
	{macsn, R_DWL, 0x2056u, R_RB, R_SK2     },
	{macs,  R_C, 0x2040u, R_RB, R_SK1       },
	{macsn, R_DWR, 0x2056u, R_LB, R_SK2     },
	{macs,  R_LB, R_LB, R_IC, R_CK          },
	{macs,  R_RB, R_RB, R_IC, R_CK          },
	{macs,  R_FL, R_IFL, R_LB, 0x204Fu      },
	{macs,  R_SL, R_ISL, R_DRL, 0x204Fu     },
	{macs,  R_FR, R_IFR, R_RB, 0x204Fu      },
	{macs,  R_SR, R_ISR, R_DRR, 0x204Fu     },
	{macs,  R_C, 0x2040u, 0x2040u, 0x2040u  },

	{macs,  0x2040u,0x2040u,0x2040u,0x2040u }, // nop
	{macs,  0x2040u,0x2040u,0x2040u,0x2040u }, // nop

	// zero sCenter 1/2 
	{macs,  R_SCNTR1,0x2040u,0x2040u,0x2040u},
	{macs,  R_SCNTR2,0x2040u,0x2040u,0x2040u}
	// W,E,S are NOT USED
	},

	{ // 5.1 mode
	{macsn, R_LB, R_IL, R_VWL, 0x2040u      },
	{macsn, R_RB, R_IR, R_VWR, 0x2040u      },
	{acc3,  R_W, R_IW, R_VWL, R_VWR         },
	{macs,  0x2040u, R_VWL, R_WFZL, R_WFB   },
	{macmv, R_WFZL, R_VWL, R_VWL, R_WFY     },
	{macs,  R_VWL, 0x2056u, R_IL, R_WFA     },
	{macs,  0x2040u, R_VWR, R_WFZR, R_WFB   },
	{macmv, R_WFZR, R_VWR, R_VWR, R_WFY     },
	{macs,  R_VWR, 0x2056u, R_IR, R_WFA     },
	{macs,  R_C, 0x2040u, R_LB, R_SK1       },
	{macsn, R_DWL, 0x2056u, R_RB, R_SK2     },
	{macs,  R_C, 0x2040u, R_RB, R_SK1       },
	{macsn, R_DWR, 0x2056u, R_LB, R_SK2     },
	{macs,  R_C, R_IC, R_LB, R_CK           },
	{macs,  R_C, R_C, R_RB, R_CK            },
	{macs,  R_FL, R_IFL, R_LB, 0x204Fu      },
	{macs,  R_SL, R_ISL, R_DRL, 0x204Fu     },
	{macs,  R_FR, R_IFR, R_RB, 0x204Fu      },
	{macs,  R_SR, R_ISR, R_DRR, 0x204Fu     },
	{macs,  R_C, R_C, 0x2040u, 0x2040u      },

	{macs,  0x2040u,0x2040u,0x2040u,0x2040u }, // nop
	{macs,  0x2040u,0x2040u,0x2040u,0x2040u }, // nop

	// zero sCenter 1/2 
	{macs,  R_SCNTR1,0x2040u,0x2040u,0x2040u},
	{macs,  R_SCNTR2,0x2040u,0x2040u,0x2040u}
	// W,E,S are NOT USED
	},

	{ // 6.1 mode
	{macsn, R_LB, R_IL, R_VWL, 0x2040u      },
	{macsn, R_RB, R_IR, R_VWR, 0x2040u      },
	{acc3,  R_W, R_IW, R_VWL, R_VWR         },
	{macs,  0x2040u, R_VWL, R_WFZL, R_WFB   },
	{macmv, R_WFZL, R_VWL, R_VWL, R_WFY     },
	{macs,  R_VWL, 0x2056u, R_IL, R_WFA     },
	{macs,  0x2040u, R_VWR, R_WFZR, R_WFB   },
	{macmv, R_WFZR, R_VWR, R_VWR, R_WFY     },
	{macs,  R_VWR, 0x2056u, R_IR, R_WFA     },
	{macs,  R_C, 0x2040u, R_LB, R_SK1       },
	{macsn, R_DWL, 0x2056u, R_RB, R_SK2     },
	{macs,  R_C, 0x2040u, R_RB, R_SK1       },
	{macsn, R_DWR, 0x2056u, R_LB, R_SK2     },
	{macs,  R_C, R_IC, R_LB, R_CK           },
	{macs,  R_C, R_C, R_RB, R_CK            },

	// prepare R_E/R_8W 
	{macs,  R_E,0x2040u,R_E,C_40000000      },
	{macs,  R_8W,0x2040u,R_8W,C_40000000    },

	{acc3,  R_FL, R_IFL, R_LB, R_8W         },  // += R_W (-6dB)
	{acc3,  R_SL, R_ISL, R_DRL, R_8W        },
	{acc3,  R_FR, R_IFR, R_RB, R_E          },  // += R_E (-6dB)
	{acc3,  R_SR, R_ISR, R_DRR, R_E         },

	{macs,  R_C, R_C, 0x2040u, 0x2040u      },

	// 's' -> sCenter1; sCenter2=muted
	{macs,  R_SCNTR1,R_S,0x2040u,0x2040u    },
	{macs,  R_SCNTR2,0x2040u,0x2040u,0x2040u},
	},


	{ // 7.1 mode
	{macsn, R_LB, R_IL, R_VWL, 0x2040u      },
	{macsn, R_RB, R_IR, R_VWR, 0x2040u      },
	{acc3,  R_W, R_IW, R_VWL, R_VWR         },
	{macs,  0x2040u, R_VWL, R_WFZL, R_WFB   },
	{macmv, R_WFZL, R_VWL, R_VWL, R_WFY     },
	{macs,  R_VWL, 0x2056u, R_IL, R_WFA     },
	{macs,  0x2040u, R_VWR, R_WFZR, R_WFB   },
	{macmv, R_WFZR, R_VWR, R_VWR, R_WFY     },
	{macs,  R_VWR, 0x2056u, R_IR, R_WFA     },
	{macs,  R_C, 0x2040u, R_LB, R_SK1       },
	{macsn, R_DWL, 0x2056u, R_RB, R_SK2     },
	{macs,  R_C, 0x2040u, R_RB, R_SK1       },
	{macsn, R_DWR, 0x2056u, R_LB, R_SK2     },
	{macs,  R_C, R_IC, R_LB, R_CK           },
	{macs,  R_C, R_C, R_RB, R_CK            },

	// calc R_S 
	{macs,  R_S, 0x2040u, R_S,C_40000000    },

	{macs,  R_FL, R_IFL, R_LB, 0x204Fu      },
	{acc3,  R_SL, R_ISL, R_DRL, R_S         },  // += R_S (/2)
	{macs,  R_FR, R_IFR, R_RB, 0x204Fu      },
	{acc3,  R_SR, R_ISR, R_DRR, R_S         },  // += R_S (/2)

	{macs,  R_C, R_C, 0x2040u, 0x2040u      },

	{macs,  0x2040u,0x2040u,0x2040u,0x2040u }, // nop

	// copy W/E -> sCenter1/2
	{macs,  R_SCNTR1,R_8W,0x2040u,0x2040u}, 
	{macs,  R_SCNTR2,R_E,0x2040u,0x2040u},
	}
};

word surr2_sub_mode_code[3][3][3] = 
{
	{{88, R_VWL, 0x2040u}, {88, R_VWR, 0x2040u}, {R_IW, 0x2040u, 0x2040u}},     // default
	{{88, R_IW, 0x204du},  {88, R_IW, 0x204du},  {0x2040u, 0x2040u, 0x2040u}},  // lfe->front (nosub)
	{{88, R_VWL, 0x204Fu}, {88, R_VWR, 0x204Fu}, {R_IW, R_VWL, R_VWR}}          // redirect 
};


#endif //
