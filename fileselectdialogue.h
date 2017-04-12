#ifndef FILESELECTDIALOGUE_H
#define FILESELECTDIALOGUE_H

#include <QDialog>

#include "song.h"

namespace Ui {
class FileSelectDialogue;
}

class FileSelectDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit FileSelectDialogue(QWidget *parent = 0);
    ~FileSelectDialogue();

    static FileSelectDialogue *get();

    QString getFilePath() { return filePath; }
    QString getTitle() { return title; }
    QString getArtist() { return artist; }
    QString getAlbum() { return album; }

private slots:
    void on_B_SelectFile_clicked();

    void on_pushButton_clicked();

private:
    Ui::FileSelectDialogue *ui;

    static FileSelectDialogue *instance;
    QString filePath;
    QString title;
    QString artist;
    QString album;
};

#endif // FILESELECTDIALOGUE_H
