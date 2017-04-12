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

void UpdateHandler::emitUSV()
{
    emit updateSongVector();
}

void UpdateHandler::emitUQV()
{
    emit updateQueueVector();
}

void UpdateHandler::emitUUV()
{
    emit updateUserVector();
}
