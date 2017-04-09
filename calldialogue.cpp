#include "calldialogue.h"
#include "ui_calldialogue.h"
#include <QMessageBox>
#include <QCloseEvent>


CallDialogue *CallDialogue::instance = nullptr;

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
CallDialogue::CallDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CallDialogue)
{
    ui->setupUi(this);
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
CallDialogue::~CallDialogue()
{
    delete ui;
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
CallDialogue *CallDialogue::get()
{
    if(instance == nullptr) {
        instance = new CallDialogue();
    }
    return instance;
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void CallDialogue::init() {
    ui->L_Contact->setText(QString::fromStdString(contact));
    // WSA Start, create socket information, try connecting to user's socket
    // Change status based on where the connection is being made, is completed etc.
}

/*------------------------------------------------------------------------------
-- FUNCTION: closeEvent
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void closeEvent(QCloseEvent *event)
--
-- PARAMETERS: QCloseEvent *event   -   close event
--
-- RETURNS: N/A
--
-- NOTES:
--  Event handler the triggers a dialogue popup when user tries to exit the
--  the program.
------------------------------------------------------------------------------*/
void CallDialogue::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit?",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel
                                                                | QMessageBox::No
                                                                | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        // Close Socket for bother user and contact
        // Complete thread function, set boolean to 0 or something
        event->accept();
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: reject
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void reject();
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  Close event subhandler, when user responds no or cancel to closing the program.
------------------------------------------------------------------------------*/
void CallDialogue::reject()
{
    QMessageBox::StandardButton resBtn = QMessageBox::Yes;
    resBtn = QMessageBox::question( this, "Exit?",
                                    tr("Are you sure?\n"),
                                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                    QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        QDialog::reject();
    }
}
