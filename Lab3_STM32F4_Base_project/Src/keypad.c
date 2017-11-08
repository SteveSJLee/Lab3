/**
  ******************************************************************************
  * @file    keypad.c
  * @author  Steve Lee
  * @version version 1.0
  * @date    08-November-2017
  * @brief   This file provides a set of functions needed to read and manipulate 
						 inputs from 4x3 keypad.
  ******************************************************************************
	*/

#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "keypad.h"
#define dtim 500

/** Configure pins as 
     PB12   ------> row1
     PB13   ------> row2
     PB14   ------> row3
     PB15   ------> row4
     PD8   	------> col1
     PD9   	------> col2
     PD10   ------> col3
*/

GPIO_InitTypeDef GPIO_InitRow;
GPIO_InitTypeDef GPIO_InitCol;
int pitch_angle;
int roll_angle;
int angle_array[2];

/**
  * @brief  Initialize GPIO pins
  * @param  None
  * @retval None
  */
void KEY_GPIO_Init(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	// pins for rows
	GPIO_InitRow.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15; 
	GPIO_InitRow.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitRow.Pull = GPIO_NOPULL;
	GPIO_InitRow.Speed = GPIO_SPEED_HIGH;	
	HAL_GPIO_Init(GPIOB, &GPIO_InitRow);
	
	// pins for columns
	GPIO_InitCol.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10; 
	GPIO_InitCol.Mode = GPIO_MODE_INPUT;
	GPIO_InitCol.Pull = GPIO_PULLDOWN;
	GPIO_InitCol.Speed = GPIO_SPEED_HIGH;	
	HAL_GPIO_Init(GPIOD, &GPIO_InitCol);
}

/**
  * @brief  Read value from 4x3 keypad
  * @param  None
  * @retval Pressed char value
  */
char read_keypad(void)
{
	// scan row 1
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == 1) {
		return '1';
	}
	else if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9) == 1) {
		return '2';
	}
	else if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10) == 1) {
		return '3';
	}
	// scan row 2
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == 1) {
		return '4';
	}
	else if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9) == 1) {
		return '5';
	}
	else if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10) == 1) {
		return '6';
	}
	// scan row 3
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_15, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == 1) {
		return '7';
	}
	else if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9) == 1) {
		return '8';
	}
	else if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10) == 1) {
		return '9';
	}
	// scan row 4
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9) == 1) {
		return '0';
	}
	else if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == 1) {
		return '*';
	}
	else if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10) == 1) {
		return '#';
	}
	// return empty char
	return 'x';
}


/**
  * @brief  convert char to int data type
  * @param  char value to convert
  * @retval converted int value
  */
int char_to_int(char c) 
{
	int convert = c - '0';
	
	return convert;
}

/**
  * @brief  Finite state machine for the keypad
  * @param  none
  * @retval none
  */
void keypadFSM(void) 
{
	int state = 0;
	int angle = 0;
	char key;
	char stored_flag = '0';
	
	while(state != 5) 
	{
		switch(state) 
		{
			// initial state
			// press # to start
			case 0:
				key = read_keypad();
				HAL_Delay(dtim);
				printf("key: %c, state: %d\n", key, state);
				if (key == '#') 
				{
					state = 1;
				}
				break;
			case 1:
				key = read_keypad();
				HAL_Delay(dtim);
				// go to delete digit state
				printf("key: %c, state: %d, angle: %d\n", key, state, angle);
				if (key == '*') 
				{
					state = 2;
				}
				// go to enter state
				else if (key == '#') 
				{
					if (stored_flag == '0') 
					{
						state = 3;
					}
					else {
						state = 4;
					}
				}
				// keep adding until * or # is pressed
				else if (key != 'x')
				{
					angle = angle*10 + char_to_int(key);
					state = 1;
				}
				break;
			case 2: 
				// shift digit by dividing angle with 10
				// then go back to state 1
				printf("state: %d\n", state);
				angle = (int) angle/10;
				state = 1;
				break;
			case 3:
				printf("state: %d\n", state);
				// store roll angle and reset angle variable
				// go back to state 2 to get pitch angle
				stored_flag = '1';
				angle_array[0] = angle;
				angle = 0;
				state = 2;
				break;
			case 4:
				printf("state: %d\n", state);
				// store pitch angle
				// all angles are stored, go to state 5
				angle_array[1] = angle;
				state = 5;
				break;
				
			default:
				// do something
				break;
		}
	}
}

/**
  * @brief  call keypadFSM method to get angle and return  for the keypad
  * @param  none
  * @retval return int array that stores roll&pitch angle
  */
int * getAngles() {
	keypadFSM();
	return angle_array;
}
