/*
 * init.c
 *
 *  Created on: Nov 20, 2015
 *      Author: Jack
 */
#include "DSP28x_Project.h"
#include "libSCI.h"

#define PWM_FREQ 3000 //set to 10k
#define DUTY_50 PWM_FREQ/2
// ECCTL1 ( ECAP Control Reg 1) //==========================
// CAPxPOL bits
#define EC_RISING 0x0
#define EC_FALLING 0x1
// CTRRSTx bits
#define EC_ABS_MODE 0x0
#define EC_DELTA_MODE 0x1
//PRESCALE bits
#define EC_BYPASS 0x0
#define EC_DIV1 0x0
#define EC_DIV2 0x1
#define EC_DIV4 0x2
#define EC_DIV6 0x3
#define EC_DIV8 0x4
#define EC_DIV10 0x5
// ECCTL2 ( ECAP Control Reg 2)
//==========================
// CONT/ONESHOT bit
#define EC_CONTINUOUS 0x0
#define EC_ONESHOT 0x1
// STOPVALUE bit
#define EC_EVENT1 0x0
#define EC_EVENT2 0x1
#define EC_EVENT3 0x2
#define EC_EVENT4 0x3
// RE-ARM bit
#define EC_ARM 0x1
// TSCTRSTOP bit
#define EC_FREEZE 0x0
#define EC_RUN 0x1
// SYNCO_SEL bit
#define EC_SYNCIN 0x0
#define EC_CTR_PRD 0x1
#define EC_SYNCO_DIS 0x2
// CAP/APWM mode bit
#define EC_CAP_MODE 0x0
#define EC_APWM_MODE 0x1
// APWMPOL bit
#define EC_ACTV_HI 0x0
#define EC_ACTV_LO 0x1
// Generic
#define EC_DISABLE 0x0
#define EC_ENABLE 0x1
#define EC_FORCE 0x1

__interrupt void ecap1_isr(void);

void InitECapture1()
{
   ECap1Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
   ECap1Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
   ECap1Regs.ECCTL1.bit.CAPLDEN = 0;          // Disable CAP1-CAP4 register loads at capture event time
   ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped

   // Configure peripheral registers
/*
   ECap1Regs.ECCTL1.bit.PRESCALE = 0;
   ECap1Regs.ECCTL2.bit.CAP_APWM = 0;         // capture mode enable
   ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;      // continuous mode enable
   ECap1Regs.ECCTL2.bit.STOP_WRAP = 3;        // circular buffer wraps around and starts again at 4 events
   ECap1Regs.ECCTL1.bit.CAP1POL = 0;          // Rising edge
   ECap1Regs.ECCTL1.bit.CAP2POL = 1;          // Falling edge
   ECap1Regs.ECCTL1.bit.CAP3POL = 0;          // Rising edge
   ECap1Regs.ECCTL1.bit.CAP4POL = 1;          // Falling edge
   ECap1Regs.ECCTL1.bit.CTRRST1 = 1;          // delta mode (reset on event)
   ECap1Regs.ECCTL1.bit.CTRRST2 = 1;
   ECap1Regs.ECCTL1.bit.CTRRST3 = 1;
   ECap1Regs.ECCTL1.bit.CTRRST4 = 1;
   ECap1Regs.ECCTL2.bit.SYNCI_EN = 0;         // disable sync in
   ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0;        // no pass through
   ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable capture units

   ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
   ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
   ECap1Regs.ECEINT.bit.CEVT1 = 0;            // Capture Event 1 Interrupt Enable
   ECap1Regs.ECEINT.bit.CEVT2 = 1;            // Capture Event 2 Interrupt Enable
   ECap1Regs.ECEINT.bit.CEVT3 = 0;            // Capture Event 3 Interrupt Enable
   ECap1Regs.ECEINT.bit.CEVT4 = 0;            // Capture Event 4 Interrupt Enable
   */
   ECap1Regs.ECCTL1.bit.CAP1POL = EC_RISING;
   ECap1Regs.ECCTL1.bit.CAP2POL = EC_FALLING;
   ECap1Regs.ECCTL1.bit.CAP3POL = EC_RISING;
   ECap1Regs.ECCTL1.bit.CAP4POL = EC_FALLING;
   ECap1Regs.ECCTL1.bit.CTRRST1 = EC_DELTA_MODE;
   ECap1Regs.ECCTL1.bit.CTRRST2 = EC_DELTA_MODE;
   ECap1Regs.ECCTL1.bit.CTRRST3 = EC_DELTA_MODE;
   ECap1Regs.ECCTL1.bit.CTRRST4 = EC_DELTA_MODE;
   ECap1Regs.ECCTL1.bit.CAPLDEN = EC_ENABLE;
   ECap1Regs.ECCTL1.bit.PRESCALE = EC_DIV1;
   ECap1Regs.ECCTL2.bit.CAP_APWM = EC_CAP_MODE;
   ECap1Regs.ECCTL2.bit.CONT_ONESHT = EC_CONTINUOUS;
   ECap1Regs.ECCTL2.bit.SYNCO_SEL = EC_SYNCO_DIS;
   ECap1Regs.ECCTL2.bit.SYNCI_EN = EC_DISABLE;
   ECap1Regs.ECCTL2.bit.TSCTRSTOP = EC_RUN;
   ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
   ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
   ECap1Regs.ECEINT.bit.CEVT1 = 0;            // Capture Event 1 Interrupt Enable
   ECap1Regs.ECEINT.bit.CEVT2 = 1;            // Capture Event 2 Interrupt Enable
   ECap1Regs.ECEINT.bit.CEVT3 = 0;            // Capture Event 3 Interrupt Enable
   ECap1Regs.ECEINT.bit.CEVT4 = 0;            // Capture Event 4 Interrupt Enable
}

void epwmInit(int freq, int duty) {
	//InitGpio();
	//freq = freq * 4500;//freq = 1 = 10 kHz
	// EPWM Module 1 config
  	//TBCLK prescaler = /1 so TBCLK = 60,000,000
	EPwm1Regs.TBPRD = freq; // Period = 2(freq)/(TBCLK) = freq/30,000
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
	EPwm3Regs.DBFED = 20; // Falling Edge Delay = 20 TBCLKs  Target value = ~1us
	EPwm3Regs.DBRED = 20; // Rising Edge Delay = 20 TBCLKs

	EPwm1Regs.CMPA.half.CMPA = (duty); // adjust duty cycles
	EPwm2Regs.CMPA.half.CMPA = (duty); //
	EPwm3Regs.CMPA.half.CMPA = (duty); //
	return;
}

void boardSetup() {
	       InitSysCtrl();	// PLL, WatchDog, enable Peripheral Clocks
		   InitFlash();
		   DINT;
		   InitPieCtrl();

		   IER = 0x0000; // Disable CPU interrupts and clear all CPU interrupt flags
		   IFR = 0x0000;

		   InitPieVectTable();

		   EALLOW;
		   GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;
		   GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;
		   PieVectTable.ECAP1_INT = &ecap1_isr;  //Group 4 PIE Peripheral Vectors
		   //setup mux gpio
		   //GpioCtrlRegs.GPAMUX1.bits.pins? = output?
		   /*
		    * 	   EALLOW;
	   GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;// Disable pull-ups
	   GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;
	   GpioCtrlRegs.GPAPUD.bit.GPIO17 = 1;
	   GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0;//set as gpio
	   GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
	   GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;
	   GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;//output
	   GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;
	   GpioCtrlRegs.GPADIR.bit.GPIO17 = 1;
	   EDIS;
	   GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;//set to 0
	   GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
	   GpioDataRegs.GPACLEAR.bit.GPIO17 = 1;
		    */
		   EDIS;

		   InitEPwmGpio();
		   epwmInit(PWM_FREQ, DUTY_50);
		   InitECap1Gpio();
		   InitSciaGpio();
		   scia_init();
		   InitECapture1();

		   IER |= M_INT4;
		   PieCtrlRegs.PIEIER4.bit.INTx1 = 1;

		   EINT;
		   ERTM;
		   //////////////////////////////////////////////

}
