#include "updatehandler.h"

UpdateHandler *UpdateHandler::instance = nullptr;

UpdateHandler::UpdateHandler(QObject *parent) : QObject(parent)
{

}

UpdateHandler *UpdateHandler::get()
{
    if(instance == nullptr) {
        instance = new UpdateHandler();
    }
    return instance;
}

void UpdateHandler::emitUSV(const std::vector<std::string> list)
{
    emit updateSongVector(list);
}

void UpdateHandler::emitUQV(const std::vector<std::string> list)
{
    emit updateQueueVector(list);
}

void UpdateHandler::emitUUV(const std::vector<std::string> list)
{
    emit updateUserVector(list);
}
