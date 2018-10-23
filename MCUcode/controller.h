/* 
 * File:   controller.h
 * Author: miait
 *
 * Created on 20 September 2018, 11:26 AM
 */

#ifndef CONTROLLER_H
#define	CONTROLLER_H

#include <stdint.h>
#include <xc.h>

extern volatile uint16_t dataAN0;
extern volatile uint16_t dataAN1;
extern volatile uint16_t duty;
extern uint16_t reference;

extern float d;

    // constants
    #define Vref        3.3
    #define duty_max    0.8             // 80 %
    #define duty_min    0.0
    #define Ts          1.000000E-04    // 10 kHz
    #define TPWM        10e-6           // 100 kHz
    #define V_g         0.5
    #define V_D         
    #define ACLK        60e6            // 60MHz
    #define DUTY_CONST  4800            // ACLK * 8 * TPWM

    
    // functions       
    void controllerInit( void );
    void updateIntegral( void );    
    void updateControlSignal(void);
    
#endif	/* CONTROLLER_H */

