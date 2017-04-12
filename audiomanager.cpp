#include "audiomanager.h"

QByteArray audio_data;
QByteArray moreData;

audioManager audioManager::instance;

audioManager::audioManager()
{

}

//Don't know how we want to extract and send the header
QByteArray audioManager::loadHeader(QString fileName) {
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
    QByteArray data;


    QFile audio_file(fileName);
    audio_file.open(QIODevice::ReadOnly);
    QByteArray audio_data = audio_file.read(43);

    /*
    QDataStream headerInfo(&audio_data,QIODevice::ReadOnly);

    headerInfo.readRawData(fileType,4); // "RIFF"

    headerInfo >> fileSize; // File Size

    headerInfo.readRawData(waveName,4); // "WAVE"

    headerInfo.readRawData(fmtName,3); // "fmt"

    headerInfo >> fmtLength; // Format length

    headerInfo >> fmtType; // Format type

    headerInfo >> numberOfChannels; // Number of channels

    headerInfo >> sampleRate; // Sample rate

    headerInfo >> sampleMultiply; // (Sample Rate * BitsPerSample * Channels) / 8

    headerInfo >> bitsMultiply; // (BitsPerSample * Channels) / 8.1

    headerInfo >> bitsPerSample; // Bits per sample

    headerInfo.readRawData(dataHeader,4); // "data" header

    headerInfo >> dataSize; // Data Size

    qDebug() << "sampleRate: " << sampleRate;
    qDebug() << "bitsPerSampe: " << bitsPerSample;
    qDebug() << "multiplier: " << bitsMultiply;
    qDebug() << "numberOfChannels: " << numberOfChannels;*/



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
