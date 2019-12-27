#ifndef __GPD_APPS_RTOS_MAIN__
#define __GPD_APPS_RTOS_MAIN__





// Proprietary Application task
#define GREEN_POWER_APP_TASK_PRIO         6u
#define GREEN_POWER_APP_TASK_STACK_SIZE   (1024 / sizeof(CPU_STK))


static CPU_STK greenPowerAppTaskStk[GREEN_POWER_APP_TASK_STACK_SIZE];
static OS_TCB  greenPowerAppTaskTCB;
static void    greenPowerAppTask(void *p_arg);



#endif