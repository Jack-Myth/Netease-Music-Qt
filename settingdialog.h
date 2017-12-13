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

    void on_CommentLimit_2_currentIndexChanged(int index);

    void on_aboutButton_clicked();

private:
    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
