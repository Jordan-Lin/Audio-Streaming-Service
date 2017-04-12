#include "calldialogue.h"
#include "ui_calldialogue.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <thread>
#include "debugwindow.h"

using namespace std;

const int BufferSize = 14096;

CallDialogue *CallDialogue::instance = nullptr;

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
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
    initializeAudio();
    CallConnect();
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
CallDialogue::~CallDialogue()
{
    delete ui;
}

void CallDialogue::CallConnect() {
    callSock = createSocket(SOCK_DGRAM);
    const char *cStrServerIp = "192.168.0.12";
    sockAdd = createAddress(inet_addr(cStrServerIp), htons(2323));
    bindSocket(callSock, sockAdd);

    //Audio output device
    m_output= m_audioOutput->start();
    //Audio input device
    m_input = m_audioInput->start();
    //connect readyRead signal to readMre slot.
    //Call readmore when audio samples fill in inputbuffer
    connect(m_input, SIGNAL(readyRead()), SLOT(readMore()));
//    connect(m_input, SIGNAL(readyRead()), SLOT(receive()));

//    std::thread callReceiveThread(&CallDialogue::receive, this);
//    callReceiveThread.detach();
}

//Initialize audio
void CallDialogue::initializeAudio()
{
    m_format.setSampleRate(8000); //set frequency to 8000
    m_format.setChannelCount(1); //set channels to mono
    m_format.setSampleSize(16); //set sample sze to 16 bit
    m_format.setSampleType(QAudioFormat::UnSignedInt ); //Sample type as usigned integer sample
    m_format.setByteOrder(QAudioFormat::LittleEndian); //Byte order
    m_format.setCodec("audio/pcm"); //set codec as simple audio/pcm

    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
    if (!infoIn.isFormatSupported(m_format))
    {
        //Default format not supported - trying to use nearest
        m_format = infoIn.nearestFormat(m_format);
    }

    QAudioDeviceInfo infoOut(QAudioDeviceInfo::defaultOutputDevice());

    if (!infoOut.isFormatSupported(m_format))
    {
       //Default format not supported - trying to use nearest
        m_format = infoOut.nearestFormat(m_format);
    }
    createAudioInput();
    createAudioOutput();
}

void CallDialogue::createAudioOutput()
{
    m_audioOutput = new QAudioOutput(m_Outputdevice, m_format, this);
}

void CallDialogue::createAudioInput()
{
    if (m_input != 0) {
        disconnect(m_input, 0, this, 0);
        m_input = 0;
    }

    m_audioInput = new QAudioInput(m_Inputdevice, m_format, this);

}

void CallDialogue::readMore()
{
    int i, nBufSize, err;
    char *buf, *buf_ptr;
    struct sockaddr_in sin ;

    nBufSize = 4096;
    buf = (char*)malloc(nBufSize) ;

    //Return if audio input is null
    if(!m_audioInput)
        return;

    //Check the number of samples in input buffer
    qint64 len = m_audioInput->bytesReady();

    //Limit sample size
    if(len > 4096)
        len = 4096;
    //Read sound samples from input device to buffer
    qint64 l = m_input->read(m_buffer.data(), len);
    if(l > 0)
    {
        //Assign sound samples to short array
        short* resultingData = (short*)m_buffer.data();


        short *outdata=resultingData;
        outdata[ 0 ] = resultingData [ 0 ];

         int iIndex;
//         if(ui->chkRemoveNoise->checkState() == Qt::Checked)
//         {
//                //Remove noise using Low Pass filter algortm[Simple algorithm used to remove noise]
//                for ( iIndex=1; iIndex < len; iIndex++ )
//                {
//                    outdata[ iIndex ] = 0.333 * resultingData[iIndex ] + ( 1.0 - 0.333 ) * outdata[ iIndex-1 ];
//                }
//          }

         for ( iIndex=0; iIndex < len; iIndex++ )
         {
             //Cange volume to each integer data in a sample
             outdata[ iIndex ] = ApplyVolumeToSample( outdata[ iIndex ]);
         }

         //write modified sond sample to outputdevice for playback audio
//          m_output->write((char*)outdata, len);
         if (sendto(callSock, (char*)outdata, len, 0, (struct sockaddr *)&sockAdd, sizeof( sockAdd )) <= 0){
             DebugWindow::get()->logd("Call: sendto failed.");
         }
//         buf[0] = 't';
//         if (sendto(callSock, (char*)buf, len, 0, (struct sockaddr *)&sockAdd, sizeof( sockAdd )) <= 0){
//             DebugWindow::get()->logd("Call: sendto failed2.");
//         }

    }
receive();
}

void CallDialogue::receive(){
    int recvRet;
    char buf[4096];
    struct sockaddr_in sin;
    int sin_len;

    sin_len = sizeof(sockAdd);

    if ((recvRet = recvfrom (callSock, buf, 4096, 0, (struct sockaddr *)&sockAdd, &sin_len)) < 0){
        DebugWindow::get()->logd("Call: receive failed.");
    }

    m_output->write((char*)buf, 4096);
    DebugWindow::get()->logd("Call: receive END");
}

int CallDialogue::ApplyVolumeToSample(short iSample)
{
    //Calculate volume, Volume limited to  max 35535 and min -35535
    return max(min(((iSample * m_iVolume) / 50) ,35535), -35535);

}


//void CallDialogue::on_pushButton_clicked()
//{
//    //Audio output device
//    m_output= m_audioOutput->start();
//     //Audio input device
//    m_input = m_audioInput->start();
//    //connect readyRead signal to readMre slot.
//    //Call readmore when audio samples fill in inputbuffer
//    connect(m_input, SIGNAL(readyRead()), SLOT(readMore()));

//}




void CallDialogue::on_horizontalSlider_valueChanged(int value)
{
    m_iVolume = value;
}



/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
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
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void CallDialogue::init() {
    ui->L_Contact->setText(QString::fromStdString(contact));
    // WSA Start, create socket information, try connecting to user's socket
    // Change status based on where the connection is being made, is completed etc.
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
