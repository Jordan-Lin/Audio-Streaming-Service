/*------------------------------------------------------------------------------
-- SOURCE FILE: audiomanager.cpp
--
-- PROGRAM: CommAudio
--
-- FUNCTIONS:
--  static audioManager& get() {return instance;}
--  QByteArray loadHeader(QString fileName);
--  QByteArray loadAudio(QString fileName);
--  void playSong() {startSong = true;}
--  void initAudio(short bits, qint32 sample, short channels);
--  void appender(QByteArray);
--  HeaderInfo audioManager::parseHeader(QByteArray headerInfo);
--  int audioManager::durationCalc(QByteArray header);
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- NOTES: file that handles the playing and retrieving of audio files.
--
------------------------------------------------------------------------------*/
#include "audiomanager.h"

QByteArray audio_data;

audioManager audioManager::instance;


/*------------------------------------------------------------------------------
-- FUNCTION: audioManager
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: audioManager::audioManager()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: constructor for the audioManager class
--
------------------------------------------------------------------------------*/
audioManager::audioManager()
{

}

/*------------------------------------------------------------------------------
-- FUNCTION: loadHeader
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: QByteArray audioManager::loadHeader(QString fileName)
--
-- PARAMETERS: QString fileName - path to the file containing the wav file
--
-- RETURNS: QByteArray - header information read from the wav file
--
-- NOTES: This function opens a wav file, reads the header information
--        and returns it.
--
------------------------------------------------------------------------------*/
QByteArray audioManager::loadHeader(QString fileName) {
    QFile audio_file(fileName);
    audio_file.open(QIODevice::ReadOnly);
    QByteArray audio_data = audio_file.read(43);
    audio_file.close();
    return audio_data;
}

/*------------------------------------------------------------------------------
-- FUNCTION: loadAudio
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: QByteArray audioManager::loadAudio(QString fileName)
--
-- PARAMETERS: QString fileName - path to the file containing the wav file
--
-- RETURNS: QByteArray - audio data for the wav file passed in
--
-- NOTES: This function reads in the audio portion of a wav file and
--        returns it.
--
------------------------------------------------------------------------------*/
QByteArray audioManager::loadAudio(QString fileName) {
    QFile audio_file(fileName);
    audio_file.open(QIODevice::ReadOnly);
    audio_file.seek(44);
    QByteArray song = audio_file.readAll();
    audio_file.close();
    return song;
}


/*------------------------------------------------------------------------------
-- FUNCTION: initAudio
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: void audioManager::initAudio(short bits, qint32 sample, short channels)
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: This function initalizes the playing of audio in an event loop.
--
------------------------------------------------------------------------------*/

QByteArray audioManager::loadSong(QString fileName) {
    QFile audio_file(fileName);
    audio_file.open(QIODevice::ReadOnly);
    QByteArray song = audio_file.readAll();
    audio_file.close();
    return song;
}

void audioManager::initAudio(short bits, qint32 sample, short channels) {
    audio_data.clear();
    QBuffer audio_buffer(&audio_data);
    audio_buffer.open(QIODevice::ReadOnly);
    QAudioFormat format;
    format.setCodec("audio/pcm");
    format.setSampleSize(bits);
    format.setSampleRate(sample / 1000 * 5);
    format.setChannelCount(channels * 2);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    QAudioOutput output(info, format);
    QEventLoop loop;
    while(startSong == false);
    output.start(&audio_buffer);
    QObject::connect(&output, SIGNAL(stateChanged(QAudio::State)), &loop, SLOT(quit()));
    do {
        loop.exec();
    } while(output.state() == QAudio::ActiveState);

    startSong = false;
}

/*------------------------------------------------------------------------------
-- FUNCTION: appender
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: void audioManager::appender(QByteArray data)
--
-- PARAMETERS: QByteArray data - song data to be appended to the QByteArray
--             that is currently playing
--
-- RETURNS: N/A
--
-- NOTES: This function appends a chunk of data passed into the parameter, to the
--        audio data that is already playing to create a stream of audio.
--
------------------------------------------------------------------------------*/
void audioManager::appender(QByteArray data) {
    audio_data.append(data);
}

/*------------------------------------------------------------------------------
-- FUNCTION: parseHeader
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: HeaderInfo audioManager::parseHeader(QByteArray header)
--
-- PARAMETERS: QByteArray header - the data that is contained within the header
--             of the wav file.
--
-- RETURNS: HeaderInfo - returns a reference to a struct containing the header info
--
-- NOTES: This function reads the header information passed into it, stores the
--        information in a structure and returns that structure
--
------------------------------------------------------------------------------*/
HeaderInfo audioManager::parseHeader(QByteArray header) {
    QDataStream analyzeHeaderDS(&header,QIODevice::ReadOnly);
    HeaderInfo headerInfo;
    char fileType[4];
    qint32 fileSize;
    char waveName[4];
    char fmtName[3];
    qint32 fmtLength;
    short fmtType;
    short numberOfChannels;
    qint32 sampleRate;
    qint32 sampleMultiply;
    short bitsMultiply;
    short bitsPerSample;
    char dataHeader[4];
    qint32 dataSize;
    analyzeHeaderDS.readRawData(fileType,4); // "RIFF"
    analyzeHeaderDS >> fileSize; // File Size
    analyzeHeaderDS.readRawData(waveName,4); // "WAVE"
    analyzeHeaderDS.readRawData(fmtName,3); // "fmt"
    analyzeHeaderDS >> fmtLength; // Format length
    analyzeHeaderDS >> fmtType; // Format type
    analyzeHeaderDS >> numberOfChannels; // Number of channels
    analyzeHeaderDS >> sampleRate; // Sample rate
    analyzeHeaderDS >> sampleMultiply; // (Sample Rate * BitsPerSample * Channels) / 8
    analyzeHeaderDS >> bitsMultiply; // (BitsPerSample * Channels) / 8.1
    analyzeHeaderDS >> bitsPerSample; // Bits per sample
    analyzeHeaderDS.readRawData(dataHeader,4); // "data" header
    analyzeHeaderDS >> dataSize; // Data Size
    strcpy(headerInfo.fileType, fileType);
    headerInfo.fileSize = fileSize;
    strcpy(headerInfo.waveName, waveName);
    strcpy(headerInfo.fmtName, fmtName);
    headerInfo.fmtLength = fmtLength;
    headerInfo.fmtType = fmtType;
    headerInfo.numberOfChannels = numberOfChannels;
    headerInfo.sampleRate = sampleRate;
    headerInfo.sampleMulti = sampleMultiply;
    headerInfo.bitsMulti = bitsMultiply;
    headerInfo.bitsPerSample = bitsPerSample;
    strcpy(headerInfo.dataHeader, dataHeader);
    headerInfo.dataSize = dataSize;
    return headerInfo;
}

/*------------------------------------------------------------------------------
-- FUNCTION: durationCalc
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: int audioManager::durationCalc(QByteArray header)
--
-- PARAMETERS: QByteArray header - the data that is contained within
--             the header of the wav file
--
-- RETURNS: int - the duration of the audio clip
--
-- NOTES: This function reads the header information of the wav file
--        and calculates the length of the audio clip based on that information.
--
------------------------------------------------------------------------------*/
int audioManager::durationCalc(QByteArray header) {
    char fileType[4];
    qint32 fileSize;
    char waveName[4];
    char fmtName[3];
    qint32 fmtLength;
    short fmtType;
    short numberOfChannels;
    qint32 sampleRate;
    qint32 sampleMultiply;
    short bitsMultiply;
    short bitsPerSample;
    char dataHeader[4];
    qint32 dataSize;
    QDataStream analyzeHeader(&header,QIODevice::ReadOnly);
    analyzeHeader.readRawData(fileType,4); // "RIFF"
    analyzeHeader >> fileSize; // File Size
    analyzeHeader.readRawData(waveName,4); // "WAVE"
    analyzeHeader.readRawData(fmtName,3); // "fmt"
    analyzeHeader >> fmtLength; // Format length
    analyzeHeader >> fmtType; // Format type
    analyzeHeader >> numberOfChannels; // Number of channels
    analyzeHeader >> sampleRate; // Sample rate
    analyzeHeader >> sampleMultiply; // (Sample Rate * BitsPerSample * Channels) / 8
    analyzeHeader >> bitsMultiply; // (BitsPerSample * Channels) / 8.1
    analyzeHeader >> bitsPerSample; // Bits per sample
    analyzeHeader.readRawData(dataHeader,4); // "data" header
    analyzeHeader >> dataSize; // Data Size
    int time = (dataSize / 8) / ((sampleRate * bitsPerSample * numberOfChannels) / 8);
    return time;
}
