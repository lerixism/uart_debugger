#ifndef PORTTHREAD_H
#define PORTTHREAD_H

#include <QObject>
#include <QThread>

class PortThread : public QObject
{
	Q_OBJECT

public:
	PortThread();

public slots:
	void SetComNum(QString);
    void Work();

signals:
    void toForm(unsigned int, unsigned int, unsigned int, float, float);
	void ThrowError(QString);
    void SafeStop();

private:
	QString comnum_str;
	unsigned short CRC16(unsigned int *, unsigned short, unsigned short);
    float max_delta;
};

#endif // PORTTHREAD_H
