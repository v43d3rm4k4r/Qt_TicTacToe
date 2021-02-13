#include "ModeWindow.h"

#include <QApplication>
#include <QDebug>
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    ModeWindow mode_window;
    mode_window.setWindowTitle("Крестики-Нолики");
    mode_window.show();
    return app.exec();
}
