#include "ModeWindow.h"
#include "MainWindow.h"

#include <QRadioButton>
#include <QPushButton>
#include <QSpacerItem>
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QMessageBox>

//=======================================================================
ModeWindow::ModeWindow(QWidget* parent)
    : QFrame(parent)
{
    labelChooseMode = new QLabel(tr("Выберите режим:"), this);
    radioButtonPlayWithHuman = new QRadioButton(tr("Игра с человеком"), this);
    radioButtonPlayWithEasyCPU = new QRadioButton(tr("Игра с компьютером (просто)"), this);
    radioButtonPlayWithHardCPU = new QRadioButton(tr("Игра с компьютером (сложно)"), this);
    radioButtonPlayWithHumanHost = new QRadioButton(tr("Игра по сети (создать игру)"), this);
    radioButtonPlayWithHumanJoin = new QRadioButton(tr("Игра по сети (подключиться к игре)"), this);

    labelIPAddress = new QLabel(tr("IP-Адрес:"), this);
    lineIPAddress = new QLineEdit(this);
    labelPort = new QLabel(tr("Адрес порта:"), this);
    linePort = new QLineEdit(this);
    pushButtonStart = new QPushButton(tr("Начать"), this);
    spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vBoxLayout = new QVBoxLayout(this);

    vBoxLayout->addWidget(labelChooseMode);
    vBoxLayout->addWidget(radioButtonPlayWithHuman);
    vBoxLayout->addWidget(radioButtonPlayWithEasyCPU);
    vBoxLayout->addWidget(radioButtonPlayWithHardCPU);
    vBoxLayout->addWidget(radioButtonPlayWithHumanHost);
    vBoxLayout->addWidget(radioButtonPlayWithHumanJoin);

    vBoxLayout->addWidget(labelIPAddress);
    vBoxLayout->addWidget(lineIPAddress);
    vBoxLayout->addWidget(labelPort);
    vBoxLayout->addWidget(linePort);
    vBoxLayout->addWidget(pushButtonStart);
    vBoxLayout->addItem(spacer);


    labelIPAddress->hide();
    lineIPAddress->hide();
    labelPort->hide();
    linePort->hide();

    setMinimumSize(350, 180);

    connect(pushButtonStart, SIGNAL(clicked()),
            this, SLOT(pushButtonStart_clicked())
            );
    connect(radioButtonPlayWithHumanJoin, SIGNAL(toggled(bool)),
            this, SLOT(radioButtonPlayWithHumanJoin_toggled(bool))
            );
}
//=======================================================================
void ModeWindow::radioButtonPlayWithHumanJoin_toggled(bool is_checked)
{
    if (is_checked)
    {
        //spacer->changeSize();
        setFixedSize(350, 270);
        labelIPAddress->show();
        lineIPAddress->show();
        labelPort->show();
        linePort->show();
    }
    else
    {
        setFixedSize(350, 180);
        labelIPAddress->hide();
        lineIPAddress->hide();
        labelPort->hide();
        linePort->hide();
    }
}
//=======================================================================
void ModeWindow::pushButtonStart_clicked()
{
    if (radioButtonPlayWithHuman->isChecked())
        createMainWindow(Mode::PlayWithHuman);
    else if (radioButtonPlayWithEasyCPU->isChecked())
        createMainWindow(Mode::PlayWithEasyCPU);
    else if (radioButtonPlayWithHardCPU->isChecked())
        createMainWindow(Mode::PlayWithHardCPU);

    else if (radioButtonPlayWithHumanHost->isChecked())
        createMainWindow(Mode::PlayWithHumanHost); // TODO: wait for join etc.
    else if (radioButtonPlayWithHumanJoin->isChecked())
    {
        // ip and port checking, trying to join
        if (lineIPAddress->text().isEmpty() || linePort->text().isEmpty())
        {
            QMessageBox::information(this, tr("Крестики-нолики"),
                                     tr("Введите IP-адрес и порт игрока."));
        }



    }
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
