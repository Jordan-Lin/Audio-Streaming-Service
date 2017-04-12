#ifndef CALLDIALOGUE_H
#define CALLDIALOGUE_H

#include <WinSock2.h>
#include <atomic>

#include <QDialog>
#include <QDebug>
#include <QByteArray>
#include <QtMultiMedia/QAudioInput>
#include <QtMultiMedia/QAudioOutput>
#include <QBuffer>

#include "utilities.h"

static constexpr int MAX_BUFF_SIZE = 65000;
static constexpr int CALL_PORT = 0;

namespace Ui {
class CallDialogue;
}

class CallDialogue : public QDialog
{
    Q_OBJECT

public:
    // Ctor Dtor
    explicit CallDialogue(QWidget *parent = 0);
    ~CallDialogue();

    // Get Instance
    static CallDialogue *get();

    void init();
    void closeEvent(QCloseEvent *event);
    void reject();

//    const int getIndex() { return index; }
    const std::string getContact() { return contact; }
    const SOCKET getSocket() { return callSock; }
//    void setIndex(const int& i);

    void setContact(const std::string& c) { contact = c; }
    void setIp(const u_long i) { ip = i; }
    void setSocket(const SOCKET& s) { callSock = s; }

//    void print(std::string p) { qDebug() << QString(p); }

private:
    void initializeAudio();
    void createAudioInput();
    void createAudioOutput();
    void CallConnect();
    void receive();

    int ApplyVolumeToSample(short iSample);

private slots:
    void readMore();
//    void on_pushButton_clicked();
    void on_horizontalSlider_valueChanged(int value);

    void on_B_HangUp_clicked();

private:
    // UI Pointer for ui manipulation
    Ui::CallDialogue *ui;

    // Singleton variable.
    static CallDialogue *instance;

    // Contact Variables
    u_long ip;
    std::string contact;

    // Socket Variables
    SOCKET callSock;
    char voiceDataBuff[MAX_BUFF_SIZE];
    sockaddr_in sockAdd;

//    Ui::MainWindow *ui;
    // Audio input/output variables
    QAudioDeviceInfo m_Inputdevice;
    QAudioDeviceInfo m_Outputdevice;
    QAudioFormat m_format;
    QAudioInput *m_audioInput;
    QAudioOutput *m_audioOutput;
    QIODevice *m_input;
    QIODevice *m_output;
    QByteArray m_buffer;
    int m_iVolume;

    // Thread handling
    std::atomic<bool> receiving;
};

#endif // CALLDIALOGUE_H
