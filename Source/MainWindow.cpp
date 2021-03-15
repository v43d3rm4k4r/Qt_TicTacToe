#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QThread>
//=======================================================================
MainWindow::MainWindow(Mode mode, QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    mode{mode}
{
    ui->setupUi(this);
    setWindowTitle("Крестики-Нолики");
    connect(ui->pushButtonX, SIGNAL(clicked()),
            this, SLOT(pushButtonX_clicked())
            );
    connect(ui->pushButtonO, SIGNAL(clicked()),
            this, SLOT(pushButtonO_clicked())
            );
    this->show();
    // по умолчанию у людей крестики ходят первыми
    if (mode == Mode::PlayWithHuman)
        ui->pushButtonO->setEnabled(false);
    else // если играем с компом, то нужно организовать выбор, ходить первым или нет
    {
        setFirstMoveMode();
    }
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
}
//=======================================================================
void MainWindow::continuePlay()
{
    ui->pushButtonX->setEnabled(false);
    ui->pushButtonO->setEnabled(true);
    AIMove();
}
//=======================================================================
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
int8_t MainWindow::checkField() const
{
    QVector<QPushButton*> buttons = getButtons();

    // если все ячейки заполнены, но победителя нет, то пишем о
    // ничье
    bool have_empty_cells = false;
    foreach (const QPushButton* current, buttons)
    {
        if (current->text() != 'O' && current->text() != 'X')
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
    ui->pushButtonX->setEnabled(true);
    ui->pushButtonO->setEnabled(false);
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

