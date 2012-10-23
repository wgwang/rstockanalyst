#include "StdAfx.h"
#include "StockInfoItem.h"
#include "DataEngine.h"

CStockInfoItem::CStockInfoItem( const QString& code, WORD market )
	: qsCode(code)
	, wMarket(market)
	, fNowVolume(NULL)
	, pLastReport(NULL)
	, fIncreaseSpeed(NULL)
	, fBuyVolume(NULL)
	, fSellVolume(NULL)
{

}

CStockInfoItem::CStockInfoItem( const qRcvBaseInfoData& info )
	: qsCode(QString::fromLocal8Bit(info.code))
	, wMarket(info.wMarket)
	, fNowVolume(NULL)
	, pLastReport(NULL)
	, fIncreaseSpeed(NULL)
	, fBuyVolume(NULL)
	, fSellVolume(NULL)
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
	pLastReport = (mapReports.end()-1).value();

	{
		//ί��������
		fBuyVolume = 0.0;
		fBuyVolume += pLastReport->fBuyVolume[0];
		fBuyVolume += pLastReport->fBuyVolume[1];
		fBuyVolume += pLastReport->fBuyVolume[2];
		fBuyVolume += pLastReport->fBuyVolume4;
		fBuyVolume += pLastReport->fBuyVolume5;

		//ί��������
		fSellVolume = 0.0;
		fSellVolume += pLastReport->fSellVolume[0];
		fSellVolume += pLastReport->fSellVolume[1];
		fSellVolume += pLastReport->fSellVolume[2];
		fSellVolume += pLastReport->fSellVolume4;
		fSellVolume += pLastReport->fSellVolume5;
	}

	if(mapReports.size()>2)
	{
		//����Report�ĶԱȼ���
		qRcvReportData* pReport2 = (mapReports.end()-2).value();
		fNowVolume = (pLastReport->fVolume)-(pReport2->fVolume);
		fIncreaseSpeed = (pLastReport->fNewPrice-pReport2->fNewPrice)/pReport2->fNewPrice;
	}

	emit stockInfoItemChanged(qsCode);
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

	emit stockInfoItemChanged(qsCode);
}

void CStockInfoItem::setBaseInfo( const qRcvBaseInfoData& info )
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));

	emit stockInfoItemChanged(qsCode);
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
	if(pLastReport)
		return pLastReport->qsName;

	return QString();
}

QString CStockInfoItem::getIncrease() const
{
	if(pLastReport)
	{
		if(pLastReport->fNewPrice<=0.0 || pLastReport->fLastClose<=0.0)
			return QString();

		return QString("%1%").arg((pLastReport->fNewPrice-pLastReport->fLastClose)*100.0/pLastReport->fLastClose,0,'f',2);
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
	if(!pLastReport)
		return QString();

	time_t tmSeconds = CDataEngine::getOpenSeconds(pLastReport->tmTime);
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

	return QString("%1").arg((pLastReport->fVolume)/((fVolume5/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60)),0,'f',2);
}

QString CStockInfoItem::getTurnRatio() const
{
	/*�����ʣ��ֲ
		������=ĳһ��ʱ���ڵĳɽ���/�����ܹ���*100%
		�����й����ɽ���/��ͨ�ܹ���*100%��
	*/
	if(baseInfo.fZgb>0&&pLastReport)
	{
		return QString("%1%").arg(pLastReport->fVolume/baseInfo.fZgb*100,0,'f',2);
	}
	return QString();
}

QString CStockInfoItem::getLastClose() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fLastClose,0,'f',2);

	return QString();
}

QString CStockInfoItem::getOpenPrice() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fOpen,0,'f',2);

	return QString();
}

QString CStockInfoItem::getHighPrice() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fHigh,0,'f',2);

	return QString();
}

QString CStockInfoItem::getLowPrice() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fLow,0,'f',2);

	return QString();
}

QString CStockInfoItem::getNewPrice() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fNewPrice,0,'f',2);

	return QString();
}

QString CStockInfoItem::getTotalVolume() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fVolume,0,'f',0);

	return QString();
}

QString CStockInfoItem::getTotalAmount() const
{
	if(pLastReport)
		return QString("%1").arg(pLastReport->fAmount/10000,0,'f',0);

	return QString();
}

QString CStockInfoItem::getNowVolume() const
{
	return QString("%1").arg(fNowVolume,0,'f',0);
}

QString CStockInfoItem::getIncSpeed() const
{
	//���磬ĳ����Ʊ5����֮ǰ�Ĺɼ���10Ԫ�������ڵļ۸���10.1Ԫ���������Ʊ��5��������Ϊ��
	if(pLastReport)
		return QString("%1%").arg(fIncreaseSpeed*100,0,'f',2);

	return QString();
}

QString CStockInfoItem::getPriceFluctuate() const
{
	//�����ǵ�����%��=����ǰ��-ǰ���̼ۣ�/ ǰ���̼� * 100
	if(pLastReport)
		return QString("%1%").arg((pLastReport->fNewPrice-pLastReport->fLastClose)/pLastReport->fLastClose*100,0,'f',2);

	return QString();
}

QString CStockInfoItem::getAmplitude() const
{
	/*
		һ����Ʊ���=������߼�/������ͼۡ�100%-100%
		������Ʊ���=��������߼ۣ�������ͼ�)/�������̼ۡ�100%
		���ݷɺ������õڶ��ּ��㷽����
	*/
	if(pLastReport)
		return QString("%1%").arg((pLastReport->fHigh-pLastReport->fLow)/pLastReport->fLastClose*100,0,'f',2);

	return QString();
}

QString CStockInfoItem::getAvePrice() const
{
	if(pLastReport)
		return QString("%1").arg((pLastReport->fAmount/pLastReport->fVolume)/100,0,'f',2);
	return QString();
}

QString CStockInfoItem::getPERatio() const
{
	//��ӯ�ʣ���̬��ӯ�ʣ�=��ͨ��ÿ���г��۸����ͨ��ÿ��ÿ��ӯ��

	return QString("UnKown");
}

QString CStockInfoItem::getLTSZ() const
{
	//��ͨ��ֵ ����ֵ������Ŀǰ��Ʊ�м۳����ܹɱ�����ֵͨ����Ŀǰ��Ʊ�м۳�����[1]ͨ�ɱ���
	if(pLastReport&&(baseInfo.fLtAg>0))
		return QString("%1").arg(baseInfo.fLtAg*pLastReport->fNewPrice,0,'f',0);
	return QString();
}

QString CStockInfoItem::getSellVOL() const
{
	//������

	return QString("UnKown");
}

QString CStockInfoItem::getBuyVOL() const
{
	//������

	return QString("UnKown");
}

QString CStockInfoItem::getBIDVOL() const
{
	//ί����
	if(pLastReport)
	{
		return QString("%1").arg(fBuyVolume,0,'f',0);
	}

	return QString();
}

QString CStockInfoItem::getASKVOL() const
{
	//ί����
	if(pLastReport)
	{
		return QString("%1").arg(fSellVolume,0,'f',0);
	}

	return QString();
}

QString CStockInfoItem::getCommRatio() const
{
	//ί��
	//(ί��������ί������)/(ί������+ί������)*100
	if(pLastReport)
		return QString("%1%").arg(((fBuyVolume-fSellVolume)/(fBuyVolume+fSellVolume))*100,0,'f',2);

	return QString("UnKown");
}

QString CStockInfoItem::getCommSent() const
{
	//ί��
	if(pLastReport)
		return QString("%1").arg(fBuyVolume-fSellVolume,0,'f',0);

	return QString("UnKown");
}
