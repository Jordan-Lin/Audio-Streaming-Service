#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QBuffer>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QEventLoop>
#include <thread>
#include "packets.h"
#include <atomic>

class audioManager
{
public:
    audioManager();

    static audioManager& get() {return instance;}

    QByteArray loadHeader(QString fileName);
    QByteArray loadAudio(QString fileName);
    void playSong() {startSong = true;}
    void initAudio(short bits, qint32 sample, short channels);
    void appender(QByteArray);
    HeaderInfo audioManager::parseHeader(QByteArray headerInfo);
    int audioManager::durationCalc(QByteArray header);
private:
    static audioManager instance;
    std::atomic<bool> startSong{false};
};

#endif // AUDIOMANAGER_H
