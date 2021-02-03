#pragma once

#include <QMainWindow>
#include <QVector>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    char field[9];
    QVector<QPushButton*> getButtons() const;
    int8_t getChosenButton() const;
    int8_t checkField() const;
    void clearField();

private slots:
    void pushButtonX_clicked();
    void pushButtonO_clicked();
};

