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
    enum class LyricType
    {
        Origin,
        Translation,
        Mixed
    };

    //装载LRC到内存中
    //返回值为装载的歌词行数
    int ParseLRC(const QString& LRCString,LyricType Type=LyricType::Origin);
    bool GetLyricLineIndex(int Time,int& LyricLine,LyricType Type=LyricType::Origin);
    int GetLyricLineTime(int Line,LyricType Type=LyricType::Origin);
    void ClearLyric(LyricType Type);
    int GenerateMixedLyric();
    QVector<LyricLine> GetLyricData(LyricType Type=LyricType::Origin);
    void ClearLyric();
private:
    int PraseLRCString(const QString& LRCString, QVector<LRCParser::LyricLine> &Container,bool IsTranslation);
    QVector<LyricLine> LyricOrigin;
    QVector<LyricLine> LyricTranslation;
    QVector<LyricLine> LyricMixed;
    bool IsStatic=true;
    int FlowCursor=0;
    int LRCOffset;
    bool __GetLyricLineIndex(int Time, int &outLine, const QVector<LyricLine> &LyricData);
};

#endif // LRCPARSER_H
