#include <p24Fxxxx.h>
#include <xc.h>

#include "airpic-timer.h"

void timer_config(unsigned short conf)
{
    T1CON = 0x0000;
    T4CON = 0x0000;
    T5CON = 0x0000;
    
    TMR1  = 0x00;
    TMR5  = 0x00;
    TMR4  = 0x00;
    
    PR1   = 249;
    PR2   = 0xffff;
    
    //Some possible values for timer period, assuming TCKPS = 01
    // 0000 9C40 -> trigger interrupt every 20ms (if this is used, use 16 bit timer instead)
    // 0001 86A0 -> trigger interrupt every 50ms (0001 86A0 = 100,000)
    // 0003 0D40 -> trigger interrupt every 100ms
    // 0006 1A80 -> trigger interrupt every 200ms
    // 000F 4240 -> trigger interrupt every 500ms
    // 001E 8480 -> trigger interrupt every 1000ms
    // 003D 0900 -> trigger interrupt every 2000ms
    // 0098 9680 -> trigger interrupt every 5000ms
    switch( conf & 0b00001110 )
    {
        case AIRPIC_TIMER_PERIOD_20MS:
            PR5 = 0x0000;
            PR4 = 0x9C3F;
            break;
        case AIRPIC_TIMER_PERIOD_50MS:
            PR5 = 0x0001;
            PR4 = 0x869F;
            break;
        case AIRPIC_TIMER_PERIOD_200MS:
            PR5 = 0x0006;
            PR4 = 0x1A7F;
            break;
        case AIRPIC_TIMER_PERIOD_500MS:
            PR5 = 0x000F;
            PR4 = 0x423F;
            break;
        case AIRPIC_TIMER_PERIOD_1000MS:
            PR5 = 0x001E;
            PR4 = 0x847F;
            break;
        case AIRPIC_TIMER_PERIOD_2000MS:
            PR5 = 0x003D;
            PR4 = 0x08FF;
            break;
        case AIRPIC_TIMER_PERIOD_5000MS:
            PR5 = 0x0098;
            PR4 = 0x967F;
            break;
        default:                                        //default is AIRPIC_TIMER_PERIOD_100MS
            PR5   = 0x0003;                             //MSW Timer period
            PR4   = 0x0D3F;                             //lSW Timer period
            break;
    }
    
    T1CONbits.TCKPS = 0b10;                     //Timer increments every 4us
    //T2CONbits.TCKPS = 0b11;                     //Timer increments every 16us
    T4CONbits.TCKPS = 0b01;                     //Timer increments every 0.5us
    
    IFS0bits.T1IF = 0;                          //Clear interrupt flag, just in case
    IFS1bits.T5IF = 0;                          //Clear interrupt flag, just in case
    //IPC7bits.T5IP = 4;
    
    if( (conf & 0x0001) == 0 )
        IEC1bits.T5IE = 1;                      //Enable interrupt
    
    T4CONbits.T32   = 1;                        //Use T4 and T5 as a 32-bit timer
    
    T1CONbits.TON   = 1;
    T4CONbits.TON   = 1;                        //Turn on timer
    
}

void sleep_msec(unsigned int delay)
{
    while(delay--)
    {
        IFS0bits.T1IF = 0;
        TMR1 = 0;
    
        while(!IFS0bits.T1IF);
        
    }
}
