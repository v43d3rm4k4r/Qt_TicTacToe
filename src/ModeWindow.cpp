#include "ModeWindow.h"
#include "MainWindow.h"

//=======================================================================
ModeWindow::ModeWindow(QWidget* parent)
    : QFrame(parent)
{
    spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    labelChooseMode = new QLabel("Выберите режим:");
    pushButtonPlayWithHuman = new QPushButton("Игра с человеком");
    pushButtonPlayWithEasyCPU = new QPushButton("Игра с компьютером (просто)");
    pushButtonPlayWithHardCPU = new QPushButton("Игра с компьютером (сложно)");
    vBoxLayout = new QVBoxLayout(this);

    vBoxLayout->addItem(spacer);
    vBoxLayout->addWidget(labelChooseMode);
    vBoxLayout->addWidget(pushButtonPlayWithHuman);
    vBoxLayout->addWidget(pushButtonPlayWithEasyCPU);
    vBoxLayout->addWidget(pushButtonPlayWithHardCPU);

    setFixedSize(350, 120);

    connect(pushButtonPlayWithHuman, SIGNAL(clicked()),
            this, SLOT(pushButtonPlayWithHuman_clicked())
            );
    connect(pushButtonPlayWithEasyCPU, SIGNAL(clicked()),
            this, SLOT(pushButtonPlayWithEasyCPU_clicked())
            );
    connect(pushButtonPlayWithHardCPU, SIGNAL(clicked()),
            this, SLOT(pushButtonPlayWithHardCPU_clicked())
            );
}
//=======================================================================
void ModeWindow::pushButtonPlayWithHuman_clicked()
{
    createMainWindow(Mode::PlayWithHuman);
}
//=======================================================================
void ModeWindow::pushButtonPlayWithEasyCPU_clicked()
{
    createMainWindow(Mode::PlayWithEasyCPU);
}
//=======================================================================
void ModeWindow::pushButtonPlayWithHardCPU_clicked()
{
    createMainWindow(Mode::PlayWithHardCPU);
}
//=======================================================================
void ModeWindow::createMainWindow(Mode mode)
{
    MainWindow* main_window = new MainWindow(mode, this);
    connect(main_window, SIGNAL(MainWindowClosed()),
            this, SLOT(showModeWindow())
            );
    main_window->show();

    hide();
}
//=======================================================================
void ModeWindow::showModeWindow()
{
    show();
}
//=======================================================================
ModeWindow::~ModeWindow()
{
}
