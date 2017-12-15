#include "utilitytools.h"
#include "globalsetting.h"
#include "mainwindow.h"
#include "downloadwindow.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QWidgetAction>
#include <libQREncode/qrencode.h>

void GetPictureFromURL::run()
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    QUrl url(UrlString);
    QNetworkAccessManager manager;
    pmanager=&manager;
    QEventLoop loop;
    // qDebug() << "Reading picture form " << url;
    QObject::connect(pmanager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    QNetworkReply *reply = manager.get(QNetworkRequest(url));
    //开启子事件循环
    loop.exec();
    if(ShouldAbandon||reply->error())
        return;
    QByteArray picData = reply->readAll();
    QFile LocalCache(GlobalSetting::CacheDir+MD5Hash);
    LocalCache.open(QIODevice::WriteOnly);
    LocalCache.write(picData);
    LocalCache.close();
    QPixmap pixmap;
    pixmap.loadFromData(picData);
    emit sendLabel(pixmap);
    //deleteLater();
}

GetPictureFromURL::GetPictureFromURL(QString _UrlString,QObject* target,std::function<void(QPixmap)> _func)
{
    MD5Hash = QString(QCryptographicHash::hash(_UrlString.toLocal8Bit(),QCryptographicHash::Md5).toHex());
    QFile LocalCache(GlobalSetting::CacheDir+MD5Hash);
    if(LocalCache.exists())
    {
        LocalCache.open(QIODevice::ReadOnly);
        QPixmap pixmap;
        pixmap.loadFromData(LocalCache.readAll());
        LocalCache.close();
        _func(pixmap);
        delete this;
        return;
    }
    connect(this,&GetPictureFromURL::sendLabel,target,_func);
    UrlString=_UrlString;
    //func=_func;
    start();
}

QPixmap GetPictureFromURL::SearchCache(QString _UrlString)
{
    QString MD5Hashs = QString(QCryptographicHash::hash(_UrlString.toLocal8Bit(),QCryptographicHash::Md5).toHex());
    QFile LocalCache(GlobalSetting::CacheDir+MD5Hashs);
    if(LocalCache.exists())
    {
        LocalCache.open(QIODevice::ReadOnly);
        QPixmap pixmap;
        pixmap.loadFromData(LocalCache.readAll());
        LocalCache.close();
        return pixmap;
    }
    return QPixmap();
}

void GetPictureFromURL::Abandon()
{
    ShouldAbandon=true;
    emit pmanager->finished(nullptr);
}


QMenu* UtitlityTools::ConstructShareMenu(MusicInfomation MusicToShare)
{
    QMenu* Shared=new QMenu(u8"分享");
    Shared->addAction(u8"分享到QQ",[=]()
    {
        auto Encoder=QTextCodec::codecForName("utf-8");
        QString Url=QString::asprintf(u8"http://connect.qq.com/widget/shareqq/index.html?site=网易云音乐Qt By JackMyth"
                                      "&title=%s"
                                      "&summary=by %s"
                                      //"&pics=http://music.163.com/api/album/getpic/%d"
                                      "&pics=%s"
                                      "&desc=Share With NeteaseMusic Qt"
                                      "&url=http://music.163.com/m/song/?id=%d"
                                      "&&from=qq%3FimageView%26thumbnail%3D120y120",
                                      Encoder->fromUnicode(MusicToShare.Name).toPercentEncoding().data(),
                                      Encoder->fromUnicode(MusicToShare.ArtistsName).toPercentEncoding().data(),
                                      //MusicToShare.Album.ID,
                                      MusicToShare.Album.PicURL.toStdString().c_str(),
                                      MusicToShare.ID);
        //Url=->fromUnicode(Url).toPercentEncoding();
        QDesktopServices::openUrl(Url);
    });
    Shared->addAction(u8"二维码分享(微信)",[=]()
    {
        //auto Encoder=QTextCodec::codecForName("utf-8");
        QString Url=QString::asprintf(u8"http://music.163.com/m/song/?id=%d",MusicToShare.ID);
        QRcode* QR=QRcode_encodeString(Url.toStdString().c_str(), 0, QR_ECLEVEL_H, QR_MODE_8, 1);
        QMessageBox QRView;
        QImage QRPic(QR->width,QR->width,QImage::Format_RGB32);
        for(int x=0;x<QR->width;x++)
            for(int y=0;y<QR->width;y++)
                QRPic.setPixel(x,y,QR->data[y*QR->width+x]&1?qRgb(0,0,0):qRgb(255,255,255));
        QPixmap p=QPixmap::fromImage(QRPic.scaled(300,300));
        //p.scaledToHeight(100);
        //p.scaledToWidth(100);
        QRView.setIconPixmap(p);
        QRView.setWindowFlags(QRView.windowFlags()&~ Qt::WindowMinMaxButtonsHint);
        QRView.setWindowTitle(u8"用微信“扫一扫”");
        QRView.addButton(u8"确定",QMessageBox::NoRole);
        auto button=QRView.addButton(u8"保存二维码",QMessageBox::YesRole);
        //QRView.event()
        QRView.exec();
        if((void*)QRView.clickedButton()==(void*)button)
        {
            //QString SelectFilter;
            QString FilePath= QFileDialog::getSaveFileName(nullptr,u8"选择保存路径",QString(),"JPEG Files(*.jpg);;PNG Files(*.png);;BMP Files(*.bmp);;");
            if(FilePath!="")
                QRPic.scaled(500,500).save(FilePath);
        }
    });
    return Shared;
}

QMenu *UtitlityTools::ConstructPicViewMenu(QPixmap PixmapToShow)
{
    QMenu* menu=new QMenu();
    QWidgetAction* ImageHoldAction=new QWidgetAction(menu);
    QLabel* ImageHolder;
    ImageHolder=new QLabel();
    if(PixmapToShow.height()>500||PixmapToShow.width()>500)
    {
        ImageHolder->resize(500,500);
        ImageHolder->setPixmap(PixmapToShow.scaled(500,500,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    }
    else
    {
        ImageHolder->resize(PixmapToShow.size());
        ImageHolder->setPixmap(PixmapToShow);
    }
    ImageHoldAction->setDefaultWidget(ImageHolder);
    //ImageHolder->move(0,0);
    menu->addActions(QList<QAction*>({ImageHoldAction}));
    menu->addAction(QString(u8"保存图片 (%1x%2)").arg(PixmapToShow.width()).arg(PixmapToShow.height()),[=]()
    {
        QString FilePath= QFileDialog::getSaveFileName(nullptr,u8"选择保存路径",QString(),"JPEG Files(*.jpg);;PNG Files(*.png);;BMP Files(*.bmp);;");
        if(FilePath!="")
            PixmapToShow.save(FilePath);
    });
    return menu;
}

QMenu *UtitlityTools::ConstructDownloadMenu(MusicInfomation MusicToDownload)
{
    QMenu* menu=new QMenu();
    menu->addAction(u8"下载",[=]()
    {
        bool IsCacheExist=false;
        QString CacheFileName;
        QString TargetFileName=GetValidFileName(MusicToDownload.Name + " - " + MusicToDownload.Artists[0].Name+".mp3");
        for(int i=MusicToDownload.Quality.size()-1;i>=GlobalSetting::DownloadQuality||i==MusicToDownload.Quality.size()-1;i--)
        {
            QString MD5Hash=QCryptographicHash::hash(QString("%1%2").arg(MusicToDownload.ID).arg(MusicToDownload.Quality[i]).toLocal8Bit(),QCryptographicHash::Md5).toHex();
            if(QFile(GlobalSetting::CacheDir+MD5Hash).exists())
            {
                CacheFileName=MD5Hash;
                IsCacheExist=true;
                break;
            }
        }
        if(IsCacheExist)
        {
            QFile::copy(GlobalSetting::CacheDir+CacheFileName,GlobalSetting::DownloadDir+TargetFileName);
        }
        else
        {
            int TargetQuality=GlobalSetting::DownloadQuality;
            if(TargetQuality>=MusicToDownload.Quality.size())
                TargetQuality=MusicToDownload.Quality.size()-1;
            CacheFileName=QCryptographicHash::hash(QString("%1%2").arg(MusicToDownload.ID).arg(MusicToDownload.Quality[TargetQuality]).toLocal8Bit(),QCryptographicHash::Md5).toHex();
            QNetworkAccessManager tmpNetworkM;
            QEventLoop EventHolder;
            QString url=QString("https://api.imjad.cn/cloudmusic/?type=song&id=")
                    +QString::asprintf("%d",MusicToDownload.ID)
                    +QString("&br=")
                    +QString::asprintf("%d",MusicToDownload.Quality[TargetQuality]);
            QNetworkReply* tmpNetworkReply=tmpNetworkM.get(QNetworkRequest(url));
            QTimer Timeout;
            EventHolder.connect(tmpNetworkReply,&QNetworkReply::finished,&EventHolder,&QEventLoop::quit);
            EventHolder.connect(&Timeout,&QTimer::timeout,&EventHolder,&QEventLoop::quit);
            Timeout.start(5000);
            EventHolder.exec();
            if(tmpNetworkReply->isFinished())
            {
                QJsonObject rootjson=QJsonDocument::fromJson(tmpNetworkReply->readAll()).object();
                auto url= rootjson.find("data").value().toArray()[0].toObject().find("url").value().toString();
                MainWindow::MyInstance->GetDownloadManager()->AddToDownload(url,MusicToDownload,CacheFileName,GlobalSetting::DownloadDir+TargetFileName);
                DownloadWindow::ShowAndRise();
            }
            else
            {
                QMessageBox::warning(NULL,"Error",u8"请求链接超时");
            }
            tmpNetworkReply->deleteLater();
        }
    });
    menu->addAction(u8"下载到...",[=]()
    {
        bool IsCacheExist=false;
        QString CacheFileName;
        QString TargetFileName=GetValidFileName(MusicToDownload.Name + " - " + MusicToDownload.Artists[0].Name+".mp3");
        for(int i=MusicToDownload.Quality.size()-1;i>=GlobalSetting::DownloadQuality||i==MusicToDownload.Quality.size()-1;i--)
        {
            QString MD5Hash=QCryptographicHash::hash(QString("%1%2").arg(MusicToDownload.ID).arg(MusicToDownload.Quality[i]).toLocal8Bit(),QCryptographicHash::Md5).toHex();
            if(QFile(GlobalSetting::CacheDir+MD5Hash).exists())
            {
                CacheFileName=MD5Hash;
                IsCacheExist=true;
                break;
            }
        }
        if(IsCacheExist)
        {
            TargetFileName=QFileDialog::getSaveFileName(nullptr,u8"选择保存地点",TargetFileName,QString("MP3File(*.mp3);;"));
            if(TargetFileName!="")
                QFile::copy(GlobalSetting::CacheDir+CacheFileName,TargetFileName);
        }
        else
        {
            int TargetQuality=GlobalSetting::DownloadQuality;
            if(TargetQuality>=MusicToDownload.Quality.size())
                TargetQuality=MusicToDownload.Quality.size()-1;
            CacheFileName=QCryptographicHash::hash(QString("%1%2").arg(MusicToDownload.ID).arg(MusicToDownload.Quality[TargetQuality]).toLocal8Bit(),QCryptographicHash::Md5).toHex();
            QEventLoop EventHolder;
            QNetworkAccessManager tmpNetworkM;
            QString url=QString("https://api.imjad.cn/cloudmusic/?type=song&id=")
                    +QString::asprintf("%d",MusicToDownload.ID)
                    +QString("&br=")
                    +QString::asprintf("%d",MusicToDownload.Quality[TargetQuality]);
            QNetworkReply* tmpNetworkReply=tmpNetworkM.get(QNetworkRequest(url));
            TargetFileName=QFileDialog::getSaveFileName(nullptr,u8"选择保存地点",TargetFileName,QString("MP3File(*.mp3);;"));
            if(TargetFileName=="")
                return;
            if(!tmpNetworkReply->isFinished())
            {
                QTimer Timeout;
                EventHolder.connect(tmpNetworkReply,&QNetworkReply::finished,&EventHolder,&QEventLoop::quit);
                EventHolder.connect(&Timeout,&QTimer::timeout,&EventHolder,&QEventLoop::quit);
                Timeout.start(5000);
                EventHolder.exec();
            }
            if(tmpNetworkReply->isFinished())
            {
                QJsonObject rootjson=QJsonDocument::fromJson(tmpNetworkReply->readAll()).object();
                auto url= rootjson.find("data").value().toArray()[0].toObject().find("url").value().toString();
                MainWindow::MyInstance->GetDownloadManager()->AddToDownload(url,MusicToDownload,CacheFileName,TargetFileName);
                DownloadWindow::ShowAndRise();
            }
            else
            {
                QMessageBox::warning(NULL,"Error",u8"请求链接超时");
            }
            tmpNetworkReply->deleteLater();
        }
    });
    menu->addSeparator();
    menu->addAction(u8"下载管理器",[]()
    {
        DownloadWindow::ShowAndRise();
    });
    menu->addSeparator();
    if(GlobalSetting::AutoCache)
    {
        menu->addAction(u8"已缓存")->setEnabled(false);
    }
    else
    {
        for(int i=MusicToDownload.Quality.size()-1;i>=GlobalSetting::DownloadQuality||i==MusicToDownload.Quality.size()-1;i--)
        {
            QString MD5Hash=QCryptographicHash::hash(QString("%1%2").arg(MusicToDownload.ID).arg(MusicToDownload.Quality[i]).toLocal8Bit(),QCryptographicHash::Md5).toHex();
            if(QFile(GlobalSetting::CacheDir+MD5Hash).exists())
            {
                menu->addAction(u8"已缓存")->setEnabled(false);
                return menu;
            }
        }
        menu->addAction(u8"缓存(未完成)");
        return menu;
    }
    return menu;
}

QString UtitlityTools::GetValidFileName(QString StrToProcess)
{
    char t[9];
    t[0] = 0x5C;
    t[1] = 0x2F;
    t[2] = 0x3A;
    t[3] = 0x2A;
    t[4] = 0x3F;
    t[5] = 0x22;
    t[6] = 0x3C;
    t[7] = 0x3E;
    t[8] = 0x7C;
    for(int i=0;i<9;i++)
        StrToProcess.replace(t[i],"");
    return StrToProcess;
}
