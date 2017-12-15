#include "settingdialog.h"
#include "globalsetting.h"
#include "ui_settingdialog.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    SettingFile = new QSettings("Setting.ini", QSettings::IniFormat);
    ui->CommentLimit->setCurrentText(QString("%1").arg(GlobalSetting::CommentLimit));
    ui->SearchLimit->setCurrentText(QString("%1").arg(GlobalSetting::SearchLimit));
    ui->Quality->setCurrentIndex(GlobalSetting::OnlineQuality);
    ui->Quality_Download->setCurrentIndex(GlobalSetting::DownloadQuality);
    ui->CachePath->setText(GlobalSetting::CacheDir);
    ui->DownloadPath->setText(GlobalSetting::DownloadDir);
    ui->Cache->setChecked(GlobalSetting::AutoCache);
}

SettingDialog::~SettingDialog()
{
    delete ui;
    delete SettingFile;
    SettingFile=NULL;
}

void SettingDialog::on_SearchLimit_currentTextChanged(const QString &arg1)
{
    if(GlobalSetting::SearchLimit!=arg1.toInt())
    {
        GlobalSetting::SearchLimit=arg1.toInt();
        SettingFile->setValue("Setting/SearchLimit",GlobalSetting::SearchLimit);
    }
}

void SettingDialog::on_CommentLimit_currentIndexChanged(const QString &arg1)
{
    if(GlobalSetting::CommentLimit!=arg1.toInt())
    {
        GlobalSetting::CommentLimit=arg1.toInt();
        SettingFile->setValue("Setting/CommentLimit",GlobalSetting::CommentLimit);
    }
}

void SettingDialog::on_Quality_currentIndexChanged(int index)
{
    if(GlobalSetting::OnlineQuality=index)
    {
        GlobalSetting::OnlineQuality=index;
        SettingFile->setValue("Setting/OnlineQuality",GlobalSetting::OnlineQuality);
    }
}

void SettingDialog::on_aboutButton_clicked()
{
    QMessageBox About;
    About.setIconPixmap(QPixmap(":/Resources/Logo.png").scaled(151,182,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    About.setWindowTitle(u8"Netease Music Qt");
    About.setText(u8"Netease Music Qt\n网易云音乐Qt\nDesigned By JackMyth\nApi From imjad.cn\nBuilt With Qt5.10");
    About.addButton(u8"确定",QMessageBox::AcceptRole);
    About.exec();
}

void SettingDialog::on_CachePath_editingFinished()
{
    if(ui->CachePath->hasFocus())
    {
        QDir CacheDir(ui->CachePath->text());
        if(!CacheDir.exists())
        {
            if (!CacheDir.mkdir(CacheDir.absolutePath()))
            {
                QMessageBox::warning(nullptr,u8"Warning",u8"缓存目录不存在且无法创建，此路径不能设置为缓存路径");
                ui->CachePath->setText(GlobalSetting::CacheDir);
                return;
            }
        }
        GlobalSetting::CacheDir=ui->CachePath->text() + "\\";
        SettingFile->setValue("Setting/CacheDir",GlobalSetting::CacheDir);
    }
}

void SettingDialog::on_BrowserCache_clicked()
{
    ui->CachePath->setText(QFileDialog::getExistingDirectory(nullptr,u8"选择缓存目录"));
    QDir CacheDir(ui->CachePath->text());
    if(!CacheDir.exists())
    {
        if (!CacheDir.mkdir(CacheDir.absolutePath()))
        {
            QMessageBox::warning(nullptr,u8"Warning",u8"目录不存在且无法创建，此路径不能设置为缓存路径");
            ui->CachePath->setText(GlobalSetting::CacheDir);
            return;
        }
    }
    GlobalSetting::CacheDir=ui->CachePath->text();
    SettingFile->setValue("Setting/CacheDir",GlobalSetting::CacheDir);
}

void SettingDialog::on_Cache_stateChanged(int arg1)
{
    if(ui->Cache->isChecked()!=GlobalSetting::AutoCache)
    {
        GlobalSetting::AutoCache=ui->Cache->isChecked();
        SettingFile->setValue("Setting/AutoCache",GlobalSetting::AutoCache);
    }
}

void SettingDialog::on_Quality_Download_currentIndexChanged(int index)
{
    if(GlobalSetting::DownloadQuality=index)
    {
        GlobalSetting::DownloadQuality=index;
        SettingFile->setValue("Setting/DownloadQuality",GlobalSetting::OnlineQuality);
    }
}

void SettingDialog::on_DownloadPath_editingFinished()
{
    if(ui->CachePath->hasFocus())
    {
        QDir DownloadDir(ui->DownloadPath->text());
        if(!DownloadDir.exists())
        {
            if (!DownloadDir.mkdir(DownloadDir.absolutePath()))
            {
                QMessageBox::warning(nullptr,u8"Warning",u8"目录不存在且无法创建，此路径不能设置为下载路径");
                ui->DownloadPath->setText(GlobalSetting::DownloadDir);
                return;
            }
        }
        GlobalSetting::DownloadDir=ui->DownloadPath->text() + "\\";
        SettingFile->setValue("Setting/DownloadDir",GlobalSetting::DownloadDir);
    }
}

void SettingDialog::on_BrowserDownload_clicked()
{
    ui->DownloadPath->setText(QFileDialog::getExistingDirectory(nullptr,u8"选择下载目录"));
    QDir DownloadDir(ui->DownloadPath->text());
    if(!DownloadDir.exists())
    {
        if (!DownloadDir.mkdir(DownloadDir.absolutePath()))
        {
            QMessageBox::warning(nullptr,u8"Warning",u8"目录不存在且无法创建，此路径不能设置为下载路径");
            ui->DownloadPath->setText(GlobalSetting::DownloadDir);
            return;
        }
    }
    GlobalSetting::DownloadDir=ui->DownloadPath->text()+"\\";
    SettingFile->setValue("Setting/DownloadDir",GlobalSetting::DownloadDir);
}
