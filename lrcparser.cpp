#include "lrcparser.h"
#include <QStringList>
#include <QtMath>

int LRCParser::PraseLRCString(const QString& LRCString, QVector<LRCParser::LyricLine> &Container,bool IsTranslation)
{
    QString LRCti;
    QString LRCar;
    QString LRCal;
    QString LRCby;
    Container.clear();
    //LRCti=LRCar=LRCal=LRCby="";
    IsStatic=false;
    LRCOffset=0;
    auto LyricList= LRCString.split("\n",QString::SkipEmptyParts);
    bool IsNormalLyric=false;
    for(int lyricit=0;lyricit<LyricList.size();lyricit++)
    {
        QString tmpTagData;
        QString tmpLyricLine;
        QString ALyric;
        QVector<int> TimeCollection;
        if(IsNormalLyric)
            goto AsNormalLyric;
        ALyric= LyricList.at(lyricit);
        while(1)
        {
            ALyric=ALyric.trimmed();
            if(ALyric==""||ALyric.at(0)!='[')
            {
                if(TimeCollection.size()==0)
                    goto AsNormalLyric;
                else break;
            }
            ALyric=ALyric.mid(1);
            int midIndex=ALyric.indexOf(']');
            if(midIndex<0)
            {
                if(TimeCollection.size()==0)
                    goto AsNormalLyric;
                else break;
            }
            tmpTagData=ALyric.mid(0,midIndex);
            tmpLyricLine=ALyric.mid(midIndex+1);
            if(tmpTagData.length()<3)
            {
                if(TimeCollection.size()==0)
                    goto AsNormalLyric;
                else break;
            }
            if(tmpTagData.mid(0,3)=="ti:")   //歌曲标题
            {
                LRCti=tmpTagData.mid(3);
                goto DirectlyContinue;
            }
            else if (tmpTagData.mid(0,3)=="ar:")  //艺术家
            {
                LRCar=tmpTagData.mid(3);
                goto DirectlyContinue;
            }
            else if(tmpTagData.mid(0,3)=="al:")  //专辑
            {
                LRCal=tmpTagData.mid(3);
                goto DirectlyContinue;
            }
            else if (tmpTagData.mid(0,3)=="by:")  //编辑者
            {
                LRCby=tmpTagData.mid(3);
                goto DirectlyContinue;
            }
            else if(tmpTagData.length()>7&&tmpTagData.mid(0,7)=="offset:")  //Offset(整体偏移量)，毫秒单位,有正负之分
            {
                bool IsOffsetSuccess;
                int TmpOffset= tmpTagData.mid(7).toInt(&IsOffsetSuccess);
                if(IsOffsetSuccess)
                    LRCOffset=TmpOffset;
            }
            else
            {
                for(int i=0;i<tmpTagData.length();i++)
                {
                    auto BytetoC=tmpTagData.at(i);
                    if(!((BytetoC.toLatin1()<='9'&&BytetoC.toLatin1()>='0')||BytetoC=='.'||BytetoC==':'))
                    {
                        if(TimeCollection.size()==0)
                            goto AsNormalLyric;
                        else
                            goto AsLRCLyric;
                    }
                }
                QStringList TimeList=tmpTagData.split(':',QString::SkipEmptyParts);
                float Second=0;
                int i=0;
                for(;i<TimeList.size();i++)
                {
                    Second+=(TimeList.end()-i-1)->toFloat()*qPow(60,i);
                }
                TimeCollection.push_back((int)(Second*1000));
                ALyric=tmpLyricLine;
            }
        }
AsLRCLyric:
        for(int i=0;i<TimeCollection.size();i++)
            Container.push_back({TimeCollection[i],ALyric});
DirectlyContinue:
        continue;
AsNormalLyric:
        Container.push_back({0,LyricList.at(lyricit)});
        IsNormalLyric=true;
        IsStatic=true;
        continue;
    }
    std::sort(Container.begin(),Container.end(),[](const LyricLine& A,const LyricLine& B){return A.TimeBegin<B.TimeBegin;});
    if(LRCby!="")
        Container.push_front({0,(IsTranslation?QString(u8"歌词由"):QString(u8"翻译由"))+LRCby+QString(u8"提供")});
    if(LRCal!="")
        Container.push_front({0,QString(u8"专辑:")+LRCal});
    if(LRCar!="")
        Container.push_front({0,QString(u8"艺术家:")+LRCar});
    if(LRCti!="")
        Container.push_front({0,QString(u8"歌曲:")+LRCti});
    return Container.size();
}

int LRCParser::ParseLRC(const QString& LRCString,LyricType Type)
{
    switch (Type) {
    case LyricType::Origin:
        return PraseLRCString(LRCString,LyricOrigin,false);
    case LyricType::Translation:
        return PraseLRCString(LRCString,LyricTranslation,true);
    }
    return 0;
}

bool LRCParser::GetLyricLineIndex(int Time, int &LyricLine, LRCParser::LyricType Type)
{

    switch (Type) {
    case LyricType::Origin:
        return __GetLyricLineIndex(Time,LyricLine,LyricOrigin);
    case LyricType::Translation:
        return __GetLyricLineIndex(Time,LyricLine,LyricTranslation);
    case LyricType::Mixed:
        return __GetLyricLineIndex(Time,LyricLine,LyricMixed);
    }
    return false;
}

bool LRCParser::__GetLyricLineIndex(int Time,int& outLine,const QVector<LyricLine> &LyricData)
{
    bool IsChanged=true;
    if(LyricData.size()==0||IsStatic)
        return false;
    if(FlowCursor<1)
    {
        FlowCursor=1;
        IsChanged=false;
    }
    else if(FlowCursor>LyricData.size()-2)
    {
        FlowCursor=LyricData.size()-2;
        IsChanged=false;
    }
    if(Time>=LyricData[FlowCursor+1].TimeBegin)
    {
        while(LyricData[FlowCursor+1].TimeBegin<Time)
        {
            FlowCursor++;
            if(FlowCursor==LyricData.size()-1)
                break;
        }
        outLine= FlowCursor;
        return IsChanged;
    }else if(Time<LyricData[FlowCursor-1].TimeBegin)
    {
        while(LyricData[FlowCursor-1].TimeBegin>Time)
        {
            FlowCursor--;
            if(FlowCursor<=1)
                break;
        }
        FlowCursor--;
        outLine= FlowCursor;
        return IsChanged;
    }
    outLine=FlowCursor;
    return false;
}

int LRCParser::GetLyricLineTime(int Line,LyricType Type)
{
    switch (Type) {
    case LyricType::Origin:
        return LyricOrigin[Line].TimeBegin;
    case LyricType::Translation:
        return LyricTranslation[Line].TimeBegin;
    case LyricType::Mixed:
        return LyricMixed[Line].TimeBegin;
    }
    return 0;
}

void LRCParser::ClearLyric(LyricType Type)
{
    switch (Type) {
    case LyricType::Origin:
        return LyricOrigin.clear();
    case LyricType::Translation:
        return LyricTranslation.clear();
    case LyricType::Mixed:
        return LyricMixed.clear();
    }
}

int LRCParser::GenerateMixedLyric()
{
    if (!LyricTranslation.size())
        return 0;
    int o=0,t=0;
    while(o<LyricOrigin.size()&&t<LyricTranslation.size())
    {
        if(LyricOrigin[o].TimeBegin!=LyricTranslation[t].TimeBegin)
        {
            if(LyricOrigin[o].TimeBegin<LyricTranslation[t].TimeBegin)
            {
                LyricMixed.push_back({LyricOrigin[o].TimeBegin,LyricOrigin[o].Line});
                o++;
            }
            else
            {
                LyricMixed.push_back({LyricTranslation[t].TimeBegin,LyricTranslation[t].Line});
                t++;
            }
        }
        else
        {
            if(LyricOrigin[o].Line!=LyricTranslation[t].Line)
                LyricMixed.push_back({LyricOrigin[o].TimeBegin,LyricOrigin[o].Line+QString("\n")+LyricTranslation[t].Line});
            else
                LyricMixed.push_back({LyricOrigin[o].TimeBegin,LyricOrigin[o].Line});
            o++,t++;
        }
    }
    if(o>=LyricOrigin.size())
        for(;t<LyricTranslation.size();t++)
            LyricMixed.push_back({LyricTranslation[t].TimeBegin,LyricTranslation[t].Line});
    else
        for(;o<LyricOrigin.size();o++)
            LyricMixed.push_back({LyricOrigin[o].TimeBegin,LyricOrigin[o].Line});
    return LyricMixed.size();
}

void LRCParser::ClearLyric()
{
    LyricOrigin.clear();
    LyricTranslation.clear();
    LyricMixed.clear();
    return;
}

QVector<LRCParser::LyricLine> LRCParser::GetLyricData(LyricType Type)
{
    switch (Type) {
    case LyricType::Origin:
        return LyricOrigin;
    case LyricType::Translation:
        return LyricTranslation;
    case LyricType::Mixed:
        if(LyricMixed.size()!=0)
            return LyricMixed;
        else
            return LyricOrigin;
    }
    return QVector<LRCParser::LyricLine>();
}


