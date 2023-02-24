#ifndef INC_HDLC_L_H_
#define INC_HDLC_L_H_


uint16_t hdlc_l_crc_computation(uint8_t array[], uint8_t array_size);
uint16_t hdlc_l_compute_packet_crc(uint8_t packet[], uint8_t packet_size);
uint8_t hdlc_l_check_packet_crc_mismatch(uint8_t packet[], uint8_t packet_size);
void hdlc_l_transmit();
void hdlc_l_pack_and_send_packet();
void hdlc_l_send_info_packet(uint8_t send_info_bytes[], uint8_t send_info_bytes_size);
void hdlc_l_send_control_packet(uint8_t control_field);
void hdlc_l_load_received_info_bytes(uint8_t received_info_bytes[] , uint8_t* received_info_bytes_size);

#endif /* INC_HDLC_L_H_ */
