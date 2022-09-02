import serial
import struct
from time import sleep

errors = [ 'NO ERROR', 'ERROR CRC', 'ERROR CMD', 'ERROR ID', 'ERROR READ', 'ERROR WRITE', 'INTERNAL ERROR' ]

register_name = [
    'TORQUE', 'POS_SP', 'POS_FB', 'POS_ACC', 'POS_ACC_THRES', 'POS_PERR', 'POS_Kp', 'POS_Ki', 'POS_Kd', 'POS_ACTIVE',
              'SPD_SP', 'SPD_FB', 'SPD_ACC', 'SPD_ACC_THRES', 'SPD_PERR', 'SPD_Kp', 'SPD_Ki', 'SPD_Kd', 'SPD_ACTIVE',
              'CUR_SP', 'CUR_FB', 'CUR_ACC', 'CUR_ACC_THRES', 'CUR_PERR', 'CUR_Kp', 'CUR_Ki', 'CUR_Kd', 'CUR_ACTIVE',
              'OUTPUT', 'OUTPUT_THRES'
]

BP_CMD_PING             = 0x01
BP_ERROR_UNKNOWN_CMD    = 0xE0
BP_CMD_CONTROL          = 0xF0

ID_DRIVE_1      = 0
ID_DRIVE_2      = 1
ID_DRIVE_3      = 2
ID_DRIVE_4      = 3
ID_GLOBAL       = 4
ID_GLOBAL_CMD   = 5

WRITE = 1
READ =  2

TORQUE          = 0
POS_SP          = 1
POS_FB          = 2
POS_ACC         = 3
POS_ACC_THRES   = 4
POS_PERR        = 5
POS_Kp          = 6
POS_Ki          = 7
POS_Kd          = 8
POS_ACTIVE      = 9
SPD_SP          = 10
SPD_FB          = 11
SPD_ACC         = 12
SPD_ACC_THRES   = 13
SPD_PERR        = 14
SPD_Kp          = 15
SPD_Ki          = 16
SPD_Kd          = 17
SPD_ACTIVE      = 18
CUR_SP          = 19
CUR_FB          = 20
CUR_ACC         = 21
CUR_ACC_THRES   = 22
CUR_PERR        = 23
CUR_Kp          = 24
CUR_Ki          = 25
CUR_Kd          = 26
CUR_ACTIVE      = 27
OUTPUT          = 28
OUTPUT_THRES    = 29

ser = serial.Serial('/dev/ttyUSB0', 57600)
ser.isOpen()

def send_multiple(cnt_id, cnt_cmd, reg, val1, val2, val3, val4):
    ser.write(struct.pack('<BIIIIffff', BP_CMD_CONTROL, 7, id, cmd, reg, val1, val2, val3, val4))

    if (cnt_cmd == READ):
        rx_data = ser.read(5+8+20)
        bp_id, bp_cmd, cnt_id, cnt_cmd, reg, data1, data2, data3, data4, crc = struct.unpack('<BIIIIffff', rx_data)
        print('MULTIPLE READ CMD')
        # print('\tID: ', cnt_id)
        # print('\tSTATUS: ', errors[status])
        print('\tREGISTER: ', register_name[reg])
        print('\tDATA: ', data1, data2, data3, data4)
        # print('\tCRC: ', crc)
        print('\n')
    else:
        rx_data = ser.read(5+8+20)
        bp_id, bp_cmd, cnt_id, cnt_cmd, reg, data1, data2, data3, data4, crc = struct.unpack('<BIIIIffff', rx_data)
        print('\tREGISTER: ', register_name[reg])
        print('\tDATA: ', data1, data2, data3, data4)
        print('\n')

def send_single(cnt_id, cnt_cmd, reg, val):
    ser.write(struct.pack('<BIIIIf', BP_CMD_CONTROL, 4, cnt_id, cnt_cmd, reg, val))
    if (cnt_cmd == READ):
        rx_data = ser.read(5+8+8)
        bp_id, bp_cmd, cnt_id, cnt_cmd, reg, data = struct.unpack('<BIIIIf', rx_data)
        print('DATA: ', bp_id, bp_cmd, cnt_id, cnt_cmd, reg, data)
        print('SINGLE READ CMD')
        # print('\tID: ', id)
        # print('\tSTATUS: ', errors[status])
        print('\tREGISTER: ', register_name[reg])
        print('\tDATA: ', data)
        # print('\tCRC: ', crc)
        print('\n')
    else:
        rx_data = ser.read(5+8+8)
        bp_id, bp_cmd, cnt_id, cnt_cmd, reg, data = struct.unpack('<BIIIIf', rx_data)
        print('SINGLE WRITE CMD')
        print('\tREGISTER: ', register_name[reg])
        print('\tDATA: ', data)
        print('\n') 


# from distutils.log import error
# from xmlrpc.client import INVALID_ENCODING_CHAR
# import serial
# import struct
# from time import sleep
# from enum import Enum

# class WC_Error(Enum):
#     NO_ERROR          = 0
#     WRONG_CRC         = 1
#     WRONG_CMD         = 2
#     WRONG_ID          = 3
#     FAILED_READ       = 4
#     FAILED_WRITE      = 5
#     INTERNAL_ERROR    = 6

# class WC_ID(Enum):
#     ID_BEGIN             = -1
#     SINGLE_DRIVE_1       = 0
#     SINGLE_DRIVE_2       = 1
#     SINGLE_DRIVE_3       = 2
#     SINGLE_DRIVE_4       = 3
#     ALL_DRIVE            = 4
#     GLOBAL_CMD           = 5
#     ID_END               = 6

# class WC_CMD(Enum):
#     WRITE_REGISTER              = 1
#     READ_REGISTER               = 2
#     BLOCK_DRIVE                 = 3
#     ENABLE_DRIVE                = 5
#     RESET_SYSTEM                = 7
#     EXIT_APP                    = 8

# class WC_Register(Enum):
#     REGISTER_BEGIN                          = -1
#     TORQUE_INPUT                            = 0
#     POSITIION_LOOP_SETPOINT                 = 1
#     POSITIION_LOOP_FEEDBACK                 = 2
#     POSITIION_LOOP_ACCUMULATOR              = 3
#     POSITIION_LOOP_ACCUMULATOR_THRESHOLD    = 4
#     POSITIION_LOOP_PREVIOUS_ERROR           = 5
#     POSITIION_LOOP_Kp                       = 6
#     POSITIION_LOOP_Ki                       = 7
#     POSITIION_LOOP_Kd                       = 8
#     POSITIION_LOOP_IS_ACTIVE                = 9
#     SPEED_LOOP_SETPOINT                     = 10
#     SPEED_LOOP_FEEDBACK                     = 11
#     SPEED_LOOP_ACCUMULATOR                  = 12
#     SPEED_LOOP_ACCUMULATOR_THRESHOLD        = 13
#     SPEED_LOOP_PREVIOUS_ERROR               = 14
#     SPEED_LOOP_Kp                           = 15
#     SPEED_LOOP_Ki                           = 16
#     SPEED_LOOP_Kd                           = 17
#     SPEED_LOOP_IS_ACTIVE                    = 18
#     CURRENT_LOOP_SETPOINT                   = 19
#     CURRENT_LOOP_FEEDBACK                   = 20
#     CURRENT_LOOP_ACCUMULATOR                = 21
#     CURRENT_LOOP_ACCUMULATOR_THRESHOLD      = 22
#     CURRENT_LOOP_PREVIOUS_ERROR             = 23
#     CURRENT_LOOP_Kp                         = 24
#     CURRENT_LOOP_Ki                         = 25
#     CURRENT_LOOP_Kd                         = 26
#     CURRENT_LOOP_IS_ACTIVE                  = 27
#     OUTPUT                                  = 28
#     OUTPUT_THRESHOLD                        = 29
#     REGISTER_END                            = 30

# class WC_device:

#     __error_names = [
#         'NO ERROR', 'ERROR CRC', 'ERROR CMD', 'ERROR ID', 'ERROR READ', 'ERROR WRITE', 'INTERNAL ERROR'
#     ]
#     __register_names = {
#         'TORQUE', 'POS_SP', 'POS_FB', 'POS_ACC', 'POS_ACC_THRES', 'POS_PERR', 'POS_Kp', 'POS_Ki', 'POS_Kd', 'POS_ACTIVE',
#               'SPD_SP', 'SPD_FB', 'SPD_ACC', 'SPD_ACC_THRES', 'SPD_PERR', 'SPD_Kp', 'SPD_Ki', 'SPD_Kd', 'SPD_ACTIVE',
#               'CUR_SP', 'CUR_FB', 'CUR_ACC', 'CUR_ACC_THRES', 'CUR_PERR', 'CUR_Kp', 'CUR_Ki', 'CUR_Kd', 'CUR_ACTIVE',
#               'OUTPUT', 'OUTPUT_THRES'
#     }

#     __port_name = "/dev/ttyUSB0"
#     __baudrate  = 57600
#     __serial = 0


#     def __init__(self, port, baud):
#         self.__port_name = port
#         self.__baudrate  = baud
#         self.__serial = serial.Serial(self.__port_name, self.__baudrate)
#         self.__serial.isOpen()

#     def get_error_string(self, error_id):
#         return self.__error_names[error_id]

#     def write_single_reg(self, drive_id, register_num, value):
#         print('write_single_reg')
#         if ((register_num > WC_Register.REGISTER_BEGIN) and (register_num < WC_Register.REGISTER_END)):
#             if ((drive_id > WC_ID.ID_BEGIN) and (drive_id < WC_ID.ALL_DRIVE)):
#                 self.__serial.write(struct.pack('<BBBfB', drive_id, WC_CMD.WRITE_REGISTER, register_num, value, 0))
#                 rx_data = self.__serial.read(2)
#                 id, status = struct.unpack('<BB', rx_data)
#                 return status
#             else:
#                 return WC_Error.WRONG_ID
#         else:
#             return WC_Error.FAILED_WRITE

#     def write_multiple_reg(self, register_num, values_list):
#         print('write_multiple_reg')
#         if ((register_num > WC_Register.REGISTER_BEGIN) and (register_num < WC_Register.REGISTER_END)):
#             if (len(values_list) == 4):
#                 self.__serial.write(struct.pack('<BBBffffB', WC_ID.ALL_DRIVE, WC_CMD.WRITE_REGISTER, register_num, values_list[0], values_list[1], values_list[2], values_list[3], 0))
#                 rx_data = self.__serial.read(2)
#                 id, status = struct.unpack('<BB', rx_data)
#                 return status
#             else:
#                 return WC_Error.FAILED_WRITE
#         else:
#             return WC_Error.FAILED_WRITE

#     def read_single_reg(self, drive_id, register_num):
#         print('read_single_reg')
#         if ((register_num > WC_Register.REGISTER_BEGIN) and (register_num < WC_Register.REGISTER_END)):
#             if ((drive_id > WC_ID.ID_BEGIN) and (drive_id < WC_ID.ALL_DRIVE)):
#                 self.__serial.write(struct.pack('<BBBfB', drive_id, WC_CMD.READ_REGISTER, register_num, 0, 0))
#                 rx_data = self.__serial.read(8)
#                 id, status, reg, data, crc = struct.unpack('<BBBfB', rx_data)
#                 return status, data
#             else:
#                 return WC_Error.WRONG_ID, 0
#         else:
#             return WC_Error.FAILED_WRITE, 0

#     def read_multiple_reg(self, register_num):
#         print('read_multiple_reg')
#         if ((register_num > WC_Register.REGISTER_BEGIN) and (register_num < WC_Register.REGISTER_END)):
#             self.__serial.write(struct.pack('<BBBffffB', WC_ID.ALL_DRIVE, WC_CMD.READ_REGISTER, register_num, 0, 0, 0, 0, 0))
#             rx_data = self.__serial.read(20)
#             id, status, reg, value_0, value_1, value_2, value_3, crc = struct.unpack('<BBBffffB', rx_data)
#             return status, [value_0, value_1, value_2, value_3]
#         else:
#             return WC_Error.FAILED_WRITE, [0, 0, 0, 0]




# ser = serial.Serial('/dev/ttyUSB0', 57600)
# ser.isOpen()

# def send_multiple(id, cmd, reg, val1, val2, val3, val4, crc):
#     ser.write(struct.pack('<BBBffffB', id, cmd, reg, val1, val2, val3, val4, crc))
    
#     if (cmd == READ):
#         rx_data = ser.read(20)
#         id, status, reg, data1, data2, data3, data4, crc = struct.unpack('<BBBffffB', rx_data)
#         print('MULTIPLE READ CMD')
#         print('\tID: ', id)
#         print('\tSTATUS: ', errors[status])
#         print('\tREGISTER: ', register_name[reg])
#         print('\tDATA: ', data1, data2, data3, data4)
#         print('\tCRC: ', crc)
#         print('\n')
#     else:
#         rx_data = ser.read(2)
#         id, status = struct.unpack('<BB', rx_data)
#         print('MULTIPLE WRITE CMD')
#         print('\tID: ', id)
#         print('\tSTATUS: ', errors[status])
#         print('\n')

# def send_single(id, cmd, reg, val, crc):
#     ser.write(struct.pack('<BBBfB', id, cmd, reg, val, crc))
#     if (cmd == READ):
#         rx_data = ser.read(8)
#         id, status, reg, data, crc = struct.unpack('<BBBfB', rx_data)
#         # print('SINGLE READ CMD')
#         # print('\tID: ', id)
#         # print('\tSTATUS: ', errors[status])
#         print('\tREGISTER: ', register_name[reg])
#         print('\tDATA: ', data)
#         # print('\tCRC: ', crc)
#         print('\n')
#     else:
#         rx_data = ser.read(2)
#         id, status = struct.unpack('<BB', rx_data)
#         print('SINGLE WRITE CMD')
#         print('\tID: ', id)
#         print('\tSTATUS: ', errors[status])
#         print('\n')