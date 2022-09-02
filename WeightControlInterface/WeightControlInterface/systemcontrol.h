#ifndef SYSTEMCONTROL_H
#define SYSTEMCONTROL_H

#include "global_config.h"
#include "bootloader.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QVector>
#include <QByteArray>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QWidget>
#endif

namespace Ui {
class SystemControl;
}

class SystemControl : public QWidget
{
    Q_OBJECT

public:
    explicit SystemControl(QWidget *parent = nullptr);
    ~SystemControl();

    Ui::Locker SerialLock;

private:
    Ui::SystemControl * ui;
    QSerialPort *       Serial;

    QTimer * TimeoutTimer       = nullptr;
    QTimer * DeviceCheckTimer   = nullptr;

    uint32_t data_awaited = 0;

    void ConsoleBasic(QString message);
    void ConsoleWarning(QString message);
    void ConsoleError(QString message);

    void ProcessIncomingData(void);

    enum BP_Commands{
        BP_PING     = 0x01,
        BP_CONTROL  = 0xF0
    };

    enum CONTROL_Commands{
        CNT_WRITE_REG       = 0x01,
        CNT_READ_REG        = 0x02,
        CNT_BLOCK_DRIVE     = 0x03,
        CNT_ENABLE_DRIVE    = 0x05
    };

    enum CONTROL_IDs{
        CNT_ID_DRIVE_1      = 0x00,
        CNT_ID_DRIVE_2      = 0x01,
        CNT_ID_DRIVE_3      = 0x02,
        CNT_ID_DRIVE_4      = 0x03,
        CNT_ID_GLOBAL       = 0x04,
        CNT_ID_GLOBAL_CMD   = 0x05
    };

    enum CONTROL_Registers{
        CNT_REG_TORQUE          = 0x00,
        CNT_REG_POS_SP          = 0x01,
        CNT_REG_POS_FB          = 0x02,
        CNT_REG_POS_ACC         = 0x03,
        CNT_REG_POS_ACC_THRES   = 0x04,
        CNT_REG_POS_PERR        = 0x05,
        CNT_REG_POS_Kp          = 0x06,
        CNT_REG_POS_Ki          = 0x07,
        CNT_REG_POS_Kd          = 0x08,
        CNT_REG_POS_ACTIVE      = 0x09,
        CNT_REG_SPD_SP          = 0x0A,
        CNT_REG_SPD_FB          = 0x0B,
        CNT_REG_SPD_ACC         = 0x0C,
        CNT_REG_SPD_ACC_THRES   = 0x0D,
        CNT_REG_SPD_PERR        = 0x0E,
        CNT_REG_SPD_Kp          = 0x0F,
        CNT_REG_SPD_Ki          = 0x10,
        CNT_REG_SPD_Kd          = 0x11,
        CNT_REG_SPD_ACTIVE      = 0x12,
        CNT_REG_CUR_SP          = 0x13,
        CNT_REG_CUR_FB          = 0x14,
        CNT_REG_CUR_ACC         = 0x15,
        CNT_REG_CUR_ACC_THRES   = 0x16,
        CNT_REG_CUR_PERR        = 0x17,
        CNT_REG_CUR_Kp          = 0x18,
        CNT_REG_CUR_Ki          = 0x19,
        CNT_REG_CUR_Kd          = 0x1A,
        CNT_REG_CUR_ACTIVE      = 0x1B,
        CNT_REG_OUTPUT          = 0x1C,
        CNT_REG_OUTPUT_THRES    = 0x1D
    };

    enum DataAwaited{
        BP_PING_AWAIT_SIZE                  = 9,
        BP_CNT_WRITE_SINGLE_AWAIT_SIZE      = 21,
        BP_CNT_WRITE_MULTIPLE_AWAIT_SIZE    = 33,
        BP_CNT_READ_SINGLE_AWAIT_SIZE       = 21,
        BP_CNT_READ_MULTIPLE_AWAIT_SIZE     = 33,
        BP_CNT_GLOBAL_CMD_AWAIT_SIZE        = 13
    };

    struct BP_Header{
        uint8_t             cmd;
        uint32_t            w_size;
        QVector<uint32_t>   payload;
        BP_Header(void){}
        BP_Header(BP_Commands c, uint32_t s){
            this->cmd = c;
            this->w_size = s;
        }
        void SetHeaderFromRaw(QByteArray data){
            this->cmd = data.at(0);
            this->w_size = *reinterpret_cast<uint32_t *>(data.data() + 1);
            for (uint32_t iter = 0; iter < this->w_size; iter++){
                this->payload.append(*reinterpret_cast<uint32_t *>(data.data() + 5 + iter * 4));
            }
        }
        QByteArray SetRawFromHeader(void){
            QByteArray data;
            QDataStream s_data(&data, QIODevice::WriteOnly);
            s_data.setByteOrder(QDataStream::LittleEndian);
            s_data << this->cmd;
            s_data << this->w_size;
            for (uint32_t iter = 0; iter < this->w_size; iter++){
                s_data << this->payload[iter];
            }
            return data;
        }
        QVector<uint32_t> PassPayload(void){
            QVector<uint32_t> tmp = this->payload;
            this->payload.clear();
            return tmp;
        }
    };

    struct CNT_Header{
        uint32_t            id;
        uint32_t            cmd;
        QVector<uint32_t>   payload;

        CNT_Header(void){};
        CNT_Header(CONTROL_IDs i, CONTROL_Commands c){
            this->cmd = c;
            this->id = i;
        }
        void SetHeaderFromBPPayload(BP_Header * header){
            this->payload = header->PassPayload();

            this->id =  this->payload[0];
            this->cmd = this->payload[1];
            this->payload.pop_front();
            this->payload.pop_front();
        }
        QByteArray SetRawFromHeader(void){
            QByteArray data;
            QDataStream s_data(&data, QIODevice::WriteOnly);
            s_data.setByteOrder(QDataStream::LittleEndian);
            s_data << this->id;
            s_data << this->cmd;
            return data;
        }
        QVector<uint32_t> PassPayload(void){
            QVector<uint32_t> tmp = this->payload;
            this->payload.clear();
            return tmp;
        }
    };

    struct CNT_Register{
        uint32_t            reg;
        QVector<uint32_t>   data;

        CNT_Register(void){};
        CNT_Register(CONTROL_Registers r, QVector<uint32_t> v){
            this->reg = r;
            for (uint32_t iter = 0; iter < v.size(); iter++){
                if (iter >= 4) break;
                this->data.append(v[iter]);
            }
        }
        void SetHeaderFromCNTPayload(CNT_Header * header){
            QVector<uint32_t> tmp = header->PassPayload();

            this->reg = tmp[0];
            tmp.pop_front();
            this->data = tmp;
        }
        QByteArray SetRawFromHeader(void){
            QByteArray data;
            QDataStream s_data(&data, QIODevice::WriteOnly);
            s_data.setByteOrder(QDataStream::LittleEndian);
            s_data << this->reg;
            for (auto iter = this->data.begin(); iter != this->data.end(); iter++){
                s_data << *iter;
            }
            return data;
        }
    };

private slots:
    void PushDataFromStream(void);


public slots:
    void slReceiveSerial(QSerialPort * p_serial){
        this->Serial = p_serial;
    }

    void C_PingSilent(void);

    void slActivate(void);

signals:
    void siSendSerial(QSerialPort * p_serial);
    void siChooseTab(uint16_t tab);

};

#endif // SYSTEMCONTROL_H
