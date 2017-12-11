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
    std::function<void(QPixmap)> func;
signals:
    void sendLabel(QPixmap Pixmap);
public:
    void run() override;
    GetPictureFromURL(QString _UrlString,QObject*,std::function<void(QPixmap)> _func);
};

namespace UtitlityTools
{
    QMenu* ConstructShareMenu(MusicInfomation MusicToShare);
}
#endif // UTILITYTOOLS_H
