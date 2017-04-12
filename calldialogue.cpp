/*------------------------------------------------------------------------------
-- SOURCE FILE:  calldialogue.cpp
--
-- PROGRAM: CommAudio
--
-- FUNCTIONS:
--          explicit CallDialogue(QWidget *parent = 0);
            ~CallDialogue();
            static CallDialogue *get();
            void init();
            void closeEvent(QCloseEvent *event);
            void reject();
            void initializeAudio();
            void createAudioInput();
            void createAudioOutput();
            void callConnect();
            void receive();
            int ApplyVolumeToSample(short iSample);
--
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Trista Huang, Jordan Lin
--
-- PROGRAMMER: Trista Huang, Jordan Lin
--
-- NOTES: This is the singleton that handles client to client VOIP
--
------------------------------------------------------------------------------*/

#include "calldialogue.h"
#include "ui_calldialogue.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <thread>
#include "debugwindow.h"
#include "defines.h"

using namespace std;

const int BufferSize = 14096;

CallDialogue *CallDialogue::instance = nullptr;

/*------------------------------------------------------------------------------
-- FUNCTION: CallDialogue::CallDialogue(QWidget *parent)
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin, Trista Huang
--
-- PROGRAMMER: Jordan Lin, Trista Huang
--
-- INTERFACE: CallDialogue::CallDialogue(QWidget *parent)
--
-- PARAMETERS: parent: QWidget from the parent
--
-- RETURNS: CallDialogue
--
-- NOTES: Constructor for CallDialogue
--
------------------------------------------------------------------------------*/
CallDialogue::CallDialogue(QWidget *parent) :
    QDialog(parent),
    m_Inputdevice(QAudioDeviceInfo::defaultInputDevice())
        ,   m_Outputdevice(QAudioDeviceInfo::defaultOutputDevice())
        ,   m_audioInput(0)
        ,   m_audioOutput(0)
        ,   m_input(0)
        ,   m_iVolume(0)
        ,   m_buffer(BufferSize, 0)
        ,
    ui(new Ui::CallDialogue)
{
    ui->setupUi(this);
}

/*------------------------------------------------------------------------------
-- FUNCTION: CallDialogue::~CallDialogue
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: CallDialogue::~CallDialogue
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Destructor for CallDialogue
--
------------------------------------------------------------------------------*/
CallDialogue::~CallDialogue()
{
    DebugWindow::get()->logd("Closing Call dialogue.\n");
    delete ui;
}

/*------------------------------------------------------------------------------
-- FUNCTION: void CallDialogue::callConnect()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Trista Huang
--
-- PROGRAMMER: Trista Huang
--
-- INTERFACE: void CallDialogue::callConnect()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Sets sockets to connect between clients.
--          Also initiates audio devices to start receiving and sending microphone input.
--
------------------------------------------------------------------------------*/
void CallDialogue::callConnect() {
    callSock = createSocket(SOCK_DGRAM);
    sockAdd = createAddress(ip, htons(7003));
    bindSocket(callSock, createAddress(INADDR_ANY, htons(7003)));

    //Audio output device
    m_output= m_audioOutput->start();

    //Audio input device
    m_input = m_audioInput->start();
    //connect readyRead signal to readMre slot.
    //Call readmore when audio samples fill in inputbuffer
    connect(m_input, SIGNAL(readyRead()), SLOT(readMore()));
    receiving = true;
    std::thread callReceiveThread(&CallDialogue::receive, this);
    callReceiveThread.detach();
}

/*------------------------------------------------------------------------------
-- FUNCTION: void CallDialogue::initializeAudio()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Trista Huang
--
-- PROGRAMMER: Trista Huang
--
-- INTERFACE: void CallDialogue::initializeAudio()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Initializes audio devices and set format for the devices.
--
------------------------------------------------------------------------------*/
void CallDialogue::initializeAudio()
{
    m_format.setSampleRate(22050); //set frequency to 8000
    m_format.setChannelCount(1); //set channels to mono
    m_format.setSampleSize(16); //set sample sze to 16 bit
    m_format.setSampleType(QAudioFormat::SignedInt ); //Sample type as usigned integer sample
    m_format.setByteOrder(QAudioFormat::LittleEndian); //Byte order
    m_format.setCodec("audio/pcm"); //set codec as simple audio/pcm

    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
    QAudioDeviceInfo infoOut(QAudioDeviceInfo::defaultOutputDevice());

    createAudioInput();
    createAudioOutput();
}

/*------------------------------------------------------------------------------
-- FUNCTION: void CallDialogue::createAudioOutput()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Trista Huang
--
-- PROGRAMMER: Trista Huang
--
-- INTERFACE: void CallDialogue::createAudioOutput()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Create AudioOutput.
--
------------------------------------------------------------------------------*/
void CallDialogue::createAudioOutput()
{
    m_audioOutput = new QAudioOutput(m_Outputdevice, m_format, this);
}

/*------------------------------------------------------------------------------
-- FUNCTION: void CallDialogue::createAudioInput()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Trista Huang
--
-- PROGRAMMER: Trista Huang
--
-- INTERFACE: void CallDialogue::createAudioInput()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Create AudioInput object.
--
------------------------------------------------------------------------------*/
void CallDialogue::createAudioInput()
{
    if (m_input != 0) {
        disconnect(m_input, 0, this, 0);
        m_input = 0;
    }

    m_audioInput = new QAudioInput(m_Inputdevice, m_format, this);

}

/*------------------------------------------------------------------------------
-- FUNCTION: void CallDialogue::readMore()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Trista Huang
--
-- PROGRAMMER: Trista Huang
--
-- INTERFACE: void CallDialogue::readMore()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Function to get input from microphone and send it over UDP socket.
--
------------------------------------------------------------------------------*/
void CallDialogue::readMore()
{
    int nBufSize = 4096;

    if(!m_audioInput)
        return;

    //Check the number of samples in input buffer
    qint64 len = m_audioInput->bytesReady();
    //Limit sample size
    if(len > 4096) len = 4096;

    qint64 l = m_input->read(m_buffer.data(), len);
    if(l > 0)
    {
        short* resultingData = (short*)m_buffer.data();
        short *outdata=resultingData;
        outdata[ 0 ] = resultingData [ 0 ];

        int iIndex;
        for ( iIndex=0; iIndex < len; iIndex++ )
        {
            //Cange volume to each integer data in a sample
            outdata[ iIndex ] = ApplyVolumeToSample( outdata[ iIndex ]);
        }

        if (sendto(callSock, (char*)outdata, len, 0, (struct sockaddr *)&sockAdd, sizeof( sockAdd )) <= 0){
            DebugWindow::get()->logd("Call: sendto failed.");
        }
        DebugWindow::get()->logd("Data Sent.\n");
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: void CallDialogue::receive()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Trista Huang
--
-- PROGRAMMER: Trista Huang
--
-- INTERFACE: void CallDialogue::receive()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Receive from UDP socket. Starts on a new thread and run at start of call.
--          Outputs to the speaker as soon as its received.
--
------------------------------------------------------------------------------*/
void CallDialogue::receive(){
    int recvRet;
    char buf[4096];
    int sin_len;

    sin_len = sizeof(sockAdd);
    while (receiving) {
        if ((recvRet = recvfrom (callSock, buf, 4096, 0, (struct sockaddr *)&sockAdd, &sin_len)) < 0){
            DebugWindow::get()->logd("Call: receive failed.");
        }

        m_output->write((char*)buf, 4096);
        DebugWindow::get()->logd("Call: receive END");
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: int CallDialogue::ApplyVolumeToSample(short sample)
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Trista Huang
--
-- PROGRAMMER: Trista Huang
--
-- INTERFACE: int CallDialogue::ApplyVolumeToSample(short sample)
--
-- PARAMETERS: sample: section of the sample input from microphone to be volume adjusted
--
-- RETURNS: int: returns the value with volume adjusted
--
-- NOTES: Adjusts volume of the sample input and returns it
--
------------------------------------------------------------------------------*/
int CallDialogue::ApplyVolumeToSample(short iSample)
{
    //Calculate volume, Volume limited to  max 35535 and min -35535
    return max(min(((iSample * m_iVolume) / 50) ,35535), -35535);

}

/*------------------------------------------------------------------------------
-- FUNCTION: void CallDialogue::on_horizontalSlider_valueChanged(int value)
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Trista Huang
--
-- PROGRAMMER: Trista Huang
--
-- INTERFACE: void CallDialogue::on_horizontalSlider_valueChanged(int value)
--
-- PARAMETERS: value: get the value of the slider to adjust the volume with
--
-- RETURNS: N/A
--
-- NOTES: Gets the value of the volume from UI and set the variable
--
------------------------------------------------------------------------------*/
void CallDialogue::on_horizontalSlider_valueChanged(int value)
{
    m_iVolume = value;
}

/*------------------------------------------------------------------------------
-- FUNCTION: CallDialogue *CallDialogue::get()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: CallDialogue *CallDialogue::get()
--
-- PARAMETERS: N/A
--
-- RETURNS: CallDialogue
--
-- NOTES: Get the instance of CallDialogue
--
------------------------------------------------------------------------------*/
CallDialogue *CallDialogue::get()
{
    if(instance == nullptr) {
        instance = new CallDialogue();
    }
    return instance;
}

/*------------------------------------------------------------------------------
-- FUNCTION: void CallDialogue::init()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void CallDialogue::init()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Initiaites the CallDialogue
--
------------------------------------------------------------------------------*/
void CallDialogue::init() {
    ui->L_Contact->setText(QString::fromStdString(contact));
    initializeAudio();
    callConnect();
}

/*------------------------------------------------------------------------------
-- FUNCTION: closeEvent
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void closeEvent(QCloseEvent *event)
--
-- PARAMETERS: QCloseEvent *event   -   close event
--
-- RETURNS: N/A
--
-- NOTES:
--  Event handler the triggers a dialogue popup when user tries to exit the
--  the program.
------------------------------------------------------------------------------*/
void CallDialogue::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit?",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel
                                                                | QMessageBox::No
                                                                | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        // Close Socket for bother user and contact
        // Complete thread function, set boolean to 0 or something
        event->accept();
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: reject
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void reject();
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  Close event subhandler, when user responds no or cancel to closing the program.
------------------------------------------------------------------------------*/
void CallDialogue::reject()
{
    QMessageBox::StandardButton resBtn = QMessageBox::Yes;
    resBtn = QMessageBox::question( this, "Exit?",
                                    tr("Are you sure?\n"),
                                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                    QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        QDialog::reject();
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: void CallDialogue::on_B_HangUp_clicked()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Trista Huang
--
-- PROGRAMMER: Trista Huang
--
-- INTERFACE: void CallDialogue::on_B_HangUp_clicked()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  Button press function to end the call session. Closes the audio device.
------------------------------------------------------------------------------*/
void CallDialogue::on_B_HangUp_clicked()
{
    DebugWindow::get()->logd("Closing Call dialogue.\n");
    receiving = false;
    m_output->close();
    if(m_input != NULL) m_input->close();
    ui->L_Contact->setText("Disconnected");
    closesocket(callSock);
}
