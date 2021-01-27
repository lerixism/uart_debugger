#include "portthread.h"
#include <math.h>
#include <QFile>
#include <QTextStream>
#include <QSerialPort>
#include <QDateTime>

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
	// Запишем принятый номер порта в переменную потока
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
		emit ThrowError(comnum_str + ". " + Port1->errorString());
		return;
	}

	// Считаем время системы для лог-файла
	QDateTime curdatetime = QDateTime::currentDateTime();
	// Составив имя файла, создадим объект для него
	QFile * outputplot = new QFile(static_cast<QString>("out_" + curdatetime.toString("yyyy-MM-dd_HHmmss") + ".log"));

	// Откроем файл для записи лога, если можно
    if (!outputplot->open(QIODevice::WriteOnly | QIODevice::Text))
	{
		// Если нельзя, выдадим ошибку
		emit ThrowError(outputplot->fileName() + ". " + outputplot->errorString());
		// Закроем порт
		Port1->close();
		return;
	}

	// Поток для простой работы с лог-файлом
	QTextStream outputplot_ts(outputplot);
	// Шапка лог-файла
	outputplot_ts << "x\ty\ty_l\tdelta\n";

	// Массив для считывания из порта
	QByteArray read_arr;

	// Начинаем работу с портом
	// Работаем пока не придёт прерывание для потока от кнопки стоп
	while (!QThread::currentThread()->isInterruptionRequested())
	{
		// Ждём приёма данных
		if (Port1->waitForReadyRead(-1))
		{
			// Чтение порта
			// Если в порту меньше 24 байтов, не интересно, ждём пока больше придёт
			if (Port1->bytesAvailable() < 24) continue;
			// Считываем всё
			read_arr = Port1->readAll();

			// Перепишем данные в 4-байтный массив
			unsigned int i_read_arr[32];

			// Начиная с крайнего возможного байта концовки ищем эту самую концовку (0x5a5a)
			for (int byte_num = 17; byte_num < read_arr.size(); byte_num++)
				if ((static_cast<int>(read_arr[byte_num-1]) == 0x5a) && (static_cast<int>(read_arr[byte_num]) == 0x5a))
					// Концовку нашли, перепишем её в 4-байтный массив
					for (int i = 0; i < 32; i++) i_read_arr[i] = static_cast<int>(read_arr[byte_num - 17 + i]) & 0xff;

			// Считаем принятую контрольную сумму
			unsigned int crc_in = i_read_arr[18] | (i_read_arr[19] << 8);
			// Рассчитаем контрольную сумму принятой посылки
			unsigned int crc_calc = CRC16(i_read_arr, 18, 0);

			// Если они совпали, посылку можно обработать
			if (crc_in == crc_calc)
			{
				// Счётчик загруженности процессора
				unsigned int freecpucnt = (i_read_arr[3] << 24) | (i_read_arr[4] << 16) | (i_read_arr[5] << 8) | i_read_arr[6];

				// Расчитанные значения проверочной формулы
				// Целочисленные значения
				unsigned int ui_x_axis = (i_read_arr[8] << 24) | (i_read_arr[9] << 16) | (i_read_arr[10] << 8) | i_read_arr[11];
				unsigned int ui_y_axis = (i_read_arr[12] << 24) | (i_read_arr[13] << 16) | (i_read_arr[14] << 8) | i_read_arr[15];

				// Для расшифровки нужно считать их как вещественные
				float *f_x_axis, *f_y_axis;
				f_x_axis = reinterpret_cast<float*>(&ui_x_axis);
				f_y_axis = reinterpret_cast<float*>(&ui_y_axis);

				// Расчёт формулы по принятому значению абсциссы

				// z = -50.0 * x ** 2 + x ** 4
				// y = 30.0 * m.cos(0.3 * z) + 20.0 * m.sin(3.0*z) - 0.005 * z**2
				float l_f_z_axis = -50.0f * *f_x_axis * *f_x_axis + *f_x_axis * *f_x_axis * *f_x_axis * *f_x_axis;
				float l_f_y_axis = 30.0f * cosf(0.3f * l_f_z_axis) + 20.0f * sinf(3.0f * l_f_z_axis) - 0.005f * l_f_z_axis * l_f_z_axis;

				// Запишем эти данные в файл, включая расчёт формулы на ПК и дельты между ПК и процессором
				outputplot_ts << *f_x_axis << "\t" << *f_y_axis << "\t" << l_f_y_axis << "\t" << abs(l_f_y_axis - *f_y_axis) << endl;

				// Выдадим на форму данные о текущей работе процессора
                emit toForm(i_read_arr[2], freecpucnt, i_read_arr[7], *f_x_axis);
			}
		}
	}

	// Закроем файл
	outputplot->close();
	// И безопасно удалим
	outputplot->deleteLater();
	// Закроем порт
	Port1->close();
}

