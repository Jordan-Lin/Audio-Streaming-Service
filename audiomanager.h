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


class audioManager
{
public:
    audioManager();

    audioManager get() {return instance;}

    QByteArray loadHeader(QString fileName);
    QByteArray loadAudio(QString fileName);
    void playAudio(short bits, qint32 sample, short channels, QByteArray audio);
    void appender(QByteArray);
private:
    static audioManager instance;
};

#endif // AUDIOMANAGER_H
