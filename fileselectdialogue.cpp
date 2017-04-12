/*------------------------------------------------------------------------------
-- SOURCE FILE: fileselectdialgue.cpp
--
-- PROGRAM: Comm Audio
--
-- FUNCTIONS:       explicit FileSelectDialogue(QWidget *parent = 0);
--                  ~FileSelectDialogue();
--                  static FileSelectDialogue *get();
--                  void on_B_SelectFile_clicked();
--
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- NOTES:
--  File select dialogue for inputting upload information for a song.
------------------------------------------------------------------------------*/
#include "fileselectdialogue.h"
#include "ui_fileselectdialogue.h"
#include "utilities.h"

#include <QDebug>
#include <QFileDialog>

FileSelectDialogue *FileSelectDialogue::instance = nullptr;

/*------------------------------------------------------------------------------
-- FUNCTION: FileSelectDialogue
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: QWidget *parent - parent window
--
-- RETURNS: N/A
--
-- NOTES:
--  FileSelectDialogue constructor.
------------------------------------------------------------------------------*/
FileSelectDialogue::FileSelectDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileSelectDialogue)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
}

/*------------------------------------------------------------------------------
-- FUNCTION: ~FileSelectDialogue
--
-- DATE:    April 10th, 2017
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
--  FileSelectDialogue destructor.
------------------------------------------------------------------------------*/
FileSelectDialogue::~FileSelectDialogue()
{
    delete ui;
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
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  getter for the instance of a FileSelectDialogue.
------------------------------------------------------------------------------*/
FileSelectDialogue *FileSelectDialogue::get()
{
    if(instance == nullptr) {
        instance = new FileSelectDialogue();
    }
    return instance;
}

/*------------------------------------------------------------------------------
-- FUNCTION: on_B_SelectFile_clicked
--
-- DATE:    April 10th, 2017
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
--  slot to handle receiving on button select file signals, opens a file select
--  window..
------------------------------------------------------------------------------*/
void FileSelectDialogue::on_B_SelectFile_clicked()
{
    // Open file select dialogue get path, open file
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open wav File"), "/home", tr("Wav Files (*.wav)"));

    if(fileName.isEmpty()) {
        showWarningMessage(this, "No File Selected", "Please selecte a file");
        return;
    }

    ui->L_FilePath->setText(fileName);
    filePath = fileName.toStdString();
    // Change File Path name to selected file path.
    // write to buffer and send to server for upload.
}

/*------------------------------------------------------------------------------
-- FUNCTION: on_pushButton_clicked
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void on_pushButton_clicked();
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  gets the user input song iformation and path for opening the file to send
--  to the server.
------------------------------------------------------------------------------*/
void FileSelectDialogue::on_pushButton_clicked()
{
    if(ui->LE_SongTitle->text().isEmpty() || ui->L_FilePath->text().isEmpty()) {
        return;
    }
    filePath = ui->L_FilePath->text();
    title = ui->LE_SongTitle->text();
    if(ui->LE_Album->text().isEmpty()) {
        album = QString("Unknown");
    } else {
        album = ui->LE_Album->text();
    }
    if(ui->LE_Artist->text().isEmpty()) {
        artist = QString("Unknown");
    } else {
        artist = ui->LE_Artist->text();
    }
    delete ui;
}
