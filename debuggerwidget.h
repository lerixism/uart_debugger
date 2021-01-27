#ifndef DEBUGGERWIDGET_H
#define DEBUGGERWIDGET_H

#include "ui_debuggerwidget.h"
#include "portthread.h"
#include <QObject>
#include <QWidget>

namespace Ui {
class DebuggerWidget;
}

class DebuggerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DebuggerWidget(QWidget *parent = nullptr);
    ~DebuggerWidget();

public slots:
    void updateForm(unsigned int, unsigned int, unsigned int, float, float);
	void openBTN();
	void closeBTN();
	void HandleError(QString);
    void StopThreading();

signals:
	void SetComNumber(QString);

private:
	Ui::DebuggerWidget *ui;
    QThread * main_thread;
	PortThread *portthread;
};

#endif // DEBUGGERWIDGET_H
