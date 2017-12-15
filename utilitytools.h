#ifndef UTILITYTOOLS_H
#define UTILITYTOOLS_H
#include "mystruct.h"

#include <QtNetwork/QtNetwork>

#include <QMainWindow>

class GetPictureFromURL : public QThread
{
    Q_OBJECT
private:
    QString UrlString;
    QNetworkAccessManager* pmanager;
    bool ShouldAbandon=false;
    std::function<void(QPixmap)> func;
    QString MD5Hash;
signals:
    void sendLabel(QPixmap Pixmap);
public:
    void run() override;
    GetPictureFromURL(QString _UrlString,QObject*,std::function<void(QPixmap)> _func);
    static QPixmap SearchCache(QString _UrlString);
    void Abandon();
};

namespace UtitlityTools
{
    QMenu* ConstructShareMenu(MusicInfomation MusicToShare);
    QMenu* ConstructPicViewMenu(QPixmap PixmapToShow);
    QMenu* ConstructDownloadMenu(MusicInfomation MusicToShare);
    QString GetValidFileName(QString StrToProcess);
}
#endif // UTILITYTOOLS_H
