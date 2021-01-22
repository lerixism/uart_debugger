#ifndef PORTTHREAD_H
#define PORTTHREAD_H

#include <QObject>
#include <QThread>

class PortThread : public QThread
{
	Q_OBJECT

	void run() override;
public:
	PortThread();

signals:
	void toForm(unsigned int second, unsigned int freecpucnt, unsigned int DSPerrcnt);

private:
	unsigned short CRC16(unsigned int *, unsigned short, unsigned short);
};

#endif // PORTTHREAD_H
