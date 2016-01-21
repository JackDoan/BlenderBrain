/*
 * drv.h
 *
 *  Created on: Jan 20, 2016
 *      Author: jack
 */

#ifndef FLAGS_H_
#define FLAGS_H_

typedef struct {
	bool dutyCycle;
	bool writeDutyCycle;
	bool dutyCycleChanged;
	bool failsafe;
	bool calibrate;
	bool controllerPresent;

} flags_obj;



#endif /* FLAGS_H_ */
