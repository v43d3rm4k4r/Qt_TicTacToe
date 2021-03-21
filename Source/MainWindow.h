#pragma once

#include <QMainWindow>



#include "ModeWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Mode mode, ClientServer* client_server = nullptr,
                        QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    Mode mode;
    bool is_first_move_mode;
    char field[9];
    void setFirstMoveMode();
    QVector<QPushButton*> getButtons() const;
    int8_t getChosenButton() const;
    int8_t checkField() const;
    void clearField();
    void AIMove();
    void AIMakingMove(int8_t cell);
    void continuePlay();

private slots:
    void pushButtonX_clicked();
    void pushButtonO_clicked();

protected:
    void closeEvent(QCloseEvent* event) override;

signals:
    void MainWindowClosed();
};

