#include "settingdialog.h"
#include "globalsetting.h"
#include "ui_settingdialog.h"

#include <QMessageBox>

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::on_SearchLimit_currentTextChanged(const QString &arg1)
{
    if(GlobalSetting::SearchLimit!=arg1.toInt())
    {
        GlobalSetting::SearchLimit=arg1.toInt();
    }
}

void SettingDialog::on_CommentLimit_currentIndexChanged(const QString &arg1)
{
    if(GlobalSetting::CommentLimit!=arg1.toInt())
    {
        GlobalSetting::CommentLimit=arg1.toInt();
    }
}

void SettingDialog::on_CommentLimit_2_currentIndexChanged(int index)
{
    if(GlobalSetting::OnlineQuality=index)
    {
        GlobalSetting::OnlineQuality=index;
    }
}

void SettingDialog::on_aboutButton_clicked()
{
    QMessageBox About;
    About.setIconPixmap(QPixmap(":/Resources/Logo.png").scaled(151,182,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    About.setWindowTitle(u8"Netease Music Qt");
    About.setText(u8"Netease Music Qt\n网易云音乐Qt\nDesigned By JackMyth");
    About.addButton(u8"确定",QMessageBox::AcceptRole);
    About.exec();
}
