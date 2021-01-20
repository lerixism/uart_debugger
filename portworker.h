#ifndef PORTWORKER_H
#define PORTWORKER_H

#include <math.h>
#include <QObject>
#include <QSerialPort>
#include <QWidget>
#include <QThread>
#include <QString>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>

class PortWorker : public QObject
{
    Q_OBJECT
public:
    explicit PortWorker(QObject *parent = nullptr);
    ~PortWorker();

signals:
    void toForm(unsigned int second, unsigned int freecpucnt, unsigned int DSPerrcnt);

public slots:
    void Work();
    void Stop();

private:
    QSerialPort *Port1;
    bool do_work;
    unsigned short CRC16(unsigned int * inp_Arr, unsigned short byte_length, unsigned short inp_crc);
};

#endif // PORTWORKER_H
