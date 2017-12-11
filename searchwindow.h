#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include "mystruct.h"

#include <QMainWindow>

namespace Ui {
class SearchWindow;
}

class SearchWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SearchWindow(QWidget *parent = 0);
    static SearchWindow* MyInstance;
    ~SearchWindow();
private slots:

    void on_Search_lineEdit_editingFinished();

    void on_pushButton_clicked();

    void on_SearchResult_Music_cellDoubleClicked(int row, int column);

    void on_SearchPre_Button_clicked();

    void on_SearchNext_Button_clicked();

    void on_SearchResult_tabWidget_currentChanged(int index);

    void on_SearchResult_Music_customContextMenuRequested(const QPoint &pos);

private:
    Ui::SearchWindow *ui;
    QVector<MusicInfomation> LastSearchResult_Music;
    int SearchPage=0;
    class QNetworkAccessManager* NetworkM;
    void BeginSearch(int Page);

};

#endif // SEARCHWINDOW_H
