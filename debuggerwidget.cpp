#include "debuggerwidget.h"
#include <QTextStream>
#include <QMessageBox>

DebuggerWidget::DebuggerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebuggerWidget)
{
	// ������ ����������
    ui->setupUi(this);

	// ��������� ������� ������ ������� - �������
	connect(ui->OPNButton, &QPushButton::clicked, this, &DebuggerWidget::openBTN);
	connect(ui->CLSButton, &QPushButton::clicked, this, &DebuggerWidget::closeBTN);

	// ������������ ������ �������
	ui->CLSButton->setEnabled(false);
}

DebuggerWidget::~DebuggerWidget()
{
    delete ui;
}

void DebuggerWidget::updateForm(unsigned int second, unsigned int freecpucnt, unsigned int DSPerrcnt)
{
	// ���������� ������ ��� ������ �� �����
	QString str;
	// ��������� ����� ��� ���������� �������
	QTextStream ts(&str);
	// ������� � ����� ����������
	ts << tr("Running time is ") << second << tr(" seconds.\n\nFreeCPU is ") << freecpucnt;
	// �������� ��������� ������ DSP
	if (DSPerrcnt) ts << tr(".\n\nDSP error is present.");
	else ts << tr(".\n\nDSP error is absent.");

	ui->outputText->setText(str);
}

void DebuggerWidget::openBTN()
{
	// ������������ ������ �������
	ui->OPNButton->setEnabled(false);
	// ���������� ������ �������
	ui->CLSButton->setEnabled(true);
	// ������������ ���� ��� ������ �����
	ui->comNUM->setEnabled(false);

	// �������� ������ ������
	portthread = new PortThread();

	// ��������� ������ ������ ������ ��������� � ����������� ������� ������
	connect(portthread, &QThread::finished, portthread, &QThread::deleteLater);
	// ���������� ������ �� ������
	connect(portthread, &PortThread::ThrowError, this, &DebuggerWidget::HandleError);
	// �������� ������ ����� � ����� � �����
	connect(this, &DebuggerWidget::SetComNumber, portthread, &PortThread::SetComNum);

	// ������ ������ (����� ���������� exec ����������� run, ������������� � PortThread)
	portthread->start();

	// ����� ��������� - ����� �������� ��� ��������� ����� �����
	emit SetComNumber(ui->comNUM->text());
}

void DebuggerWidget::closeBTN()
{
	// ���������� ������ �������
	ui->OPNButton->setEnabled(true);
	// ������������ ������ �������
	ui->CLSButton->setEnabled(false);
	// ���������� ���� ��� ������ �����
	ui->comNUM->setEnabled(true);

	// ��������� ����� ����������� (��� �� ���������, ������� ����� ������� ����������)
	portthread->requestInterruption();
	// ���������� ����� (deleteLater(), ����� �� ����� ����������)
	portthread->deleteLater();
	// �� ������ ������ ��� ���� �� �������� ������
	portthread->wait();
}

void DebuggerWidget::HandleError(QString errorstr)
{
	// ���������� ������ �������
	ui->OPNButton->setEnabled(true);
	// ������������ ������ �������
	ui->CLSButton->setEnabled(false);
	// ���������� ���� ��� ������ �����
	ui->comNUM->setEnabled(true);

	// ��� ����������� ������������ ����� ���� ���������
	QMessageBox msgBox;
	// ����� ������
	msgBox.setText(tr("Error: ") + errorstr);
	// ������ ��������� - ��������� (������� ��������������� ����)
	msgBox.setIcon(QMessageBox::Critical);
	// ������� ���� ������������
	msgBox.exec();
}
