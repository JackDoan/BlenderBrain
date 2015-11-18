/*
 * main.c
 */

#include "DSP28x_Project.h"
#define PWM_FREQ 4500
#define DUTY_50 PWM_FREQ/2

void epwmInit(int freq, int duty);
void InitECapture1();
__interrupt void ecap1_isr(void);
void epwmChangeDC(int pin, int dc);
void epwmChangeDC(int*dc);

int muxState;
int handyDandy;
int outputs[3];
int failSale;
int updateDC;

int main(void) {
	//  memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize); //only needed for flash

	   InitSysCtrl();	// PLL, WatchDog, enable Peripheral Clocks
	   //InitFlash();
	   DINT;

	   InitPieCtrl();

	   IER = 0x0000; // Disable CPU interrupts and clear all CPU interrupt flags
	   IFR = 0x0000;

	   InitPieVectTable();

	   EALLOW;
	   PieVectTable.ECAP1_INT = &ecap1_isr;  //Group 4 PIE Peripheral Vectors
	   //setup mux gpio
	   //GpioCtrlRegs.GPAMUX1.bits.pins? = output?
	   EDIS;

	   InitECap1Gpio();
	   InitECapture1();
	   IER |= M_INT4;
	   PieCtrlRegs.PIEIER4.bit.INTx1 = 1;
	   outputs[0] = 0;
	   outputs[1] = 0;
	   outputs[2] = 0;
	   InitEPwmGpio();
	   epwmInit(PWM_FREQ, DUTY_50);

	   EINT;
	   ERTM;

	   //////////////////////////////////////////////////////////////////////
	   muxState = 0;
	   updateDC = 0;
	   failSafe = true;
	   float calibMin[] = {3,3,3};
	   float calibMax[] = {20,20,20};
	   float calConst[] = {calibMin[0]/calibMax[0],calibMin[1]/calibMax[1],calibMin[2]/calibMax[2]};
	   int dc[] = {0,0,0};
	   while (failSafe)
	   {
		   if(updateDC){
		   	   //convert outputs[n] to duty cycle values
		   	   	dc[0] = (int)invert(outputs[n]*calConst[n]);//invert(outputs[n]*calConst[n]) = (0.% rep of DC)^-1.
				dc[1] = (int)invert(outputs[n]*calConst[n]);
				dc[2] = (int)invert(outputs[n]*calConst[n]);
				epwmChangeDC(dc);
				updateDC = 0;
		   }
	   }
	return 0;
}

__interrupt void ecap1_isr(void)
{
   updateDC = 1;
   int DutyOnTime1 = ECap1Regs.CAP2-ECap1Regs.CAP1; // Calculate On time
   //int DutyOffTime1 = ECap1Regs.CAP3-ECap1Regs.CAP2; // Calculate Off time

   outputs[muxState] = DutyOnTime1;
   muxState++;
   if (muxState >= 3)
	   muxState = 0;

   ECap1Regs.ECCLR.bit.CEVT1 = 1;      //clears the CEVT1 flag condition
   ECap1Regs.ECCLR.bit.CEVT2 = 1;      //clears the CEVT2 flag condition
   ECap1Regs.ECCLR.bit.CEVT3 = 1;      //clears the CEVT3 flag condition
   ECap1Regs.ECCLR.bit.CEVT4 = 1;      //clears the CEVT4 flag condition
   ECap1Regs.ECCLR.bit.INT = 1;        //clears the INT flag and enable further interrupts to be generated ---global
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;  // Acknowledge this interrupt
}

void InitECapture1()
{
   ECap1Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
   ECap1Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
   ECap1Regs.ECCTL1.bit.CAPLDEN = 0;          // Disable CAP1-CAP4 register loads at capture event time
   ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped

   // Configure peripheral registers

   ECap1Regs.ECCTL1.bit.PRESCALE = 0;
   ECap1Regs.ECCTL2.bit.CAP_APWM = 0;         // capture mode enable
   ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;      // continuous mode enable
   ECap1Regs.ECCTL2.bit.STOP_WRAP = 3;        // circular buffer wraps around and starts again at 4 events
   ECap1Regs.ECCTL1.bit.CAP1POL = 0;          // Rising edge
   ECap1Regs.ECCTL1.bit.CAP2POL = 1;          // Falling edge
   ECap1Regs.ECCTL1.bit.CAP3POL = 0;          // Rising edge
   ECap1Regs.ECCTL1.bit.CAP4POL = 1;          // Falling edge
   ECap1Regs.ECCTL1.bit.CTRRST1 = 0;          // absolute modes (no reset on event)
   ECap1Regs.ECCTL1.bit.CTRRST2 = 0;
   ECap1Regs.ECCTL1.bit.CTRRST3 = 0;
   ECap1Regs.ECCTL1.bit.CTRRST4 = 0;
   ECap1Regs.ECCTL2.bit.SYNCI_EN = 0;         // Enable sync in
   ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0;        // Pass through
   ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable capture units

   ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
   ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
   ECap1Regs.ECEINT.bit.CEVT1 = 1;            // Capture Event 1 Interrupt Enable
   ECap1Regs.ECEINT.bit.CEVT2 = 1;            // Capture Event 2 Interrupt Enable
   ECap1Regs.ECEINT.bit.CEVT3 = 0;            // Capture Event 3 Interrupt Enable
   ECap1Regs.ECEINT.bit.CEVT4 = 0;            // Capture Event 4 Interrupt Enable
}

void epwmChangeDC(int pin, int dc) {
	// 0 = 0%; 4500 = 100%
	switch (pin) {
		case 1:
			EPwm1Regs.CMPA.half.CMPA = (duty);
			break;
		case 2:
			EPwm2Regs.CMPA.half.CMPA = (duty);
			break;
		case 3:
			EPwm3Regs.CMPA.half.CMPA = (duty);
			break;
		default:
			//
			break;
	}

	return;
}

void epwmChangeDC(int*dc) {
	// 0 = 0%; 4500 = 100%

	EPwm1Regs.CMPA.half.CMPA = dc[0];

	EPwm2Regs.CMPA.half.CMPA = dc[1];

	EPwm3Regs.CMPA.half.CMPA = dc[2];

	return;
}


void epwmInit(int freq, int duty) {
	//InitGpio();
	//freq = freq * 4500;//freq = 1 = 10 kHz
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

	EPwm1Regs.CMPA.half.CMPA = (duty); // adjust duty cycles
	EPwm2Regs.CMPA.half.CMPA = (duty); //
	EPwm3Regs.CMPA.half.CMPA = (duty); //
	return;
}
