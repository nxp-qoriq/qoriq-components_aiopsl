/* Copyright 2014 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**************************************************************************//*
 @File          fsl_dpci_cmd.h

 @Description   defines dprc portal commandsF

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPCI_CMD_H
#define _FSL_DPCI_CMD_H

/* DPCI Version */
#define DPCI_VER_MAJOR				2
#define DPCI_VER_MINOR				0

/* cmd IDs */
#define DPCI_CMDID_CLOSE				0x800
#define DPCI_CMDID_OPEN					0x807
#define DPCI_CMDID_CREATE				0x907
#define DPCI_CMDID_DESTROY				0x900

#define DPCI_CMDID_SET_RX_QUEUE				0x184
#define DPCI_CMDID_ENABLE				0x186
#define DPCI_CMDID_DISABLE				0x187
#define DPCI_CMDID_RESET				0x188
#define DPCI_CMDID_GET_ATTR				0x189
#define DPCI_CMDID_SET_IRQ				0x18a
#define DPCI_CMDID_GET_IRQ				0x18b
#define DPCI_CMDID_SET_IRQ_ENABLE			0x18c
#define DPCI_CMDID_GET_IRQ_ENABLE			0x18d
#define DPCI_CMDID_SET_IRQ_MASK				0x18e
#define DPCI_CMDID_GET_IRQ_MASK				0x18f
#define DPCI_CMDID_GET_IRQ_STATUS			0x190
#define DPCI_CMDID_CLEAR_IRQ_STATUS			0x191
#define DPCI_CMDID_GET_LINK_STATE			0x192

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_CMD_OPEN(cmd, dpci_id) \
	MC_CMD_OP(cmd, 0, 0, 32, int, 	    dpci_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_CMD_CREATE(cmd, cfg) \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  cfg->num_of_priorities)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_CMD_SET_RX_QUEUE(cmd, priority, dest_cfg, rx_user_ctx) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, dest_cfg->dpio_id);\
	MC_CMD_OP(cmd, 0, 16, 16, uint16_t, dest_cfg->dpcon_id);\
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  dest_cfg->priority);\
	MC_CMD_OP(cmd, 0, 40, 8,  uint8_t,  priority);\
	MC_CMD_OP(cmd, 0, 48, 4,  enum dpci_dest, dest_cfg->type);\
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, rx_user_ctx);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_RSP_GET_LINK_STATE(cmd, up) \
	MC_RSP_OP(cmd, 0, 0,  1,  int,	    up)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_RSP_GET_ATTR(cmd, attr) \
do { \
	MC_RSP_OP(cmd, 0, 0,  32, int,	    attr->id);\
	MC_RSP_OP(cmd, 0, 32, 8,  uint8_t,  attr->peer_attached);\
	MC_RSP_OP(cmd, 0, 40, 8,  uint8_t,  attr->peer_id);\
	MC_RSP_OP(cmd, 1, 0,  8,  uint8_t,  attr->num_of_priorities);\
	MC_RSP_OP(cmd, 2, 0,  32, uint32_t, attr->dpci_prio_attr[0].tx_qid);\
	MC_RSP_OP(cmd, 2, 32, 32, uint32_t, attr->dpci_prio_attr[0].rx_qid);\
	MC_RSP_OP(cmd, 3, 0,  32, uint32_t, attr->dpci_prio_attr[1].tx_qid);\
	MC_RSP_OP(cmd, 3, 32, 32, uint32_t, attr->dpci_prio_attr[1].rx_qid);\
	MC_RSP_OP(cmd, 4, 0,  64, uint64_t, \
	          	  	  	  attr->dpci_prio_attr[0].rx_user_ctx);\
	MC_RSP_OP(cmd, 5, 0,  64, uint64_t, \
	          	  	  	  attr->dpci_prio_attr[1].rx_user_ctx);\
	MC_RSP_OP(cmd, 6, 0,  32, uint32_t, attr->version.major);\
	MC_RSP_OP(cmd, 6, 32, 32, uint32_t, attr->version.minor);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_CMD_SET_IRQ(cmd, irq_index, irq_paddr, irq_val, user_irq_id) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  irq_index);\
	MC_CMD_OP(cmd, 0, 32, 32, uint32_t, irq_val);\
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, irq_paddr);\
	MC_CMD_OP(cmd, 2, 0,  32, int,	    user_irq_id); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_CMD_GET_IRQ(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_RSP_GET_IRQ(cmd, type, irq_paddr, irq_val, user_irq_id) \
do { \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, irq_val); \
	MC_RSP_OP(cmd, 1, 0,  64, uint64_t, irq_paddr);\
	MC_RSP_OP(cmd, 2, 0,  32, int,	    user_irq_id); \
	MC_RSP_OP(cmd, 2, 32, 32, int,	    type); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_CMD_SET_IRQ_ENABLE(cmd, irq_index, enable_state) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  enable_state); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_CMD_GET_IRQ_ENABLE(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_RSP_GET_IRQ_ENABLE(cmd, enable_state) \
	MC_RSP_OP(cmd, 0, 0,  8,  uint8_t,  enable_state)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_CMD_SET_IRQ_MASK(cmd, irq_index, mask) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, uint32_t, mask); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_CMD_GET_IRQ_MASK(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_RSP_GET_IRQ_MASK(cmd, mask) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, mask)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_CMD_GET_IRQ_STATUS(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_RSP_GET_IRQ_STATUS(cmd, status) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, status)

/*                cmd, param, offset, width, type, arg_name */
#define DPCI_CMD_CLEAR_IRQ_STATUS(cmd, irq_index, status) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, uint32_t, status); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)

#endif /* _FSL_DPCI_CMD_H */
