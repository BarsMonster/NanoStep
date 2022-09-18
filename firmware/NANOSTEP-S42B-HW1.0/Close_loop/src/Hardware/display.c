#include "main.h"
#include "display.h"
#include "oled.h"
#include "string.h"
#include "stdlib.h"
#include "tle5012b.h"
#include "buttons.h"

// Declare menu and menu items (should be in global space)
//struct Menu menuMain;
struct Menu menuMain;

struct menuItem menuItemCalibrate;
struct menuItem menuItemCurrent;
struct menuItem menuItemStepSize;
struct menuItem menuItemEnablePin;
//struct menuItem menuItemDirectionPin;
struct menuItem menuItemPID_P;
struct menuItem menuItemPID_I;
struct menuItem menuItemPID_D;
struct menuItem menuItemOLEDFreq;
struct menuItem menuItemSave;
struct menuItem menuItemExit;
struct menuItem menuItemMag;

//idle counter for screensaver
uint32_t idle_cycles = 0;


// Convert to mA by multiplying by 6.5
uint16_t Converter_Current(uint16_t valueIn)
{
  uint16_t valueOut = (valueIn * 13) / 2;
  return valueOut;
}


// Convert to microstep ratio
uint16_t Converter_Stepsize(uint16_t valueIn)
{
  uint16_t valueOut = 128 / valueIn;
  return valueOut;
}

void magDisplay()
{
  menuActive = 0;
  OLED_Clear();
  //Show splashscreen
  for(int x=0;x<128;x++)OLED_DrawPoint(x,18, x&3);
  OLED_Refresh_Gram();
 
  char buffer[100];
 
  uint8_t key;
  tickCount+=101;
  while(KeyScan(&key))
  {
    tickCount+=100;
    LL_mDelay(50);
  }
  __disable_irq();
  while(!KeyScan(&key))
  {
    //1_0000_0_000010_0001
    int16_t mx = ReadValue_16bit(0x8100);//ADC - X
    int16_t my = ReadValue_16bit(0x8110);//ADC - Y
    int16_t mcx = ReadValue_16bit(0x80a0);//Center offset X - DC
    int16_t mcy = ReadValue_16bit(0x80b0);//Center offset Y - DC
    float d = powf(powf(mx,2)+powf(my,2),0.5f)/32768.0f*100.0f;

    snprintf(buffer, 100, "D:%5.1f%% MagView", (double)d);//printf cannot consume float anyways
    OLED_ShowString(0, -3, buffer);
    snprintf(buffer, 100, "X:%5.1f%%  %5.1f%%", (double)(mx/328.0f), (double)((mcx>>4)/20.48f));
    OLED_ShowString(0, 22, buffer);
    snprintf(buffer, 100, "Y:%5.1f%%  %5.1f%%", (double)(my/328.0f), (double)((mcy>>4)/20.48f));
    OLED_ShowString(0, 42, buffer);
    OLED_Refresh_Gram();
    LL_mDelay(50);
    tickCount+=50;//To enable key debouncing, maybe a little too fast - but IRQs are disabled, so we can't do timer updates. 50 = 2 tics per button press
    // Kick the dog
    LL_IWDG_ReloadCounter(IWDG);

  }
  __enable_irq();

  //Going back to regular display, menu off
  OLED_Clear();
  ShowInfoScreen();
}


// Override function for the default item value changer
void Changer_StepSize(struct Menu *menu, int16_t val)
{
  uint8_t newVal;

  if (val > 0)
  {
    newVal = *(uint8_t*)menu->items[menu->selectedItemIndex]->variable.value >> 1;
    if (newVal >= menu->items[menu->selectedItemIndex]->variable.minValue)
      *(uint8_t*)menu->items[menu->selectedItemIndex]->variable.value = newVal;
  }
  else
  {
    newVal = *(uint8_t*)menu->items[menu->selectedItemIndex]->variable.value << 1;
    if (newVal <= menu->items[menu->selectedItemIndex]->variable.maxValue)
      *(uint8_t*)menu->items[menu->selectedItemIndex]->variable.value = newVal;
  } 
}

void ShowInfoScreen()
{
  OLED_Refresh_Gram();
  //OLED_ShowString(0,2,"CPU Idle:   0000");
  char buffer[32];
  sprintf(buffer, "NanoStep v%s", fw_version);
  OLED_ShowString(8,-3, buffer);
  for(int x=0;x<128;x++)OLED_DrawPoint(x,18, x&3);
  OLED_Refresh_Gram();
  OLED_ShowString(0,22,"Err:       0.00\"");
  OLED_Refresh_Gram();
  OLED_ShowString(0,42,"Deg:       0.00\"");
  OLED_Refresh_Gram();
}

void ShowBootloaderScreen()
{
    OLED_Clear();
    OLED_ShowString(32,25,"Running");
    OLED_ShowString(16,45,"Bootloader");
}

void ExitMenu()
{
  menuActive = 0;
  OLED_Clear();
  ShowInfoScreen();
}


void BuildMenu()
{
  // Register function that will be used for drawing
  Menu_Register_Draw(OLED_ShowString);

  Menu_Register_Clear(OLED_Clear);

  Menu_Item_Init(&menuItemCalibrate);
  menuItemCalibrate.title = "Reset Settings";
  menuItemCalibrate.type = MENU_ITEM_TYPE_ACTION;
  menuItemCalibrate.action = &CalibrateEncoder;

  Menu_Item_Init(&menuItemCurrent);
  menuItemCurrent.title = "Current";
  menuItemCurrent.type = MENU_ITEM_TYPE_VARIABLE_UINT8;
  menuItemCurrent.variable.value = &Currents;
  menuItemCurrent.variable.maxValue = 300;                          // Limit max current to just below 2A
  menuItemCurrent.variable.valueConverter = &Converter_Current;

  Menu_Item_Init(&menuItemStepSize);
  menuItemStepSize.title = "Step Size";
  menuItemStepSize.type = MENU_ITEM_TYPE_VARIABLE_UINT8;                            
  menuItemStepSize.variable.value = &stepangle;  
  menuItemStepSize.variable.maxValue = 32;
  menuItemStepSize.variable.minValue = 1; 
  menuItemStepSize.variable.valueConverter = &Converter_Stepsize;
  menuItemStepSize.variable.change = &Changer_StepSize;             // Override the default value change function        

  Menu_Item_Init(&menuItemEnablePin);
  menuItemEnablePin.title = "EN Pin";                   
  menuItemEnablePin.type = MENU_ITEM_TYPE_VARIABLE_UINT8;
  menuItemEnablePin.variable.value = &Motor_ENmode_flag;  
  menuItemEnablePin.variable.maxValue = 1;

/*  Menu_Item_Init(&menuItemDirectionPin);
  menuItemDirectionPin.title = "DIR Pin";
  menuItemDirectionPin.type = MENU_ITEM_TYPE_VARIABLE_UINT8;
  menuItemDirectionPin.variable.value = &Motor_Dir;
  menuItemDirectionPin.variable.maxValue = 1;*/

  Menu_Item_Init(&menuItemMag);
  menuItemMag.title = "Mag.Field";
  menuItemMag.type = MENU_ITEM_TYPE_ACTION;
  menuItemMag.action = &magDisplay;

  Menu_Item_Init(&menuItemPID_P);
  menuItemPID_P.title = "PID P";
  menuItemPID_P.type = MENU_ITEM_TYPE_VARIABLE_UINT16;
  menuItemPID_P.variable.value = &kp; 
  
  Menu_Item_Init(&menuItemPID_I);
  menuItemPID_I.title = "PID I";
  menuItemPID_I.type = MENU_ITEM_TYPE_VARIABLE_UINT16;
  menuItemPID_I.variable.value = &ki;

  Menu_Item_Init(&menuItemPID_D);
  menuItemPID_D.title = "PID D";
  menuItemPID_D.type = MENU_ITEM_TYPE_VARIABLE_UINT16;
  menuItemPID_D.variable.value = &kd;

  Menu_Item_Init(&menuItemOLEDFreq);
  menuItemOLEDFreq.title = "OLED Freq";
  menuItemOLEDFreq.type = MENU_ITEM_TYPE_ACTION_VAR_UINT8;
  menuItemOLEDFreq.variable.value = &oledClock;
  menuItemOLEDFreq.action = &ChangeOLEDClock;

  Menu_Item_Init(&menuItemSave);
  menuItemSave.title = "Save";
  menuItemSave.type = MENU_ITEM_TYPE_ACTION;
  menuItemSave.action = &StoreCurrentParameters;

  Menu_Item_Init(&menuItemExit);
  menuItemExit.title = "Exit";
  menuItemExit.type = MENU_ITEM_TYPE_ACTION;
  menuItemExit.action = &ExitMenu;

  Menu_Init(&menuMain);
  Menu_Add_Item(&menuMain, &menuItemCalibrate);
  Menu_Add_Item(&menuMain, &menuItemMag);
  Menu_Add_Item(&menuMain, &menuItemCurrent);
  Menu_Add_Item(&menuMain, &menuItemStepSize);
  Menu_Add_Item(&menuMain, &menuItemEnablePin);
//  Menu_Add_Item(&menuMain, &menuItemDirectionPin);
  Menu_Add_Item(&menuMain, &menuItemPID_P);
  Menu_Add_Item(&menuMain, &menuItemPID_I);
  Menu_Add_Item(&menuMain, &menuItemPID_D);
  Menu_Add_Item(&menuMain, &menuItemOLEDFreq);
  Menu_Add_Item(&menuMain, &menuItemSave);
  Menu_Add_Item(&menuMain, &menuItemExit);

}


//Always 2 digits after the point
void ftoa_embedded(float data, char* buffer, size_t buffer_size, int16_t padding)
{
  buffer[0] = (data < 0) ? '-' : ' ';

  data = fabs(data);
  int tmpInt1 = data;
  itoa(tmpInt1, buffer+1, 10);
  //Now pad with spaces - shift to the end of designated area
  int int_length = strlen(buffer);
  if(int_length<padding)
  {
    for(int i=int_length-1;i>=0;i--)
      buffer[i+padding-int_length] = buffer[i];
    for(int i=0;i<padding-int_length;i++)
      buffer[i] = ' ';
  }

  float fractional = data - tmpInt1;      // Get fraction (0.0123).
  int tmpInt2 = trunc(fractional * 100);  // Turn into integer (123).

  // Print fractional part, appending int part
  snprintf (buffer+padding, buffer_size-padding, ".%02d", tmpInt2);
}

void wakeup()
{
  if(idle_cycles>=SCREENSAVER_DELAY)
    ShowInfoScreen();//Restore screen content

  idle_cycles = 0;
}

void Motor_data_dis(void)
{
    // Switch LED on error. Do not limit angle error - it's harder to recover. 
//    if((tim6Counter&255)==12)//We only update LED once per 256 cycles
    if(pid_e > 3200 || pid_e < -3200)LED_H;else LED_L;


    //uint8_t temp[17]={0};
    if(!enmode)
    {
      idle_cycles++;
      if(idle_cycles==SCREENSAVER_DELAY)
      {
        //Enable screensaver
        OLED_Clear();
      };

      if(idle_cycles>=SCREENSAVER_DELAY)
        return;//Do not show anything else
    }
    else
    {
      wakeup();//We were in screensaver before - so show screen back, motor is enabled
    }

#ifdef DEBUG_CPU_LOAD    
    char strRPM[20];// = {0};
    snprintf(strRPM, 20, "%5ld", cpu_idle);
    OLED_ShowString(48+8*5, 2, strRPM);
#endif
    char strERR[20] = {0};
    char strDEG[20] = {0};

//    static int32_t prev_pid_e = 0;
    
    //if(prev_pid_e != pid_e /*&& Menu_Num==1*/)
        //prev_pid_e = pid_e;
        Data_update_flag=0;
/*************SIMP*****************************************************/
//        Motor_speed = cpu_idle;//
        //We do longer display CPU idle data, it's only needed for loop debug


       ftoa_embedded((pid_e*angle_conversion), strERR, 20, 4);
       ftoa_embedded((yw*angle_conversion), strDEG, 20, 6);

       OLED_ShowString(48+8*2, 22, strERR);
       OLED_ShowString(48-8*0, 42, strDEG);

       OLED_Refresh_Gram();
}























