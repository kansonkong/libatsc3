#ifndef    _Si2168B_PLATFORM_DEFINITION_H_
#define    _Si2168B_PLATFORM_DEFINITION_H_
             
//Edit by ITE             
#define TER_TUNER_SILABS
#define Si2168B_40_COMPATIBLE

/*****************************************************************************/
/* Possible DTV standards                                                    */
/*****************************************************************************/
#ifdef    DEMOD_Si2168B
  #define DEMOD_DVB_S_S2_DSS
#endif /* DEMOD_Si2168B */

/******************************************************************************/
/* TER Tuner FEF management options */
/******************************************************************************/
#define Si2168B_FEF_MODE_SLOW_NORMAL_AGC  0
#define Si2168B_FEF_MODE_FREEZE_PIN       1
#define Si2168B_FEF_MODE_SLOW_INITIAL_AGC 2

/******************************************************************************/
/* TER Tuner FEF management selection (possible values are defined above) */
/* NB : This selection is the ‘preferred?solution.                           */
/* The code will use more compilation flags to slect the final mode based     */
/*  on what the TER tuner can actually do.                                    */
/******************************************************************************/
#define Si2168B_FEF_MODE    Si2168B_FEF_MODE_FREEZE_PIN

/******************************************************************************/
/* Tuners selection (one terrestrial tuner, one satellite tuner )             */
/******************************************************************************/

#ifdef    TER_TUNER_SILABS
  #include  "SiLabs_TER_Tuner_API.h"
#endif /* TER_TUNER_SILABS */

#ifndef   TER_TUNER_SILABS

   "If you get a compilation error on this line, it means that no terrestrial tuner has been selected. Define TER_TUNER_xxxx at project-level!";

#endif /* TER_TUNER_SILABS */



/******************************************************************************/
/* Clock sources definition (allows using 'clear' names for clock sources)    */
/******************************************************************************/
typedef enum Si2168B_CLOCK_SOURCE {
  Si2168B_Xtal_clock = 0,
  Si2168B_TER_Tuner_clock,
  Si2168B_SAT_Tuner_clock
} Si2168B_CLOCK_SOURCE;

/******************************************************************************/
/* TER and SAT clock source selection (used by Si2168B_switch_to_standard)     */
/* ( possible values are those defined above in Si2168B_CLOCK_SOURCE )         */
/******************************************************************************/
#ifdef   SILABS_EVB
  #define Si2168B_TER_CLOCK_SOURCE            Si2168B_TER_Tuner_clock
  #define Si2168B_SAT_CLOCK_SOURCE            Si2168B_Xtal_clock
#else
  #define Si2168B_TER_CLOCK_SOURCE            Si2168B_TER_Tuner_clock
  #define Si2168B_SAT_CLOCK_SOURCE            Si2168B_SAT_Tuner_clock
#endif /* SILABS_EVB */

#include "Si2168B_L1_API.h"
#include "Si2168B_Properties_Functions.h"

#endif /* _Si2168B_PLATFORM_DEFINITION_H_ */
