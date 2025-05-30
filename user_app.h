#ifndef USER_APP_H
#define	USER_APP_H

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "xc.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "portmacro.h"

void config_user_app();
void config_ports();

void acelerador();
void controle_central();
void injecao_eletronica();

void config_adc();
uint16_t read_adc();

void config_pwm();
void start_pwm(uint16_t new_dc);
void stop_pwm();

void interruption_handler();
void __attribute__((__interrupt__, __auto_psv__)) _INT3Interrupt(void);

#endif	/* USER_APP_H */

