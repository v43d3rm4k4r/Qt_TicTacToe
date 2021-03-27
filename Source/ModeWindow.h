#pragma once

#include "ClientServer.h"
#include <QFrame>

class QSpacerItem;
class QLabel;
class QRadioButton;
class QPushButton;
class QVBoxLayout;
class QLineEdit;

//=======================================================================
enum class Mode : uint8_t
{
    PlayWithHuman,
    PlayWithEasyCPU,
    PlayWithHardCPU,
    PlayWithHumanHost,
    PlayWithHumanJoin
};
//=======================================================================
class ModeWindow : public QFrame
{
    Q_OBJECT

public:
    explicit ModeWindow(QWidget* parent = nullptr, uint16_t port = 1234);
    ~ModeWindow();

private:
    QSpacerItem* spacer                        = nullptr;
    QLabel* labelChooseMode                    = nullptr;
    QRadioButton* radioButtonPlayWithHuman     = nullptr;
    QRadioButton* radioButtonPlayWithEasyCPU   = nullptr;
    QRadioButton* radioButtonPlayWithHardCPU   = nullptr;
    QRadioButton* radioButtonPlayWithHumanHost = nullptr;
    QRadioButton* radioButtonPlayWithHumanJoin = nullptr;

    // hided if radioButtonPlayWithHumanJoin is not checked
    QLabel* labelIPAddress                     = nullptr;
    QLineEdit* lineIPAddress                   = nullptr;
    QLabel* labelPort                          = nullptr;
    QLineEdit* linePort                        = nullptr;

    QPushButton* pushButtonStart               = nullptr;
    QLabel* labelPushESCToExit                 = nullptr;
    QVBoxLayout* vBoxLayout                    = nullptr;

    ClientServer* client_server                = nullptr;
    bool opponent_joined                       = false;
    bool esc_pressed                           = false;
    QTimer* timer                              = nullptr;

    QHostAddress outer_IPv4_address;
    uint16_t port;

    void getOuterIPv4Address();
    void setAllRadioButtonsEnabled(bool enabled);

protected:
    virtual void keyPressEvent(QKeyEvent* event) override;

private slots:
    void pushButtonStart_clicked();
    void radioButtonPlayWithHumanJoin_toggled(bool is_checked);
    void startGameAsHost();
    void startGameAsJoin();
    void createMainWindow(Mode mode, ClientServer* client_server = nullptr);
    void waitingForOpponent();

public slots:
    void showModeWindow();
};
