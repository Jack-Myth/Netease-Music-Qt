#ifndef COMMENTWINDOW_H
#define COMMENTWINDOW_H

#include <QMainWindow>

namespace Ui {
class CommentWindow;
}

class CommentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CommentWindow(QWidget *parent = 0);
    ~CommentWindow();

private:
    Ui::CommentWindow *ui;
};

#endif // COMMENTWINDOW_H
