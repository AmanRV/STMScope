/*
 * ui.c
 *
 *  Created on: Aug 20, 2024
 *  Author: Aman
 *
 *  This file contains all of the functions that draw to the LCD display
 *
 */

#include "lcd.h"

#include "scope.h"

#include <stdint.h>

#include <stdio.h>

#include <string.h>

#include "ui.h"

static const uint16_t rising_bmp[110] = {
    // ∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙██∙∙∙∙
    // ∙∙∙∙∙█∙∙∙∙∙
    // ∙∙∙∙∙█∙∙∙∙∙
    // ∙∙∙∙███∙∙∙∙
    // ∙∙∙█∙█∙█∙∙∙
    // ∙∙█∙∙█∙∙█∙∙
    // ∙∙∙∙∙█∙∙∙∙∙
    // ∙∙∙███∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

static const uint16_t falling_bmp[110] = {
    // ∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙██∙∙∙∙
    // ∙∙∙∙∙█∙∙∙∙∙
    // ∙∙█∙∙█∙∙█∙∙
    // ∙∙∙█∙█∙█∙∙∙
    // ∙∙∙∙███∙∙∙∙
    // ∙∙∙∙∙█∙∙∙∙∙
    // ∙∙∙∙∙█∙∙∙∙∙
    // ∙∙∙███∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

static const uint16_t none_bmp[110] = {
    // ∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙█∙∙∙∙∙█∙∙
    // ∙∙∙█∙∙∙█∙∙∙
    // ∙∙∙∙█∙█∙∙∙∙
    // ∙∙∙∙∙█∙∙∙∙∙
    // ∙∙∙∙█∙█∙∙∙∙
    // ∙∙∙█∙∙∙█∙∙∙
    // ∙∙█∙∙∙∙∙█∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

UG_BMP rising = {
    rising_bmp,
    11, // Width
    10, // Height
    BMP_BPP_16,
    BMP_RGB565
};

UG_BMP falling = {
    falling_bmp,
    11, // Width
    10, // Height
    BMP_BPP_16,
    BMP_RGB565
};

UG_BMP none = {
    none_bmp,
    11, // Width
    10, // Height
    BMP_BPP_16,
    BMP_RGB565
};

void drawWaveform(uint16_t * values, uint16_t * old_values, uint16_t wave_color) {

	//erases any old waveform pixels
    int j = 0;
    for (int i = 0; i < 311; i++) {
        if (!((old_values[i] == values[i]) && (old_values[i + 1] == values[i + 1]))) {
            UG_DrawLine(j + 3, old_values[i], j + 4, old_values[i + 1], C_BLACK);
        }

        j++;
    }

    //draws new waveform
    int x = 0;
    for (int i = 0; i < 311; i++) {
        UG_DrawLine(x + 3, values[i], x + 4, values[i + 1], wave_color);
        //LCD_DrawPixel(x+3, values[i], wave_color);

        x++;
    }
}

void scopeMenu() {

    //title
    LCD_PutStr(7, 9, "STMScope", FONT_5X12, C_WHITE, C_BLACK);

    //left border
    LCD_DrawLine(0, 0, 0, 240, C_WHITE);
    LCD_DrawLine(1, 0, 1, 240, C_WHITE);
    LCD_DrawLine(2, 0, 2, 240, C_WHITE);

    //right border
    LCD_DrawLine(319, 0, 319, 240, C_WHITE);
    LCD_DrawLine(318, 0, 318, 240, C_WHITE);
    LCD_DrawLine(317, 0, 317, 240, C_WHITE);

    //top border
    LCD_DrawLine(0, 0, 319, 0, C_WHITE);
    LCD_DrawLine(0, 1, 319, 1, C_WHITE);
    LCD_DrawLine(0, 2, 319, 2, C_WHITE);

    //bottom border
    LCD_DrawLine(0, 239, 319, 239, C_WHITE);
    LCD_DrawLine(0, 238, 319, 238, C_WHITE);
    LCD_DrawLine(0, 237, 319, 237, C_WHITE);

    //top divider
    LCD_DrawLine(3, 28, 316, 28, C_WHITE);
    LCD_DrawLine(3, 29, 316, 29, C_WHITE);
    LCD_DrawLine(3, 30, 316, 30, C_WHITE);

    //bottom divider
    LCD_DrawLine(3, 190, 316, 190, C_WHITE);
    LCD_DrawLine(3, 191, 316, 191, C_WHITE);
    LCD_DrawLine(3, 192, 316, 192, C_WHITE);
}

void drawGrid() {
    //vertical graph lines
    for (int i = 0; i < 7; i++) {
        LCD_DrawLine(40 + (40 * i), 31, 40 + (40 * i), 189, C_WHITE_39);

    }

    //horizontal graph lines
    for (int i = 0; i < 4; i++) {
        LCD_DrawLine(3, 50 + (i * 40), 316, 50 + (i * 40), C_WHITE_39);

    }
}

void drawOscButtons(uint8_t selected_option, uint16_t * values, int cursor_x, int cursor_y, int afe_toggle) {

	//finds peak to peak
    float VPP = findVPP(values, afe_toggle);
    char VPP_str[12];
    sprintf(VPP_str, "VPP: %.2f V", VPP);
    printf("%s\n", VPP_str);

    //mode button
    if (selected_option == 0) {
        UG_DrawFrame(11, 200, 39, 228, C_BLUE);
        UG_FillFrame(12, 201, 38, 227, C_DARK_BLUE);
        LCD_PutStr(18, 201 + 7, "USB", FONT_5X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_DrawFrame(11, 200, 39, 228, C_WHITE);
        UG_FillFrame(12, 201, 38, 227, C_BLACK);
        LCD_PutStr(18, 201 + 7, "USB", FONT_5X12, C_WHITE, C_BLACK);
    }

    //auto set button
    if (selected_option == 1) {
        UG_DrawFrame(43, 200, 71, 228, C_BLUE);
        UG_FillFrame(44, 201, 70, 227, C_DARK_BLUE);
        LCD_PutStr(50, 208, "TRG", FONT_5X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_DrawFrame(43, 200, 71, 228, C_WHITE);
        UG_FillFrame(44, 201, 70, 227, C_BLACK);
        LCD_PutStr(50, 201 + 7, "TRG", FONT_5X12, C_WHITE, C_BLACK);
    }

    //scale button
    if (selected_option == 2) {
        UG_DrawFrame(75, 200, 103, 228, C_BLUE);
        UG_FillFrame(76, 201, 102, 227, C_DARK_BLUE);
        LCD_PutStr(82, 201 + 7, "SCA", FONT_5X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_DrawFrame(75, 200, 103, 228, C_WHITE);
        UG_FillFrame(76, 201, 102, 227, C_BLACK);
        LCD_PutStr(82, 201 + 7, "SCA", FONT_5X12, C_WHITE, C_BLACK);
    }

    //display options button
    if (selected_option == 3) {
        UG_DrawFrame(107, 200, 135, 228, C_BLUE);
        UG_FillFrame(108, 201, 134, 227, C_DARK_BLUE);
        LCD_PutStr(114, 201 + 7, "OPT", FONT_5X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_DrawFrame(107, 200, 135, 228, C_WHITE);
        UG_FillFrame(108, 201, 134, 227, C_BLACK);
        LCD_PutStr(114, 201 + 7, "OPT", FONT_5X12, C_WHITE, C_BLACK);
    }

    //measurements + x/y cursor button
    if (selected_option == 4) {
        UG_DrawFrame(144, 199, 308, 230, C_BLUE);
        //LCD_PutStr(147,202, "X:0.00V", FONT_8X12, C_WHITE, C_DARK_BLUE);
        //LCD_PutStr(147,216, "Y:0.00MS", FONT_8X12, C_WHITE, C_DARK_BLUE);
        LCD_PutStr(250, 202, VPP_str, FONT_5X12, C_WHITE, C_BLACK);
        //LCD_PutStr(213,216, "FREQ:10 KHZ", FONT_8X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_DrawFrame(144, 199, 308, 230, C_WHITE);
        //LCD_PutStr(147,202, "X:0.00V", FONT_8X12, C_WHITE, C_BLACK);
        //LCD_PutStr(147,216, "Y:0.00MS", FONT_8X12, C_WHITE, C_BLACK);
        LCD_PutStr(250, 202, VPP_str, FONT_5X12, C_WHITE, C_BLACK);
        //LCD_PutStr(213,216, "FRQ:10 KHZ", FONT_8X12, C_WHITE, C_BLACK);
    }

}

//draws usb mode text
void drawUsbMode(int usb_mode) {
    if (usb_mode) {
        LCD_PutStr(245, 216, " USB Mode ON", FONT_5X12, C_WHITE, C_BLACK);
    } else {
        LCD_PutStr(245, 216, "USB Mode OFF", FONT_5X12, C_WHITE, C_BLACK);
    }

}

//USB mode button
void drawMenu1() {
    UG_DrawFrame(33, 62, 286, 153, C_WHITE);
    UG_DrawFrame(34, 63, 285, 152, C_WHITE);
    UG_DrawFrame(35, 64, 284, 151, C_WHITE);
    UG_DrawFrame(36, 65, 283, 150, C_WHITE);

    UG_DrawLine(37, 85, 282, 85, C_WHITE);
    UG_DrawLine(37, 86, 282, 86, C_WHITE);
    UG_DrawLine(37, 87, 282, 87, C_WHITE);
    UG_DrawLine(37, 88, 282, 88, C_WHITE);
    LCD_PutStr(40, 69, "Select the mode of operation", FONT_8X12, C_WHITE, C_BLACK);
}

//triggers
void drawMenu2(int selected_option) {
    UG_DrawFrame(33, 62, 286, 153, C_WHITE);
    UG_DrawFrame(34, 63, 285, 152, C_WHITE);
    UG_DrawFrame(35, 64, 284, 151, C_WHITE);
    UG_DrawFrame(36, 65, 283, 150, C_WHITE);

    UG_DrawLine(37, 85, 282, 85, C_WHITE);
    UG_DrawLine(37, 86, 282, 86, C_WHITE);
    UG_DrawLine(37, 87, 282, 87, C_WHITE);
    UG_DrawLine(37, 88, 282, 88, C_WHITE);
    LCD_PutStr(40, 69, "Trigger options", FONT_8X12, C_WHITE, C_BLACK);

    UG_DrawLine(37, 110, 282, 110, C_WHITE);
    UG_DrawLine(37, 110 + 22, 282, 110 + 22, C_WHITE);

    if (selected_option == 0) {
        UG_FillFrame(37, 88, 282, 109, C_DARK_BLUE);
        LCD_PutStr(40, 93, "Modify trigger level", FONT_8X12, C_WHITE, C_DARK_BLUE);

    } else {
        UG_FillFrame(37, 88, 282, 109, C_BLACK);
        LCD_PutStr(40, 93, "Modify trigger level", FONT_8X12, C_WHITE, C_BLACK);
    }

    if (selected_option == 1) {
        UG_FillFrame(37, 111, 282, 131, C_DARK_BLUE);
        LCD_PutStr(40, 115, "Toggle rising edge", FONT_8X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_FillFrame(37, 111, 282, 131, C_BLACK);
        LCD_PutStr(40, 115, "Toggle rising edge", FONT_8X12, C_WHITE, C_BLACK);
    }

    if (selected_option == 2) {
        UG_FillFrame(37, 133, 282, 149, C_DARK_BLUE);
        LCD_PutStr(40, 135, "Toggle falling edge", FONT_8X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_FillFrame(37, 133, 282, 149, C_BLACK);
        LCD_PutStr(40, 135, "Toggle falling edge", FONT_8X12, C_WHITE, C_BLACK);
    }
}

//scale and shifts
void drawMenu3(int selected_option) {
    UG_DrawFrame(33, 62, 286, 153, C_WHITE);
    UG_DrawFrame(34, 63, 285, 152, C_WHITE);
    UG_DrawFrame(35, 64, 284, 151, C_WHITE);
    UG_DrawFrame(36, 65, 283, 150, C_WHITE);

    UG_DrawLine(37, 85, 282, 85, C_WHITE);
    UG_DrawLine(37, 86, 282, 86, C_WHITE);
    UG_DrawLine(37, 87, 282, 87, C_WHITE);
    UG_DrawLine(37, 88, 282, 88, C_WHITE);
    LCD_PutStr(40, 69, "Scale and shift X/Y axis", FONT_8X12, C_WHITE, C_BLACK);

    UG_DrawLine(37, 110, 282, 110, C_WHITE);
    UG_DrawLine(37, 110 + 22, 282, 110 + 22, C_WHITE);

    if (selected_option == 0) {
        UG_FillFrame(37, 88, 282, 109, C_DARK_BLUE);
        LCD_PutStr(40, 93, "Shift X/Y axis", FONT_8X12, C_WHITE, C_DARK_BLUE);

    } else {
        UG_FillFrame(37, 88, 282, 109, C_BLACK);
        LCD_PutStr(40, 93, "Shift X/Y axis", FONT_8X12, C_WHITE, C_BLACK);
    }

    if (selected_option == 1) {
        UG_FillFrame(37, 111, 282, 131, C_DARK_BLUE);
        LCD_PutStr(40, 115, "Scale X axis", FONT_8X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_FillFrame(37, 111, 282, 131, C_BLACK);
        LCD_PutStr(40, 115, "Scale X axis", FONT_8X12, C_WHITE, C_BLACK);
    }

    if (selected_option == 2) {
        UG_FillFrame(37, 133, 282, 149, C_DARK_BLUE);
        LCD_PutStr(40, 135, "Scale Y axis", FONT_8X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_FillFrame(37, 133, 282, 149, C_BLACK);
        LCD_PutStr(40, 135, "Scale Y axis", FONT_8X12, C_WHITE, C_BLACK);
    }
}

//options
void drawMenu4(int selected_option) {
    UG_DrawFrame(33, 62, 286, 153, C_WHITE);
    UG_DrawFrame(34, 63, 285, 152, C_WHITE);
    UG_DrawFrame(35, 64, 284, 151, C_WHITE);
    UG_DrawFrame(36, 65, 283, 150, C_WHITE);

    UG_DrawLine(37, 85, 282, 85, C_WHITE);
    UG_DrawLine(37, 86, 282, 86, C_WHITE);
    UG_DrawLine(37, 87, 282, 87, C_WHITE);
    UG_DrawLine(37, 88, 282, 88, C_WHITE);
    LCD_PutStr(40, 69, "Options", FONT_8X12, C_WHITE, C_BLACK);

    UG_DrawLine(37, 110, 282, 110, C_WHITE);
    UG_DrawLine(37, 110 + 22, 282, 110 + 22, C_WHITE);

    if (selected_option == 0) {
        UG_FillFrame(37, 88, 282, 109, C_DARK_BLUE);
        LCD_PutStr(40, 93, "Toggle grid lines", FONT_8X12, C_WHITE, C_DARK_BLUE);

    } else {
        UG_FillFrame(37, 88, 282, 109, C_BLACK);
        LCD_PutStr(40, 93, "Toggle grid lines", FONT_8X12, C_WHITE, C_BLACK);
    }

    if (selected_option == 1) {
        UG_FillFrame(37, 111, 282, 131, C_DARK_BLUE);
        LCD_PutStr(40, 115, "Toggle rolling mode", FONT_8X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_FillFrame(37, 111, 282, 131, C_BLACK);
        LCD_PutStr(40, 115, "Toggle rolling mode", FONT_8X12, C_WHITE, C_BLACK);
    }

    if (selected_option == 2) {
        UG_FillFrame(37, 133, 282, 149, C_DARK_BLUE);
        LCD_PutStr(40, 135, "Change waveform color", FONT_8X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_FillFrame(37, 133, 282, 149, C_BLACK);
        LCD_PutStr(40, 135, "Change waveform color", FONT_8X12, C_WHITE, C_BLACK);
    }
}

//measurements
void drawMenu5(int selected_option) {
    UG_DrawFrame(33, 62, 286, 153, C_WHITE);
    UG_DrawFrame(34, 63, 285, 152, C_WHITE);
    UG_DrawFrame(35, 64, 284, 151, C_WHITE);
    UG_DrawFrame(36, 65, 283, 150, C_WHITE);

    UG_DrawLine(37, 85, 282, 85, C_WHITE);
    UG_DrawLine(37, 86, 282, 86, C_WHITE);
    UG_DrawLine(37, 87, 282, 87, C_WHITE);
    UG_DrawLine(37, 88, 282, 88, C_WHITE);
    LCD_PutStr(40, 69, "Toggle X/Y cursors", FONT_8X12, C_WHITE, C_BLACK);

    UG_DrawLine(37, 110, 282, 110, C_WHITE);
    UG_DrawLine(37, 110 + 22, 282, 110 + 22, C_WHITE);

    if (selected_option == 0) {
        UG_FillFrame(37, 88, 282, 109, C_DARK_BLUE);
        LCD_PutStr(40, 93, "Show X and Y cursors", FONT_8X12, C_WHITE, C_DARK_BLUE);

    } else {
        UG_FillFrame(37, 88, 282, 109, C_BLACK);
        LCD_PutStr(40, 93, "Show X and Y cursors", FONT_8X12, C_WHITE, C_BLACK);
    }

    if (selected_option == 1) {
        UG_FillFrame(37, 111, 282, 131, C_DARK_BLUE);
        LCD_PutStr(40, 115, "Move X cursor", FONT_8X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_FillFrame(37, 111, 282, 131, C_BLACK);
        LCD_PutStr(40, 115, "Move X cursor", FONT_8X12, C_WHITE, C_BLACK);
    }

    if (selected_option == 2) {
        UG_FillFrame(37, 133, 282, 149, C_DARK_BLUE);
        LCD_PutStr(40, 135, "Move Y cursor", FONT_8X12, C_WHITE, C_DARK_BLUE);
    } else {
        UG_FillFrame(37, 133, 282, 149, C_BLACK);
        LCD_PutStr(40, 135, "Move Y cursor", FONT_8X12, C_WHITE, C_BLACK);
    }

}

uint16_t drawMenu6(int selected_option) {
    if (selected_option == 0) {
        LCD_PutStr(40, 135, "Change waveform color", FONT_8X12, C_BLACK, C_WHITE);
        UG_FillFrame(37, 133, 282, 149, C_WHITE);
        return 0xFFFF;
    } else if (selected_option == 1) {
        LCD_PutStr(40, 135, "Change waveform color", FONT_8X12, C_WHITE, C_RED);
        UG_FillFrame(37, 133, 282, 149, C_RED);
        return 0xF800;

    } else if (selected_option == 2) {
        LCD_PutStr(40, 135, "Change waveform color", FONT_8X12, C_WHITE, C_BLUE);
        UG_FillFrame(37, 133, 282, 149, C_BLUE);
        return 0x001F;

    } else if (selected_option == 3) {
        LCD_PutStr(40, 135, "Change waveform color", FONT_8X12, C_BLACK, C_YELLOW);
        UG_FillFrame(37, 133, 282, 149, C_YELLOW);
        return 0xFFE0;

    } else if (selected_option == 4) {
        LCD_PutStr(40, 135, "Change waveform color", FONT_8X12, C_BLACK, C_GREEN);
        UG_FillFrame(37, 133, 282, 149, C_GREEN);
        return 0x07E0;

    } else {
        return 0xFFFF;
    }

}

void clearMenuFrame() {
    UG_FillFrame(33, 62, 286, 158, C_BLACK);
}

void clearFullFrame() {
    UG_FillFrame(3, 31, 316, 189, C_BLACK);
}

void drawTriggerLevel(int current_trigger, int old_trigger_level) {
    UG_DrawLine(3, old_trigger_level, 316, old_trigger_level, C_BLACK);
    UG_DrawLine(3, current_trigger, 316, current_trigger, C_PALE_VIOLET_RED);

}

void drawTriggerIcon(int trigger_mode) {
    if (trigger_mode == 1) {
        UG_DrawBMP(106, 10, & rising);
    } else if (trigger_mode == 2) {
        UG_DrawBMP(106, 10, & falling);
    } else {
        UG_DrawBMP(106, 10, & none);
    }
}

void drawCursors(int show_cursors, int cursor_x, int cursor_y, int prev_cursor_x, int prev_cursor_y, int PSC, int ARR, uint16_t * map_vals, uint16_t * values, int afe_toggle) {
    if (show_cursors) {
        UG_DrawLine(prev_cursor_x, 31, prev_cursor_x, 189, C_BLACK);
        UG_DrawLine(3, prev_cursor_y, 316, prev_cursor_y, C_BLACK);
        UG_DrawLine(cursor_x, 31, cursor_x, 189, C_WHITE);
        UG_DrawLine(3, cursor_y, 316, cursor_y, C_WHITE);

    }

    double one_tick = ((double)(PSC + 1) * (double)(ARR + 1)) / 60000000.0; //calculates one tick of main clock in seconds
    float x_val = one_tick * (cursor_x - 3);
    char x_val_str[13] = {
        0
    }; // Initialize with zeros

    float y_val = 3.3 - ((3.3 / 158.0) * ((float)(cursor_y - 31)));
    char y_val_str[11];

    const char * units[] = {
        " ns",
        " us",
        " ms",
        " s"
    };
    int unit_index = 0;
    double value = x_val;

    //converts to nanoseconds first
    value *= 1e9;

    //converts to appropriate unit
    while (value >= 10000 && unit_index < 3) {
        value /= 1000;
        unit_index++;
    }

    //formats the string
    if (value < 1) {
        sprintf(x_val_str, "X: 0000.000%s", units[unit_index]);
    } else if (value < 10) {
        sprintf(x_val_str, "X: 000%1.3f%s", value, units[unit_index]);
    } else if (value < 100) {
        sprintf(x_val_str, "X: 00%2.3f%s", value, units[unit_index]);
    } else if (value < 1000) {
        sprintf(x_val_str, "X: 0%3.3f%s", value, units[unit_index]);
    } else {
        sprintf(x_val_str, "X: %4.3f%s", value, units[unit_index]);
    }


    //y cursor logic, checks value before and after scaling/division and maps to voltage
    int found_measurement = 0;
    for (int i = 0; i < 312; i++) {
        if (values[i] == cursor_y) {
            y_val = map_vals[i] / (4095 / 3.3);

            //remove the shift done in mapScreen if afe_toggle is on
            if(afe_toggle == 1){
            	y_val = y_val-3.3;
            }else{
            	y_val = y_val;
            }

            found_measurement = 1;
        }
    }

    sprintf(y_val_str, "Y: %04.3f V", y_val);

    LCD_PutStr(147, 216, x_val_str, FONT_5X12, C_WHITE, C_BLACK);

    if (found_measurement) {
        LCD_PutStr(147, 202, y_val_str, FONT_5X12, C_WHITE, C_BLACK);
    } else {
        LCD_PutStr(147, 202, "Y: N/A V  ", FONT_5X12, C_WHITE, C_BLACK);
    }

    if(afe_toggle == 1){
    	LCD_PutStr(6, 34, "AFE Mode ON ", FONT_5X12, C_WHITE, C_BLACK);
    }else{
    	LCD_PutStr(6, 34, "AFE Mode OFF", FONT_5X12, C_WHITE, C_BLACK);
    }



}

void drawDiv(int x_div_option, int * PSC, int * ARR, int y_div_option) {
    if (x_div_option == 0) {
        LCD_PutStr(218, 9, "3.13ms/div - 320 HZ", FONT_5X12, C_WHITE, C_BLACK); // 320 Hz -> 1/320 = 3.13 ms/div
        * PSC = 0;
        * ARR = 4814;
    } else if (x_div_option == 1) {
        LCD_PutStr(218, 9, "1.00ms/div - 1.0KHZ", FONT_5X12, C_WHITE, C_BLACK); // 1 kHz -> 1/1000 = 1.00 ms/div
        * PSC = 0;
        * ARR = 1537;
    } else if (x_div_option == 2) {
        LCD_PutStr(218, 9, " 417us/div - 2.4KHZ", FONT_5X12, C_WHITE, C_BLACK); // 2.4 kHz -> 1/2400 = 417 µs/div
        * PSC = 0;
        * ARR = 640;
    } else if (x_div_option == 3) {
        LCD_PutStr(218, 9, " 200us/div - 5.0KHZ", FONT_5X12, C_WHITE, C_BLACK); // 5 kHz -> 1/5000 = 200 µs/div
        * PSC = 0;
        * ARR = 306;
    } else if (x_div_option == 4) {
        LCD_PutStr(218, 9, " 100us/div - 10 KHZ", FONT_5X12, C_WHITE, C_BLACK); // 10 kHz -> 1/10000 = 100 µs/div
        * PSC = 0;
        * ARR = 152;
    } else if (x_div_option == 5) {
        LCD_PutStr(218, 9, "41.7us/div - 24 KHZ", FONT_5X12, C_WHITE, C_BLACK); // 24 kHz -> 1/24000 = 41.7 µs/div
        * PSC = 0;
        * ARR = 63;
    } else if (x_div_option == 6) {
        LCD_PutStr(218, 9, "20.0us/div - 50 KHZ", FONT_5X12, C_WHITE, C_BLACK); // 50 kHz -> 1/50000 = 20.0 µs/div
        * PSC = 0;
        * ARR = 29;
    } else if (x_div_option == 7) {
        LCD_PutStr(218, 9, "10.0us/div - 100KHZ", FONT_5X12, C_WHITE, C_BLACK); // 100 kHz -> 1/100000 = 10.0 µs/div
        * PSC = 0;
        * ARR = 14;
    } else if (x_div_option == 8) {
        LCD_PutStr(218, 9, "4.17us/div - 240KHZ", FONT_5X12, C_WHITE, C_BLACK); // 240 kHz -> 1/240000 = 4.17 µs/div
        * PSC = 0;
        * ARR = 5;
    } else if (x_div_option == 9) {
        LCD_PutStr(218, 9, "2.00us/div - 500KHZ", FONT_5X12, C_WHITE, C_BLACK); // 500 kHz -> 1/500000 = 2.00 µs/div
        * PSC = 0;
        * ARR = 2;
    } else if (x_div_option == 10) {
        LCD_PutStr(218, 9, "1.00us/div - 1.0MHZ", FONT_5X12, C_WHITE, C_BLACK); // 1 MHz -> 1/1000000 = 1.00 µs/div
        * PSC = 0;
        * ARR = 1;
    }

    if (y_div_option == 0) {
        LCD_PutStr(136, 9, "825.00 mV/div", FONT_5X12, C_WHITE, C_BLACK);
    } else if (y_div_option == 1) {
        LCD_PutStr(136, 9, "500.00 mV/div", FONT_5X12, C_WHITE, C_BLACK);
    } else if (y_div_option == 2) {
        LCD_PutStr(136, 9, "250.00 mV/div", FONT_5X12, C_WHITE, C_BLACK);
    } else if (y_div_option == 3) {
        LCD_PutStr(136, 9, "100.00 mV/div", FONT_5X12, C_WHITE, C_BLACK);
    } else if (y_div_option == 4) {
        LCD_PutStr(136, 9, "050.00 mV/div", FONT_5X12, C_WHITE, C_BLACK);
    }

    LCD_PutStr(207, 9, "-", FONT_5X12, C_WHITE, C_BLACK);
    LCD_PutStr(125, 9, "-", FONT_5X12, C_WHITE, C_BLACK);

}
