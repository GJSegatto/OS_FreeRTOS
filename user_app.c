#include "user_app.h"
#include "queue.h"

SemaphoreHandle_t mutex;
QueueHandle_t pipe;
uint16_t adc_value = 0;

void config_adc() {
    AD1CON3bits.ADRC    = 1;            // Clock interno
    AD1CON3bits.SAMC    = 0b11111;      // Tad = 31
    AD1CON3bits.ADCS    = 0b00111111;   //  64 * TCY
    AD1CHSbits.CH0SA    = 0b0000;
    AD1CON2bits.VCFG    = 0b011;
    AD1CON1bits.SAMP    = 0;  
    AD1CON1bits.ADON    = 1;            // Liga conversor AD

}

uint16_t read_adc() {
    AD1CON1bits.SAMP   = 1;
    vTaskDelay(1);
    AD1CON1bits.SAMP   = 0;
    
    while (!AD1CON1bits.DONE);
    
    return ADC1BUF0;
}

void config_ports() {

    //PINOS
    TRISDbits.TRISD0 = 0;           //Saída de bicos e motor
    TRISAbits.TRISA14 = 1;          //Pino de Interrupção - IN

    //INTERRUPÇÃO EXTERNA
    __builtin_enable_interrupts();  //Ativa interrupções externas
    //INTCON1bits.NSTDIS = 1;         //Desativa interrupções aninhadas
    IEC3bits.INT3IE = 1;            //Habilita interrupção externa 3
    INTCON2bits.INT3EP = 0;         //Borda de subida
    IFS3bits.INT3IF = 0;            //Limpa a flag da interrupção externa 3
    IPC13bits.INT3IP = 0b111;       //Prioridade da interrupção externa (MÁXIMA)
}

void __attribute__((__interrupt__, __auto_psv__)) _INT0Interrupt(void) {
    IFS0bits.INT0IF = 0;
    while(PORTFbits.RF6) {
        LATDbits.LATD0 = 1;
    }
    LATDbits.LATD0 = 0;
}

void acelerador()
{
    while (1) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        adc_value = read_adc();
        xSemaphoreGive(mutex);
        vTaskDelay(10);
    }
}

void controle_central()
{
    while (1) {        
        xSemaphoreTake(mutex, portMAX_DELAY);
        if(adc_value > 500) {
            LATDbits.LATD0 = 1;
        }
        else {
            LATDbits.LATD0 = 0;
        }
        xSemaphoreGive(mutex);
        vTaskDelay(10);
    }
}

void injecao_eletronica()
{
    while (1) {        
        vTaskDelay(10);
    }
}

void config_user_app()
{
    pipe = xQueueCreate(5, sizeof(uint16_t));
    mutex = xSemaphoreCreateMutex();
    xSemaphoreGive(mutex);
    config_ports();
    config_adc();
}
