/*
 * init.h
 *
 *  Created on: Nov 20, 2015
 *      Author: Jack
 */

#ifndef INIT_H_
#define INIT_H_

void boardSetup();
void epwmInit(int, int);
void InitECapture1();

#define PWM_FREQ 4500
#define DUTY_50 PWM_FREQ/2


#endif /* INIT_H_ */
