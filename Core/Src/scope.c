/*
 * scope.c
 *
 *  Created on: Aug 20, 2024
 *  Author: Aman
 *
 *  This file contains functions which process the input waveform
 *
 */

#include "stm32f4xx_hal.h"

#include <stdint.h>

#include <stdio.h>

#include <string.h>


void stopScope(TIM_HandleTypeDef * timer) {
    HAL_TIM_Base_Stop(timer);
}

void startScope(TIM_HandleTypeDef * timer, int PSC, int ARR) {
    __HAL_TIM_SET_PRESCALER(timer, PSC);
    __HAL_TIM_SET_AUTORELOAD(timer, ARR);
    HAL_TIM_Base_Start(timer);
}

float findVPP(uint16_t *values, int afe_toggle) {
    int largest = 0;
    int smallest = 10000;
    float VPP = 0.0;

    for (int i = 0; i < 312; i++) {
        if (values[i] > largest) {
            largest = values[i];
        }

        if (values[i] < smallest) {
            smallest = values[i];
        }
    }

    //adjust for afe if toggled
    if(afe_toggle){
    	largest = (largest-2047.5)*2;
    	smallest = (smallest-2047.5)*2;
    }

    VPP = (largest-smallest)/(4095/3.3) ;  // Assuming the scaling factor is correct
    return VPP;
}

void mapScreen(uint16_t * array, int y_shift, int y_div_option, uint16_t * map_vals, int afe_toggle) {

    float scale = 0.0;
    int ADC_size = 4095;

    //adjusts for the analog front end via the afe formula: vout = (vin/2) + 3.3/2, and increases the adc value size to fit graph
    //additionally, shift by an additional 3.3v to stay within uint_16 bounds (no negatives)
    //this additional shift is rectified when graph is drawn
    //only activate if afe is toggled
    if(afe_toggle == 1){
    	for(int i=0;i<312;i++){
    	    	array[i] = 4095+ ((array[i]-2047.5)*2);
    	    	ADC_size = 8190;
    	}
    }else{
    	ADC_size = 4095;
    }


    //selects which y div factor to use
    if (y_div_option == 0) {
        scale = 1.0;
    } else if (y_div_option == 1) {
        scale = 1.65;
    } else if (y_div_option == 2) {
        scale = 3.3;
    } else if (y_div_option == 3) {
        scale = 8.25;
    } else if (y_div_option == 4) {
        scale = 16.5;
    }

    for (int i = 0; i < 312; i++) {


    	//center y shift around 10000 to avoid overflows below 0
        if (y_shift > 10000 && y_shift < 21000) {
            y_shift = (21000 - y_shift) * -1;
        }

        map_vals[i] = array[i];

        //scale and shift
        int val = array[i] - (y_shift * 15);
        val = val * scale;

        //convert to display coordinates
        val = 189 - (val / (ADC_size / 158));

        //clamp pixels at top and bottom of display
        if (val < 31) {
            array[i] = (uint16_t) 31;
        } else if (val > 189) {
            array[i] = (uint16_t) 189;
        } else {
            array[i] = (uint16_t) val;
        }
    }
}

int findTriggerRising(uint16_t * DMA_array, uint16_t * trigger_array) {
    int shift_index = 9999;

    for (int i = 0; i < 1000; i++) {
        if ((DMA_array[160] > 1000 / 1.60 && DMA_array[160] < 1000 * 1.60) && (DMA_array[160] > DMA_array[160 - 3])) {
            shift_index = i;
            break;
        }
    }

    if (shift_index == 9999) {
        return 1;
    } else {
        for (int i = 0; i < 312; i++) {
            trigger_array[i] = DMA_array[i + shift_index];
        }
        return 0;
    }

}
