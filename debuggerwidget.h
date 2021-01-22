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
    void updateForm(unsigned int second, unsigned int freecpucnt, unsigned int DSPerrcnt);
	void openBTN();
	void closeBTN();
	void OpenError(QString);

signals:
	void SetComNumber(QString);

private:
	Ui::DebuggerWidget *ui;
	PortThread *portthread;
};

#endif // DEBUGGERWIDGET_H
