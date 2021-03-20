#include "ModeWindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    ModeWindow mode_window;
    mode_window.setWindowTitle("Крестики-Нолики");
    mode_window.setWindowIcon(QIcon("../images/TicTacToeWindow.png"));
    mode_window.show();
    return app.exec();
}
