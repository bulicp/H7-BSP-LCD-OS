/*
 * OBD2_lcd.c
 *
 *  Created on: 6 Jan 2023
 *      Author: patriciobulic
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"


extern uint32_t x_size;
extern uint32_t y_size;


void Display_InitialContent_CAN_Layer1(void)
{

  BSP_LCD_SetActiveLayer(0, LTDC_LAYER_1);
  UTIL_LCD_SetLayer(LTDC_LAYER_1);
  BSP_LCD_SetLayerVisible(0, LTDC_LAYER_1, ENABLE);
  BSP_LCD_SetLayerVisible(0, LTDC_LAYER_2, DISABLE);
  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  UTIL_LCD_SetFont(&Font16);

  /* Clear the LCD */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
  BSP_LCD_FillRect(0, 0, 0, x_size, y_size, UTIL_LCD_COLOR_BLACK);

  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);

  /* Display LCD messages */

  UTIL_LCD_SetFont(&Font20);
  UTIL_LCD_DisplayStringAt(0, 10, (uint8_t *)"OBD-II CAN Analyzer", CENTER_MODE);
  //UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"Vgrajeni sistemi", CENTER_MODE);
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_DisplayStringAt(0, 35, (uint8_t *)"Pa3cio UL FRI 2022", CENTER_MODE);

  /* Draw Bitmap */
  //UTIL_LCD_DrawBitmap((x_size - 80)/2, 65, (uint8_t *)stlogo);

  //UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
  //UTIL_LCD_SetFont(&Font16);
  //UTIL_LCD_DisplayStringAt(0, y_size - 20, (uint8_t *)"Copyright (c) Pa3cio 2022", CENTER_MODE);

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ORANGE);


  BSP_LCD_FillRect(0, 0, YROW1, x_size/3-2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, x_size/3 + 2, YROW1, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, 2*x_size/3 + 4,YROW1, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);


  UTIL_LCD_DisplayStringAt(43, YROW1+2, (uint8_t *)"SPEED", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(220, YROW1+2, (uint8_t *)"RPM", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(340, YROW1+2, (uint8_t *)"ENGINE LOAD", LEFT_MODE);



  BSP_LCD_FillRect(0, 0, YROW2, x_size/3-2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, x_size/3 + 2, YROW2, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, 2*x_size/3 + 4,YROW2, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(17, YROW2+2, (uint8_t *)"COOLANT TEMP", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(190, YROW2+2, (uint8_t *)"INTAKE AIR", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(345, YROW2+2, (uint8_t *)"FUEL STATUS", LEFT_MODE);



  BSP_LCD_FillRect(0, 0, YROW3, x_size/3-2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, x_size/3 + 2, YROW3, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, 2*x_size/3 + 4,YROW3, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);

  UTIL_LCD_DisplayStringAt(30, YROW3+2, (uint8_t *)"OXY SEN1", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(195, YROW3+2, (uint8_t *)"OXY SEN2", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(335, YROW3+2, (uint8_t *)"THROTTLE POS", LEFT_MODE);


  BSP_LCD_FillRect(0, 0, YROW4, x_size/3-2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, x_size/3 + 2, YROW4, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, 2*x_size/3 + 4, YROW4, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);

  UTIL_LCD_DisplayStringAt(30, YROW4+2, (uint8_t *)"MAF RATE", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(195, YROW4+2, (uint8_t *)"RUN TIME", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(345, YROW4+2, (uint8_t *)"MANIFOLD P", LEFT_MODE);

  //UTIL_LCD_DrawBitmap(0, 0, (uint8_t *)dash);

}


void Display_InitialContent_CAN_Layer2(void)
{
  BSP_LCD_SetActiveLayer(0, LTDC_LAYER_2);
  UTIL_LCD_SetLayer(LTDC_LAYER_2);
  BSP_LCD_SetLayerVisible(0, LTDC_LAYER_2, ENABLE);
  BSP_LCD_SetLayerVisible(0, LTDC_LAYER_1, DISABLE);
  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  UTIL_LCD_SetFont(&Font16);

  /* Clear the LCD */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
  BSP_LCD_FillRect(0, 0, 0, x_size, y_size, UTIL_LCD_COLOR_BLACK);

  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);

  /* Display LCD messages */

  UTIL_LCD_SetFont(&Font20);
  UTIL_LCD_DisplayStringAt(0, 10, (uint8_t *)"OBD-II CAN Analyzer", CENTER_MODE);
  //UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"Vgrajeni sistemi", CENTER_MODE);
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_DisplayStringAt(0, 35, (uint8_t *)"Pa3cio UL FRI 2022", CENTER_MODE);

  /* Draw Bitmap */
  //UTIL_LCD_DrawBitmap((x_size - 80)/2, 65, (uint8_t *)stlogo);

  //UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
  //UTIL_LCD_SetFont(&Font16);
  //UTIL_LCD_DisplayStringAt(0, y_size - 20, (uint8_t *)"Copyright (c) Pa3cio 2022", CENTER_MODE);

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_YELLOW );


  BSP_LCD_FillRect(0, 0, YROW1, x_size/3-2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, x_size/3 + 2, YROW1, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, 2*x_size/3 + 4,YROW1, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);


  UTIL_LCD_DisplayStringAt(10, YROW1+2, (uint8_t *)"FUEL PRESSURE", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(185, YROW1+2, (uint8_t *)"DISTANCE CC", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(345, YROW1+2, (uint8_t *)"ABS BARO P", LEFT_MODE);



  BSP_LCD_FillRect(0, 0, YROW2, x_size/3-2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, x_size/3 + 2, YROW2, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, 2*x_size/3 + 4,YROW2, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(10, YROW2+2, (uint8_t *)"CATALYST TEMP", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(185, YROW2+2, (uint8_t *)"CTRL MODULE", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(335, YROW2+2, (uint8_t *)"AMBIENT AIR", LEFT_MODE);



  BSP_LCD_FillRect(0, 0, YROW3, x_size/3-2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, x_size/3 + 2, YROW3, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, 2*x_size/3 + 4,YROW3, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);

  UTIL_LCD_DisplayStringAt(25, YROW3+2, (uint8_t *)"ACC PEDAL D", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(185, YROW3+2, (uint8_t *)"ACC PEDAL E", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(335, YROW3+2, (uint8_t *)"FUEL INJ TIM", LEFT_MODE);


  BSP_LCD_FillRect(0, 0, YROW4, x_size/3-2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, x_size/3 + 2, YROW4, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, 2*x_size/3 + 4, YROW4, x_size/3 - 2, YSIZE, UTIL_LCD_COLOR_BLUE);

  UTIL_LCD_DisplayStringAt(10, YROW4+2, (uint8_t *)"THROTTLE POS", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(190, YROW4+2, (uint8_t *)"EGR ERROR", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(375, YROW4+2, (uint8_t *)"EGR", LEFT_MODE);

}


void DrawSpeedLine(uint32_t speed, uint32_t Color){
	float rad;
	uint32_t x, y;

	if ((speed >= 0) && (speed <= 60)) {
		rad = (speed) * (2.0*PI/240.0);
		y = 136 + (uint32_t) (99.0*cos(rad));
		x = 120 - (uint32_t) (99.0*sin(rad));

	}

	else if ((speed > 60) && (speed <= 120)) {
		speed = speed - 60;
		rad = (speed) * (2.0*PI/240.0);
		y = 136 - (uint32_t) (99.0*sin(rad));
		x = 120 - (uint32_t) (99.0*cos(rad));

	}
	else if ((speed > 120) && (speed <= 180)) {
		speed = speed - 120;
		rad = (speed) * (2.0*PI/240.0);
		y = 136 - (uint32_t) (99.0*cos(rad));
		x = 120 + (uint32_t) (99.0*sin(rad));
	}
	else {
		speed = speed - 180;
		rad = (speed) * (2.0*PI/240.0);
		y = 136 + (uint32_t) (99.0*sin(rad));
		x = 120 + (uint32_t) (99.0*cos(rad));
	}

	UTIL_LCD_DrawLine(120, 136, x, y, Color);
}

void DrawRPMLine(uint32_t rpms, uint32_t Color){
	float rad;
	uint32_t x, y;

	if ((rpms >= 0) && (rpms <= 1500)) {
		rad = (rpms) * (2.0*PI/6000.0);
		y = 136 + (uint32_t) (99.0*cos(rad));
		x = 363 - (uint32_t) (99.0*sin(rad));

	}

	else if ((rpms > 1500) && (rpms <= 3000)) {
		rpms = rpms - 1500;
		rad = (rpms) * (2.0*PI/6000.0);
		y = 136 - (uint32_t) (99.0*sin(rad));
		x = 363 - (uint32_t) (99.0*cos(rad));

	}
	else if ((rpms > 3000) && (rpms <= 4500)) {
		rpms = rpms - 3000;
		rad = (rpms) * (2.0*PI/6000.0);
		y = 136 - (uint32_t) (99.0*cos(rad));
		x = 363 + (uint32_t) (99.0*sin(rad));
	}
	else {
		rpms = rpms - 4500;
		rad = (rpms) * (2.0*PI/6000.0);
		y = 136 + (uint32_t) (99.0*sin(rad));
		x = 363 + (uint32_t) (99.0*cos(rad));
	}

	UTIL_LCD_DrawLine(363, 136, x, y, Color);
}

