#include "dsp.h"
/******************************************************************************/
/******************************************************************************/
#define N_2E_15     1L>>15					// 2^(-15); i think
#define	N_2E15		1L<<15					// 2^15
#define	N_2E16		1L<<16					// 2^16
/******************************************************************************/
#if	DATA_TYPE == FRACTIONAL		/* [ */


fractional myFloat2Fract ( float aVal ) {
    long int n_2e15 = N_2E15;
   	long int fractVal = 0.0;
   	double decimalVal = 0.0;
   	double dummy = 0.0;
   	int isOdd = 0;
   	/**************************************************************************/
   	/* Convert with convergent rounding and saturation. */
   	decimalVal = aVal*n_2e15;
   	/**************************************************************************/
   	if (aVal >= 0) {
   		fractVal = floor (decimalVal);
   		dummy = fractVal/2.0;
   		isOdd = (int) ((dummy - floor (dummy))*2.0);
   		dummy = decimalVal -fractVal;
   		/**********************************************************************/
   		if ((dummy > 0.5) || ((dummy == 0.5) && isOdd)) {
   			fractVal += 1.0;
   		}
   		/**********************************************************************/
   		if (fractVal >= n_2e15) {
   			fractVal = n_2e15 - 1.0;
   		}
   	/**************************************************************************/	
   	} else {	/* aVal < 0 */
   		fractVal = ceil (decimalVal);
   		/**********************************************************************/
   		if (fractVal != decimalVal) {
   			fractVal -= 1.0;
   		}
   		/**********************************************************************/
   		dummy = fractVal/2.0;
   		isOdd = (int) ((dummy - floor (dummy))*2.0);
   		dummy = decimalVal -fractVal;
   		/**********************************************************************/
   		if ((dummy > 0.5) || ((dummy == 0.5) && isOdd)) {
   			fractVal += 1.0;
   		}
   		/**********************************************************************/
   		if (fractVal < -N_2E15) {
   			fractVal = -N_2E15;
   		}
   	}
   	/***************************************************************************/
   	/* Return fractional value. */
   	return ((fractional) fractVal);
}
#endif	/* ] */
/******************************************************************************/
/******************************************************************************/
float myFract2Float ( fractional aVal ) {
#if	DATA_TYPE==FRACTIONAL				/* [ */
	/**************************************************************************/
//	double scale = pow (2.0, -15.0);			// 2^(-15); replaced by N_2E_15 
	double n_2e_15 = N_2E_15;
    double decimalVal = 0.0;
	/**************************************************************************/
	/* Convert. */
	if (aVal >= N_2E15) {
		aVal -= N_2E16;
	}
	/**************************************************************************/
	decimalVal = ((double) aVal)*n_2e_15;
	/**************************************************************************/
	/* Return decimal value in floating point. */
	return ((float) decimalVal);
#else	/* ] [ */
	/**************************************************************************/
   	/* Return input value in floating point. */
   	return ((float) aVal);
#endif	/* ] */
}
/******************************************************************************/
/******************************************************************************/