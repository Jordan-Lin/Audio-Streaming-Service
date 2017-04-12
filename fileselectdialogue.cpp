#include "fileselectdialogue.h"
#include "ui_fileselectdialogue.h"
#include "utilities.h"

#include <QDebug>
#include <QFileDialog>

FileSelectDialogue *FileSelectDialogue::instance = nullptr;

FileSelectDialogue::FileSelectDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileSelectDialogue)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
}

FileSelectDialogue::~FileSelectDialogue()
{
    delete ui;
}

FileSelectDialogue *FileSelectDialogue::get()
{
    if(instance == nullptr) {
        instance = new FileSelectDialogue();
    }
    return instance;
}

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
