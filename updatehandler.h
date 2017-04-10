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

    void emitUSV(const std::vector<std::string> list);
    void emitUQV(const std::vector<std::string> list);
    void emitUUV(const std::vector<std::string> list);

signals:
    void updateSongVector(std::vector<std::string> list);
    void updateQueueVector(std::vector<std::string> list);
    void updateUserVector(std::vector<std::string> list);

public:
    static UpdateHandler *instance;
};

#endif // UPDATEHANDLER_H
