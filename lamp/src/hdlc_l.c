#include <SI_EFM8BB52_Register_Enums.h>
#include "hdlc_l.h"
#include "state_machine.h"


#define HDLC_L_START_FLAG 0x7E
#define HDLC_L_END_FLAG 0x7F
#define STUFFING_BYTE 0x7D
#define STUFFING_MASK 0x20

#define HDLC_L_CRC_SEED 0xFFFF
#define HDLC_L_CRC_POLY 0x1021
#define HDLC_L_PACKET_MIN_SIZE 3 // Fixed by protocol (control byte + crc bytes)
#define INFO_FIELD_MAX_SIZE 10 // Variable
#define PACKET_MAX_SIZE HDLC_L_PACKET_MIN_SIZE + INFO_FIELD_MAX_SIZE

#define NO_RECEPTION 0xFF

typedef enum {
    HDLC_L_CLEAR,
    HDLC_L_INVALID_PACKET_SIZE,
    HDLC_L_CRC_MISMATCH,
    HDLC_L_INVALID_CONTROL,
    HDLC_L_RECEIVE_OVERFLOW,
    HDLC_L_TRANSMIT_OVERFLOW
} hdlc_l_control_field_values;




uint8_t in_packet_index = NO_RECEPTION; // Current index of in_packet It is 0xFF till start flag is seen
uint8_t in_packet_size = 0; // Size of in_packet
uint8_t xdata in_packet[PACKET_MAX_SIZE]; // Packet being received
uint8_t receive_overflow_flag = 0; // in_packet overflowed

uint8_t out_packet_index = 0; // Current index of out_packet
uint8_t out_packet_size = 0; // Size of out_packet
uint8_t xdata out_packet[PACKET_MAX_SIZE]; // Packet being transmitted
uint8_t start_flag_sent = 0; // Start flag was sent
uint8_t sending_byte_to_be_stuffed = 0; // current next byte is to be stuffed
uint8_t end_flag_sent = 0; // End flag was sent

/*
 Computes the CRC of an array of bytes using the same algorithm used by the
 CRC module of the EFM8BB52. CRC16-CCITT with seed 0xFFFF, and polynomial 0x1021
 with no reflection of xor or input or output.
 */
uint16_t hdlc_l_crc_computation(uint8_t array[], uint8_t array_size){
    uint16_t crc_value = HDLC_L_CRC_SEED;
    uint8_t CRC_input;

    uint8_t array_byte; // array loop counter
    uint8_t i; // bit loop counter

    // Update CRC with each byte in the array
    for(array_byte = 0; array_byte < array_size; array_byte++){
        CRC_input = array[array_byte];

        crc_value = crc_value ^ (CRC_input << 8);

        // Update CRC with each bit of the input byte
        for (i = 0; i < 8; i++){

          // Check if the MSB bit of crc_value is 1 to see if it can be
          // divided by the polynomial
          if (crc_value & (1 << 15)){
              // Shift then divide
              crc_value = crc_value << 1;
              crc_value ^= HDLC_L_CRC_POLY;
          }
          else {
              // Shift only
              crc_value = crc_value << 1;
          }
        }
    }

    return crc_value;
}

// Compute crc of an hdlc_l_packet over control and info fields and check if it
// does not match the value within the packet
uint8_t hdlc_l_check_packet_crc_mismatch(uint8_t packet[], uint8_t packet_size){

  uint16_t packet_crc_computed = hdlc_l_crc_computation(packet, packet_size-2);

  uint16_t packet_crc = (packet[packet_size-2] << 8) + packet[packet_size-1];

  return (packet_crc_computed != packet_crc);
}

// Start transmission of hdlc_l packet from start
void hdlc_l_transmit(){
  // Put index of transmission at start of out_packet
  out_packet_index = 0;
  start_flag_sent = 0;
  end_flag_sent = 0;
  // Trigger transmission by raising the UART0 transmission flag
  SBUF0 |= SCON0_TI__BMASK;
}

// Take the control and info fields of a packet and add the rest to make it a full
// packet before transmission (start/end flags and CRC)
void hdlc_l_pack_and_send_packet(){
  uint16_t packet_crc;
  // Compute and add the crc to the packet
  packet_crc = hdlc_l_crc_computation(out_packet, out_packet_size);
  out_packet[out_packet_size] = packet_crc / (1 << 8);
  out_packet[out_packet_size + 1] = packet_crc % (1 << 8);
  out_packet_size += 2;
  // Start transmission of out_packet
  hdlc_l_transmit();
}

// Form an info packet out of an array of info bytes
void hdlc_l_send_info_packet(uint8_t send_info_bytes[], uint8_t send_info_bytes_size){
  uint8_t i;

  // Discard packet if it would lead to buffer overflow
  if (send_info_bytes_size > INFO_FIELD_MAX_SIZE){
      hdlc_l_send_control_packet(HDLC_L_TRANSMIT_OVERFLOW);
      return;
  }

  // Load control byte into packet
  out_packet[0] = HDLC_L_CLEAR;
  // Load info bytes into packet
  for(i = 0; i < send_info_bytes_size; i++)
    out_packet[i+1] = send_info_bytes[i];
  out_packet_size = send_info_bytes_size+1;
  hdlc_l_pack_and_send_packet();
}

// Form a control packet out of a control byte
void hdlc_l_send_control_packet(uint8_t control_byte){
  // Load control byte into packet
  out_packet[0] = control_byte;
  out_packet_size = 1;
  hdlc_l_pack_and_send_packet();
}


// Make a copy of in_packet's info field into an array
void hdlc_l_load_received_info_bytes(uint8_t received_info_bytes[] , uint8_t* received_info_bytes_size){
  int i;
  *received_info_bytes_size = in_packet_size-HDLC_L_PACKET_MIN_SIZE;

  // Get info bytes from packet
  for(i = 0; i < *received_info_bytes_size; i++)
    received_info_bytes[i] = in_packet[i+1];

}

void destuff_in_packet(){
  // Start destuffing packet data starting after start flag with two cursors
  uint8_t i = 0;
  uint8_t j;

  // Run j cursor as an index over packet
  for(j = 0; j < in_packet_size; j++){
      // If stuffing byte found, move j cursor ahead to fetch
      // stuffed byte and destuff it. This reduces the packet size by 1
      if (in_packet[j] == STUFFING_BYTE)
          in_packet[i] = in_packet[++j] ^ STUFFING_MASK;
      else
          in_packet[i] = in_packet[j];
      // Move cursors
      i++;
  }
  // Update in_packet size to where i cursor ends
  in_packet_size = i;
}


// HDLC-L communication protocol over UART0
SI_INTERRUPT (UART0_ISR, UART0_IRQn)
  {
  uint8_t received_packet_byte;
  uint8_t control_byte;
  uint8_t sending_byte;

  // Change SFR page to that of UART0
  uint8_t SFRPAGE_save = SFRPAGE;
  SFRPAGE = 0x00;

  // Interrupt reason: byte transmitted
  if ((SCON0 & SCON0_TI__BMASK)){
      // Lower transmission flag
      SCON0 &= ~SCON0_TI__BMASK;

      // Check if start flag has not been sent yet
      if (!start_flag_sent){
          start_flag_sent = 1;
          SBUF0 = HDLC_L_START_FLAG;
      }

      // Check if there is still a byte to send in the packet
      else if (out_packet_index < out_packet_size){

          sending_byte = out_packet[out_packet_index];

          // Check if there is a need for stuffing. This happens when the byte
          // to be transmitted is not a start or end flag, and it has the same value
          // as a start or end flag or a stuffing byte. And it has not already been
          // stuffed
          if ((out_packet_index !=0) && (out_packet_index != out_packet_size-1) &&
              ((sending_byte == HDLC_L_START_FLAG) ||
                  (sending_byte == HDLC_L_END_FLAG) ||
                  (sending_byte == STUFFING_BYTE) )){
              if(!sending_byte_to_be_stuffed){
                  // Marking byte to be stuffed and sending stuffing byte
                  sending_byte_to_be_stuffed = 1;
                  SBUF0 = STUFFING_BYTE;
              }
              else {
                  // Removing mark for stuffing and sending stuffed byte
                  sending_byte_to_be_stuffed = 0;
                  SBUF0 = sending_byte ^ STUFFING_MASK;
                  // Increment out packet index
                  out_packet_index += 1;
              }
          }
          else{
              // Send next out packet byte over UART0
              SBUF0 = sending_byte;
              // Increment out packet index
              out_packet_index += 1;
          }

      }
      // Check if end flag has not been sent yet
      else if (!end_flag_sent){
          end_flag_sent = 1;
          SBUF0 = HDLC_L_END_FLAG;
      }
  }

  // Interrupt reason: byte received
  else{
      // Get the received byte
      received_packet_byte = SBUF0;

      // Receiving a byte discontinues transmission
      out_packet_size = 0;

      // Discard packet (reset reading) if it would lead to buffer overflow and raise the
      // receive buffer overflow flag so a message is sent about it
      if (in_packet_index == PACKET_MAX_SIZE){
          in_packet_index = 0;
          receive_overflow_flag = 1;
      }

      // If start flag, reset reception buffer
      if (received_packet_byte == HDLC_L_START_FLAG){
          in_packet_index = 0;
      }
      // If start flag already seen
      else if (in_packet_index != NO_RECEPTION){

          // If end flag, finalize reception
          if (received_packet_byte == HDLC_L_END_FLAG){
              // Store size of in_packet
              in_packet_size = in_packet_index;
              // Stop reception
              in_packet_index = NO_RECEPTION;
              // Check if receive buffer overflowed
              if (receive_overflow_flag){
                  hdlc_l_send_control_packet(HDLC_L_RECEIVE_OVERFLOW);
                  receive_overflow_flag = 0;
              }
              // Check if packet does not have the minimum size
              else if (in_packet_size < HDLC_L_PACKET_MIN_SIZE){
                  hdlc_l_send_control_packet(HDLC_L_INVALID_PACKET_SIZE);
              }
              else{
                  // Destuff bytes of packet
                  destuff_in_packet();
                  control_byte = in_packet[0];
                  // Check if there is a CRC mismatch (asking for retransmission)
                  if (hdlc_l_check_packet_crc_mismatch(in_packet, in_packet_size)){
                      hdlc_l_send_control_packet(HDLC_L_CRC_MISMATCH);
                  }
                  // Check if the control field has invalid values
                  else if (control_byte != HDLC_L_CLEAR && control_byte != HDLC_L_CRC_MISMATCH){
                      hdlc_l_send_control_packet(HDLC_L_INVALID_CONTROL);
                  }
                  // Check if the control field is asking for a retransmission due to CRC mismatch
                  else if (control_byte == HDLC_L_CRC_MISMATCH){
                      // Start retransmission of out_packet
                      hdlc_l_transmit();
                  }
                  // Packet data is ready, forward command to state machine's command handler
                  // and disable UART0 interrupt to avoid modifying in_packet or using out_packet
                  // while the main state machine is modifying them
                  else{
                      IE &= ~IE_ES0__BMASK; // IE is on all SFR pages, so no need to change the SFR page
                      current_event = EV_COMMAND;
                  }
              }

          }
          else {
              // Add received byte to in_packet and increment in_packet index
              in_packet[in_packet_index] = received_packet_byte;
              in_packet_index += 1;
          }
      }
  }

    // Restore SFR page
    SFRPAGE = SFRPAGE_save;
  }
