#pragma once

#include <QPushButton>
#include <QFrame>
#include <QSpacerItem>
#include <QLabel>
#include <QVBoxLayout>


enum class Mode
{
    PlayWithHuman,
    PlayWithEasyCPU,
    PlayWithHardCPU
};

class ModeWindow : public QFrame
{
    Q_OBJECT
public:
    ModeWindow(QWidget* parent = nullptr);
    ~ModeWindow();

private:
    QSpacerItem* spacer                    = nullptr;
    QLabel* labelChooseMode                = nullptr;
    QPushButton* pushButtonPlayWithHuman   = nullptr;
    QPushButton* pushButtonPlayWithEasyCPU = nullptr;
    QPushButton* pushButtonPlayWithHardCPU = nullptr;
    QVBoxLayout* vBoxLayout                = nullptr;

private slots:
    void pushButtonPlayWithHuman_clicked();
    void pushButtonPlayWithEasyCPU_clicked();
    void pushButtonPlayWithHardCPU_clicked();
    void createMainWindow(Mode mode);
public slots:
    void showModeWindow();
};
