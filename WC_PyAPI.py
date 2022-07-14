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

def send_multiple(id, cmd, reg, val1, val2, val3, val4, crc):
    ser.write(struct.pack('<BBBffffB', id, cmd, reg, val1, val2, val3, val4, crc))
    
    if (cmd == READ):
        rx_data = ser.read(20)
        id, status, reg, data1, data2, data3, data4, crc = struct.unpack('<BBBffffB', rx_data)
        print('MULTIPLE READ CMD')
        print('\tID: ', id)
        print('\tSTATUS: ', errors[status])
        print('\tREGISTER: ', register_name[reg])
        print('\tDATA: ', data1, data2, data3, data4)
        print('\tCRC: ', crc)
        print('\n')
    else:
        rx_data = ser.read(2)
        id, status = struct.unpack('<BB', rx_data)
        print('MULTIPLE WRITE CMD')
        print('\tID: ', id)
        print('\tSTATUS: ', errors[status])
        print('\n')

def send_single(id, cmd, reg, val, crc):
    ser.write(struct.pack('<BBBfB', id, cmd, reg, val, crc))
    if (cmd == READ):
        rx_data = ser.read(8)
        id, status, reg, data, crc = struct.unpack('<BBBfB', rx_data)
        # print('SINGLE READ CMD')
        # print('\tID: ', id)
        # print('\tSTATUS: ', errors[status])
        print('\tREGISTER: ', register_name[reg])
        print('\tDATA: ', data)
        # print('\tCRC: ', crc)
        print('\n')
    else:
        rx_data = ser.read(2)
        id, status = struct.unpack('<BB', rx_data)
        print('SINGLE WRITE CMD')
        print('\tID: ', id)
        print('\tSTATUS: ', errors[status])
        print('\n')