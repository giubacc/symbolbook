#include "main_dlg.h"
#include <QApplication>

#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setApplicationVersion(QUOTE(APP_VERSION));
    qApp->setApplicationName(QUOTE(APP_NAME));
    qApp->setOrganizationDomain("https://github.com/richter82/symbolbook");
    MainDlg w;
    w.setWindowTitle(qApp->applicationName() + QString(" - ") + qApp->applicationVersion() + " - " + __DATE__);
    w.show();
    return a.exec();
}
