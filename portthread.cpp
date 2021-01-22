#include "portthread.h"
#include <math.h>
#include <QFile>
#include <QTextStream>
#include <QSerialPort>
#include <QDateTime>
#include <QMessageBox>

PortThread::PortThread()
{

}

unsigned short PortThread::CRC16(unsigned int * inp_Arr, unsigned short byte_length, unsigned short inp_crc)
{
	unsigned short crc;
	// Полином для расчёта
	unsigned short polynom = 0x1021;
	// Начало либо заявленное, либо 0xffff
	if (inp_crc) crc = inp_crc; else crc = 0xffff;

	 while (byte_length--)
	 {
		 crc ^= *inp_Arr++ << 8;
		for (int bit = 0; bit < 8; bit++)
		{
			if(crc & 0x8000)
			{
				crc <<= 1;
				crc ^= polynom;
			}
			else crc <<= 1;
		}
	 }
	return crc;
}

void PortThread::SetComNum(QString comnumber)
{
	comnum_str = comnumber;
}

void PortThread::run()
{
	// Создадим порт
	QSerialPort * Port1;
	Port1 = new QSerialPort();
	// Настроим порт
	Port1->setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections);
	// Выберем порт
	Port1->setPortName(comnum_str);

	// Откроем порт, если можно
	if (!Port1->open(QSerialPort::ReadOnly))
	{
		// Если нельзя, выдадим ошибку
		emit PortError(comnum_str + ". " + Port1->errorString());
		return;
	}

	QDateTime curdatetime = QDateTime::currentDateTime();
	QFile * outputplot = new QFile(static_cast<QString>("out_" + curdatetime.toString("yyyy-MM-dd_HHmmss") + ".log"));

	if (!outputplot->open(QIODevice::WriteOnly | QIODevice::Text)) QMessageBox::information(nullptr, tr("Call to file error"), outputplot->errorString());

	QTextStream outputplot_ts(outputplot);
	outputplot_ts << "x\ty\ty_l\tdelta\n";

	QByteArray read_arr;

	// Начинаем работу с портом
	// Работаем пока не придёт прерывание от кнопки стоп
	while (!QThread::currentThread()->isInterruptionRequested())
	{
		// Ждём приёма данных
		if (Port1->waitForReadyRead(-1))
		{
			// Чтение порта
			if (Port1->bytesAvailable() < 24) continue;
			read_arr = Port1->readAll();

			// Перепишем данные в 4-байтный массив
			unsigned int i_read_arr[32];

			for (int byte_num = 17; byte_num < read_arr.size(); byte_num++)
			{
				if ((static_cast<int>(read_arr[byte_num-1]) == 0x5a) && (static_cast<int>(read_arr[byte_num]) == 0x5a))
				{
					for (int i = 0; i < 32; i++) i_read_arr[i] = static_cast<int>(read_arr[byte_num - 17 + i]) & 0xff;
				}
			}

			unsigned int crc_in = i_read_arr[18] | (i_read_arr[19] << 8);
			unsigned int crc_calc = CRC16(i_read_arr, 18, 0);
			if (crc_in == crc_calc)
			{
				unsigned int freecpucnt = (i_read_arr[3] << 24) | (i_read_arr[4] << 16) | (i_read_arr[5] << 8) | i_read_arr[6];

				unsigned int ui_x_axis = (i_read_arr[8] << 24) | (i_read_arr[9] << 16) | (i_read_arr[10] << 8) | i_read_arr[11];
				unsigned int ui_y_axis = (i_read_arr[12] << 24) | (i_read_arr[13] << 16) | (i_read_arr[14] << 8) | i_read_arr[15];
				float *f_x_axis, *f_y_axis;
				f_x_axis = reinterpret_cast<float*>(&ui_x_axis);
				f_y_axis = reinterpret_cast<float*>(&ui_y_axis);

				// 	y = 4.0 * m.cos(x) + 0.5 * m.sin(10.0*x) - 0.005 * x**2 + 50.0 * m.atan(x)
				float l_f_y_axis = 4.0f * cosf(*f_x_axis) + 0.5f * sinf(10.0f * *f_x_axis) - 0.005f * *f_x_axis * *f_x_axis;

				outputplot_ts << *f_x_axis << "\t" << *f_y_axis << "\t" << l_f_y_axis << "\t" << abs(l_f_y_axis - *f_y_axis) << endl;

				emit toForm(i_read_arr[2], freecpucnt, i_read_arr[7]);
			}
		}
	}

	outputplot->close();
	outputplot->deleteLater();
	Port1->close();
}

