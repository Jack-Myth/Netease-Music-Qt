#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H
#include "mystruct.h"

#include <QObject>

class DownloadItem :public QObject
{
    Q_OBJECT

    class QNetworkAccessManager* NetworkM;
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    class QNetworkReply* NetworkReply=nullptr;
    class DownloadManager* Manager;
public:
    MusicInfomation MusicInfo;
    qint64 Downloaded=0;
    qint64 Size=0;
    QString DownloadPath;
    QString DownloadURL;
    QString CacheFileName;
    QString TargetFileName;
    void Start();
    void Pause();
    DownloadItem(QString DownloadURL,MusicInfomation DownloadInfo,QString CacheFileName,QString TargetFileName,DownloadManager* Dmanager);
    ~DownloadItem();
    //void Stop();
};

class DownloadManager
{
protected:
    friend class DownloadItem;
    void FinishedAndRemove(DownloadItem* ItemFinished);
private:

    QVector<DownloadItem*> DownloadItemCollection;
public:
    DownloadManager();
    DownloadItem* Add(QString DownloadURL,MusicInfomation MusicToDownload,QString CacheFileName,QString TargetFileName);
    void AddToDownload(QString DownloadURL,MusicInfomation MusicToDownload,QString CacheFileName,QString TargetFileName);
    QVector<DownloadItem*> GetDownloadItems();
    DownloadItem* FindDownloadItemByMusicID(int MusicID);
};

#endif // DOWNLOADMANAGER_H
