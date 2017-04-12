/*------------------------------------------------------------------------------
-- SOURCE FILE: updatehandler.cpp
--
-- PROGRAM: Comm Audio
--
-- FUNCTIONS:   explicit UpdateHandler(QObject *parent = 0);
--              static UpdateHandler *get();
--              void emitUSV();
--              void emitUQV();
--              void emitUUV();
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--           Jordan Lin
--
-- PROGRAMMER: Brody McCrone
--             Jordan Lin
--
-- NOTES:
--  Update signal emitter singleton. Handles emitting signals from anywhere.
------------------------------------------------------------------------------*/
#include "updatehandler.h"

UpdateHandler *UpdateHandler::instance = nullptr;

/*------------------------------------------------------------------------------
-- FUNCTION: UpdateHandler
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: UpdateHandler(QObject *parent)
--
-- PARAMETERS: QObject Parent - parent window
--
-- RETURNS: N/A
--
-- NOTES:
--  UpdateHandler construtor
------------------------------------------------------------------------------*/
UpdateHandler::UpdateHandler(QObject *parent) : QObject(parent)
{

}

/*------------------------------------------------------------------------------
-- FUNCTION: get
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: UpdateHandler *get();
--
-- PARAMETERS: N/A
--
-- RETURNS: UpdateHandler *
--
-- NOTES:
--  Get the instance of the update handler to run a function. If it doesn't
--  exist, the call the constructor.
------------------------------------------------------------------------------*/
UpdateHandler *UpdateHandler::get()
{
    if(instance == nullptr) {
        instance = new UpdateHandler();
    }
    return instance;
}

/*------------------------------------------------------------------------------
-- FUNCTION: emitUSV
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void emitUSV()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  Emit Update Song Vector signal
------------------------------------------------------------------------------*/
void UpdateHandler::emitUSV()
{
    emit updateSongVector();
}

/*------------------------------------------------------------------------------
-- FUNCTION: emitUQV
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void emitUQV()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  Emit Update Queue Vector signal
------------------------------------------------------------------------------*/
void UpdateHandler::emitUQV()
{
    emit updateQueueVector();
}

/*------------------------------------------------------------------------------
-- FUNCTION: emitUUV
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void emitUUV()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  Emit Update User Vector signal
------------------------------------------------------------------------------*/
void UpdateHandler::emitUUV()
{
    emit updateUserVector();
}

/*------------------------------------------------------------------------------
-- FUNCTION: emitUUV
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void emitUUV()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  Emit Update User Vector signal
------------------------------------------------------------------------------*/
void UpdateHandler::emitUpload()
{
    emit uploadSignal();
}
