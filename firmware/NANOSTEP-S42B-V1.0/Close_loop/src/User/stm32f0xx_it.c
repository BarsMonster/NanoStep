#include "stm32f0xx.h"
#include "stm32f0xx_it.h"
#include "tle5012b.h"
#include "main.h"
#include <stdlib.h> 

 int32_t rampup_current = 0;//We use it to slowly start motor after enable

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
} 


void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}

// Handle System Timer interrupt. Configured for 1ms ticks.
void SysTick_Handler(void)
{
  tickCount++;
}


// Handle DIR pin interrupt
void EXTI0_1_IRQHandler(void)
{
  //Cutting all interrupt & configuration checks to reduce DIR setup timing requirement
//  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_1) != RESET)
  {
    if (DIRIN == 1)
    {
      //if (Motor_Dir == 1)
      {
        LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_UP);
      }
      /*else
      {
        LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_DOWN);
      }*/
    }
    else
    {
      //if (Motor_Dir == 1)
      {
        LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_DOWN);
      }
      /*else
      {
        LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_UP);
      }*/
    }
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1);//As an exception, clear it at the end to reduce latency. We don't expect multiple interrupts anyways
  }
}

// Handle EN pin interrupt
void EXTI2_3_IRQHandler(void)
{
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_2) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);

    __disable_irq();//Just in case
    handle_en();
    __enable_irq();
  }
}


// Handle Timer 6 interrupt which performs the control loop.
void TIM6_IRQHandler(void)
{
  if(LL_TIM_IsActiveFlag_UPDATE(TIM6) == 1)
  {
    LL_TIM_ClearFlag_UPDATE(TIM6);
    //LL_TIM_DisableCounter(TIM6);//Allow to init while control loop is disabled
    
    // Kick the dog
    LL_IWDG_ReloadCounter(IWDG);
    //yw+=1;

    // Increase counter (used elsewhere for 100us ticks)
    tim6Counter++;

    static int32_t speed;
    static int16_t step_timeout = 0;
    static int32_t error_history = 0;//slow trailing error indicator. Used to identify that motor has lost sync
    static int32_t iterm_to_share = 0;//Slicing iterm extra across 32 timeslots
    static int32_t iterm_timeslots_remaining = 0;
    static int32_t maxspeed = 4;

    //Read angle anyways, even when motor is disabled - to show that sensor is working normally
    y = ReadValue(READ_ANGLE_VALUE);

    if(y != y_1)
    {
      if (y - y_1 > 16384)
      {
        wrap_count--;
      }
      else if (y - y_1 < -16384)
      {
        wrap_count++;
      };
      y_1 = y;

      // Calculate the actual angle (yw)
      yw = y + 32768 * wrap_count;
    }


    if(enmode==1)
    {
      if(closemode==1) 
      { 

        // Read counter steps
        s = LL_TIM_GetCounter(TIM1);
        if(s!=s_1)
        {
          //We had a step
          step_timeout = 20;//Wait 20 steps without error correction, i.e. around 0.6ms
          iterm = 0;//On step old iterm is no longer relevant

          // Handles cases where the 16bit TIM1 ticks over. 
          // s_sum (which is an offset for the desired angle) is then adjusted. 
          if(s - s_1 < -32768)
            s_sum += stepangle * 65536;
          else if(s - s_1 > 32768)
            s_sum -= stepangle * 65536;

          // Calculate the desired angle (r)
          r = s_sum + stepangle * s;
          s_1 = s;
        }
        
        // Calculate the error          
        int32_t pid_e_internal = r - yw;

      
        /*
        if(iterm > 10000)iterm = 10000;
        if(iterm < -10000)iterm = -10000;
        int32_t PID_result = (kp * pid_e_internal + iterm + dterm) / 1280; //PID*/

        if(abs(pid_e_internal)>1 && (tim6Counter&31)==0)//Do not accumulate large iterm too often
        {
          iterm += (ki * pid_e_internal * 1024) / 25;
          if(abs(pid_e_internal)<10)
            iterm=limitabs(iterm, 8*4*1024);//Damping accumulated iterm
        }

        if((tim6Counter&31)==0)
        {
          if(abs(pid_e_internal)<5 || abs(iterm)>1024*32)
            iterm=(iterm*14)>>4;//Damping accumulated iterm when close to target
          iterm = limitabs(iterm, 1024*1024);
        }

//        dterm = LPFA * dterm / 128 + 0 * LPFB * kd * (yw - yw_1) / 32;

        //Native P: 16384 per cycle
        //Err
        int32_t PID_result_delta = 0;
//        if(pid_e_internal>1000)
         //
         //PID_result_delta = ((int32_t)kp * pid_e_internal + iterm /*+ dterm*/) / (int32_t)96;*/
        /*if(PID_result_delta>32)PID_result_delta=16;
        if(PID_result_delta<-32)PID_result_delta=-16;*/
        
        //32768 per rotation, 163.84 angle readings per step
        //4096 per 4 step, 1024 per step. 
        //6.25 conversion factor
        int32_t PID_result;
        if(step_timeout>0)
        {
          PID_result_delta = 0;
          iterm=0;
          iterm_to_share=0;
          step_timeout--;
        } else
        {
          pid_e = pid_e_internal;//Only show error when we are not in step timeout
        }

        //Do correction when there was no step recently or error is more than 1.5 steps, i.e. 245 - so that we don't react to 1 step errors in the beginning of the step
        if(step_timeout==0 || abs(pid_e_internal)>245)
        {
          
          maxspeed = min(maxspeed, speed);//We are allowed to go as fast as we were going on previous step
          if((tim6Counter&3)==0)maxspeed++;//increase speed with 1/4 acceleration. 
          maxspeed = min(128, maxspeed);//absolute maximum speed of error correction; We don't exceed it so that we don't loose steps. We can detect lost steps, but this is slow. 
          maxspeed = min(abs(pid_e_internal)+1, maxspeed);//slowing down when getting to target
          
          if(abs(pid_e_internal)>error_history)
          {
            maxspeed = min(16, maxspeed);//Do not accelerate motor if error is not decreasing
            error_history = abs(pid_e_internal)*1024;//Temporary inflate error history to ensure we don't re-trigger immidiately. 
          }

          PID_result_delta = ((int32_t)kp * pid_e_internal) / (int32_t)256;//correcting large errors

          //Now add iterm proportional across 32 timelots, i.e. it is slow
          if((tim6Counter&31)==0)
          {
            iterm_to_share = iterm;
            iterm_timeslots_remaining = 32;
          }

          if(abs(iterm_to_share)<=4096*iterm_timeslots_remaining && abs(iterm_to_share)>=4096)//less than 1 per remaining timeslot
          {
            PID_result_delta+=1*sign(iterm_to_share);
            iterm_to_share-=4096*sign(iterm_to_share);
          };
          if(iterm_to_share>4096*2*iterm_timeslots_remaining && iterm_timeslots_remaining>0)//2 or more iterm correction per remaining timeslot
          {
            int32_t iterm_executed = iterm_to_share/iterm_timeslots_remaining/4096;
            PID_result_delta+=iterm_executed;
            iterm_to_share-=iterm_executed*4096;
          };

          if(iterm_timeslots_remaining>0)
            iterm_timeslots_remaining--;

          //if(abs(PID_result_delta)<=1)PID_result_delta = 0;//Deadzone of 1 for correction to reduce noise
        }
        
        PID_result_delta = limitabs(PID_result_delta, maxspeed);//limiting max speed / acceleration / deceleration
        
        /*if(step_timeout>0) -- we still correct large errors even if we are in step timeout
          PID_result_delta = 0;*/

        PID_result = angle_prev + (r-r_1)*25 + PID_result_delta;//Steps feed forward
        

        Output(PID_result/4, rampup_current);    //We divide late not to accumulate errors over time

        if(rampup_current < Currents && (tim6Counter&31)==19)//ramp current by 1 every 1ms - soft start
          rampup_current++;

//        Output(10, 10);    

        yw_1 = yw;
        r_1 = r;
        speed = abs(angle_prev - PID_result);
        angle_prev = PID_result;
        error_history = (error_history*255+abs(pid_e_internal))>>8;

        //Test for overwrap - too far away from start. Will only be active on next call
        //We don't need to test for overwrap every time
        if ((tim6Counter & 31) == 10)
        {
          if (yw > 32768 * 10) // 10 rotations max
          {
            wrap_count -= 10;             // Going back 9 turns
            s_sum -= (int32_t)32768 * 10; // Going back 9 turns
          }
          else if (yw < -32768 * 10) // 10 rotations max
          {
            wrap_count += 10;             // Going forward 9 turns
            s_sum += (int32_t)32768 * 10; // Going forward 9 turns
          };
        }
      }
    }
  }

  //LL_TIM_EnableCounter(TIM6);
  //bufferData(); - not needed as we use direct measurement
}




