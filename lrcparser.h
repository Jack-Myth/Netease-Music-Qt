#ifndef LRCPARSER_H
#define LRCPARSER_H

#include <QString>
#include <QVector>

class LRCParser
{
public:
    struct LyricLine
    {
        int TimeBegin=-1;
        QString Line;
    };

    //装载LRC到内存中
    //返回值为装载的歌词行数
    //前一个LRC的数据必将清除
    int ParseLRCString(QString LRCString);
    bool GetLyricLineIndex(int Time,int& LyricLine);
    int GetLyricLineTime(int Line);
    void ClearLyric();
    QVector<LyricLine> GetLyricData();
private:
    QVector<LyricLine> LyricData;
    bool IsStatic=true;
    QString LRCti;
    QString LRCar;
    QString LRCal;
    QString LRCby;
    int FlowCursor=0;
    int LRCOffset;
};

#endif // LRCPARSER_H
