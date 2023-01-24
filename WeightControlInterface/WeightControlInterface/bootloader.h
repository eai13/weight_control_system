#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "global_config.h"

#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QWidget>
#endif

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QByteArray>
#include <QTimer>
#include <QFile>
#include <QList>
#include <QDebug>

namespace Ui {
class Locker{
private:
    bool is_locked = false;

public:
    void Unlock(void){
        this->is_locked = false;
    }
    bool Lock(void){
        if (this->is_locked){
            return false;
        }
        else{
            this->is_locked = true;
            return true;
        }
    }
};

class Bootloader;
}

class Bootloader : public QWidget
{
    Q_OBJECT

private:
    Ui::Bootloader *ui;

public:
    explicit Bootloader(QWidget *parent = nullptr);
    ~Bootloader();

    QSerialPort * Serial = nullptr;

    Ui::Locker SerialLock;

public slots:
    void C_Ping(void);
    void C_PingSilent(void);
    void C_Read(void);
    void C_Write(void);
    void C_Verify(void);
    void C_Erase(void);
    void C_Jump(void);

    void slReceiveSerial(QSerialPort * p_serial){
        this->Serial = p_serial;
    }

    void slActivate(void);

private slots:
    void Timeout(void){
        if (this->DeviceCheckTimer->isActive()) this->DeviceCheckTimer->stop();
//        this->Serial->readAll();
        this->SerialLock.Unlock();
        this->data_awaited = 0;
        this->flash_after = 0;
        if (this->file != nullptr){
            if (this->file->isOpen()){
                this->file->close();
                delete this->file;
                this->file = nullptr;
            }
        }

        ConsoleError("Connection Timeout");

        disconnect(this->Serial, &QSerialPort::readyRead, this, &Bootloader::PushDataFromStream);
        this->console_enabled = false;

        if (!(this->DevicePingTimer->isActive())){
            this->DevicePingTimer->start(500);
        }
    }
    void DevicePing(void);
    void PushDataFromStream(void);
    void BrowseFile(void);


private:

    enum FlashMapping{
        FLASH_MAP_APP_1 = 40960,
        FLASH_MAP_APP_2 = 32768,
        FLASH_MAP_APP_USER = 40960
    };

    QTimer * TimeoutTimer       = nullptr;
    QTimer * DevicePingTimer    = nullptr;
    QTimer * DeviceCheckTimer   = nullptr;

    bool console_enabled = false;
    void ConsoleBasic(QString message);
    void ConsoleWarning(QString message);
    void ConsoleError(QString message);

    enum ErrorCodes{
        WRONG_PARTITION,
        FILE_NOT_CHOSEN
    };

    enum Commands{
        PING    = 0x01,
        VERIFY  = 0x02,
        JUMP    = 0x03,
        READ    = 0x04,
        WRITE   = 0x05,
        ERASE   = 0x06,

        INTERNAL_ERROR  = 0x08,
        CRC_ERROR       = 0x09,
        CMD_ERROR       = 0x0A,
        PARTITION_ERROR = 0x0B,
        LENGTH_ERROR    = 0x0C,
        DATA_ERROR      = 0x0D
    };

    enum DataAwaited{
        PING_AWAIT_SIZE     = 11, //9,
        VERIFY_AWAIT_SIZE   = 11, //9,
        JUMP_AWAIT_SIZE     = 7, //5,
        READ_AWAIT_SIZE     = 11, //9,
        WRITE_AWAIT_SIZE    = 7, //5,
        ERASE_AWAIT_SIZE    = 7 //5
    };

    struct MsgHeader{
        uint8_t             start_A5;
        uint8_t             start_5A;
        uint8_t             cmd;
        uint32_t            w_size;
        QVector<uint32_t>   payload;
        MsgHeader(void){}
        MsgHeader(Commands c, uint32_t s){
            this->start_5A = 0x5A;
            this->start_A5 = 0xA5;
            this->cmd = c;
            this->w_size = s;
        }
        int8_t SetHeaderFromRaw(QByteArray data){
            this->start_A5 = data.at(0);
            this->start_5A = data.at(1);
            this->cmd = data.at(2);
            this->w_size = *reinterpret_cast<uint32_t *>(data.data() + 3);
            if ((this->w_size * 4) != (data.size() - 7)){
//                qDebug() << "BOOTLOADER Wrong Size";
                return -1;
            }
            qDebug() << "Received W_Size " << this->w_size;
            qDebug() << "CMD " << this->cmd;
            for (uint32_t iter = 0; iter < this->w_size; iter++){
                this->payload.append(*reinterpret_cast<uint32_t *>(data.data() + 7 + iter * 4));
            }
            return 0;
        }
        QByteArray SetRawFromHeader(void){
            QByteArray data;
            QDataStream s_data(&data, QIODevice::WriteOnly);
            s_data.setByteOrder(QDataStream::LittleEndian);
            s_data << this->start_A5;
            s_data << this->start_5A;
            s_data << this->cmd;
            s_data << this->w_size;
            for (uint32_t iter = 0; iter < this->w_size; iter++){
                s_data << this->payload[iter];
            }
            return data;
        }
    };

    void ProcessIncomingData(void);

    uint32_t    data_awaited = 0;
    uint8_t     flash_after = 0;

    QFile *     file = nullptr;
    uint32_t    filesize = 0;

    QList<QSerialPortInfo>  available_ports;

signals:
    void siChooseTab(uint16_t tab);
    void siSendSerial(QSerialPort * p_serial);
};

#endif // BOOTLOADER_H
