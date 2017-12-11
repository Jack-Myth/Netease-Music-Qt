#include "lrcparser.h"
#include <QStringList>
#include <QtMath>

int LRCParser::ParseLRCString(QString LRCString)
{
    IsStatic=false;
    LyricData.clear();
    LRCti=LRCar=LRCal=LRCby="";
    LRCOffset=0;
    auto LyricList= LRCString.split("\n",QString::SkipEmptyParts);
    bool IsNormalLyric=false;
    for(int lyricit=0;lyricit<LyricList.size();lyricit++)
    {
        QString tmpTagData;
        QString tmpLyricLine;
        QString ALyric;
        if(IsNormalLyric)
            goto AsNormalLyric;
        ALyric= LyricList.at(lyricit);
        ALyric=ALyric.trimmed();
        if(ALyric.at(0)!='[')
            goto AsNormalLyric;
        ALyric=ALyric.mid(1);
        int midIndex=ALyric.indexOf(']');
        if(midIndex<0)
            goto AsNormalLyric;
        tmpTagData=ALyric.mid(0,midIndex);
        tmpLyricLine=ALyric.mid(midIndex+1);
        if(tmpTagData.length()<3)
            goto AsNormalLyric;
        if(tmpTagData.mid(0,3)=="ti:")   //歌曲标题
        {
            LRCti=tmpTagData.mid(3);
        }
        else if (tmpTagData.mid(0,3)=="ar:")  //艺术家
        {
            LRCar=tmpTagData.mid(3);
        }
        else if(tmpTagData.mid(0,3)=="al:")  //专辑
        {
            LRCal=tmpTagData.mid(3);
        }
        else if (tmpTagData.mid(0,3)=="by:")  //编辑者
        {
            LRCby=tmpTagData.mid(3);
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
                    goto AsNormalLyric;
            }
            QStringList TimeList=tmpTagData.split(':',QString::SkipEmptyParts);
            float Second=0;
            int i=0;
            for(;i<TimeList.size();i++)
            {
                Second+=(TimeList.end()-i-1)->toFloat()*qPow(60,i);
            }
            LyricData.push_back({(int)(Second*1000),tmpLyricLine});
        }
        continue;
AsNormalLyric:
        LyricData.push_back({-1,LyricList.at(lyricit)});
        IsNormalLyric=true;
        IsStatic=true;
        continue;
    }
    if(LRCby!="")
        LyricData.push_front({0,LRCby});
    if(LRCal!="")
        LyricData.push_front({0,LRCal});
    if(LRCar!="")
        LyricData.push_front({0,LRCar});
    if(LRCti!="")
        LyricData.push_front({0,LRCti});
    return LyricData.size();
}

bool LRCParser::GetLyricLineIndex(int Time,int& outLine)
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

int LRCParser::GetLyricLineTime(int Line)
{
    return LyricData[Line].TimeBegin;
}

void LRCParser::ClearLyric()
{
    LyricData.clear();
}

QVector<LRCParser::LyricLine> LRCParser::GetLyricData()
{
    return LyricData;
}
