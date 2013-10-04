#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef enum {
        LED_STATE_ALL_OFF = 0,
        LED_STATE_ALL_ON,
        LED_STATE_SINGLE,
        LED_STATE_AUTO,
        LED_STATE_NONE
    } LED_STATE;
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void readPendingDatagrams();

    void on_listenButton_clicked();
    void on_sendButton_clicked();
    void on_autoButton_clicked();
    void on_allOnButton_clicked();
    void on_allOffButton_clicked();
    void on_resendButton_clicked();
    void on_posSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QUdpSocket *mUdpSocket;

    void appendGuiParameters(QByteArray &array);
};

#endif // MAINWINDOW_H
