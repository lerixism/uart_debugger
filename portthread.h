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

public slots:
	void SetComNum(QString);

signals:
	void toForm(unsigned int second, unsigned int freecpucnt, unsigned int DSPerrcnt);
	void PortError(QString);

private:
	QString comnum_str;
	unsigned short CRC16(unsigned int *, unsigned short, unsigned short);
};

#endif // PORTTHREAD_H
