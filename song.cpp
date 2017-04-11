#include "song.h"
#include "string.h"
#include "debugwindow.h"
#include "utilities.h"
#include "debugwindow.h"

Song::Song(const char *songInfoPkt) {

}

int Song::getInfoPktLen(bool artwork) {
    if (artwork == false) {
        int offset = sizeof(PktIds::SONG_INFO) + title.size() + 1 + sizeof(int) +
                artist.size() + 1 + sizeof(int) + album.size() + 1 + sizeof(int) +
                sizeof(int) + sizeof(int);
        DebugWindow::get()->logd(QString("SONG LEN: ") + itoq(offset));
        return offset;
    } else {
        return -1;
    }
}

void Song::getInfoPktized(char *buf, bool withArtwork) {
    memset(buf, 0, sizeof(buf));

    PktIds pktId = PktIds::SONG_INFO;

    int titleLen = title.size() + 1;
    std::string strTitle = title.toStdString();
    int artistLen = artist.size() + 1;
    std::string strArtist = artist.toStdString();
    int albumLen = album.size() + 1;
    std::string strAlbum = album.toStdString();
    const char *cStrTitle = strTitle.c_str();
    const char *cStrArtist = strArtist.c_str();
    const char *cStrAlbum = strAlbum.c_str();


    int offset = 0;
    memcpy(buf, &pktId, sizeof(PktIds::SONG_INFO));
    offset += sizeof(PktIds::SONG_INFO);

    memcpy(buf + offset, &id, sizeof(id));
    offset += sizeof(id);

    DebugWindow::get()->logpo(QString("<SongInfo> id: ") + itoq(id));

    memcpy(buf + offset, &titleLen, sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, cStrTitle, titleLen);
    DebugWindow::get()->logpo(QString(", title: ") + QString(buf + offset));

    offset += titleLen;

    memcpy(buf + offset, &artistLen, sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, cStrArtist, artistLen);
    DebugWindow::get()->logpo(QString(", artist: ") + QString(buf + offset));
    offset += artistLen;

    memcpy(buf + offset, &albumLen, sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, cStrAlbum, albumLen);
    DebugWindow::get()->logpo(QString(", album: ") + QString(buf + offset)
        + QString("\n"));
    offset += albumLen;



    if (withArtwork && hasArtwork) {
        //unimplemented
    } else {
        int zero = 0;
        memcpy(buf + offset, &zero, sizeof(int));
        offset += sizeof(int);
    }
    DebugWindow::get()->logd(QString("SONG OFFSET: ") + itoq(offset));
}

std::vector<Audio> Song::genAudioPkts() {
    //unimplemented
    return std::vector<Audio>();
}
