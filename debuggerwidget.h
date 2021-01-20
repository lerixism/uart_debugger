#ifndef DEBUGGERWIDGET_H
#define DEBUGGERWIDGET_H

#include "ui_debuggerwidget.h"
#include "portworker.h"

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
    void StopThread();
    void updateForm(unsigned int second, unsigned int freecpucnt, unsigned int DSPerrcnt);
	void openBTN();
	void closeBTN();

private:
    Ui::DebuggerWidget *ui;
    QThread *portthread;
    PortWorker *portworker;
};

#endif // DEBUGGERWIDGET_H
