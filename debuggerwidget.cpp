#include "debuggerwidget.h"

DebuggerWidget::DebuggerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebuggerWidget)
{
    ui->setupUi(this);

    portthread = new QThread();
    portworker = new PortWorker();

    // Привяжем кнопки к началу и концу работы
    connect(ui->OPNButton, SIGNAL(clicked()), portthread, SLOT(start()));
    connect(ui->CLSButton, SIGNAL(clicked()), this, SLOT(StopThread()));
    // Свяжем поток и рабочего
    connect(portthread, SIGNAL(started()), portworker, SLOT(Work()));
	// Свяжем всё необходимое для обновления формы
    connect(portworker, SIGNAL(toForm(unsigned int, unsigned int, unsigned int)), this, SLOT(updateForm(unsigned int, unsigned int, unsigned int)));

	// Включение-отключение кнопок
	connect(ui->OPNButton, SIGNAL(clicked()), this, SLOT(openBTN()));
	connect(ui->CLSButton, SIGNAL(clicked()), this, SLOT(closeBTN()));

	ui->CLSButton->setEnabled(false);

    portworker->moveToThread(portthread);
}

DebuggerWidget::~DebuggerWidget()
{
    portworker->deleteLater();
    portthread->deleteLater();
    delete ui;
}

void DebuggerWidget::StopThread()
{
    // Вызовем прерывание в потоке для его остановки
    portthread->requestInterruption();
}

void DebuggerWidget::updateForm(unsigned int second, unsigned int freecpucnt, unsigned int DSPerrcnt)
{
	// Сформируем строку для выдачи на форму
	QString str;
	// Текстовый поток для управления строкой
	QTextStream ts(&str);
	// Запишем в поток содержимое
    ts << "Time is " << second << " seconds.\n\nFreeCPU is " << freecpucnt;
	// Условная индикация ошибки DSP
	if (DSPerrcnt) ts << ".\n\nDSP error is present.";
	else ts << ".\n\nDSP error is absent.";

	ui->outputText->setText(str);
}

void DebuggerWidget::openBTN()
{
	ui->OPNButton->setEnabled(false);
	ui->CLSButton->setEnabled(true);
}

void DebuggerWidget::closeBTN()
{
	ui->OPNButton->setEnabled(true);
	ui->CLSButton->setEnabled(false);
}
