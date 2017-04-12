/*------------------------------------------------------------------------------
-- SOURCE FILE: song.cpp
--
-- PROGRAM: CommAudio
--
-- FUNCTIONS:
-- Song::Song(const char *songInfoPkt)
-- Song::~Song()
-- int Song::getInfoPktLen(bool artwork)
-- void Song::getInfoPktized(char *buf, bool withArtwork)
-- std::vector<Audio> Song::genAudioPkts()
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- NOTES: file that handles the playing and retrieving of audio files.
--
------------------------------------------------------------------------------*/
#include "song.h"
#include "string.h"
#include "debugwindow.h"
#include "utilities.h"
#include "debugwindow.h"

/*------------------------------------------------------------------------------
-- FUNCTION: Song
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: Song::Song(const char *songInfoPkt)
--
-- PARAMETERS: const char *songInfoPkt - packet containing the song information
--
-- RETURNS: N/A
--
-- NOTES: constructor for the Song class
--
------------------------------------------------------------------------------*/
Song::Song(const char *songInfoPkt) {

}

/*------------------------------------------------------------------------------
-- FUNCTION: ~Song
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: Song::~Song()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: destructor for the Song class
--
------------------------------------------------------------------------------*/
Song::~Song() {

}

/*------------------------------------------------------------------------------
-- FUNCTION: getInfoPktLen
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: int Song::getInfoPktLen(bool artwork)
--
-- PARAMETERS: bool artwork - boolean value indicating if artwork was added
--
-- RETURNS: int returns the size of the packet
--
-- NOTES: This function claculates the size of the packet and returns it
--
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
-- FUNCTION: getInfoPktized
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: void Song::getInfoPktized(char *buf, bool withArtwork)
--
-- PARAMETERS: char *buf - holds the information for the song info packet
--             bool withArtwork - indicates whether or not the packet has artwork
--
-- RETURNS: N/A
--
-- NOTES: This function packetizes the song information.
--
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
-- FUNCTION: genAudioPkts
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: std::vector<Audio> Song::genAudioPkts()
--
-- PARAMETERS: N/A
--
-- RETURNS: vector<Audio>
--
-- NOTES:
--
------------------------------------------------------------------------------*/
std::vector<Audio> Song::genAudioPkts() {
    //unimplemented
    return std::vector<Audio>();
}
