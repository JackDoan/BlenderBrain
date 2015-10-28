/*
 * main.c
 */

#include "DSP28x_Project.h"

void epwmInit(int freq, int duty);

int main(void) {
	//  memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize); //only needed for flash
	// Step 1. Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	   InitSysCtrl();
	   //InitFlash();
	// Step 2. Initalize GPIO:
	   InitEPwmGpio();
	   epwmInit(1, 2);
	   while (true)
	   {
		   asm("nop");
	   }
	return 0;
}

void epwmInit(int freq, int duty) {
	//InitGpio();
	freq = freq * 4500;//freq = 1 = 10 kHz
	// EPWM Module 1 config
  	//TBCLK prescaler = /1 so TBCLK = 90,000,000
	EPwm1Regs.TBPRD = freq; // Period = 2(freq)/(TBCLK) = freq/45,000
	EPwm1Regs.TBPHS.half.TBPHS = 0; // Set Phase register to zero
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Symmetrical mode
	EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE; // Master module
	EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // Sync down-stream module
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET; // set actions for EPWM1A
	EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
	EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary
	EPwm1Regs.DBFED = 20; // FED = 20 TBCLKs  Target value = ~1us
	EPwm1Regs.DBRED = 20; // RED = 20 TBCLKs

	EPwm2Regs.TBPRD = freq; // Period = 2(freq)/(TBCLK) = freq/40,000
	EPwm2Regs.TBPHS.half.TBPHS = 0; // Set Phase register to zero
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Symmetrical mode
	EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE; // Master module
	EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // Sync down-stream module
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm2Regs.AQCTLA.bit.CAU = AQ_SET; // set actions for EPWM1A
	EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
	EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary
	EPwm2Regs.DBFED = 20; // FED = 20 TBCLKs  Target value = ~1us
	EPwm2Regs.DBRED = 20; // RED = 20 TBCLKs

	EPwm3Regs.TBPRD = freq; // Period = 2(freq)/(TBCLK) = freq/40,000
	EPwm3Regs.TBPHS.half.TBPHS = 0; // Set Phase register to zero
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Symmetrical mode
	EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE; // Master module
	EPwm3Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // Sync down-stream module
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero
	EPwm3Regs.AQCTLA.bit.CAU = AQ_SET; // set actions for EPWM1A
	EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
	EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary
	EPwm3Regs.DBFED = 20; // FED = 20 TBCLKs  Target value = ~1us
	EPwm3Regs.DBRED = 20; // RED = 20 TBCLKs

	EPwm1Regs.CMPA.half.CMPA = (freq / duty); // adjust duty cycles
	EPwm2Regs.CMPA.half.CMPA = (freq / duty); //
	EPwm3Regs.CMPA.half.CMPA = (freq / duty); //
	return;
}
