/*
 * main.c
 */

#include "DSP28x_Project.h"
#include "init.h"
#include <stdio.h>
#include "libSCI.h"
void epwmChangeDC(int*dc);

int muxState;
int handyDandy;
long outputs[3];
int failSafe;
int updateDC;
int errors;
int writeOutputsFlag;
long longestTime;
long tooLong;
int calibrationFlag;
float calibMin[3];
float calibScale[3];
Uint16 sciData;
int main(void) {
	memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize); //only needed for flash
	boardSetup();
	errors = 0;
	int dcHasChanged = 1;
	int notConnected = 0;
	sciData = 0;
	writeOutputsFlag = 0;
	outputs[0] = 0;
	outputs[1] = 0;
	outputs[2] = 0;
	muxState = 0;
	updateDC = 0;
	failSafe = true;
	calibrationFlag = 0;
	calibMin[0] = 6600;
	calibMin[1] = 6600;
	calibMin[2] = 6600;
	longestTime = 0;
	tooLong = 1000000;
	calibScale[0] = 11400-calibMin[0];
	calibScale[1] = 11400-calibMin[1];
	calibScale[2] = 11400-calibMin[2];

	int dc[] = {0,0,0};
	const char badCommand[] = "Unrecognized command";
	const char clearScreen[] = "\033[2J\033[0;0H";
	const char title[] = "UTD BATTLEBOTS ROBOT CONTROL UNIT\n\r---------------------------------";
	const char helpPrompt[] = "Press ? for a list of commands.";
	const char ctrlLevels[] = "Control Status:";
	const char calMenu[] = "Calibration:";
	const char noController[] = "The controller is not connected";
	const char wrongcPressed[] = "Press C, not c, to perform calibration";
	char printBuffer[60] = "\0";

	//////////////////////////////////////////////////////////////////////


	scia_ln(clearScreen);
	scia_ln(title);
	scia_ln(helpPrompt);

	while (failSafe) {
		if(updateDC) { //update PWM outputs:
			//convert outputs[n] to duty cycle values
			dc[0] = (int)((outputs[0]-calibMin[0])*((float)PWM_FREQ)/(calibScale[0]));
			dc[1] = (int)((outputs[1]-calibMin[1])*((float)PWM_FREQ)/(calibScale[1]));
			dc[2] = (int)((outputs[2]-calibMin[2])*((float)PWM_FREQ)/(calibScale[2]));
			epwmChangeDC(dc);
			updateDC = 0;
			dcHasChanged = 1;
		}
		else if (((dc[0] == 0)&&(dc[1] == 0)&&(dc[2] == 0)) || (notConnected)) {
			failSafe = 0;
			scia_ln(clearScreen);
			scia_ln(title);
			scia_ln(noController);
		}


		if (SciaRegs.SCIFFRX.bit.RXFFST){ //check serial data:
			sciData = scia_read();
		}
		else {
			sciData = 0;
		}
		if (sciData) {
			switch (sciData) {
			case (int)'q':
				writeOutputsFlag = !writeOutputsFlag;
			break;

			case (int)'c':
				scia_ln(clearScreen);
				scia_ln(title);
				scia_ln(wrongcPressed);
				break;

			case (int)'C':
				calibrationFlag = 1;
				break;

			case (int)'o':
				scia_ln(calMenu);
			break;

			default:
				scia_ln(clearScreen);
				scia_ln(title);
				scia_ln(badCommand);
				scia_ln(helpPrompt);
				break;
			}
			sciData = 0;
		}
		//menu subroutines:
		if(writeOutputsFlag && dcHasChanged) {
			scia_ln(clearScreen);
			scia_ln(title);
			scia_ln(ctrlLevels);
			sprintf(printBuffer, "Left Motor:  %d\r\n",dc[0]);
			scia_ln(printBuffer);
			sprintf(printBuffer, "Right Motor: %d\r\n",dc[1]);
			scia_ln(printBuffer);
			sprintf(printBuffer, "Spin Motors: %d\r\n",dc[2]);
			scia_ln(printBuffer);
			dcHasChanged = 0;
		}

		if(calibrationFlag) {
			/*
			 * for each (input) {
			 * measureMiddle
			 * waitbutton
			 * measurept1
			 * waitbutton
			 * measure pt2
			 * waitbutton
			 * writeToFlash
			 * }
			 */
			 calibrationFlag = 0;
		}
	}

	while(!failSafe) {
		if(updateDC)
			failSafe = 1;
	}


	return 0;
}

__interrupt void ecap1_isr(void)
{
	updateDC = 1;
	long min = (long)calibMin[muxState];
	long max = (long)calibScale[muxState]+min;
	long tooMuch = max + max/10;
	long tooLittle = min-min/10;

	unsigned long duty = ECap1Regs.CAP2/10; // Calculate On time


	if ((duty > tooMuch) || (duty < tooLittle)) {
		//ignore or fail to neutral?
		outputs[muxState] = max/2; //fail to neutral
		errors++;
	}
	else if(duty < min+min/20){
		outputs[muxState] = min+min/20;
	}
	else if(duty > max-max/20) {
		outputs[muxState] = max-max/20;
	}
	else {
		outputs[muxState] = duty;
	}
	muxState++;
	if (muxState >= 3)
		muxState = 0;

	if ((ECap1Regs.CAP1 > longestTime) && (longestTime < tooLong))
		longestTime = ECap1Regs.CAP1;

	ECap1Regs.ECCLR.bit.CEVT1 = 1;      //clears the CEVT1 flag condition
	ECap1Regs.ECCLR.bit.CEVT2 = 1;      //clears the CEVT2 flag condition
	ECap1Regs.ECCLR.bit.CEVT3 = 1;      //clears the CEVT3 flag condition
	ECap1Regs.ECCLR.bit.CEVT4 = 1;      //clears the CEVT4 flag condition
	ECap1Regs.ECCLR.bit.INT = 1;        //clears the INT flag and enable further interrupts to be generated ---global
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;  // Acknowledge this interrupt
}



void epwmChangeDC(int*dc) {
	// 0 = 0%; 4500 = 100%

	EPwm1Regs.CMPA.half.CMPA = dc[0];

	EPwm2Regs.CMPA.half.CMPA = dc[1];

	EPwm3Regs.CMPA.half.CMPA = dc[2];

	return;
}



