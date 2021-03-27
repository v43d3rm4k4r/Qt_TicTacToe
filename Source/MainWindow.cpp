#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QVector>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>

//=======================================================================
MainWindow::MainWindow(Mode mode, ClientServer* client_server, QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    mode{mode},
    client_server{client_server}
{
    ui->setupUi(this);
    setWindowTitle(tr("Крестики-Нолики"));
    connect(ui->pushButtonX, SIGNAL(clicked()),
            this, SLOT(pushButtonX_clicked())
            );
    connect(ui->pushButtonO, SIGNAL(clicked()),
            this, SLOT(pushButtonO_clicked())
            );
    if (mode == Mode::PlayWithHumanHost || mode == Mode::PlayWithHumanJoin)
    {
        connect(client_server, SIGNAL(signalOpponentMoveNumReceived(int8_t)),
                this, SLOT(opponentMoveNumReceived(int8_t))
                );
    }

    this->show();

    /// primary field settings depending on game mode:

    // по умолчанию у людей крестики ходят первыми, а также в случае, если игра по сети,
    // первым ходит хост
    if (mode == Mode::PlayWithHuman || mode == Mode::PlayWithHumanHost)
    {
        ui->pushButtonO->setEnabled(false);
        if (mode == Mode::PlayWithHumanHost)
        {
            state = NetworkGameState::MakingMove;
            ui->statusbar->showMessage(tr("Ваш ход"));
        }
    }

    if (mode == Mode::PlayWithHumanJoin)
    {
        state = NetworkGameState::WaitingForOpponent;
        ui->pushButtonX->setEnabled(false);
        ui->pushButtonO->setEnabled(false);
        ui->statusbar->showMessage(tr("Противник размышляет..."));
    }

    // если играем с компом, то нужно организовать выбор, ходить первым или нет
    if (mode == Mode::PlayWithEasyCPU || mode == Mode::PlayWithHardCPU)
        setFirstMoveMode();


}
//=======================================================================
void MainWindow::setFirstMoveMode()
{
    QMessageBox::StandardButton reply =
    QMessageBox::question(this, tr("Крестики-нолики"), tr("Ходить первым?"),
                                    QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        is_first_move_mode = true;
        ui->pushButtonO->setEnabled(false);
        // вызвать когда игрок сделает ход:
        //AIMove();
    }
    else
    {
        is_first_move_mode = false;
        AIMove();
    }
}
//=======================================================================
void MainWindow::pushButtonX_clicked()
{
    // сначала узнаём, какая кнопка выбрана, и выбрана ли
    // вообще
    QVector<QPushButton*> buttons = getButtons();
    int8_t chosen_button_num = getChosenButton();

    if (chosen_button_num == -1)
    {
        QMessageBox::warning(this, tr("Крестики-Нолики"),
                             tr("Сначала выберите ячейку!"));
        return;
    }

    // если выбранная ячейка уже заполнена - выходим
    if (buttons[chosen_button_num]->text() == 'X' ||
        buttons[chosen_button_num]->text() == 'O')
        return;

    // теперь мы знаем какая кнопка была выбрана,
    // сначала изменить палитру(цвет текста кнопки)
    // на красный + поменять размер шрифта теста
    QPalette palette;
    QBrush brush1(QColor(255, 0, 0, 255));
    //buttons[chosen_button_num]->setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, brush1);
    buttons[chosen_button_num]->setPalette(palette);
    QFont font;
    font.setPointSize(70);
    buttons[chosen_button_num]->setFont(font);
    buttons[chosen_button_num]->setText("X");


    // меняем модель поля
    field[chosen_button_num] = 'X';

    // если играем по сети, отправляем номер ячейки, но только в случае, если не зашли в
    // метод для отображения хода противника - в этом случае не нужно ничего отправлять
    if ((mode == Mode::PlayWithHumanHost || mode == Mode::PlayWithHumanJoin)
        && state != NetworkGameState::WaitingForOpponent && state != NetworkGameState::GameOver)
    client_server->sendDatagram(':' + QString::number(chosen_button_num), client_server->opponentAddress(),
                                client_server->opponentPort());

    // после чего проверям обстановку на поле
    if (checkField() == 1)
    {
        QMessageBox::information(this, tr("Крестики-Нолики"),
                                 tr("Победили нолики!"));
        clearField();
        if ((mode == Mode::PlayWithHardCPU || mode == Mode::PlayWithEasyCPU)
                && !is_first_move_mode)
            continuePlay();
        return;
    }

    else if (checkField() == -1)
    {
        QMessageBox::information(this, tr("Крестики-Нолики"),
                                 tr("Победили крестики!"));
        clearField();
        if ((mode == Mode::PlayWithHardCPU || mode == Mode::PlayWithEasyCPU)
                && !is_first_move_mode)
            continuePlay();
        return;
    }
    else if (checkField() == -2)
    {
        QMessageBox::information(this, tr("Крестики-Нолики"),
                                 tr("Ничья!"));
        clearField();
        if ((mode == Mode::PlayWithHardCPU || mode == Mode::PlayWithEasyCPU)
                && !is_first_move_mode)
            continuePlay();
        /*if (mode == Mode::PlayWithHumanHost || mode == Mode::PlayWithHumanHost)
            continueNetworkPlay(mode);*/
        return;
    }

    // если играем с компьютером, и ходим первыми, после нашего хода ходит компьютер
    if ((mode == Mode::PlayWithHardCPU || mode == Mode::PlayWithEasyCPU)
            && is_first_move_mode)
        AIMove();

    if (mode == Mode::PlayWithHuman)
    {
        ui->pushButtonX->setEnabled(false);
        ui->pushButtonO->setEnabled(true);
    }

    if (mode == Mode::PlayWithHumanHost && state != NetworkGameState::GameOver)
    {
        ui->pushButtonX->setEnabled(false);
        state = NetworkGameState::WaitingForOpponent;
        ui->statusbar->showMessage(tr("Противник размышляет..."));
    }
}
//=======================================================================
void MainWindow::continuePlay()
{
    ui->pushButtonX->setEnabled(false);
    ui->pushButtonO->setEnabled(true);
    AIMove();
}
/*//=======================================================================
void MainWindow::continueNetworkPlay(Mode mode)
{
    if (mode == Mode::PlayWithHumanHost)
    {
        ui->pushButtonX->setEnabled(true);
        ui->pushButtonO->setEnabled(false);
    }
    else if (mode == Mode::PlayWithHumanJoin)
    {
        ui->pushButtonX->setEnabled(false);
        ui->pushButtonO->setEnabled(false);
    }
    //state = NetworkGameState::GameOver;
}
//=======================================================================*/
QVector<QPushButton*> MainWindow::getButtons() const
{
    QVector<QPushButton*> vec;
    vec += ui->pushButton_1;
    vec += ui->pushButton_2;
    vec += ui->pushButton_3;
    vec += ui->pushButton_4;
    vec += ui->pushButton_5;
    vec += ui->pushButton_6;
    vec += ui->pushButton_7;
    vec += ui->pushButton_8;
    vec += ui->pushButton_9;
    return vec;
}
//=======================================================================
void MainWindow::pushButtonO_clicked()
{
    QVector<QPushButton*> buttons = getButtons();
    int8_t chosen_button_num = getChosenButton();

    if (buttons[chosen_button_num]->text() == 'X' ||
        buttons[chosen_button_num]->text() == 'O')
        return;

    if (chosen_button_num == -1)
        QMessageBox::warning(this, tr("Крестики-Нолики"),
                             tr("Сначала выберите ячейку!"));

    QPalette palette;
    QBrush brush1(QColor(51, 129, 255, 255));
    //buttons[chosen_button_num]->setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, brush1);
    buttons[chosen_button_num]->setPalette(palette);
    QFont font;
    font.setPointSize(70);
    buttons[chosen_button_num]->setFont(font);
    buttons[chosen_button_num]->setText("O");

    field[chosen_button_num] = 'O';

    // если играем по сети, отправляем номер ячейки, но только в случае, если не зашли в
    // метод для отображения хода противника - в этом случае не нужно ничего отправлять
    if ((mode == Mode::PlayWithHumanHost || mode == Mode::PlayWithHumanJoin)
            && state != NetworkGameState::WaitingForOpponent)
    client_server->sendDatagram(':' + QString::number(chosen_button_num), client_server->opponentAddress(),
                                client_server->opponentPort());

    if (checkField() == 1)
    {
        QMessageBox::information(this, tr("Крестики-Нолики"),
                             tr("Победили нолики!"));
        clearField();
        if ((mode == Mode::PlayWithHardCPU || mode == Mode::PlayWithEasyCPU)
                && !is_first_move_mode)
            continuePlay();
        /*if (mode == Mode::PlayWithHumanHost || mode == Mode::PlayWithHumanHost )
            continueNetworkPlay(mode);*/
        return;
    }

    else if (checkField() == -1)
    {
        QMessageBox::information(this, tr("Крестики-Нолики"),
                             tr("Победили крестики!"));
        clearField();
        if ((mode == Mode::PlayWithHardCPU || mode == Mode::PlayWithEasyCPU)
                && !is_first_move_mode)
            continuePlay();
        return;
    }
    else if (checkField() == -2)
    {
        QMessageBox::information(this, tr("Крестики-Нолики"),
                                 tr("Ничья!"));
        clearField();
        if ((mode == Mode::PlayWithHardCPU || mode == Mode::PlayWithEasyCPU)
                && !is_first_move_mode)
            continuePlay();
        return;
    }

    // если играем с компьютером, и ходим вторыми, после нашего хода ходит компьютер
    if ((mode == Mode::PlayWithHardCPU || mode == Mode::PlayWithEasyCPU)
            && !is_first_move_mode)
        AIMove();

    if (mode == Mode::PlayWithHuman)
    {
        ui->pushButtonO->setEnabled(false);
        ui->pushButtonX->setEnabled(true);
    }

    if (mode == Mode::PlayWithHumanJoin && state != NetworkGameState::GameOver)
    {
        ui->pushButtonO->setEnabled(false);
        state = NetworkGameState::WaitingForOpponent;
        ui->statusbar->showMessage(tr("Противник размышляет..."));
    }
}
//=======================================================================
int8_t MainWindow::getChosenButton() const
{
    QVector<QPushButton*> buttons = getButtons();
    int8_t chosen_button_num = 0;
    bool some_button_checked = false;
    foreach (const QPushButton* current, buttons)
    {
        if (current->isChecked())
        {
            some_button_checked = true;
            break;
        }
        ++chosen_button_num;
    }
    if (!some_button_checked)
    {
        return -1;
    }
    return chosen_button_num;
}
//=======================================================================
void MainWindow::opponentMoveNumReceived(int8_t cell)
{
    ui->statusbar->showMessage(tr("Ваш ход"));
    // Здесь нужно проигнорить (обработать) последний сигнал от оппонента,
    // когда итак известно чем закончилась игра!
    if (mode == Mode::PlayWithHumanJoin)
        field[cell] = 'X';
    if (mode == Mode::PlayWithHumanHost)
        field[cell] = 'O';

    // если получаем 1, -1 или -2 - игра окончена
    if (checkField())
         state = NetworkGameState::GameOver;



    // выделить соответствующую ячейку
    QVector<QPushButton*> buttons = getButtons();
    buttons[cell]->setChecked(true);

    // вызвать метод в зависимости от того, кто играет,
    // если играет хост, то вызвать нажатие О, и наоборот
    // в конце разблокировать кнопку в зависимости от хоста или джоина
    if (mode == Mode::PlayWithHumanJoin)
    {
        pushButtonX_clicked(); // отображаем ход противника
        // если при отображении хода противника игра закончилась
        // (state == GameOver), то меняем state в зависимости
        // от хоста/джоина и выходим отсюда, чтобы не менять кнопки,
        // т.к. они уже изменены методом continueNetworkPlay()
        if (state == NetworkGameState::GameOver)
        {
            state = NetworkGameState::WaitingForOpponent;
            return;
        }

        ui->pushButtonO->setEnabled(true);
        state = NetworkGameState::MakingMove;
    }

    else if (mode == Mode::PlayWithHumanHost)
    {
        pushButtonO_clicked(); // отображаем ход противника
        // то же самое - если при ходе оппонента произошла
        // ничья или вы проиграли - выходим
        if (state == NetworkGameState::GameOver)
        {
            state = NetworkGameState::MakingMove;
            return;
        }

        ui->pushButtonX->setEnabled(true);
        state = NetworkGameState::MakingMove;
    }
}
//=======================================================================
int8_t MainWindow::checkField() const
{
    QVector<QPushButton*> buttons = getButtons();

    // если все ячейки заполнены, но победителя нет, то пишем о
    // ничье
    bool have_empty_cells = false;
    for (const char& current : field)
    {
        if (current != 'X' && current != 'O')
            have_empty_cells = true;
    }
    if (!have_empty_cells)
        return -2;

    // возвращаем -1 если победили крестики,
    // 0 если пока никто и 1 если нолики
    if ((field[0] == 'X' && field[3] == 'X' && field[6] == 'X') ||
        (field[1] == 'X' && field[4] == 'X' && field[7] == 'X') ||
        (field[2] == 'X' && field[5] == 'X' && field[8] == 'X') ||
        (field[0] == 'X' && field[1] == 'X' && field[2] == 'X') ||
        (field[3] == 'X' && field[4] == 'X' && field[5] == 'X') ||
        (field[6] == 'X' && field[7] == 'X' && field[8] == 'X') ||
        (field[0] == 'X' && field[4] == 'X' && field[8] == 'X') ||
        (field[2] == 'X' && field[4] == 'X' && field[6] == 'X'))
    {
        return -1;
    }
    else if
        ((field[0] == 'O' && field[3] == 'O' && field[6] == 'O') ||
         (field[1] == 'O' && field[4] == 'O' && field[7] == 'O') ||
         (field[2] == 'O' && field[5] == 'O' && field[8] == 'O') ||
         (field[0] == 'O' && field[1] == 'O' && field[2] == 'O') ||
         (field[3] == 'O' && field[4] == 'O' && field[5] == 'O') ||
         (field[6] == 'O' && field[7] == 'O' && field[8] == 'O') ||
         (field[0] == 'O' && field[4] == 'O' && field[8] == 'O') ||
         (field[2] == 'O' && field[4] == 'O' && field[6] == 'O'))
    {
        return 1;
    }
    else
        return 0;
}
//=======================================================================
void MainWindow::AIMove()
{

    if (mode == Mode::PlayWithEasyCPU)
    {
        ui->pushButtonO->setEnabled(false);
        ui->pushButtonX->setEnabled(false);
        srand(time(nullptr));
        bool is_find_free_cell = false;
        int8_t cell = 0;
        ui->statusbar->showMessage(tr("Компьютер думает..."));

        while (!is_find_free_cell)
        {
            cell = rand() % 9;
            // проверяем свободна ли ячейка
            if (field[cell] != 'X' && field[cell] != 'O')
                is_find_free_cell = true;
        }
        ui->statusbar->clearMessage();

        // когда нашли свободную ячейку, вызываем соответсвующий метод
        AIMakingMove(cell);
    }

    // TODO:
    // если компьютер ходит первым
    /*if (!is_first_move_mode && mode == Mode::PlayWithHardCPU)
    {

    }
    else if ((is_first_move_mode && mode == Mode::PlayWithHardCPU))
    {

    }*/
}
//=======================================================================
void MainWindow::AIMakingMove(int8_t cell)
{
    QVector<QPushButton*> buttons = getButtons();
    buttons[cell]->setChecked(true);

    // Если игрок ходит вторым, компьютер нажимает на X
    if (!is_first_move_mode)
    {
        ui->pushButtonO->setEnabled(true);
        ui->pushButtonX->setEnabled(false);
        pushButtonX_clicked();
    }
    else
    {
        ui->pushButtonO->setEnabled(false);
        ui->pushButtonX->setEnabled(true);
        pushButtonO_clicked();
    }
}
//=======================================================================
void MainWindow::clearField()
{
    for (int8_t i = 0; i < 9; ++i)
        field[i] = 0;
    QVector<QPushButton*> buttons = getButtons();
    foreach (QPushButton* current, buttons)
        current->setText("");
    if (mode == Mode::PlayWithHuman || mode == Mode::PlayWithEasyCPU ||
        mode == Mode::PlayWithHardCPU)
    {
        ui->pushButtonX->setEnabled(true);
        ui->pushButtonO->setEnabled(false);
    }
    else // network mode
    {
        if (mode == Mode::PlayWithHumanHost)
        {
            ui->statusbar->showMessage(tr("Ваш ход"));
            ui->pushButtonX->setEnabled(true);
            ui->pushButtonO->setEnabled(false);
        }
        else if (mode == Mode::PlayWithHumanJoin)
        {
            ui->statusbar->showMessage(tr("Противник размышляет..."));
            ui->pushButtonX->setEnabled(false);
            ui->pushButtonO->setEnabled(false);
        }
    }
}
//=======================================================================
void MainWindow::closeEvent(QCloseEvent* event)
{
    emit MainWindowClosed();
    event->accept();
}
//=======================================================================
MainWindow::~MainWindow()
{
    delete ui;
}

