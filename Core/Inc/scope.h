/*
 * scope.h
 *
 *  Created on: Aug 20, 2024
 *      Author: Aman
 */

#ifndef INC_SCOPE_H_
#define INC_SCOPE_H_

void stopScope(TIM_HandleTypeDef*);
void startScope(TIM_HandleTypeDef*, int, int);
float findVPP(uint16_t*, int);
void mapScreen(uint16_t*, int, int, uint16_t*, int);
int findTriggerRising(uint16_t*, uint16_t*);


#endif /* INC_SCOPE_H_ */
