#include "downloadmanager.h"
#include "globalsetting.h"
#include <QtNetwork>

void DownloadManager::FinishedAndRemove(DownloadItem *ItemFinished)
{
    DownloadItemCollection.removeAll(ItemFinished);
}

DownloadManager::DownloadManager()
{

}

DownloadItem* DownloadManager::Add(QString DownloadURL, MusicInfomation MusicToDownload, QString CacheFileName, QString TargetFileName)
{
    DownloadItem* newDownloadItem=new DownloadItem(DownloadURL,MusicToDownload,CacheFileName,TargetFileName,this);
    DownloadItemCollection.push_back(newDownloadItem);
    return newDownloadItem;
}

void DownloadManager::AddToDownload(QString DownloadURL, MusicInfomation MusicToDownload, QString CacheFileName, QString TargetFileName)
{
    Add(DownloadURL,MusicToDownload,CacheFileName,TargetFileName)->Start();
}

QVector<DownloadItem *> DownloadManager::GetDownloadItems()
{
    return DownloadItemCollection;
}

DownloadItem *DownloadManager::FindDownloadItemByMusicID(int MusicID)
{
    for(int i=0;i<DownloadItemCollection.size();i++)
    {
        if(DownloadItemCollection[i]->MusicInfo.ID==MusicID)
            return DownloadItemCollection[i];
    }
}

void DownloadItem::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Downloaded=bytesReceived;
    Size=bytesTotal;
}

void DownloadItem::downloadFinished()
{
    qDebug()<<"CacheFilePath:"+GlobalSetting::CacheDir+ CacheFileName;
    qDebug()<<"TargetFilePath:"+TargetFileName;
    QFile TargetFile(GlobalSetting::CacheDir+ CacheFileName);
    TargetFile.open(QIODevice::WriteOnly);
    TargetFile.write(NetworkReply->readAll());
    TargetFile.close();
    TargetFile.copy(TargetFile.fileName(),TargetFileName);
    Manager->FinishedAndRemove(this);
    delete this;
}

void DownloadItem::Start()
{
    if(NetworkReply)
        return;
    NetworkReply= NetworkM->get(QNetworkRequest(QUrl(DownloadURL)));
    NetworkReply->connect(NetworkReply,&QNetworkReply::downloadProgress,this,&DownloadItem::downloadProgress);
    NetworkReply->connect(NetworkReply,&QNetworkReply::finished,this,&DownloadItem::downloadFinished);
}

void DownloadItem::Pause()
{
    NetworkReply->abort();
    NetworkReply->deleteLater();
    NetworkReply=NULL;
}

DownloadItem::DownloadItem(QString DownloadURL,MusicInfomation DownloadInfo,QString CacheFileName,QString TargetFileName,DownloadManager* Dmanager)
{
    NetworkM=new QNetworkAccessManager();
    MusicInfo=DownloadInfo;
    DownloadItem::CacheFileName=CacheFileName;
    DownloadItem::DownloadURL=DownloadURL;
    DownloadItem::TargetFileName=TargetFileName;
    Manager=Dmanager;
}

DownloadItem::~DownloadItem()
{
    delete NetworkReply;
    delete NetworkM;
}
