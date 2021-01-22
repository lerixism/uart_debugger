#include "debuggerwidget.h"
#include <QTextStream>
#include <QMessageBox>

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
	ui->comNUM->setEnabled(false);

	portthread = new PortThread();

	connect(portthread, &QThread::finished, portthread, &QThread::deleteLater);
	connect(portthread, &PortThread::PortError, this, &DebuggerWidget::OpenError);
	connect(this, &DebuggerWidget::SetComNumber, portthread, &PortThread::SetComNum);

	portthread->start();

	emit SetComNumber(ui->comNUM->text());
}

void DebuggerWidget::closeBTN()
{
	ui->OPNButton->setEnabled(true);
	ui->CLSButton->setEnabled(false);
	ui->comNUM->setEnabled(true);

	portthread->requestInterruption();
	portthread->deleteLater();
	portthread->wait();
}

void DebuggerWidget::OpenError(QString errorstr)
{
	ui->OPNButton->setEnabled(true);
	ui->CLSButton->setEnabled(false);
	ui->comNUM->setEnabled(true);

	QMessageBox msgBox;
	msgBox.setText(tr("Error with port: ") + errorstr);
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.exec();
}
