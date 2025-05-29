#include "user_app.h"
#include "queue.h"

void config_user_app()
{
    TRISDbits.TRISD0    = 0;
}

void tarefa_teste()
{
    while (1) {        
        PORTDbits.RD0 = 1;
        vTaskDelay(20);        
        PORTDbits.RD0 = 0;
        vTaskDelay(20);        
    }
}
