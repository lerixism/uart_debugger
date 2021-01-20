#include "debuggerwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DebuggerWidget w;
    w.show();

    return a.exec();
}
