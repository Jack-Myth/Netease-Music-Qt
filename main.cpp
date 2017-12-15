#include "mainwindow.h"
#include "searchwindow.h"
#include "globalsetting.h"
#include <QFileInfo>
#include <QApplication>
#include <QMessageBox>

void LoadSetting();
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoadSetting();
    MainWindow w;
    w.show();
    return a.exec();
}

void LoadSetting()
{
    QSettings *SettingFile = new QSettings("Setting.ini", QSettings::IniFormat);
    GlobalSetting::CacheDir = SettingFile->value("Setting/CacheDir",".\\.Cache\\").toString();
    GlobalSetting::DownloadDir = SettingFile->value("Setting/DownloadDir",".\\Download\\").toString();
    GlobalSetting::CommentLimit = SettingFile->value("Setting/CommentLimit",20).toInt();
    GlobalSetting::SearchLimit=SettingFile->value("Setting/SearchLimit",20).toInt();
    GlobalSetting::OnlineQuality = SettingFile->value("Setting/OnlineQuality",1).toInt();
    GlobalSetting::DownloadQuality = SettingFile->value("Setting/DownloadQuality",2).toInt();
    GlobalSetting::AutoCache = SettingFile->value("Setting/AutoCache",true).toBool();
    delete SettingFile;
    if(!QFileInfo(GlobalSetting::CacheDir).isDir())
    {
        auto dir= QDir(GlobalSetting::CacheDir);
        if(!dir.mkdir(dir.absolutePath()))
        {
            QMessageBox::critical(nullptr,u8"Critical Error!",u8"缓存目录不存在且创建失败，退出！");
            exit(-1);
        }
    }
}
