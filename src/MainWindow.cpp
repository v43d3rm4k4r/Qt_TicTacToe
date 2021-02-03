#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
//=======================================================================
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButtonX, SIGNAL(clicked()),
            this, SLOT(pushButtonX_clicked())
            );
    connect(ui->pushButtonO, SIGNAL(clicked()),
            this, SLOT(pushButtonO_clicked())
            );
    ui->pushButtonO->setEnabled(false);
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
        return;
    }

    else if (checkField() == -1)
    {
        QMessageBox::information(this, tr("Крестики-Нолики"),
                                 tr("Победили крестики!"));
        clearField();
        return;
    }
    else if (checkField() == -2)
    {
        QMessageBox::information(this, tr("Крестики-Нолики"),
                                 tr("Ничья!"));
        clearField();
        return;
    }

    ui->pushButtonX->setEnabled(false);
    ui->pushButtonO->setEnabled(true);
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
        return;
    }

    else if (checkField() == -1)
    {
        QMessageBox::information(this, tr("Крестики-Нолики"),
                             tr("Победили крестики!"));
        clearField();
        return;
    }
    else if (checkField() == -2)
    {
        QMessageBox::information(this, tr("Крестики-Нолики"),
                                 tr("Ничья!"));
        clearField();
        return;
    }
    ui->pushButtonO->setEnabled(false);
    ui->pushButtonX->setEnabled(true);
}
//=======================================================================
int8_t MainWindow::getChosenButton() const
{
    QVector<QPushButton*> buttons = getButtons();
    int8_t chosen_button_num = 0;
    bool some_button_checked = false;
    for (const QPushButton* current : buttons)
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
    for (const QPushButton* current : buttons)
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
void MainWindow::clearField()
{
    for (int8_t i = 0; i < 9; ++i)
        field[i] = 0;
    QVector<QPushButton*> buttons = getButtons();
    for (QPushButton* current : buttons)
        current->setText("");
    ui->pushButtonX->setEnabled(true);
    ui->pushButtonO->setEnabled(false);
}
//=======================================================================
MainWindow::~MainWindow()
{
    delete ui;
}

