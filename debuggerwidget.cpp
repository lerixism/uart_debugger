#include "debuggerwidget.h"

DebuggerWidget::DebuggerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebuggerWidget)
{
    ui->setupUi(this);

    portthread = new QThread();
    portworker = new PortWorker();

    // �������� ������ � ������ � ����� ������
    connect(ui->OPNButton, SIGNAL(clicked()), portthread, SLOT(start()));
    connect(ui->CLSButton, SIGNAL(clicked()), this, SLOT(StopThread()));
    // ������ ����� � ��������
    connect(portthread, SIGNAL(started()), portworker, SLOT(Work()));
	// ������ �� ����������� ��� ���������� �����
    connect(portworker, SIGNAL(toForm(unsigned int, unsigned int, unsigned int)), this, SLOT(updateForm(unsigned int, unsigned int, unsigned int)));

	// ���������-���������� ������
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
    // ������� ���������� � ������ ��� ��� ���������
    portthread->requestInterruption();
}

void DebuggerWidget::updateForm(unsigned int second, unsigned int freecpucnt, unsigned int DSPerrcnt)
{
	// ���������� ������ ��� ������ �� �����
	QString str;
	// ��������� ����� ��� ���������� �������
	QTextStream ts(&str);
	// ������� � ����� ����������
    ts << "Time is " << second << " seconds.\n\nFreeCPU is " << freecpucnt;
	// �������� ��������� ������ DSP
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
