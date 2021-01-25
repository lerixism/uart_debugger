#include "debuggerwidget.h"
#include <QTextStream>
#include <QMessageBox>

DebuggerWidget::DebuggerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebuggerWidget)
{
	// Запуск интерфейса
    ui->setupUi(this);

	// Обработка нажатия кнопок Открыть - Закрыть
	connect(ui->OPNButton, &QPushButton::clicked, this, &DebuggerWidget::openBTN);
	connect(ui->CLSButton, &QPushButton::clicked, this, &DebuggerWidget::closeBTN);

	// Деактивируем кнопку Закрыть
	ui->CLSButton->setEnabled(false);
}

DebuggerWidget::~DebuggerWidget()
{
    delete ui;
}

void DebuggerWidget::updateForm(unsigned int second, unsigned int freecpucnt, unsigned int DSPerrcnt)
{
	// Сформируем строку для выдачи на форму
	QString str;
	// Текстовый поток для управления строкой
	QTextStream ts(&str);
	// Запишем в поток содержимое
	ts << tr("Running time is ") << second << tr(" seconds.\n\nFreeCPU is ") << freecpucnt;
	// Условная индикация ошибки DSP
	if (DSPerrcnt) ts << tr(".\n\nDSP error is present.");
	else ts << tr(".\n\nDSP error is absent.");

	ui->outputText->setText(str);
}

void DebuggerWidget::openBTN()
{
	// Деактивируем кнопку Открыть
	ui->OPNButton->setEnabled(false);
	// Активируем кнопку Закрыть
	ui->CLSButton->setEnabled(true);
	// Деактивируем поле для номера порта
	ui->comNUM->setEnabled(false);

	// Создадим объект потока
	portthread = new PortThread();

	// Окончание работы потока должно приводить к уничтожению объекта потока
	connect(portthread, &QThread::finished, portthread, &QThread::deleteLater);
	// Обработчик ошибок от потока
	connect(portthread, &PortThread::ThrowError, this, &DebuggerWidget::HandleError);
	// Передача номера порта с формы в поток
	connect(this, &DebuggerWidget::SetComNumber, portthread, &PortThread::SetComNum);

	// Запуск потока (после выполнения exec выполняется run, перегруженная в PortThread)
	portthread->start();

	// Поток запустили - можно сообщить ему выбранный номер порта
	emit SetComNumber(ui->comNUM->text());
}

void DebuggerWidget::closeBTN()
{
	// Активируем кнопку Открыть
	ui->OPNButton->setEnabled(true);
	// Деактивируем кнопку Закрыть
	ui->CLSButton->setEnabled(false);
	// Активируем поле для номера порта
	ui->comNUM->setEnabled(true);

	// Остановим поток прерыванием (оно не снимается, поэтому поток придётся уничтожить)
	portthread->requestInterruption();
	// Уничтожаем поток (deleteLater(), чтобы он успел доработать)
	portthread->deleteLater();
	// На всякий случай ждём пока он закончит работу
	portthread->wait();
}

void DebuggerWidget::HandleError(QString errorstr)
{
	// Активируем кнопку Открыть
	ui->OPNButton->setEnabled(true);
	// Деактивируем кнопку Закрыть
	ui->CLSButton->setEnabled(false);
	// Активируем поле для номера порта
	ui->comNUM->setEnabled(true);

	// Для уведомления пользователя нужно окно сообщений
	QMessageBox msgBox;
	// Текст ошибки
	msgBox.setText(tr("Error: ") + errorstr);
	// Иконка сообщения - критичное (красный восклицательный знак)
	msgBox.setIcon(QMessageBox::Critical);
	// Покажем окно пользователю
	msgBox.exec();
}
