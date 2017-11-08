#ifndef __KEYPAD_H
#define __KEYPAD_H
	 
void KEY_GPIO_Init(void);
char read_keypad(void);
void keypadFSM(void);
int * getAngles();

#endif