from time import sleep
from WC_PyAPI import *

# send_single(ID_DRIVE_4, READ, SPD_Kp, 0, 0)
# send_single(ID_DRIVE_4, READ, SPD_Ki, 0, 0)
# send_single(ID_DRIVE_4, READ, SPD_Kd, 0, 0)

while(1):
    send_single(ID_DRIVE_4, READ, POS_FB, 0)
    send_single(ID_DRIVE_1, READ, CUR_FB, 0)
    # send_single(ID_DRIVE_4, READ, SPD_ACC, 0, 0)
    # send_single(ID_DRIVE_4, READ, SPD_PERR, 0, 0)
    # send_single(ID_DRIVE_4, READ, CUR_SP, 0, 0)
    # send_single(ID_DRIVE_4, READ, SPD_SP, 0, 0)
    send_single(ID_DRIVE_4, READ, OUTPUT, 0)
    print('-----------------')
    sleep(1)