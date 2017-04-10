#include "song.h"
#include "string.h"

Song::Song(const char *songInfoPkt) {

}

int Song::getInfoPktized(char *buf, bool withArtwork) {
    memset(buf, 0, sizeof(buf));

    PktIds pktId = PktIds::SONG_INFO;

    int titleLen = title.size() + 1;
    std::string strTitle;
    int artistLen = artist.size() + 1;
    std::string strArtist;
    int albumLen = album.size() + 1;
    std::string strAlbum;
    const char *cStrTitle = strTitle.c_str();
    const char *cStrArtist = strArtist.c_str();
    const char *cStrAlbum = strAlbum.c_str();


    int offset = 0;
    memcpy(buf + offset, &pktId, sizeof(PktIds::SONG_INFO));
    offset += sizeof(PktIds::SONG_INFO);

    memcpy(buf + offset, &id, sizeof(id));
    offset += id;

    memcpy(buf + offset, &titleLen, sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, cStrTitle, titleLen);
    offset += titleLen;

    memcpy(buf + offset, &artistLen, sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, cStrArtist, albumLen);
    offset += artistLen;

    memcpy(buf + offset, &albumLen, sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, cStrAlbum, albumLen);
    offset += albumLen;

    if (withArtwork && hasArtwork) {
        //unimplemented
    } else {
        memset(buf + offset, 0, sizeof(int));
        offset += sizeof(int);
    }

    return offset;
}

std::vector<Audio> Song::genAudioPkts() {
    //unimplemented
    return std::vector<Audio>();
}
