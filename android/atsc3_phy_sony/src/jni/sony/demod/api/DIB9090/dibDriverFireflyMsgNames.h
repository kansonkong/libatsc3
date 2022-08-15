/**************************************************************************************************
* @file "DibDriverFireflyMsgNames.h"
* @brief Firefly specific messages names.
*
***************************************************************************************************/
/*#####################################################################################################
*-------- WARNING: Any change of message id must be updated in DibBridge<CHIP>Registers.h  -----------
*####################################################################################################*/

      /* Messages transmitted by the HOST */
      DIB_MSG_OUT(OUT_MSG_CONF_SVC)                /* 19 */
      DIB_MSG_OUT(OUT_MSG_SET_HBM)
      DIB_MSG_OUT(OUT_MSG_INIT_DEMOD)
      DIB_MSG_OUT(OUT_MSG_ENABLE_DIVERSITY)
      DIB_MSG_OUT(OUT_MSG_SET_OUTPUT_MODE)
      DIB_MSG_OUT(OUT_MSG_SET_PRIORITARY_CHANNEL)
      DIB_MSG_OUT(OUT_MSG_ACK_FRG)                 /* REQUIRES NOTIFICATION IF OFFSET IS CHANGED - FF25 */
      DIB_MSG_OUT(OUT_MSG_INIT_PMU)

      /* Messages received by host */
      DIB_MSG_IN(IN_MSG_ACK_CHANGE_SVC)   /* 15 */
      DIB_MSG_IN(IN_MSG_HBM_PROF)      
      
