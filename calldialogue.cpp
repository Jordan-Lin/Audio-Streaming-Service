#include "calldialogue.h"
#include "ui_calldialogue.h"
#include <QMessageBox>
#include <QCloseEvent>


CallDialogue *CallDialogue::instance = nullptr;

CallDialogue::CallDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CallDialogue)
{
    ui->setupUi(this);
}

CallDialogue::~CallDialogue()
{
    delete ui;
}

CallDialogue *CallDialogue::get()
{
    if(instance == nullptr) {
        instance = new CallDialogue();
    }
    return instance;
}

void CallDialogue::init() {
    ui->L_Contact->setText(QString::fromStdString(contact));
    // WSA Start, create socket information, try connecting to user's socket
    // Change status based on where the connection is being made, is completed etc.
}

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
