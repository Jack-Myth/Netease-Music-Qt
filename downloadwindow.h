#ifndef DOWNLOADWINDOW_H
#define DOWNLOADWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class DownloadWindow;
}

class DownloadWindow : public QMainWindow
{
    Q_OBJECT

public:
    ~DownloadWindow();
    static DownloadWindow* ShowAndRise();
    static DownloadWindow* MyInstance;
private slots:

private:
    explicit DownloadWindow(QWidget *parent = 0);
    QTimer TimerForUpdate;
    Ui::DownloadWindow *ui;
    void UpdateDownloadProgress();
};

#endif // DOWNLOADWINDOW_H
