#ifndef UPDATEHANDLER_H
#define UPDATEHANDLER_H

#include <QObject>

#include <string>
#include <vector>

class UpdateHandler : public QObject
{
    Q_OBJECT
public:
    explicit UpdateHandler(QObject *parent = 0);

    static UpdateHandler *get();

    void emitUSV();
    void emitUQV();
    void emitUUV();
    void emitUpload();

signals:
    void updateSongVector();
    void updateQueueVector();
    void updateUserVector();
    void uploadSignal();
    void signalSongTitle(QString song);

public:
    static UpdateHandler *instance;
};

#endif // UPDATEHANDLER_H
