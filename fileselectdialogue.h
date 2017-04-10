#ifndef FILESELECTDIALOGUE_H
#define FILESELECTDIALOGUE_H

#include <QDialog>

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

    std::string getFilePath() { return filePath; }
    std::string getTitle() { return title; }
    std::string getArtist() { return artist; }
    std::string getAlbum() { return album; }

private slots:
    void on_B_SelectFile_clicked();

private:
    Ui::FileSelectDialogue *ui;

    static FileSelectDialogue *instance;
    std::string filePath;
    std::string title;
    std::string artist;
    std::string album;
};

#endif // FILESELECTDIALOGUE_H
