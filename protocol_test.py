from time import sleep
import WC_PyAPI

while(1):
    WC_PyAPI.send_single(WC_PyAPI.ID_DRIVE_4, WC_PyAPI.READ, WC_PyAPI.POS_FB, 0, 0)
    WC_PyAPI.send_single(WC_PyAPI.ID_DRIVE_4, WC_PyAPI.READ, WC_PyAPI.SPD_ACC, 0, 0)
    WC_PyAPI.send_single(WC_PyAPI.ID_DRIVE_4, WC_PyAPI.READ, WC_PyAPI.SPD_PERR, 0, 0)
    WC_PyAPI.send_single(WC_PyAPI.ID_DRIVE_4, WC_PyAPI.READ, WC_PyAPI.CUR_SP, 0, 0)
    WC_PyAPI.send_single(WC_PyAPI.ID_DRIVE_4, WC_PyAPI.READ, WC_PyAPI.SPD_SP, 0, 0)
    WC_PyAPI.send_single(WC_PyAPI.ID_DRIVE_4, WC_PyAPI.READ, WC_PyAPI.OUTPUT, 0, 0)
    print('-----------------')
    sleep(1)