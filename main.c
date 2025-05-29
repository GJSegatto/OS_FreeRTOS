#include "user_app.h"

void main( void )
{
    config_user_app();
  
    xTaskCreate(acelerador, "Acelerador", configMINIMAL_STACK_SIZE, NULL, 4, NULL); 
    xTaskCreate(controle_central, "Controle", configMINIMAL_STACK_SIZE, NULL, 2, NULL); 
    xTaskCreate(injecao_eletronica, "Injecao", configMINIMAL_STACK_SIZE, NULL, 6, NULL); 
    
    vTaskStartScheduler();

    for( ; ; )
    {
    
    }
}
