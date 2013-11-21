/**************************************************************************//**
@File		fsl_l4.h

@Description	This file contains the AIOP Header Modification - L4 HM API

@Cautions	Please note that the parse results must be updated before
		calling functions defined in this file.

		Copyright 2013 Freescale Semiconductor, Inc.

*//***************************************************************************/

#ifndef AIOP_LIB_L4_HM_OF_H
#define AIOP_LIB_L4_HM_OF_H

/**************************************************************************//**
@addtogroup	FSL_HM FSL_AIOP_Header_Modification

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		HM_L4_Modes HM L4 Modes

@Description	L4 Header Modification Modes

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		HMUDPModeBits UDP header modification mode bits

@{
*//***************************************************************************/

	/** If set, update L4 checksum.*/
#define HM_UDP_MODIFY_MODE_L4_CHECKSUM 0x01
	/** If set, the original UDP Src port will be replaced.*/
#define HM_UDP_MODIFY_MODE_UDPSRC 0x02
	/** If set, the original UDP Dst port will be replaced.*/
#define HM_UDP_MODIFY_MODE_UDPDST 0x04

/* @} end of group HMUDPModeBits */


/**************************************************************************//**
@Group		HMTCPModeBits TCP header modification mode bits

@{
*//***************************************************************************/

	/** If set, update L4 checksum.*/
#define HM_TCP_MODIFY_MODE_L4_CHECKSUM 0x01
	/** If set, the original TCP Src port will be replaced.*/
#define HM_TCP_MODIFY_MODE_TCPSRC 0x02
	/** If set, the original TCP Dst port will be replaced.*/
#define HM_TCP_MODIFY_MODE_TCPDST 0x04
	/** If set, the original acknowledgment number will be updated.
		The tcp_seq_num_delta signed integer will be added/subtracted
		to/from the SeqNum value.*/
#define HM_TCP_MODIFY_MODE_SEQNUM 0x08
	/** If set, the original acknowledgment number will be updated.
		The tcp_seq_num_delta signed integer will be added/subtracted
		to/from the AckNum value.*/
#define HM_TCP_MODIFY_MODE_ACKNUM 0x10
	/** If set, the original maximum segment size will be replaced.*/
#define HM_TCP_MODIFY_MODE_MSS 0x20

/* @} end of group HMTCPModeBits */
/* @} end of group HM_L4_Modes */

/**************************************************************************//**
@Group		FSL_HM_L4_Functions HM L4 related functions

@Description	L4 related Header Modification functions

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	hm_udp_header_modification

@Description	Replace addresses in the UDP header (if exist) of
		a frame. It optionally can update the UDP checksum.

		If the original UDP checksum!= 0, the UDP checksum is
		recalculated based on original checksum and the change in
		relevant header fields. In case the UDP checksum == 0 the
		checksum will be calculated from scratch (a costly operation).


@Param[in]	flags - \link HMUDPModeBits UDP modification mode bits
			\endlink

@Param[in]	udp_src_port - The Src port header to be replaced.
@Param[in]	udp_dst_port - The Dst port header to be replaced.

@Return		Success or Failure (There was no UDP header in the frame).

@Cautions	The parse results must be updated before calling this
		operation.
*//***************************************************************************/
int32_t hm_udp_header_modification(uint8_t flags,
		uint16_t udp_src_port, uint16_t udp_dst_port);


/*************************************************************************//**
@Function	hm_tcp_header_modification

@Description	Replace fields in the TCP header (if exist) of a frame.

		The TCP checksum is recalculated based on original checksum
		and the change in relevant header fields.

		This function takes care of wrapping around of seq/ack numbers.

@Param[in]	flags - \link HMTCPModeBits TCP modification mode bits
			\endlink

@Param[in]	tcp_src_port - The Src port header to be replaced.
@Param[in]	tcp_dst_port - The Dst port header to be replaced.
@Param[in]	tcp_seq_num_delta - This signed integer will be
		added/subtracted to/from the SeqNum value.
@Param[in]	tcp_ack_num_delta - This signed integer will be
		added/subtracted to/from the AckNum value.
@Param[in]	tcp_mss - The MSS header to be replaced.

@Return		Success or Failure
		Failure in case:\n
		1. There was no TCP header in the frame.\n
		2. in case the MSS was needed to be replaced and no MSS was
		found an error will be returned. In this case all the rest of
		the fields except the MSS will be replaced correctly and, if
		requested, the TCP checksum will be calculated.

@Cautions	The parse results must be updated before
		calling this operation.
*//***************************************************************************/
int32_t hm_tcp_header_modification(uint8_t flags, uint16_t tcp_src_port,
		uint16_t tcp_dst_port, int16_t tcp_seq_num_delta,
		int16_t tcp_ack_num_delta, uint16_t tcp_mss);

/*************************************************************************//**
@Function	hm_set_tp_src

@Description	Replace TCP/UDP source port. The UDP/TCP checksum is updated
		automatically.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	src_port - The new TCP/UDP source port.

@Return		Success or Failure (There was no TCP/UDP header in the frame).

@Cautions	The parse results must be updated before calling this
		operation.\n
		This function assumes the original TCP header checksum is valid.
*//***************************************************************************/
int32_t hm_set_tp_src(uint16_t src_port);


/*************************************************************************//**
@Function	hm_set_tp_dst

@Description	Replace TCP/UDP destination port. The UDP/TCP checksum is
		updated automatically.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	dst_port - The new TCP/UDP destination port.

@Return		Success or Failure (There was no TCP/UDP header in the frame).

@Cautions	The parse results must be updated before calling this
		operation.\n
		This function assumes the original TCP header checksum is valid.

*//***************************************************************************/
int32_t hm_set_tp_dst(uint16_t dst_port);


/* @} end of group FSL_HM_L4_Functions */
/* @} end of group FSL_HM */

#endif /* AIOP_LIB_L4_HM_OF_H */
