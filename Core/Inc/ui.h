/*
 * ui.h
 *
 *  Created on: Aug 20, 2024
 *      Author: Aman
 */

#ifndef INC_UI_H_
#define INC_UI_H_

void drawWaveform(uint16_t * , uint16_t * , uint16_t);
void scopeMenu(void);
void drawGrid(void);
void drawOscButtons(uint8_t, uint16_t * , int, int, int);
void drawUsbMode(int);
void drawMenu1(void);
void drawMenu2(int);
void drawMenu3(int);
void drawMenu4(int);
void drawMenu5(int);
uint16_t drawMenu6(int);
void clearMenuFrame(void);
void clearFullFrame(void);
void drawTriggerLevel(int, int);
void drawTriggerIcon(int);

void drawCursors(int, int, int, int, int, int, int, uint16_t * , uint16_t *, int);
void drawDiv(int, int * , int * , int);

#endif /* INC_UI_H_ */
