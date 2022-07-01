import serial
import struct

errors = [ 'NO ERROR', 'ERROR CRC', 'ERROR CMD', 'ERROR ID', 'ERROR READ', 'ERROR WRITE', 'INTERNAL ERROR' ]

ID_DRIVE_1 = 0
ID_DRIVE_2 = 1
ID_DRIVE_3 = 2
ID_DRIVE_4 = 3
ID_GLOBAL = 4
ID_GLOBAL_CMD = 5

WRITE = 1
READ = 2

TORQUE = 0
POS_SP = 1
POS_FB = 2
POS_ACC = 3
POS_Kp = 4
POS_Ki = 5
POS_Kd = 6
SPD_SP = 7
SPD_FB = 8
SPD_ACC = 9
SPD_Kp = 10
SPD_Ki = 11
SPD_Kd = 12
CUR_SP = 13
CUR_FB = 14
CUR_ACC = 15
CUR_Kp = 16
CUR_Ki = 17
CUR_Kd = 18
OUTPUT = 19

ser = serial.Serial('/dev/ttyUSB1', 57600)
ser.isOpen()

def send_multiple(id, cmd, reg, val1, val2, val3, val4, crc):
    ser.write(struct.pack('<BBBffffB', id, cmd, reg, val1, val2, val3, val4, crc))
    
    if (cmd == READ):
        rx_data = ser.read(20)
        id, status, reg, data1, data2, data3, data4, crc = struct.unpack('<BBBffffB', rx_data)
        print('MULTIPLE READ CMD')
        print('\tID: ', id)
        print('\tSTATUS: ', errors[status])
        print('\tREGISTER: ', reg)
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
        print('SINGLE READ CMD')
        print('\tID: ', id)
        print('\tSTATUS: ', errors[status])
        print('\tREGISTER: ', reg)
        print('\tDATA: ', data)
        print('\tCRC: ', crc)
        print('\n')
    else:
        rx_data = ser.read(2)
        id, status = struct.unpack('<BB', rx_data)
        print('SINGLE WRITE CMD')
        print('\tID: ', id)
        print('\tSTATUS: ', errors[status])
        print('\n')

# send_single(ID_DRIVE_1, WRITE, POS_SP, 100, 0)
send_single(ID_DRIVE_1, READ, POS_SP, 0, 0)
# send_multiple(ID_GLOBAL, READ, POS_SP, 0, 0, 0, 0, 0)