#include "audiomanager.h"

QByteArray audio_data;
QByteArray moreData;

audioManager audioManager::instance;

audioManager::audioManager()
{

}

//Don't know how we want to extract and send the header
QByteArray audioManager::loadHeader(QString fileName) {


    QFile audio_file(fileName);
    audio_file.open(QIODevice::ReadOnly);
    QByteArray audio_data = audio_file.read(43);

    audio_file.close();

    return audio_data;
}

QByteArray audioManager::loadAudio(QString fileName) {
    QFile audio_file(fileName);
    audio_file.open(QIODevice::ReadOnly);
    audio_file.seek(44);
    QByteArray song = audio_file.readAll();
    audio_file.close();
    return song;
}

void audioManager::playAudio(short bits, qint32 sample, short channels, QByteArray audio) {

    audio_data = audio;
    moreData = "";

    QBuffer audio_buffer(&audio);
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
    int i = 0;
    //while data(check boolean in another file?)
    while(i < 1) {
        output.start(&audio_buffer);
        //line2 = data from buffer sent

        QEventLoop loop;
        QObject::connect(&output, SIGNAL(stateChanged(QAudio::State)), &loop, SLOT(quit()));
        do {
            std::thread first([this] { appender(moreData); });
            first.detach();
            loop.exec();

        } while(output.state() == QAudio::ActiveState);
        i++;

    }

}

void audioManager::appender(QByteArray line2) {
    audio_data.append(line2);
}

void audioManager::parseHeader(QByteArray header) {
    QDataStream analyzeHeaderDS(&header,QIODevice::ReadOnly);

    HeaderInfo headerInfo;

    analyzeHeaderDS.readRawData(headerInfo.fileType,4); // "RIFF"

    analyzeHeaderDS >> headerInfo.fileSize; // File Size

    analyzeHeaderDS.readRawData(headerInfo.waveName,4); // "WAVE"

    analyzeHeaderDS.readRawData(headerInfo.fmtName,3); // "fmt"

    analyzeHeaderDS >> headerInfo.fmtLength; // Format length

    analyzeHeaderDS >> headerInfo.fmtType; // Format type

    analyzeHeaderDS >> headerInfo.numberOfChannels; // Number of channels

    analyzeHeaderDS >> headerInfo.sampleRate; // Sample rate

    analyzeHeaderDS >> headerInfo.sampleMulti; // (Sample Rate * BitsPerSample * Channels) / 8

    analyzeHeaderDS >> headerInfo.bitsMulti; // (BitsPerSample * Channels) / 8.1

    analyzeHeaderDS >> headerInfo.bitsPerSample; // Bits per sample

    analyzeHeaderDS.readRawData(headerInfo.dataHeader,4); // "data" header

    analyzeHeaderDS >> headerInfo.dataSize; // Data Size
}
