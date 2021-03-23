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

void DebuggerWidget::updateForm(unsigned int second, unsigned int freecpucnt, unsigned int DSPerrcnt, float f_x_axis, float delta)
{
	// Сформируем строку для выдачи на форму
	QString str;
	// Текстовый поток для управления строкой
	QTextStream ts(&str);
	// Запишем в поток содержимое
    ts << tr("Running time is ") << second << tr(" seconds.\n\nFreeCPU is ") << freecpucnt << "\n\nX axis is " << f_x_axis;

    ts << "\n\nMaximum delta is " << delta;
    // Условная индикация ошибки DSP
    if (DSPerrcnt) ts << tr(".\n\nDSP error is present.");

	ui->outputText->setText(str);
}

void DebuggerWidget::StopThreading()
{
    main_thread->quit();
    main_thread->wait();
    main_thread->deleteLater();

    // Деактивируем кнопку Открыть
    ui->OPNButton->setEnabled(true);
    // Активируем кнопку Закрыть
    ui->CLSButton->setEnabled(false);
    // Деактивируем поле для номера порта
    ui->comNUM->setEnabled(true);
}

void DebuggerWidget::openBTN()
{
    main_thread = new QThread();
	// Создадим объект потока
	portthread = new PortThread();
    portthread->moveToThread(main_thread);

    // Свяжем начало работы потока с работой рабочего
    connect(main_thread, &QThread::started, portthread, &PortThread::Work);
	// Окончание работы потока должно приводить к уничтожению объекта потока
    connect(main_thread, &QThread::finished, portthread, &QThread::deleteLater);
	// Обработчик ошибок от потока
	connect(portthread, &PortThread::ThrowError, this, &DebuggerWidget::HandleError);
	// Передача номера порта с формы в поток
    connect(this, &DebuggerWidget::SetComNumber, portthread, &PortThread::SetComNum, Qt::DirectConnection);
    // Обновление формы
    connect(portthread, &PortThread::toForm, this, &DebuggerWidget::updateForm);
    // Безопасная остановка потока
    connect(portthread, &PortThread::SafeStop, this, &DebuggerWidget::StopThreading);

	// Запуск потока (после выполнения exec выполняется run, перегруженная в PortThread)
    main_thread->start();

	// Поток запустили - можно сообщить ему выбранный номер порта
	emit SetComNumber(ui->comNUM->text());

    // Деактивируем кнопку Открыть
    ui->OPNButton->setEnabled(false);
    // Активируем кнопку Закрыть
    ui->CLSButton->setEnabled(true);
    // Деактивируем поле для номера порта
    ui->comNUM->setEnabled(false);
}

void DebuggerWidget::closeBTN()
{
	// Остановим поток прерыванием (оно не снимается, поэтому поток придётся уничтожить)
    main_thread->requestInterruption();
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
