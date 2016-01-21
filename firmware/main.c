/*
 * main.c
 */

#include "DSP28x_Project.h"
#include "init.h"
#include "flags.h"
#include <stdio.h>
#include "libSCI.h"
void epwmChangeDC(int*dc);

int muxState;
long outputs[3];
int errors;
long longestTime;
long tooLong;
float calibMin[3];
float calibScale[3];
Uint16 sciData;

flags_obj flags = {0};

int main(void) {

	memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize); //only needed for flash
	boardSetup();
	errors = 0;
	sciData = 0;
	outputs[0] = 0;
	outputs[1] = 0;
	outputs[2] = 0;
	muxState = 0;
	flags.dutyCycle = 0;
	flags.writeDutyCycle = 0;
	flags.failsafe = 1;
	flags.controllerPresent = 0;
	flags.calibrate = 0;

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

	while (flags.failsafe) {
		if(flags.dutyCycle) { //update PWM outputs:
			//convert outputs[n] to duty cycle values
			dc[0] = (int)((outputs[0]-calibMin[0])*((float)PWM_FREQ)/(calibScale[0]));
			dc[1] = (int)((outputs[1]-calibMin[1])*((float)PWM_FREQ)/(calibScale[1]));
			dc[2] = (int)((outputs[2]-calibMin[2])*((float)PWM_FREQ)/(calibScale[2]));
			epwmChangeDC(dc);
			flags.dutyCycle = 0;
			flags.dutyCycleChanged = 1;
		}
		else if (((dc[0] == 0)&&(dc[1] == 0)&&(dc[2] == 0)) || !(flags.controllerPresent)) {
			flags.failsafe = 0;
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
				flags.writeDutyCycle = !flags.writeDutyCycle;
			break;

			case (int)'c':
				scia_ln(clearScreen);
				scia_ln(title);
				scia_ln(wrongcPressed);
				break;

			case (int)'C':
				flags.calibrate = 1;
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
		if(flags.writeDutyCycle && flags.dutyCycleChanged) {
			scia_ln(clearScreen);
			scia_ln(title);
			scia_ln(ctrlLevels);
			sprintf(printBuffer, "Left Motor:  %d\r\n",dc[0]);
			scia_ln(printBuffer);
			sprintf(printBuffer, "Right Motor: %d\r\n",dc[1]);
			scia_ln(printBuffer);
			sprintf(printBuffer, "Spin Motors: %d\r\n",dc[2]);
			scia_ln(printBuffer);
			flags.dutyCycleChanged = 0;
		}

		if(flags.calibrate) {
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
			flags.calibrate= 0;
		}
	}

	while(!flags.failsafe) {
		if(flags.dutyCycle)
			flags.failsafe = 1;
	}


	return 0;
}

__interrupt void ecap1_isr(void)
{
	flags.dutyCycle = 1;
	long min = (long)calibMin[muxState];
	long max = (long)calibScale[muxState]+min;
	long tooMuch   = max + (max/10);
	long tooLittle = min - (min/10);

	unsigned long duty = ECap1Regs.CAP2/10; // Calculate On time


	if ((duty > tooMuch) || (duty < tooLittle)) {
		//TODO: ignore this cycle or fail to neutral?
		outputs[muxState] = max/2; //fail to neutral for now. maybe make this configurable.
		errors++;
	}
	else if(duty < min+min/20){
		outputs[muxState] = min+min/20; //if we're within 5%
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



