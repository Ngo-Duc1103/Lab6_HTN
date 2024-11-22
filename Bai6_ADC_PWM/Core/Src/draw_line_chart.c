/*
 * draw_line_chart.c
 *
 *  Created on: Nov 21, 2024
 *      Author: HELLO
 */

#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "sensor.h"
#include "buzzer.h"
#include <stdlib.h>
#include <time.h>

#define MAX_HISTORY 100
#define DATA_POINTS_SIZE 100

int data_points[DATA_POINTS_SIZE] = {};
int isRandomActive = 1;

int history[MAX_HISTORY][DATA_POINTS_SIZE] = {0};
int history_index = 0;
int current_index = 0;
int status = 0;
int current_position = DATA_POINTS_SIZE - 10; // Start showing the last 10 points

void draw_line_chart() {
    // Clear the LCD
    lcd_Fill(0, 0, 240, 320, BLACK);

    // Define chart area
    int chart_x0 = 30;   // x-origin of the chart
    int chart_y0 = 270;  // y-origin of the chart
    int chart_width = 200;  // Width of the chart area
    int chart_height = 200; // Height of the chart area

    // Draw the axes
    lcd_DrawLine(chart_x0, chart_y0, chart_x0, 0, WHITE);  // Y axis
    lcd_DrawLine(chart_x0, chart_y0, chart_x0 + chart_width, chart_y0, WHITE); // X axis


    // Draw y-axis labels
    for (int i = 0; i <= 13; i++) {
        int y = chart_y0 - i * (chart_height / 10);						// divide the axis into equally-sized pieces
        char label[4];
        sprintf(label, "%d", i * 20);
        lcd_ShowStr(chart_x0 - 30, y - 10, label, WHITE, BLACK, 16, 0);
        lcd_ShowChar(chart_x0 - 5, y - 8, '-', WHITE, BLACK, 16, 1);
    }

    // Draw x-axis labels
    for (int i = 0; i <= 10; i++) {
        int x = chart_x0 + i * (chart_width / 10);
        char label[3];
        sprintf(label, "%d", i + current_position);
        //lcd_ShowStr(x - 5, chart_y0 + 10, label, WHITE, BLACK, 16, 0); // số thứ tự
    }

    // Plot the data points within the current window
    for (int i = 0; i < 10; i++) {
        int x1 = chart_x0 + i * (chart_width / 10);											// x-coordinate of 1st point
        int y1 = chart_y0 - data_points[current_position + i] * (chart_height / 200);		// y-coordinate of 1st point
        int x2 = chart_x0 + (i + 1) * (chart_width / 10);									// x-coordinate of 2nd point
        int y2 = chart_y0 - data_points[current_position + i + 1] * (chart_height / 200);	// y-coordinate of 2nd point
        lcd_DrawLine(x1, y1, x2, y2, WHITE);

        char value_label[4];
        sprintf(value_label, "%d", data_points[current_position + i]);
        lcd_ShowStr(x1, y1 - 20, value_label, WHITE, BLACK, 16, 0);					// show value at each data
    }

    // Highlight or show the current value
/*    char current_value_label[4];
    sprintf(current_value_label, "%d", data_points[current_position + 10]);
    int x_current = chart_x0 + 10 * (chart_width / 10);
    int y_current = chart_y0 - data_points[current_position + 10] * (chart_height / 100);
    lcd_ShowStr(x_current - 10, y_current - 20, current_value_label, YELLOW, BLACK, 16, 0); // Use YELLOW to highlight the current position
*/
}

void SaveHistory() {
    for (int i = 0; i < DATA_POINTS_SIZE; i++) {
        history[history_index][i] = data_points[i];
    }
    history_index = (history_index + 1) % MAX_HISTORY;
}

void RestorePreviousHistory() {
    if (history_index == 0) {
        history_index = MAX_HISTORY - 1;
    } else {
        history_index--;
    }
    for (int i = 0; i < DATA_POINTS_SIZE; i++) {
        data_points[i] = history[history_index][i];
    }
    draw_line_chart();
}

void RestoreNextHistory() {
    history_index = (history_index + 1) % MAX_HISTORY;
    for (int i = 0; i < DATA_POINTS_SIZE; i++) {
        data_points[i] = history[history_index][i];
    }
    draw_line_chart();
}

void LineChart() {
//    if (RollBack() && !isRandomActive) {
//        if (current_position > 0) {
//            current_position--;
//            draw_line_chart();
//        }
//        return;
//    }
//
//    if (RollNext() && !isRandomActive) {
//        if (current_position < DATA_POINTS_SIZE - 10) {
//            current_position++;
//            draw_line_chart();
//        }
//        return;
//    }
//
//    if (Stop()) {
//        isRandomActive = !isRandomActive;
//        if (!isRandomActive) {
//            draw_line_chart();
//        }
//    }

//    if (isRandomActive) {
        if (status == 0) {
//            SaveHistory();
            for (int i = 0; i < DATA_POINTS_SIZE - 1; i++) {
                data_points[i] = data_points[i + 1];
            }
            sensor_Read();
            data_points[DATA_POINTS_SIZE - 1] = sensor_GetVoltage() * sensor_GetCurrent();
            current_position = DATA_POINTS_SIZE - 10; // Reset to the latest data points

            draw_line_chart();
        }
        status = (status + 1) % 300;
//    }
}
