
#ifndef _REV2_DSP_TAGS_H_
#define _REV2_DSP_TAGS_H_

//................................................................................

#define I_LOCUTEKL       0x04u   
#define I_LOCUTEKR       0x2cu 
#define R_HICUTEKL       0x0du 
#define R_HICUTEKR       0x35u
#define R_LOCUTREVKL     0x11u
#define R_LOCUTREVKR     0x39u 

/*
#define R_AL1			0x801du
#define R_AR1			0x8024u

#define R_DA1LW			0x805au
#define R_DA1RW			0x8072u
*/
//                        L>L    R>L    R>R    L>R
const word I_FBPHASE[] = {0x13u, 0x12u, 0x3Bu, 0x3Au};

const word I_TAPS[2][8] = 
{
	24, 27, 30, 33, 36, 37, 40, 0,
	64, 67, 70, 73, 76, 77, 80, 0
};

/*
#define R_DPL           0x808Au
#define R_DPL_          (R_DPL + 2u)    
#define R_DPR           (R_DPL + 4u)        
#define R_DPR_          (R_DPL + 6u)    
*/
//................................................................................ 
    
#endif // _REV2_DSP_TAGS_H_
