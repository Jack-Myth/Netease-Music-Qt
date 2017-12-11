#include "utilitytools.h"
#include <QDesktopServices>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <libQREncode/qrencode.h>

void GetPictureFromURL::run()
{
    QUrl url(UrlString);
    QNetworkAccessManager manager;
    QEventLoop loop;
    // qDebug() << "Reading picture form " << url;
    QNetworkReply *reply = manager.get(QNetworkRequest(url));
    //请求结束并下载完成后，退出子事件循环
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    //开启子事件循环
    loop.exec();
    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(jpegData);
    emit sendLabel(pixmap);
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    //deleteLater();
}

GetPictureFromURL::GetPictureFromURL(QString _UrlString,QObject* target,std::function<void(QPixmap)> _func)
{
    UrlString=_UrlString;
    func=_func;
    connect(this,&GetPictureFromURL::sendLabel,target,func);
    start();
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
        auto Encoder=QTextCodec::codecForName("utf-8");
        QString Url=QString::asprintf(u8"http://music.163.com/m/song/?id=%d",MusicToShare.ID);
        QRcode* QR=QRcode_encodeString(Url.toStdString().c_str(), 0, QR_ECLEVEL_H, QR_MODE_8, 1);
        QMessageBox QRView;
        QImage QRPic(QR->width,QR->width,QImage::Format_RGB32);
        for(int x=0;x<QR->width;x++)
            for(int y=0;y<QR->width;y++)
            {
                QRPic.setPixel(x,y,QR->data[y*QR->width+x]&1?qRgb(0,0,0):qRgb(255,255,255));
            }
        QPixmap p=QPixmap::fromImage(QRPic.scaled(500,500));
        p.scaledToHeight(100);
        p.scaledToWidth(100);
        QRView.setIconPixmap(p);
        QRView.setWindowFlags(QRView.windowFlags()&~ Qt::WindowMinMaxButtonsHint);
        QRView.setWindowTitle(u8"用微信“扫一扫”");
        QRView.addButton(u8"确定",QMessageBox::NoRole);
        auto button=QRView.addButton(u8"保存二维码",QMessageBox::YesRole);
        //QRView.event()
        QRView.exec();
        if((void*)QRView.clickedButton()==(void*)button)
        {
            QFile::set
        }
    });
    return Shared;
}
