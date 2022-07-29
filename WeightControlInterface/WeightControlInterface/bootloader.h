#ifndef BOOTLOADER_H
#define BOOTLOADER_H

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

namespace Ui {
class Bootloader;
}

class Bootloader : public QWidget
{
    Q_OBJECT

public:
    explicit Bootloader(QWidget *parent = nullptr);
    ~Bootloader();

    QSerialPort * Serial;

public slots:
    void C_Ping(void);
    void C_Read(void);
    void C_Write(void);
    void C_Verify(void);
    void C_Erase(void);
    void C_Jump(void);

    void ErrorCatch(uint32_t error_code);

private slots:
    void Timeout(void){
        this->Serial->readAll();
        this->data_awaited = 0;
        if (this->file != nullptr){
            if (this->file->isOpen()){
                this->file->close();
                delete this->file;
            }
        }
        ConsoleError("Connection Timeout");
        this->UIUnlock(true);
    }
    void UIUnlock(bool lock);
    void ProcessIncomingData(void);
    void PushDataFromStream(void);


private:

    QTimer * TimeoutTimer = nullptr;

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
        PING_AWAIT_SIZE     = 9,
        VERIFY_AWAIT_SIZE   = 9,
        JUMP_AWAIT_SIZE     = 5,
        READ_AWAIT_SIZE     = 9,
        WRITE_AWAIT_SIZE    = 5,
        ERASE_AWAIT_SIZE    = 5
    };

    struct MsgHeader{
        uint8_t             cmd;
        uint32_t            w_size;
        QVector<uint32_t>   payload;
        MsgHeader(void){}
        MsgHeader(Commands c, uint32_t s){
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
    };

    QByteArray  serial_stream;
    uint32_t    data_awaited = 0;
    uint8_t     flash_after = 0;

    QFile *     file = nullptr;
    uint32_t    filesize = 0;

    Ui::Bootloader *ui;


signals:
    void ErrorSignal(uint32_t error_code);
};

#endif // BOOTLOADER_H
