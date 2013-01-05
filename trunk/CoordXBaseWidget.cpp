#include "StdAfx.h"
#include "CoordXBaseWidget.h"
#include "DataEngine.h"

//�����ʱ���ݵĺ�����ʱ��
int getTimeMapByMin(QMap<time_t,int>& mapTimes,time_t& tmBegin, time_t& tmEnd, int iSize = 60/*second*/)
{
	if(tmBegin>tmEnd)
		return 0;

	time_t tmCur = tmEnd;
	while(tmCur>=tmBegin)
	{
//		QString qsTime = QDateTime::fromTime_t(tmCur).toString();
		mapTimes.insert(tmCur,mapTimes.size());
		tmCur = tmCur-iSize;
	}
	return 1;
}

int getDayMapByHistory(QMap<time_t,int>& mapTimes,const QList<qRcvHistoryData*>& historys)
{
	foreach(qRcvHistoryData* p,historys)
	{
		mapTimes.insert(p->time,mapTimes.size());
	}

	return 1;
}

int getWeekMapByHistory(QMap<time_t,int>& mapTimes,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	int iCurWeek = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
		iCurWeek = tmDate.weekNumber();
	}

	QList<qRcvHistoryData*> weekHis;		//�����ڽ��й����Ժ����������
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			if(tmDate.weekNumber(&iCurYear)!=iCurWeek)
			{
				if(weekHis.size()>0)
				{
					mapTimes.insert(weekHis.first()->time,mapTimes.size());
					weekHis.clear();
				}
			}
			iCurYear = tmDate.year();
			iCurWeek = tmDate.weekNumber();
		}
		else if(tmDate.weekNumber()!=iCurWeek)
		{
			iCurWeek = tmDate.weekNumber();

			if(weekHis.size()>0)
			{
				mapTimes.insert(weekHis.first()->time,mapTimes.size());
				weekHis.clear();
			}
		}
		weekHis.push_back(pHistory);
	}

	if(weekHis.size()>0)
	{
		//���ʣ���Ҳ����
		mapTimes.insert(weekHis.first()->time,mapTimes.size());
		weekHis.clear();
	}


	return 1;
}

int getMonthMapByHistory(QMap<time_t,int>& mapTimes,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	int iCurMonth = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
		iCurMonth = tmDate.month();
	}

	QList<qRcvHistoryData*> monthHis;		//�����ڽ��й����Ժ����������
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			iCurYear = tmDate.year();
			iCurMonth = tmDate.month();
			if(monthHis.size()>0)
			{
				mapTimes.insert(monthHis.first()->time,mapTimes.size());
				monthHis.clear();
			}
		}
		else if(tmDate.month()!=iCurMonth)
		{
			iCurMonth = tmDate.month();

			if(monthHis.size()>0)
			{
				mapTimes.insert(monthHis.first()->time,mapTimes.size());
				monthHis.clear();
			}
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//���ʣ���Ҳ����
		mapTimes.insert(monthHis.first()->time,mapTimes.size());
		monthHis.clear();
	}

	return 1;
}

int getMonth3MapByHistory(QMap<time_t,int>& mapTimes,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	int iCurMonth = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
		iCurMonth = tmDate.month();
	}

	QList<qRcvHistoryData*> monthHis;		//�����ڽ��й����Ժ����������
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			iCurYear = tmDate.year();
			iCurMonth = tmDate.month();
			if(monthHis.size()>0)
			{
				mapTimes.insert(monthHis.first()->time,mapTimes.size());
				monthHis.clear();
			}
		}
		else if(((tmDate.month()-1)/3)!=((iCurMonth-1)/3))
		{
			iCurMonth = tmDate.month();

			if(monthHis.size()>0)
			{
				mapTimes.insert(monthHis.first()->time,mapTimes.size());
				monthHis.clear();
			}
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//���ʣ���Ҳ����
		mapTimes.insert(monthHis.first()->time,mapTimes.size());
		monthHis.clear();
	}

	return 1;
}

int getYearMapByHistory(QMap<time_t,int>& mapTimes,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
	}

	QList<qRcvHistoryData*> monthHis;		//�����ڽ��й����Ժ����������
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			iCurYear = tmDate.year();
			if(monthHis.size()>0)
			{
				mapTimes.insert(monthHis.first()->time,mapTimes.size());
				monthHis.clear();
			}
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//���ʣ���Ҳ����
		mapTimes.insert(monthHis.first()->time,mapTimes.size());
		monthHis.clear();
	}

	return 1;
}




CCoordXBaseWidget::CCoordXBaseWidget(CBaseWidget* parent /*= 0*/, WidgetType type /*= CBaseWidget::Basic*/)
	: CBaseWidget(parent,type)
	, m_typeCircle(Min1)

{

}


CCoordXBaseWidget::~CCoordXBaseWidget(void)
{
}


void CCoordXBaseWidget::updateTimesH()
{
	//���µ�ǰ�ĺ��������ݣ��Ӻ���ǰ����ʱ��
	m_mapTimes.clear();

//	int iCount = 1024;				//����1024��ʱ��
	if(m_typeCircle<Day)
	{
		time_t tmCur = CDataEngine::getCurrentTime();
		time_t tmLast = ((tmCur/(3600*24))*3600*24)+3600*(9-8)+60*25;	//9��25����
		time_t tmCurrent = (tmCur+m_typeCircle)/m_typeCircle*m_typeCircle;//���϶Է���ȡ��
		time_t tmNoon1 = ((tmCur/(3600*24))*3600*24)+3600*(11-8)+60*30;
		time_t tmNoon2 = ((tmCur/(3600*24))*3600*24)+3600*(13-8);

		if((tmCurrent%(3600*24))>3600*7)
		{
			tmCurrent = (tmCurrent/(3600*24))*3600*24 + 3600*7 + m_typeCircle;		//3������(���һ������)
		}
		/*�����Ϻ����¶����һ������*/
		if(tmCurrent>tmNoon2)
		{
			time_t tmBegin = tmNoon2-m_typeCircle;
			//time_t tmEnd = tmCurrent+m_typeCircle*2;
			getTimeMapByMin(m_mapTimes,tmBegin,tmCurrent,m_typeCircle);
		}
		if(tmCurrent>tmNoon1)
		{
			time_t tmBegin = tmLast-m_typeCircle;
			time_t tmEnd = tmNoon1+m_typeCircle;
			getTimeMapByMin(m_mapTimes,tmBegin,tmEnd,m_typeCircle);
		}
	}
	else
	{
		if(m_typeCircle == Day)
		{
		}
		else if(m_typeCircle == DayN)
		{
			//Ŀǰδʹ��
		}
		else if(m_typeCircle == Week)
		{
		}
		else if(m_typeCircle == Month)
		{
		}
		else if(m_typeCircle == Month3)
		{
		}
		else if(m_typeCircle == Year)
		{
		}
	}
}


void CCoordXBaseWidget::drawCoordX(QPainter& p,const QRect& rtCoordX, float fGridSize)	//����X������
{
		//����������ƺ�����
	float fBeginX = rtCoordX.right();
	float fEndX = rtCoordX.left();
	float fCBWidth = fBeginX-fEndX;
	if(fCBWidth<0)
		return;

	QList<time_t> listTimes = m_mapTimes.keys();
	float fCurX = fBeginX;
	float fLastX = fCurX;
	int iCount = listTimes.size()-1;

	int iTimeCount = 0;				//ֻ����������ʱ�����ɫ��������ɫ�����ڲ鿴��
	while(fCurX>fEndX && iCount>=0)
	{
		if(m_typeCircle<Day)
		{
			if((fLastX-fCurX)>30)
			{
				p.setPen( iTimeCount%2 ? QColor(255,0,0) : QColor(0,255,255));
				p.drawLine(fCurX,rtCoordX.top(),fCurX,rtCoordX.top()+2);
				p.drawText(fCurX-14,rtCoordX.top()+2,30,rtCoordX.height()-2,
					Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("hh:mm"));
				fLastX = fCurX;
				++iTimeCount;
			}
		}

		--iCount;
		fCurX = fCurX-fGridSize;
	}
	return;
}