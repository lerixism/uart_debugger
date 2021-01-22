#include "debuggerwidget.h"
#include <QTextStream>

DebuggerWidget::DebuggerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebuggerWidget)
{
    ui->setupUi(this);

	// Включение-отключение кнопок
	connect(ui->OPNButton, SIGNAL(clicked()), this, SLOT(openBTN()));
	connect(ui->CLSButton, SIGNAL(clicked()), this, SLOT(closeBTN()));

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

	portthread = new PortThread();

	connect(portthread, &QThread::finished, portthread, &QThread::deleteLater);
	portthread->start();
}

void DebuggerWidget::closeBTN()
{
	ui->OPNButton->setEnabled(true);
	ui->CLSButton->setEnabled(false);

	portthread->requestInterruption();
	portthread->deleteLater();
	portthread->wait();
}
