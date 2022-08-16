#ifndef DEMOD_DIB7000M_FW_PRIV_H
#define DEMOD_DIB7000M_FW_PRIV_H

#define I_AM_JUST_A_FRONTEND

#define SDK_BUILD 0
#define FIRMWARE_FIREFLY 0

#undef DIB_MSG_OUT
#define DIB_MSG_OUT(a)
#undef DIB_MSG_IN
#define DIB_MSG_IN(a) a,
enum dib7000m_in_messages {
#include "dib7000m_messages.h"
#include "dibDriverFireflyMsgNames.h"
};

#undef DIB_MSG_OUT
#define DIB_MSG_OUT(a) a,
#undef DIB_MSG_IN
#define DIB_MSG_IN(a)
enum dib7000m_out_messages {
#include "dib7000m_messages.h"
#include "dibDriverFireflyMsgNames.h"
};

#define REG_MAC_RESET           1227

extern int dib7000m_mbx_fe_reset(struct dib7000m_state *state);

extern int dib7000m_fw_boot(struct dib7000m_state *state, const uint8_t *code_A, uint32_t lenA, const uint8_t *code_B, uint32_t lenB);

extern int dib7000m_mbx_send_attr(struct dib7000m_state *state, uint8_t id, uint16_t *data, uint8_t len, uint16_t attr);
extern int dib7000m_mbx_get_message_attr(struct dib7000m_state *state, uint16_t id, uint16_t *msg, uint8_t *size, uint16_t attr);

#define dib7000m_mbx_send(state, id, data, len) dib7000m_mbx_send_attr(state, id, data, len, 0)
#define dib7000m_mbx_get_message(state, id, msg, len) dib7000m_mbx_get_message_attr(state, id, msg, len, 0)

extern int dib7000m_risc_mem_read(struct dib7000m_state *state, uint8_t cmd, uint8_t *b, uint16_t len);
extern int dib7000m_risc_mem_write(struct dib7000m_state *state, uint8_t cmd, const uint8_t *b);
extern int dib7000m_risc_mem_setup_cmd(struct dib7000m_state *state, uint32_t addr, uint32_t len, uint8_t reading);
extern int dib7000m_risc_mem_read_chunks(struct dib7000m_state *state, uint8_t *b, uint32_t len);
extern int dib7000m_risc_mem_write_chunks(struct dib7000m_state *state, const uint8_t *b, uint32_t len);

/* memory_access requests */
#define FE_MM_W_CHANNEL                   0
#define FE_MM_W_FE_INFO                   1
#define FE_MM_RW_SYNC                     2

#define FE_SYNC_CHANNEL          1
#define FE_SYNC_W_GENERIC_MONIT	 2
#define FE_SYNC_COMPONENT_ACCESS 3

#define FE_MM_R_CHANNEL_SEARCH_STATE      3
#define FE_MM_R_CHANNEL_UNION_CONTEXT     4
#define FE_MM_R_FE_INFO                   5
#define FE_MM_R_FE_MONITOR                6

#define FE_MM_W_CHANNEL_HEAD              7
#define FE_MM_W_CHANNEL_UNION             8
#define FE_MM_W_CHANNEL_CONTEXT           9
#define FE_MM_R_CHANNEL_UNION            10
#define FE_MM_R_CHANNEL_CONTEXT          11
#define FE_MM_R_CHANNEL_TUNE_STATE       12

#define FE_MM_R_GENERIC_MONITORING_SIZE	 13
#define FE_MM_W_GENERIC_MONITORING	     14
#define FE_MM_R_GENERIC_MONITORING	     15

#define FE_MM_W_COMPONENT_ACCESS         16
#define FE_MM_RW_COMPONENT_ACCESS_BUFFER 17

#endif
