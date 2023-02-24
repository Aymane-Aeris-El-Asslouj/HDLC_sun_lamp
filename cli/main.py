import serial
import time
from typing import List
from hdlc import HDLCDriver

MAX_ATTEMPTS = 5
BYTE_RATE = 100

def open_lamp_usb_communication():

    while True:
        print("Connecting to lamp over USB...")
        try:
            usb_serial_obj =  serial.Serial('COM3', 9600, timeout=0.1, parity=serial.PARITY_NONE,
                                stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
            print("Connected to lamp over USB.")
            break
        except serial.serialutil.SerialException:
            print("USB port not found, will reattempt in 5 seconds.\n")
            time.sleep(5)

    return usb_serial_obj



lamp_commands_table = {
    # name : (opcode, number_of_arguments)
    "set_lightness": (1, 1)
}

response_table = [
    "SUCCESSFUL",
    "INVALID_ARGUMENTS",
    "UNSUCCESSFUL",
    "NO_COMMAND",
    "UNKNOWN_COMMAND",
    "INCORRECT_NUMBER_OF_ARGUMENTS"
]

def send_lamp_command(hdlc_driver: HDLCDriver, args: List[str], mute=False):

    # check if command name is present
    if len(args) == 0:
        print("lamp command name missing")
        return

    # check if command name is valid
    command_name = args[0]
    if command_name not in lamp_commands_table:
        print("Undefined lamp command.")
        return

    # get table for command
    lamp_command_data = lamp_commands_table[command_name]

    # convert command name to opcode
    args[0]: str = str(lamp_command_data[0])

    # Check if function has right number of arguments
    arg_num = lamp_command_data[1]
    if len(args[1:]) != arg_num:
        print(f"This lamp command takes {arg_num} argument(s)")
        return

    # check if arguments are integer strings
    if any([(not arg.isdigit()) for arg in args]):
        print("Only integer arguments are allowed for lamp commands")
        return

    # convert arguments to integers
    args: List[int] = [int(arg) for arg in args]

    # Check if arguments are byte-sized
    if any([(arg > 255) for arg in args]):
        print("Only integer arguments smaller than 255 are allowed for lamp commands")
        return

    # Send arguments
    received_info_bytes = hdlc_driver.send_packet_with_response(args)

    if received_info_bytes is not None and (not mute):

        if len(received_info_bytes) == 0:
            print("LAMP: ---")
        elif len(received_info_bytes) == 1:
            print(f"LAMP: {response_table[received_info_bytes[0]]}")
        else:
            print(f"LAMP: {response_table[received_info_bytes[0]]} "
                  f"{[hex(arg_byte) for arg_byte in received_info_bytes[1:]]}")




def ramp_brightness(hdlc_driver, args):

    while True:
        for num in range(0,255, 10):
            send_lamp_command(hdlc_driver, ["set_lightness", str(num)], mute=True)
            time.sleep(0.04)
        for num in range(255,0, -10):
            send_lamp_command(hdlc_driver, ["set_lightness", str(num)], mute=True)
            time.sleep(0.04)


script_table = {
    "ramp": (ramp_brightness, 0)
}

def start_script(hdlc_driver: HDLCDriver, args: List[str]):

    # check if script name is present
    if len(args) == 0:
        print("script name missing")
        return

    # check if script name is valid
    script_name = args[0]
    if script_name not in script_table:
        print("Undefined script.")
        return

    # get table for command
    script_data = script_table[script_name]

    # Check if script has right number of arguments
    arg_num = script_data[1]
    if len(args[1:]) != arg_num:
        print(f"Script takes {arg_num} argument(s)")
        return

    script_data[0](hdlc_driver, args)


def command_line_interface():

    # open a connection
    usb_serial_obj = open_lamp_usb_communication()

    hdlc_driver = HDLCDriver(usb_serial_obj, BYTE_RATE, MAX_ATTEMPTS)

    # connection loop
    while True:

        # get command from user
        command = input(">>> ")
        #command = "lamp set_lightness 50" # input(">>> ")
        args = command.split(" ")
        args = list(filter(lambda x: x != "", args))

        # call command with arguments
        if len(args) == 0:
            continue
        if args[0] == "exit":
            print("Exiting...")
            exit(0)
        elif args[0] == "lamp":
            send_lamp_command(hdlc_driver, args[1:])
        elif args[0] == "script":
            start_script(hdlc_driver, args[1:])
        else:
            print("Unknown command.")

        #break


if __name__ == '__main__':

    command_line_interface()
