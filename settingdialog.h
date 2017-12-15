#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

private slots:
    void on_SearchLimit_currentTextChanged(const QString &arg1);

    void on_CommentLimit_currentIndexChanged(const QString &arg1);

    void on_aboutButton_clicked();

    void on_CachePath_editingFinished();

    void on_BrowserCache_clicked();

    void on_Quality_currentIndexChanged(int index);

    void on_Cache_stateChanged(int arg1);

    void on_Quality_Download_currentIndexChanged(int index);

    void on_DownloadPath_editingFinished();

    void on_BrowserDownload_clicked();

private:
    Ui::SettingDialog *ui;
    class QSettings *SettingFile=NULL;
};

#endif // SETTINGDIALOG_H
