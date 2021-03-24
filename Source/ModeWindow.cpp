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

// TODO:
// - при создании игры вывести сообщение как отменить ожидание подключения
// - включить кнопку после отмены ожидания подключения

//=======================================================================
ModeWindow::ModeWindow(QWidget* parent, uint16_t port)
    : QFrame(parent), port{port}
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
    labelPushESCToExit = new QLabel(tr("Для отмены нажмите ESC"), this);
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
    vBoxLayout->addWidget(labelPushESCToExit);
    vBoxLayout->addItem(spacer);


    labelIPAddress->hide();
    lineIPAddress->hide();
    labelPort->hide();
    linePort->hide();
    labelPushESCToExit->hide();

    setMinimumSize(390, 180);

    getOuterIPv4Address();

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
        setAllRadioButtonsEnabled(false);
        labelPushESCToExit->show();
        setFixedSize(390, 200);


        // if user choose join, than host, or vice versa
        if (client_server != nullptr)
            delete client_server;

        // позже нужно будет записать IP и порт подключившегося
        client_server = new ClientServer(outer_IPv4_address, port,
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
        if (client_server != nullptr)
            delete client_server;
        pushButtonStart->setText(tr("Начать"));
        pushButtonStart->setEnabled(true);

        client_server = new ClientServer(outer_IPv4_address, port,
                                         QUdpSocket::DefaultForPlatform, this);
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
        connect(client_server, SIGNAL(signalHostReadyToStart()),
                this, SLOT(startGameAsJoin())
                );
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
void ModeWindow::startGameAsJoin()
{
    createMainWindow(Mode::PlayWithHumanJoin, client_server);
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
        setAllRadioButtonsEnabled(true);
        labelPushESCToExit->hide();
        setFixedSize(390, 180);
        call = -1;
        timer->stop();
        delete timer;
        return;
    }

    if (call == -1)
    {
        pushButtonStart->setEnabled(false);

        radioButtonPlayWithHumanHost->setText(tr(QString("Игра по сети (создать игру) [ваш IP: %1][ваш порт: %2]")
                                                     .arg(outer_IPv4_address.toString(), 0, 10).arg(port, 0, 10)
                                                     .toStdString().c_str()
                                                 ));
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
void ModeWindow::getOuterIPv4Address()
{
    QHostAddress localhost(QHostAddress::LocalHost);
    foreach (QHostAddress address, QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
        {
            outer_IPv4_address = address;
            break;
        }
    }
}
//=======================================================================
void ModeWindow::setAllRadioButtonsEnabled(bool enabled)
{
    radioButtonPlayWithHuman->setEnabled(enabled);
    radioButtonPlayWithEasyCPU->setEnabled(enabled);
    radioButtonPlayWithHardCPU->setEnabled(enabled);
    //radioButtonPlayWithHumanHost->setEnabled(enabled); // not necessary to unable
    radioButtonPlayWithHumanJoin->setEnabled(enabled);
}
//=======================================================================
ModeWindow::~ModeWindow()
{
}
