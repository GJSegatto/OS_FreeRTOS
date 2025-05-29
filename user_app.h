#ifndef USER_APP_H
#define	USER_APP_H

#include "FreeRTOS.h"
#include "task.h"
#include "xc.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"

void config_user_app();
void config_ports();

void acelerador();
void controle_central();
void injecao_eletronica();

void config_adc();
uint16_t read_adc();

void __attribute__((__interrupt__, __auto_psv__)) _INT3Interrupt(void);

#endif	/* USER_APP_H */

