#include "ModeWindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char* argv[])
{
    // TODO: set port (--port)
    QApplication app(argc, argv);
    ModeWindow mode_window(nullptr, 5353);
    mode_window.setWindowTitle(QApplication::tr("Крестики-Нолики"));
    mode_window.setWindowIcon(QIcon("../images/TicTacToeWindow.png"));
    mode_window.show();
    return app.exec();
}
