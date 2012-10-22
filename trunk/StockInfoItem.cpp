#include "StdAfx.h"
#include "StockInfoItem.h"
#include "DataEngine.h"

CStockInfoItem::CStockInfoItem( const QString& code, WORD market )
	: qsCode(code)
	, wMarket(market)
{

}

CStockInfoItem::CStockInfoItem( const qRcvBaseInfoData& info )
	: qsCode(QString::fromLocal8Bit(info.code))
	, wMarket(info.wMarket)
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));
}

CStockInfoItem::~CStockInfoItem(void)
{
}

void CStockInfoItem::appendReport( qRcvReportData* p )
{
	if(mapReports.contains(p->tmTime))
	{
		qRcvReportData* pBefore = mapReports[p->tmTime];
		if(pBefore!=p)
			delete pBefore;
	}
	mapReports[p->tmTime] = p;
}

qRcvReportData* CStockInfoItem::getLastReport() const
{
	if(mapReports.size()<1)
		return NULL;

	return (mapReports.end()-1).value();
}

QList<qRcvHistoryData*> CStockInfoItem::getHistoryList()
{
	return mapHistorys.values();
}

void CStockInfoItem::appendHistorys( const QList<qRcvHistoryData*>& list )
{
	foreach(qRcvHistoryData* p,list)
	{
		if(mapHistorys.contains(p->time))
		{
			qRcvHistoryData* pBefore = mapHistorys[p->time];
			if(pBefore!=p)
				delete pBefore;
		}
		mapHistorys[p->time] = p;
	}
}

void CStockInfoItem::setBaseInfo( const qRcvBaseInfoData& info )
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));
}


QString CStockInfoItem::getCode() const
{
	return qsCode;
}

WORD CStockInfoItem::getMarket() const
{
	return wMarket;
}

QString CStockInfoItem::getName() const
{
	if(mapReports.size()>0)
		return (*mapReports.begin())->qsName;

	return QString();
}

QString CStockInfoItem::getIncrease() const
{
	qRcvReportData* pReport = this->getLastReport();
	if(pReport)
	{
		if(pReport->fNewPrice<=0.0 || pReport->fLastClose<=0.0)
			return QString();

		return QString("%1%").arg((pReport->fNewPrice-pReport->fLastClose)*100.0/pReport->fLastClose,0,'f',2);
	}
	else
	{
		return QString();
	}
}

QString CStockInfoItem::getVolumeRatio() const
{
	/*���ȼ��㣺
		���ȣ��ֳɽ�����/����ȥ5��ƽ��ÿ���ӳɽ���*�����ۼƿ���ʱ�䣨�֣��� 
		�����ȴ���1ʱ��˵������ÿ���ӵ�ƽ���ɽ���Ҫ���ڹ�ȥ5�յ�ƽ����ֵ�����ױȹ�ȥ5�ջ𱬣�
		��������С��1ʱ��˵�����ڵĳɽ��Ȳ��Ϲ�ȥ5�յ�ƽ��ˮƽ��
	*/
	if(mapHistorys.size()<5)
		return QString();

	//�ж����µ������Ƿ��ǽ��쿪�к������
	qRcvReportData* pReport = getLastReport();
	if(!pReport)
		return QString();

	time_t tmSeconds = CDataEngine::getOpenSeconds(pReport->tmTime);
	if(tmSeconds<1)
		return QString();

	time_t* pLast5Day = CDataEngine::getLast5DayTime();
	float fVolume5 = 0.0;
	for(int i=0;i<5;++i)
	{
		if(!mapHistorys.contains(pLast5Day[i]))
			return QString();
		fVolume5 = (fVolume5 + mapHistorys.value(pLast5Day[i])->fVolume);
	}

	return QString("%1").arg((pReport->fVolume)/((fVolume5/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60)));
}

QString CStockInfoItem::getTurnRatio() const
{
	/*�����ʣ��ֲ
		������=ĳһ��ʱ���ڵĳɽ���/�����ܹ���*100%
		�����й����ɽ���/��ͨ�ܹ���*100%��
	*/
	return QString();
}

QString CStockInfoItem::getLastClose() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fLastClose,0,'f',2);

	return QString();
}

QString CStockInfoItem::getOpenPrice() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fOpen,0,'f',2);

	return QString();
}

QString CStockInfoItem::getHighPrice() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fHigh,0,'f',2);

	return QString();
}

QString CStockInfoItem::getLowPrice() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fLow,0,'f',2);

	return QString();
}

QString CStockInfoItem::getNewPrice() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fNewPrice,0,'f',2);

	return QString();
}

QString CStockInfoItem::getTotalVolume() const
{
	qRcvReportData* pReport = getLastReport();
	if(pReport)
		return QString("%1").arg(pReport->fVolume,0,'f',0);

	return QString();
}
