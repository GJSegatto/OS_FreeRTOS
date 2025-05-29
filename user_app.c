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

void config_pwm() {
    TRISDbits.TRISD0 = 0;
    LATDbits.LATD0 = 0;

    OC1CONbits.OCTSEL = 0;                              //Usa Timer2
    PR2 = 199;                                          //Periodo do Timer2
    T2CONbits.TCKPS = 0b00;                             //
    OC1RS = ((uint16_t)(PR2 + 1) * 0) / 100;            //DC que passa pra OCR1 no estouro de clock
    OC1R = OC1RS;
}

void start_pwm(uint16_t new_dc) {
    uint16_t temp = ((uint16_t)(PR2 + 0) * new_dc) / 100;       //Cálculo do DC
    OC1RS = temp;                                               //Atribuição do novo DC
    OC1CONbits.OCM = 0b110;                                     //Ativa modo de PWM
    T2CONbits.TON = 1;                                          //Inicia o Timer2
}

void stop_pwm() {
    OC1CONbits.OCM = 0b000;         //Desativa o PWM
    T2CONbits.TON = 0;              //Para o Timer2
    LATDbits.LATD0 = 0;             //Zera a saída do PWM
}

void config_ports() {

    //PINOS
    TRISDbits.TRISD0 = 0;           //Saída de bicos e motor
    TRISGbits.TRISG0 = 0;           //Saída do freio motor
    TRISAbits.TRISA14 = 1;          //Pino de Interrupção - IN

    //INTERRUPÇÃO EXTERNA
    __builtin_enable_interrupts();  //Ativa interrupções externas

    IEC3bits.INT3IE = 1;            //Habilita interrupção externa 3
    INTCON2bits.INT3EP = 0;         //Borda de subida
    IPC13bits.INT3IP = 0b111;       //Prioridade da interrupção externa (MÁXIMA)
    IFS3bits.INT3IF = 0;            //Limpa a flag da interrupção externa 3
}

void __attribute__((__interrupt__, __auto_psv__)) _INT3Interrupt(void) {
    IFS3bits.INT3IF = 0;
    stop_pwm();
    while(PORTAbits.RA14) {
        LATGbits.LATG0 = 1;
    }
    LATGbits.LATG0 = 0;
    LATDbits.LATD0 = 1;
    config_pwm();
    start_pwm(1);
}

void acelerador()
{
    uint16_t temp = 0;
    while (1) {
        temp = read_adc();
        xQueueSend(pipe, &temp, portMAX_DELAY);
        vTaskDelay(10);
    }
}

void controle_central()
{
    while (1) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        xQueueReceive(pipe, &adc_value, portMAX_DELAY);
        xSemaphoreGive(mutex);
        vTaskDelay(10);
    }
}

void injecao_eletronica()
{
    uint16_t dc = 0;
    while (1) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        dc = (uint16_t)((uint32_t)adc_value * 100 / 1023);
        xSemaphoreGive(mutex);

        if(dc > 0) {
            start_pwm(dc);
        } else {
            stop_pwm();
        }
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
    config_pwm();
}
