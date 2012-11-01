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
	pCurrentReport = new qRcvReportData;
	pLastReport = new qRcvReportData;
}

CStockInfoItem::~CStockInfoItem(void)
{
	delete pLastReport;
	delete pCurrentReport;
}

qRcvReportData* CStockInfoItem::getCurrentReport() const
{
	return pCurrentReport;
}

void CStockInfoItem::setReport( qRcvReportData* p )
{
	if(p->tmTime<=pCurrentReport->tmTime)
		return;
	pLastReport->resetItem(pCurrentReport);
	pCurrentReport->resetItem(p);
	
	updateItemInfo();
}

void CStockInfoItem::setReport( RCV_REPORT_STRUCTExV3* p )
{
	if(p->m_time<=pCurrentReport->tmTime)
		return;
	pLastReport->resetItem(pCurrentReport);
	pCurrentReport->resetItem(p);

	updateItemInfo();
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
			if(pCurrentReport)
			{
				time_t tmSeconds = CDataEngine::getOpenSeconds(pCurrentReport->tmTime);
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
				fVolumeRatio = (pCurrentReport->fVolume)/((fVolume5/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60));
			}
		}
	}

	emit stockInfoItemChanged(qsCode);
}

QList<qRcvMinuteData*> CStockInfoItem::getMinuteList()
{
	return mapMinutes.values();
}

void CStockInfoItem::appendMinutes( const QList<qRcvMinuteData*>& list )
{
	foreach(qRcvMinuteData* p,list)
	{
		if(mapMinutes.contains(p->tmTime))
		{
			qRcvMinuteData* pBefore = mapMinutes[p->tmTime];
			if(pBefore!=p)
				delete pBefore;
		}
		mapMinutes[p->tmTime] = p;
	}
}

QList<qRcvPowerData*> CStockInfoItem::getPowerList()
{
	return mapPowers.values();
}

void CStockInfoItem::appendPowers( const QList<qRcvPowerData*>& list )
{
	foreach(qRcvPowerData* p,list)
	{
		if(mapPowers.contains(p->tmTime))
		{
			qRcvPowerData* pBefore = mapPowers[p->tmTime];
			if(pBefore!=p)
				delete pBefore;
		}
		mapPowers[p->tmTime] = p;
	}
}

void CStockInfoItem::setBaseInfo( const qRcvBaseInfoData& info )
{
	memcpy(&baseInfo,&info,sizeof(qRcvBaseInfoData));


	//��ӯ��
	if(pCurrentReport&&baseInfo.fMgsy>0)
		fPERatio = pCurrentReport->fNewPrice/baseInfo.fMgsy/2;	//������2����ɺ������ݲ�ƥ��

	//��ͨ��ֵ
	if(pCurrentReport&&baseInfo.fLtAg>0)
		fLTSZ = baseInfo.fLtAg*pCurrentReport->fNewPrice;

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
	if(pCurrentReport)
		return pCurrentReport->fLastClose;
	return FLOAT_NAN;
}

float CStockInfoItem::getOpenPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fOpen;

	return FLOAT_NAN;
}

float CStockInfoItem::getHighPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fHigh;

	return FLOAT_NAN;
}

float CStockInfoItem::getLowPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fLow;

	return FLOAT_NAN;
}

float CStockInfoItem::getNewPrice() const
{
	if(pCurrentReport)
		return pCurrentReport->fNewPrice;

	return FLOAT_NAN;
}

float CStockInfoItem::getTotalVolume() const
{
	if(pCurrentReport)
		return pCurrentReport->fVolume;

	return FLOAT_NAN;
}

float CStockInfoItem::getTotalAmount() const
{
	if(pCurrentReport)
		return pCurrentReport->fAmount;

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

void CStockInfoItem::updateItemInfo()
{
//���ù�Ʊ����
	if(qsName.isEmpty())
		qsName = pCurrentReport->qsName;
	if(baseInfo.code[0]==0)
	{
		strcpy_s(baseInfo.code,qsCode.toLocal8Bit().data());
		baseInfo.wMarket = wMarket;
	}

	//�Ƿ�
	if(pCurrentReport->fNewPrice>0.0 && pCurrentReport->fLastClose>0.0)
		fIncrease = (pCurrentReport->fNewPrice-pCurrentReport->fLastClose)*100.0/pCurrentReport->fLastClose;
	/*�����ʣ��ֲ
		������=ĳһ��ʱ���ڵĳɽ���/�����ܹ���*100%
		�����й����ɽ���/��ͨ�ܹ���*100%��
	*/
	if(baseInfo.fZgb>0)
		fTurnRatio = pCurrentReport->fVolume/baseInfo.fZgb*100;

	//��ӯ��
	if(baseInfo.fMgsy>0)
		fPERatio = pCurrentReport->fNewPrice/baseInfo.fMgsy/2;	//������2����ɺ������ݲ�ƥ��
	//��ͨ��ֵ
	if(baseInfo.fLtAg>0)
		fLTSZ = baseInfo.fLtAg*pCurrentReport->fNewPrice;

	//�ǵ����۸񲨶�
	fPriceFluctuate = (pCurrentReport->fNewPrice-pCurrentReport->fLastClose);

	//���
	fAmplitude = (pCurrentReport->fHigh-pCurrentReport->fLow)/pCurrentReport->fLastClose;

	//����
	fAvePrice = (pCurrentReport->fAmount/pCurrentReport->fVolume)/100;

	{
		//ί��������
		fBuyVolume = 0.0;
		fBuyVolume += pCurrentReport->fBuyVolume[0];
		fBuyVolume += pCurrentReport->fBuyVolume[1];
		fBuyVolume += pCurrentReport->fBuyVolume[2];
		fBuyVolume += pCurrentReport->fBuyVolume4;
		fBuyVolume += pCurrentReport->fBuyVolume5;

		//ί��������
		fSellVolume = 0.0;
		fSellVolume += pCurrentReport->fSellVolume[0];
		fSellVolume += pCurrentReport->fSellVolume[1];
		fSellVolume += pCurrentReport->fSellVolume[2];
		fSellVolume += pCurrentReport->fSellVolume4;
		fSellVolume += pCurrentReport->fSellVolume5;

		//ί��
		if(pCurrentReport&&(fBuyVolume>0||fSellVolume>0))
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

	if(pLastReport->fNewPrice <= 0.0)
	{
		//�����̼���
		if(pCurrentReport->fNewPrice>=pCurrentReport->fLastClose)
			fSellVOL = pCurrentReport->fVolume;
		else
			fBuyVOL = pCurrentReport->fVolume;
	}
	else
	{
		//����Report�ĶԱȼ���
		fNowVolume = (pCurrentReport->fVolume)-(pLastReport->fVolume);
		fIncreaseSpeed = (pCurrentReport->fNewPrice-pLastReport->fNewPrice)/pLastReport->fNewPrice;

		//�����̼���
		if(pCurrentReport->fNewPrice>=pLastReport->fNewPrice)
			fSellVOL += fNowVolume;
		else
			fBuyVOL += fNowVolume;
	}

	emit stockInfoItemChanged(qsCode);
}
