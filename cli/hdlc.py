import time
import serial


class HDLCDriver:

    # hdlc start and end flag
    START_FLAG = 0x7E
    END_FLAG = 0x7F
    STUFFING_BYTE = 0x7D
    STUFFING_MASK = 0x20

    # crc polynomial for crc_ccitt
    HDLC_L_CRC_SEED = 0xFFFF
    HDLC_L_CRC_POLY = 0x1021

    # crc polynomial for crc_ccitt
    HDLC_L_CLEAR = 0x00
    HDLC_L_INVALID_PACKET_SIZE = 0x01
    HDLC_L_CRC_MISMATCH = 0x02
    HDLC_L_INVALID_CONTROL = 0x03
    HDLC_L_RECEIVE_OVERFLOW = 0x04
    HDLC_L_TRANSMIT_OVERFLOW = 0x05
    HDLC_L_CONTROL_FIELD_NUMBER = 6

    def __init__(self, usb_serial_obj: serial.Serial, byte_rate: int, max_attempts: int):

        self.usb_serial_obj: serial.Serial = usb_serial_obj
        self.byte_rate: int = byte_rate
        self.max_attempts = max_attempts

    def send_single_byte(self, single_byte):
        """Send single byte over usb"""
        self.usb_serial_obj.write(single_byte.to_bytes(1, "big"))
        time.sleep(1/self.byte_rate)

    def send_single_byte_with_stuffing(self, single_byte):

        if single_byte in [self.START_FLAG, self.END_FLAG, self.STUFFING_BYTE]:
            self.send_single_byte(self.STUFFING_BYTE)
            self.send_single_byte(single_byte ^ self.STUFFING_MASK)
        else:
            self.send_single_byte(single_byte)

    def destuff(self, received_bytes):

        destuffed_received_bytes = []
        stuffed_byte = False
        for received_byte in received_bytes:
            if stuffed_byte:
                destuffed_received_bytes.append(received_byte ^ self.STUFFING_MASK)
                stuffed_byte = False
            elif received_byte == self.STUFFING_BYTE:
                stuffed_byte = True
            else:
                destuffed_received_bytes.append(received_byte)

        return destuffed_received_bytes

    def send_packet_with_response(self, send_bytes, attempt=1, crc_mismatch=False):

        if attempt == self.max_attempts:
            print("Max attempts reached. Lamp command dropped")
            return None

        if not crc_mismatch:
            self.send_information([self.HDLC_L_CLEAR] + send_bytes)
        else:
            self.send_information([self.HDLC_L_CRC_MISMATCH])

        packet_received, received_bytes = self.receive_information()

        if not packet_received:
            print("CLI: No packet received from lamp")
            return None

        elif len(received_bytes) < 3:
            print("CLI: Lamp messages has less than 3 bytes")
            return None

        received_bytes = self.destuff(received_bytes)

        # split received bytes into their fields
        control_byte = received_bytes[0]
        data_bytes = received_bytes[1:-2]
        crc1, crc0 = received_bytes[-2:]

        # Compute CRC of received data bytes
        crc1_c, crc0_c = self.crc16_ccitt(received_bytes[0:-2])

        if crc1 != crc1_c or crc0 != crc0_c:
            print("CLI: crc mismatch, sending crc_mismatch to lamp to ask for retransmission.")
            print(f"Attempt number: {attempt+1}/{self.max_attempts}")
            return self.send_packet_with_response(send_bytes, attempt+1, crc_mismatch=True)
        elif control_byte >= self.HDLC_L_CONTROL_FIELD_NUMBER:
            print("LAMP: Control field has non-defined value")
            return None
        elif control_byte == self.HDLC_L_INVALID_CONTROL:
            print("LAMP: Invalid control for sent packet")
            return None
        elif control_byte == self.HDLC_L_INVALID_PACKET_SIZE:
            print("LAMP: Invalid packet size for sent packet")
            return None
        elif control_byte == self.HDLC_L_RECEIVE_OVERFLOW:
            print("LAMP: Receive buffer overflow")
            return None
        elif control_byte == self.HDLC_L_TRANSMIT_OVERFLOW:
            print("LAMP: Transmit buffer overflow")
            return None
        elif control_byte == self.HDLC_L_CRC_MISMATCH:
            print("LAMP: crc mismatch, retransmit")
            if crc_mismatch:
                print("Double CRC_MISMATCH error, dropping lamp command")
                return None
            else:
                print(f"Retransmitting, attempt number: {attempt+1}/{self.max_attempts}")
                return self.send_packet_with_response(send_bytes, attempt + 1)

        return data_bytes

    def send_information(self, info_bytes):

        # send flag
        self.send_single_byte(self.START_FLAG)

        # send info bytes
        for info_byte in info_bytes:
            self.send_single_byte_with_stuffing(info_byte)

        # Get the crc as the fcs field
        crc1, crc0 = self.crc16_ccitt(info_bytes)
        self.send_single_byte_with_stuffing(crc1)
        self.send_single_byte_with_stuffing(crc0)

        # send flag
        self.send_single_byte(self.END_FLAG)

    def receive_information(self):

        info_bytes = []

        packet_received = False

        start_found = False
        while True:
            single_byte = self.usb_serial_obj.read()
            if len(single_byte) == 0:
                break

            single_byte = int(single_byte[0])

            if start_found:
                if single_byte == self.END_FLAG:
                    packet_received = True
                    break
                else:
                    info_bytes.append(single_byte)
            else:
                if single_byte == self.START_FLAG:
                    start_found = True

        return packet_received, info_bytes

    def crc16_ccitt(self, info_bytes):

        # crc seed
        crc_acc = self.HDLC_L_CRC_SEED

        # crc each byte of info
        for info_byte in info_bytes:

            crc_acc = crc_acc ^ (info_byte << 8)

            for i in range(8):
                if crc_acc & 1 << 15:
                    crc_acc = crc_acc << 1
                    crc_acc ^= self.HDLC_L_CRC_POLY
                else:
                    crc_acc = crc_acc << 1

        # get byte 1 (LSB) and byte 0 (LSB)
        return (crc_acc // (1 << 8)) % (1 << 8), crc_acc % (1 << 8)
