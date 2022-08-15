#ifndef    _Si2168_PLATFORM_DEFINITION_H_
#define    _Si2168_PLATFORM_DEFINITION_H_

#define TER_TUNER_Si2148
#define Si2168_20_COMPATIBLE



/*****************************************************************************/
/* Default values and demod address                                          */
/*****************************************************************************/
#ifdef    SILABS_EVB
  #define DEMOD_ADDRESS                  0xc8
#else  /* SILABS_EVB */
  #define DEMOD_ADDRESS                  0xce
#endif /* SILABS_EVB */

/*****************************************************************************/
/* Possible DTV standards                                                    */
/*****************************************************************************/
#ifdef    DEMOD_Si2168
#ifndef   NO_SAT
  #define DEMOD_DVB_S_S2_DSS
#endif /* NO_SAT */
#endif /* DEMOD_Si2168 */

/******************************************************************************/
/* TER Tuner FEF management options */
/******************************************************************************/
#define Si2168_FEF_MODE_SLOW_NORMAL_AGC  0
#define Si2168_FEF_MODE_FREEZE_PIN       1
#define Si2168_FEF_MODE_SLOW_INITIAL_AGC 2

/******************************************************************************/
/* TER Tuner FEF management selection (possible values are defined above) */
/* NB : This selection is the ‘preferred?solution.                           */
/* The code will use more compilation flags to slect the final mode based     */
/*  on what the TER tuner can actually do.                                    */
/******************************************************************************/
#define Si2168_FEF_MODE    Si2168_FEF_MODE_FREEZE_PIN

/******************************************************************************/
/* Tuners selection (one terrestrial tuner, one satellite tuner )             */
/******************************************************************************/

#ifdef    TER_TUNER_Si2146
  #include  "Si2168_Si2146.h"
#endif /* TER_TUNER_Si2146 */

#ifdef    TER_TUNER_Si2148
  #include  "Si2168_Si2148.h"
#endif /* TER_TUNER_Si2148 */

#ifdef    TER_TUNER_Si2156
  #include  "Si2168_Si2156.h"
#endif /* TER_TUNER_Si2156 */

#ifdef    TER_TUNER_Si2158
  #include  "Si2168_Si2158.h"
#endif /* TER_TUNER_Si2158 */

#ifdef    TER_TUNER_Si2173
  #include  "Si2168_Si2173.h"
#endif /* TER_TUNER_Si2173 */

#ifdef    TER_TUNER_Si2176
  #include  "Si2168_Si2176.h"
#endif /* TER_TUNER_Si2176 */

#ifdef    TER_TUNER_Si2178
  #include  "Si2168_Si2178.h"
#endif /* TER_TUNER_Si2178 */

#ifdef    TER_TUNER_Si2190
  #include  "Si2168_Si2190.h"
#endif /* TER_TUNER_Si2190 */

#ifdef    TER_TUNER_Si2196
  #include  "Si2168_Si2196.h"
#endif /* TER_TUNER_Si2196 */

#ifdef    TER_TUNER_DTT759x
  #include  "Si2168_DTT759x.h"
#endif /* TER_TUNER_DTT759x */

#ifdef    TER_TUNER_NO_TER
  #include  "Si2168_NO_TER.h"
#endif /* TER_TUNER_NO_TER */

#ifdef    TER_TUNER_CUSTOMTER
  #include  "Si2168_CUSTOMTER.h"
#endif /* TER_TUNER_CUSTOMTER */

#ifndef   TER_TUNER_Si2146
 #ifndef   TER_TUNER_Si2148
  #ifndef   TER_TUNER_Si2156
   #ifndef   TER_TUNER_Si2158
    #ifndef   TER_TUNER_Si2173
     #ifndef   TER_TUNER_Si2176
      #ifndef   TER_TUNER_Si2178
       #ifndef   TER_TUNER_Si2190
        #ifndef   TER_TUNER_Si2196
         #ifndef   TER_TUNER_DTT759x
          #ifndef   TER_TUNER_NO_TER
           #ifndef   TER_TUNER_CUSTOMTER
   "If you get a compilation error on this line, it means that no terrestrial tuner has been selected. Define TER_TUNER_xxxx at project-level!";
            #endif /* TER_TUNER_CUSTOMTER */
           #endif /*TER_TUNER_NO_TER  */
         #endif /* TER_TUNER_DTT759x */
        #endif /* TER_TUNER_Si2196 */
       #endif /* TER_TUNER_Si2190 */
      #endif /* TER_TUNER_Si2178 */
     #endif /* TER_TUNER_Si2176 */
    #endif /* TER_TUNER_Si2173 */
   #endif /* TER_TUNER_Si2158 */
  #endif /* TER_TUNER_Si2156 */
 #endif /* TER_TUNER_Si2148 */
#endif /* TER_TUNER_Si2146 */


#ifndef   NO_SAT
#endif /* NO_SAT */
/******************************************************************************/
/* Clock sources definition (allows using 'clear' names for clock sources)    */
/******************************************************************************/
typedef enum Si2168_CLOCK_SOURCE {
  Si2168_Xtal_clock = 0,
  Si2168_TER_Tuner_clock,
  Si2168_SAT_Tuner_clock
} Si2168_CLOCK_SOURCE;

/******************************************************************************/
/* TER and SAT clock source selection (used by Si2168_switch_to_standard)     */
/* ( possible values are those defined above in Si2168_CLOCK_SOURCE )         */
/******************************************************************************/
#ifdef   SILABS_EVB
  #define Si2168_TER_CLOCK_SOURCE            Si2168_TER_Tuner_clock
  #define Si2168_SAT_CLOCK_SOURCE            Si2168_Xtal_clock
#else
  #define Si2168_TER_CLOCK_SOURCE            Si2168_TER_Tuner_clock
  #define Si2168_SAT_CLOCK_SOURCE            Si2168_SAT_Tuner_clock
#endif /* SILABS_EVB */

#include "Si2168_L1_API.h"
#include "Si2168_Properties_Strings.h"

#endif /* _Si2168_PLATFORM_DEFINITION_H_ */
