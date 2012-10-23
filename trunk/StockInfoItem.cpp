#include "StdAfx.h"
#include "StockInfoItem.h"
#include "DataEngine.h"
#define	FLOAT_NAN	(std::numeric_limits<float>::quiet_NaN())

CStockInfoItem::CStockInfoItem( const QString& code, WORD market )
	: qsCode(code)
	, wMarket(market)
	, fNowVolume(FLOAT_NAN)
	, pLastReport(NULL)
	, fIncreaseSpeed(FLOAT_NAN)
	, fBuyVolume(FLOAT_NAN)
	, fSellVolume(FLOAT_NAN)
	, fIncrease(FLOAT_NAN)
	, fVolumeRatio(FLOAT_NAN)
	, fTurnRatio(FLOAT_NAN)
	, fPriceFluctuate(FLOAT_NAN)
	, fAmplitude(FLOAT_NAN)
	, fAvePrice(FLOAT_NAN)
	, fCommRatio(FLOAT_NAN)
	, fCommSent(FLOAT_NAN)
	, fPERatio(FLOAT_NAN)
	, fLTSZ(FLOAT_NAN)
	, fSellVOL(0.0)
	, fBuyVOL(0.0)
{

}

CStockInfoItem::CStockInfoItem( const qRcvBaseInfoData& info )
	: qsCode(QString::fromLocal8Bit(info.code))
	, wMarket(info.wMarket)
	, fNowVolume(FLOAT_NAN)
	, pLastReport(NULL)
	, fIncreaseSpeed(FLOAT_NAN)
	, fBuyVolume(FLOAT_NAN)
	, fSellVolume(FLOAT_NAN)
	, fIncrease(FLOAT_NAN)
	, fVolumeRatio(FLOAT_NAN)
	, fTurnRatio(FLOAT_NAN)
	, fPriceFluctuate(FLOAT_NAN)
	, fAmplitude(FLOAT_NAN)
	, fAvePrice(FLOAT_NAN)
	, fCommRatio(FLOAT_NAN)
	, fCommSent(FLOAT_NAN)
	, fPERatio(FLOAT_NAN)
	, fLTSZ(FLOAT_NAN)
	, fSellVOL(0.0)
	, fBuyVOL(0.0)
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

	
	//���ù�Ʊ����
	if(qsName.isEmpty())
		qsName = p->qsName;
	//�Ƿ�
	if(pLastReport->fNewPrice>0.0 && pLastReport->fLastClose>0.0)
		fIncrease = (pLastReport->fNewPrice-pLastReport->fLastClose)*100.0/pLastReport->fLastClose;
	/*�����ʣ��ֲ
		������=ĳһ��ʱ���ڵĳɽ���/�����ܹ���*100%
		�����й����ɽ���/��ͨ�ܹ���*100%��
	*/
	if(baseInfo.fZgb>0)
		fTurnRatio = pLastReport->fVolume/baseInfo.fZgb*100;

	//��ӯ��
	if(baseInfo.fMgsy>0)
		fPERatio = pLastReport->fNewPrice/baseInfo.fMgsy/2;	//������2����ɺ������ݲ�ƥ��
	//��ͨ��ֵ
	if(baseInfo.fLtAg>0)
		fLTSZ = baseInfo.fLtAg*pLastReport->fNewPrice;

	//�ǵ����۸񲨶�
	fPriceFluctuate = (pLastReport->fNewPrice-pLastReport->fLastClose);

	//���
	fAmplitude = (pLastReport->fHigh-pLastReport->fLow)/pLastReport->fLastClose;

	//����
	fAvePrice = (pLastReport->fAmount/pLastReport->fVolume)/100;

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

		//ί��
		if(pLastReport&&(fBuyVolume>0||fSellVolume>0))
		{
			fCommRatio = ((fBuyVolume-fSellVolume)/(fBuyVolume+fSellVolume))*100;
			fCommSent = fBuyVolume-fSellVolume;
		}
		else
		{
			fCommRatio = FLOAT_NAN;
			fCommSent = FLOAT_NAN;
		}
	}


	if(mapReports.size()<2)
	{

		//������/������
		if(pLastReport->fNewPrice>=pLastReport->fLastClose)
			fSellVOL = pLastReport->fVolume;
		else
			fBuyVOL = pLastReport->fVolume;
	}
	if(mapReports.size()>1)
	{
		qRcvReportData* pReport2 = (mapReports.end()-2).value();
		//����Report�ĶԱȼ���
		fNowVolume = (pLastReport->fVolume)-(pReport2->fVolume);
		fIncreaseSpeed = (pLastReport->fNewPrice-pReport2->fNewPrice)/pReport2->fNewPrice;

		//�����̼���
		if(pLastReport->fNewPrice>=pReport2->fNewPrice)
			fSellVOL += fNowVolume;
		else
			fBuyVOL += fNowVolume;
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
	{
		/*���ȼ��㣺
			���ȣ��ֳɽ�����/����ȥ5��ƽ��ÿ���ӳɽ���*�����ۼƿ���ʱ�䣨�֣��� 
			�����ȴ���1ʱ��˵������ÿ���ӵ�ƽ���ɽ���Ҫ���ڹ�ȥ5�յ�ƽ����ֵ�����ױȹ�ȥ5�ջ𱬣�
			��������С��1ʱ��˵�����ڵĳɽ��Ȳ��Ϲ�ȥ5�յ�ƽ��ˮƽ��
		*/
		if(mapHistorys.size()>5)
		{
			//�ж����µ������Ƿ��ǽ��쿪�к������
			if(pLastReport)
			{
				time_t tmSeconds = CDataEngine::getOpenSeconds(pLastReport->tmTime);
				if(tmSeconds<1)
					return;

				time_t* pLast5Day = CDataEngine::getLast5DayTime();
				float fVolume5 = 0.0;
				for(int i=0;i<5;++i)
				{
					if(!mapHistorys.contains(pLast5Day[i]))
						return;
					fVolume5 = (fVolume5 + mapHistorys.value(pLast5Day[i])->fVolume);
				}
				fVolumeRatio = (pLastReport->fVolume)/((fVolume5/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60));
			}
		}
	}

	emit stockInfoItemChanged(qsCode);
}

void CStockInfoItem::setBaseInfo( const qRcvBaseInfoData& info )
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));


	//��ӯ��
	if(pLastReport&&baseInfo.fMgsy>0)
		fPERatio = pLastReport->fNewPrice/baseInfo.fMgsy/2;	//������2����ɺ������ݲ�ƥ��

	//��ͨ��ֵ
	if(pLastReport&&baseInfo.fLtAg>0)
		fLTSZ = baseInfo.fLtAg*pLastReport->fNewPrice;

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

	return qsName;
}

float CStockInfoItem::getIncrease() const
{
	return fIncrease;
}

float CStockInfoItem::getVolumeRatio() const
{
	return fVolumeRatio;
}

float CStockInfoItem::getTurnRatio() const
{
	return fTurnRatio;
}

float CStockInfoItem::getLastClose() const
{
	if(pLastReport)
		return pLastReport->fLastClose;
	return FLOAT_NAN;
}

float CStockInfoItem::getOpenPrice() const
{
	if(pLastReport)
		return pLastReport->fOpen;

	return FLOAT_NAN;
}

float CStockInfoItem::getHighPrice() const
{
	if(pLastReport)
		return pLastReport->fHigh;

	return FLOAT_NAN;
}

float CStockInfoItem::getLowPrice() const
{
	if(pLastReport)
		return pLastReport->fLow;

	return FLOAT_NAN;
}

float CStockInfoItem::getNewPrice() const
{
	if(pLastReport)
		return pLastReport->fNewPrice;

	return FLOAT_NAN;
}

float CStockInfoItem::getTotalVolume() const
{
	if(pLastReport)
		return pLastReport->fVolume;

	return FLOAT_NAN;
}

float CStockInfoItem::getTotalAmount() const
{
	if(pLastReport)
		return pLastReport->fAmount;

	return FLOAT_NAN;
}

float CStockInfoItem::getNowVolume() const
{
	return fNowVolume;
}

float CStockInfoItem::getIncSpeed() const
{
	//���磬ĳ����Ʊ5����֮ǰ�Ĺɼ���10Ԫ�������ڵļ۸���10.1Ԫ���������Ʊ��5��������Ϊ��
	return fIncreaseSpeed;
}

float CStockInfoItem::getPriceFluctuate() const
{
	//�����ǵ�����%��=����ǰ��-ǰ���̼ۣ�/ ǰ���̼� * 100
	return fPriceFluctuate;
}

float CStockInfoItem::getAmplitude() const
{
	/*
		һ����Ʊ���=������߼�/������ͼۡ�100%-100%
		������Ʊ���=��������߼ۣ�������ͼ�)/�������̼ۡ�100%
		���ݷɺ������õڶ��ּ��㷽����
	*/
	return fAmplitude;
}

float CStockInfoItem::getAvePrice() const
{
	return fAvePrice;
}

float CStockInfoItem::getPERatio() const
{
	//��ӯ�ʣ���̬��ӯ�ʣ�=���¼ۣ�ÿ������
	return fPERatio;
}

float CStockInfoItem::getLTSZ() const
{
	//��ͨ��ֵ ����ֵ������Ŀǰ��Ʊ�м۳����ܹɱ�����ֵͨ����Ŀǰ��Ʊ�м۳�����[1]ͨ�ɱ���
	return fLTSZ;
}

float CStockInfoItem::getSellVOL() const
{
	//������
	return fSellVOL;
}

float CStockInfoItem::getBuyVOL() const
{
	//������
	return fBuyVOL;
}

float CStockInfoItem::getBIDVOL() const
{
	//ί����
	return fBuyVolume;
}

float CStockInfoItem::getASKVOL() const
{
	//ί����
	return fSellVolume;
}

float CStockInfoItem::getCommRatio() const
{
	//ί��
	//(ί��������ί������)/(ί������+ί������)*100
	return fCommRatio;
}

float CStockInfoItem::getCommSent() const
{
	//ί��
	return fCommSent;
}
