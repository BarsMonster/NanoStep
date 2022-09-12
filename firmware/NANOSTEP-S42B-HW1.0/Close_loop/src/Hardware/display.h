#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "main.h"
#include "menu.h"

//in 100ms intervals, defaults to 60sec
#define SCREENSAVER_DELAY 600
//#define DEBUG_CPU_LOAD


extern struct Menu menuMain;
extern uint32_t idle_cycles;

// Create the menu
void BuildMenu(void);

void Motor_data_dis(void);

void ShowStartupScreen();

void ShowInfoScreen();

void ShowCalibrateScreen();

void ShowCalibrateOKScreen();

void ShowCalibrateCompleteScreen();

void ShowBootloaderScreen();

void ExitMenu();

void wakeup();


#endif

