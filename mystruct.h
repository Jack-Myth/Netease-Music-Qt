#ifndef MYSTRUCT_H
#define MYSTRUCT_H
#include <QVector>

struct AlbumInfomation
{
    int ID;
    //int64_t PicID;
    QString Name;
    QString PicURL;
};

struct ArtistInfomation
{
    int ID;
    QString Name;
};

struct MusicInfomation
{
    int ID;
    QString Name;
    AlbumInfomation Album; //只有一个专辑
    QVector<ArtistInfomation> Artists; //可能存在多个艺术家
    QString ArtistsName;
    QVector<int> Quality;   //64000,128000,198000,320000
    int Length; //毫秒单位
    QString URL;
    bool operator ==(const MusicInfomation& MusicInfo)const
    {
        return MusicInfo.ID==this->ID;
    }
};
#endif // MYSTRUCT_H
