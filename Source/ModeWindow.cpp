#include "ModeWindow.h"
#include "MainWindow.h"

#include <QRadioButton>
#include <QPushButton>
#include <QSpacerItem>
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QThread>
#include <QKeyEvent>
#include <QTimer>
#include <QNetworkInterface>

#define DEFAULT_PORT 2424

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

    setMinimumSize(390, 180);

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
        setFixedSize(390, 270);
        labelIPAddress->show();
        lineIPAddress->show();
        labelPort->show();
        linePort->show();
    }
    else
    {
        setFixedSize(390, 180);
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
    {
        // позже нужно будет записать IP и порт подключившегося
        client_server = new ClientServer(QHostAddress::LocalHost, DEFAULT_PORT,
                                         QUdpSocket::DefaultForPlatform, this);

        // когда получаем специальную датаграмму - начинаем игру
        connect(client_server, SIGNAL(signalOpponentJoined()),
                this, SLOT(startGameAsHost())
                );

        timer = new QTimer;
        timer->setInterval(500);
        timer->start();
        connect(timer, SIGNAL(timeout()),
                this, SLOT(waitingForOpponent())
                );
    }

    else if (radioButtonPlayWithHumanJoin->isChecked())
    {
        // ip and port checking, trying to join
        if (lineIPAddress->text().isEmpty() || linePort->text().isEmpty())
        {
            QMessageBox::information(this, tr("Крестики-нолики"),
                                     tr("Введите IP-адрес и порт игрока."));
            return;
        }

        // послать по заданному адресу датаграмму "readytostart" и
        // ожидать ответ "go"
        client_server->sendDatagram("readytostart", QHostAddress(lineIPAddress->text()),
                                    linePort->text().toUInt());
    }
}
//=======================================================================
void ModeWindow::createMainWindow(Mode mode, ClientServer* client_server/*=nullptr*/)
{
    MainWindow* main_window = new MainWindow(mode, client_server, this);
    connect(main_window, SIGNAL(MainWindowClosed()),
            this, SLOT(showModeWindow())
            );
    main_window->show();
    hide();
}
//=======================================================================
void ModeWindow::startGameAsHost()
{
    opponent_joined = true;
    client_server->sendDatagram("go", client_server->opponentAddress(),
                                client_server->opponentPort());  // sending "go" answer
    createMainWindow(Mode::PlayWithHumanHost, client_server);
}
//=======================================================================
void ModeWindow::showModeWindow()
{
    show();
}
//=======================================================================
void ModeWindow::keyPressEvent(QKeyEvent* event)
{
    qDebug() << "event->key(): " << event->key();
    if (event->key() == Qt::Key_Escape)
    {
        esc_pressed = true;
    }
}
//=======================================================================
void ModeWindow::waitingForOpponent()
{
    static int call = -1;

    if (opponent_joined || esc_pressed)
    {
        pushButtonStart->setText(tr("Начать"));
        pushButtonStart->setEnabled(true);
        radioButtonPlayWithHumanHost->setText(tr("Игра по сети (создать игру)"));
        opponent_joined = esc_pressed = false;
        call = -1;
        timer->stop();
        delete timer;
        return;
    }

    if (call == -1)
    {
        pushButtonStart->setEnabled(false);
        // finding out and outputting local_IPv4 address
        const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
        foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
        {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
                radioButtonPlayWithHumanHost->setText(tr(QString("Игра по сети (создать игру) [ваш IP: %1][ваш порт: %2]")
                                              .arg(address.toString(), 0, 10).arg(DEFAULT_PORT, 0, 10)
                                                         .toStdString().c_str()
                                              ));
        }

        ++call;
    }
    else if (call == 0)
    {
        pushButtonStart->setText(tr("Ожидаем оппонента."));
        ++call;
    }
    else if (call == 1)
    {
        pushButtonStart->setText(tr("Ожидаем оппонента.."));
        ++call;
    }
    else if (call == 2)
    {
        pushButtonStart->setText(tr("Ожидаем оппонента..."));
        call = 0;
    }
}
//=======================================================================
ModeWindow::~ModeWindow()
{
}
