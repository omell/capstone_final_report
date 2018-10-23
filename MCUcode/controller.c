#include "controller.h"
#include <dsp.h>
#include <stdbool.h>

#include "define.h"
#include <libpic30.h>

float signal_feed = 0;
float signal_int = 0;
float signal_IMP = 0;
float signal_antiwindup = 0;
float signal_control_complete = 0;
float signal_control_clipped = 0;

// ADC
float Vout;
float Vin;
fractional frac_Vin;
fractional frac_Vout;

// duty ratio globals
float d;
double hat_d = 0.0;

// state feedback globals
float u[4][1] = {{0}}; //{0, 0, 0, 0};
float hat_u[4][1]= {{0}}; //{ {0, 0, 0, 0};
float x[4][1] = {{0}};  //{0, 0, 0, 0};
float hat_x[4][1] = {{0}};  //{0, 0, 0, 0};

float y = 0;
float hat_y = 0;

// integral action globals
double int_in = 0;
double int_out = 0;

// steady-state quantities - linearised about duty ratio 50 %
float U[4][1] = {{5}, {0.36}, {0}, {0}};

float X[4][1] = {{9.82414541782378}, {-5.04922999242616},
                        {-0.0504922999242615}, {-0.0504922999242616}};

float Y = -5.010617; // -5.04922999242615;

// observer matricies (ZOH discretised @ 10 kHz)
const float A_bar[4][4] = 
        {{-1.095542e-04, -2.095784e-01,  2.496537e-03,  2.068269e-01},
        { 4.098552e-04,  -3.014558e-03,  1.758555e-04,  1.494895e-02},
        { -6.400300e-02,  8.591395e-01, -3.264673e-02, -2.741612},
        { 9.868499e-04,  -8.892412e-03,  4.479672e-04,  3.780580e-02}};

const float b_bar[4][4] = 
        {{+3.260718E-02, -1.000110E+00, +0.000000E+00, +0.000000E+00},
        {-2.937079E-04, +4.098552E-04, +0.000000E+00, +0.000000E+00},
        {+4.314424E-02, -6.400300E-02, +0.000000E+00, +0.000000E+00},
        {-8.191777E-04, +9.868499E-04, +0.000000E+00, +0.000000E+00}};

const float bD_bar[4][1] = 
    {{-1.820186E+01}, {2.002734E-03}, {-5.675197E-01}, {2.633244E-03}};

const float L_bar[4][1] = 
    {{-1.784800E+00}, {+1.003400E+00}, {-9.079246E-01}, {+1.969041E-02}};

// integral action gain
float K_i = -10.0;

// State feedback gain
float K_f[4] =  {0.000742149825844293, 0.000729995239324206,
                        0.00730858935162900, -0.00721782485740900};

// controller variables
float signal_control_int;
float signal_control_antiwindup;
float signal_control_complete;
float signal_control_feedback;

int i, j;

extern bool require_update; 

fractional* frac_u[4][1] = {{0}};
fractional* frac_hat_u[4][1] = {{0}};
fractional* frac_x[4][1] = {{0}};
fractional* frac_hat_x[4][1] = {{0}};

fractional* frac_U[4][1] = {{0}};
fractional* frac_X[4][1] = {{0}};

fractional* A_frac[4][4] = {{0}};
fractional* B_frac[4][4] = {{0}};
fractional* bD_frac[4][1] = {{0}};
fractional* L_frac[4][1] = {{0}};

fractional* frac_x_out_hat[4][1] = {{0}};
fractional* frac_u_hat[4][1] = {{0}};
fractional frac_hat_d = 0;
fractional frac_hat_y = 0;

fractional* A_by_x[4][1] = {{0}};
fractional* B_by_u[4][1] = {{0}};
fractional* bD_by_d[4][1] = {{0}};
fractional* L_by_y[4][1] = {{0}};

fractional* Q1[4][1] = {{0}};
fractional* Q2[4][1] = {{0}};

fractional* frac_K_f[4] = {0};
fractional frac_control_feedback;

fractional frac_V_g;
fractional frac_Y;


fractional myFloat2Fract ( float aVal );
float myFract2Float (fractional aVal );

/******************************************************************************/
// Convert to Fractional
/******************************************************************************/
void controllerInit( void ){
    frac_V_g = myFloat2Fract(V_g);
    frac_Vin = frac_V_g;
    
    frac_Y = myFloat2Fract(Y);
    
    for (i=0; i<4; i++){
        *bD_frac[i][0] = myFloat2Fract(bD_bar[i][0]);
        *L_frac[i][0] = myFloat2Fract(L_bar[i][0]);
        *frac_K_f[i] = myFloat2Fract(K_f[i]);
        
        for (j=0; j<4; j++){    // FIND BETTER WAY
            *A_frac[i][j] = myFloat2Fract(A_bar[i][j]);
            *B_frac[i][j] = myFloat2Fract(b_bar[i][j]);
        }
    }
    
}
/******************************************************************************/
// Controller
/******************************************************************************/
void updateIntegral( void ){
    // forward euler => use int_in from previous timestep
	 Vout = myFract2Float(dataAN0)*Vref;
	// now can update int_in (will be used next timestep)
	int_in = reference - Vout;
	// return integral action control signal
	// note: this signal enters loop via negative
	signal_control_int = K_i*int_out;
      // make complete control signal
    signal_control_complete = - signal_int - signal_antiwindup;
	/**************************************************************************/
    // begin: clip control signal     MAYBE TRY USING CASES?
            
    // too big
	if (duty_max < signal_control_complete) {
		signal_control_clipped = duty_max;
	// too small
	} else if (signal_control_complete < duty_min) {
		signal_control_clipped = duty_min;
	} else {
		signal_control_clipped = signal_control_complete;
	}
    // end: clip control signal
	/**************************************************************************/
	hat_d = signal_control_clipped;
    // update duty radio according to new control signal
    d = V_D + hat_d;
	/**************************************************************************/
    // update anti-windup
    signal_control_antiwindup = signal_control_complete
                                    - signal_control_clipped;
}


void updateControlSignal(void){
    LATBbits.LATB4 = LOW; 
	/**************************************************************************/
    // INTEGRAL ACTION
	// begin: make integral action signal
	// forward euler => use int_in from previous timestep
	 Vout = myFract2Float(dataAN0)*Vref;
	// now can update int_in (will be used next timestep)
	int_in = reference - Vout;
	// return integral action control signal
	// note: this signal enters loop via negative
	signal_control_int = K_i*int_out;
	// end: make integral action signal
    /**************************************************************************/
     LATBbits.LATB4 = HIGH; 
    // OBSERVER    
    Vin = myFract2Float(dataAN1)*Vref;
//	hat_y = Vout - Y;
//	frac_hat_y = myFloat2Fract(hat_y);
    frac_hat_y = myFloat2Fract(Vout - Y);  // ** FASTER?
	/**************************************************************************/
//	hat_u[0][0] = Vin - V_g;
//	*frac_hat_u[0][0] = myFloat2Fract(hat_u[0][0]);
    *frac_hat_u[0][0] = myFloat2Fract(Vin - V_g);
    frac_hat_d = myFloat2Fract(hat_d);
	/**************************************************************************/
	MatrixMultiply(4, 4, 1, A_by_x[0][0], A_frac[0][0], frac_hat_x[0][0]);
	MatrixMultiply(4, 4, 1, B_by_u[0][0], B_frac[0][0], frac_hat_u[0][0]);

	MatrixScale(4, 1, bD_by_d[0][0], bD_frac[0][0], frac_hat_d);
	MatrixScale(4, 1, L_by_y[0][0],  L_frac[0][0],  frac_hat_y);

	MatrixAdd(4, 1, Q1[0][0],         A_by_x[0][0],  B_by_u[0][0]);
	MatrixAdd(4, 1, Q2[0][0],         bD_by_d[0][0], L_by_y[0][0]);
	MatrixAdd(4, 1, frac_hat_x[0][0], Q1[0][0],      Q2[0][0]);
    /**************************************************************************/
    MatrixMultiply(1, 4, 1, &frac_control_feedback, frac_K_f[0],
                    frac_hat_x[0][0]);
    signal_control_feedback = Fract2Float(frac_control_feedback);

    // make complete control signal
    signal_control_complete = - signal_int - signal_control_feedback
                                - signal_antiwindup;
	/**************************************************************************/
    // begin: clip control signal     MAYBE TRY USING CASES?
            
    // too big
	if (duty_max < signal_control_complete) {
		signal_control_clipped = duty_max;
	// too small
	} else if (signal_control_complete < duty_min) {
		signal_control_clipped = duty_min;
	} else {
		signal_control_clipped = signal_control_complete;
	}
    // end: clip control signal
	/**************************************************************************/
	hat_d = signal_control_clipped;
    // update duty radio according to new control signal
    d = V_D + hat_d;
	/**************************************************************************/
    // update anti-windup
    signal_control_antiwindup = signal_control_complete
                                    - signal_control_clipped;
}
