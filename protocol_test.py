import serial
import struct

ser = serial.Serial('/dev/ttyUSB1', 57600)
ser.isOpen()

ser.write(struct.pack('<BBBffffB', 4, 2, 10, 0, 0, 0, 0, 0))
while(1):
    rx_data = ser.read(20)
    id, status, reg, data1, data2, data3, data4, crc = struct.unpack('<BBBffffB', rx_data)
    print('ID: ', id)
    print('STATUS: ', status)
    print('REGISTER: ', reg)
    print('DATA: ', data1, data2, data3, data4)
    print('CRC: ', crc)
    