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

void DebuggerWidget::updateForm(unsigned int second, unsigned int freecpucnt, unsigned int DSPerrcnt, float f_x_axis, float delta)
{
	// ���������� ������ ��� ������ �� �����
	QString str;
	// ��������� ����� ��� ���������� �������
	QTextStream ts(&str);
	// ������� � ����� ����������
    ts << tr("Running time is ") << second << tr(" seconds.\n\nFreeCPU is ") << freecpucnt << "\n\nX axis is " << f_x_axis;

    ts << "\n\nMaximum delta is " << delta;
    // �������� ��������� ������ DSP
    if (DSPerrcnt) ts << tr(".\n\nDSP error is present.");

	ui->outputText->setText(str);
}

void DebuggerWidget::StopThreading()
{
    main_thread->quit();
    main_thread->wait();
    main_thread->deleteLater();

    // ������������ ������ �������
    ui->OPNButton->setEnabled(true);
    // ���������� ������ �������
    ui->CLSButton->setEnabled(false);
    // ������������ ���� ��� ������ �����
    ui->comNUM->setEnabled(true);
}

void DebuggerWidget::openBTN()
{
    main_thread = new QThread();
	// �������� ������ ������
	portthread = new PortThread();
    portthread->moveToThread(main_thread);

    // ������ ������ ������ ������ � ������� ��������
    connect(main_thread, &QThread::started, portthread, &PortThread::Work);
	// ��������� ������ ������ ������ ��������� � ����������� ������� ������
    connect(main_thread, &QThread::finished, portthread, &QThread::deleteLater);
	// ���������� ������ �� ������
	connect(portthread, &PortThread::ThrowError, this, &DebuggerWidget::HandleError);
	// �������� ������ ����� � ����� � �����
    connect(this, &DebuggerWidget::SetComNumber, portthread, &PortThread::SetComNum, Qt::DirectConnection);
    // ���������� �����
    connect(portthread, &PortThread::toForm, this, &DebuggerWidget::updateForm);
    // ���������� ��������� ������
    connect(portthread, &PortThread::SafeStop, this, &DebuggerWidget::StopThreading);

	// ������ ������ (����� ���������� exec ����������� run, ������������� � PortThread)
    main_thread->start();

	// ����� ��������� - ����� �������� ��� ��������� ����� �����
	emit SetComNumber(ui->comNUM->text());

    // ������������ ������ �������
    ui->OPNButton->setEnabled(false);
    // ���������� ������ �������
    ui->CLSButton->setEnabled(true);
    // ������������ ���� ��� ������ �����
    ui->comNUM->setEnabled(false);
}

void DebuggerWidget::closeBTN()
{
	// ��������� ����� ����������� (��� �� ���������, ������� ����� ������� ����������)
    main_thread->requestInterruption();
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
