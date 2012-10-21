#pragma once
#include <QtCore>

struct qRcvHistoryData
{
	time_t	time;				//UCT
	float	fOpen;			//����
	float	fHigh;			//���
	float	fLow;				//���
	float	fClose;			//����
	float	fVolume;			//��
	float	fAmount;			//��
	WORD	wAdvance;			//����,��������Ч
	WORD	wDecline;			//����,��������Ч

	qRcvHistoryData(RCV_HISTORY_STRUCTEx* p)
	{
		memcpy(&time,&p->m_time,sizeof(qRcvHistoryData));
	}
};

struct qRcvReportData
{
	QDateTime	tmTime;			//�ɽ�ʱ��
	DWORD		wMarket;		//��Ʊ�г�����
	QString		qsCode;			//��Ʊ����
	QString		qsName;			//��Ʊ����

	float	fLastClose;			//����
	float	fOpen;				//��
	float	fHigh;				//���
	float	fLow;				//���
	float	fNewPrice;			//����
	float	fVolume;			//�ɽ���
	float	fAmount;			//�ɽ���
	float	fBuyPrice[3];		//�����1,2,3
	float	fBuyVolume[3];		//������1,2,3
	float	fSellPrice[3];		//������1,2,3
	float	fSellVolume[3];		//������1,2,3

	float	fBuyPrice4;			//�����4
	float	fBuyVolume4;		//������4
	float	fSellPrice4;		//������4
	float	fSellVolume4;		//������4

	float	fBuyPrice5;			//�����5
	float	fBuyVolume5;		//������5
	float	fSellPrice5;		//������5
	float	fSellVolume5;		//������5

	QMap<time_t,qRcvHistoryData> mapHistorys;

	qRcvReportData(RCV_REPORT_STRUCTExV3* p)
	{
		tmTime = QDateTime::fromTime_t(p->m_time);
		wMarket = p->m_wMarket;
		qsCode = QString::fromLocal8Bit(p->m_szLabel);
		qsName = QString::fromLocal8Bit(p->m_szName);

		//ֱ�ӿ���ʣ�������float����
		memcpy(&fLastClose,&p->m_fLastClose,sizeof(float)*27);
/*		fLastClose = p->m_fLastClose;
		fOpen = p->m_fOpen;				//��
		fHigh = p->m_fHigh;				//���
		fLow = p->m_fLow;				//���
		fNewPrice = p->m_fNewPrice;		//����
		fVolume = p->m_fVolume;			//�ɽ���
		fAmount = p->m_fAmount;			//�ɽ���

		memcpy(fBuyPrice,p->m_fBuyPrice,sizeof(float)*3);	//�����1,2,3
		memcpy(fBuyVolume,p->m_fBuyVolume,sizeof(float)*3);	//������1,2,3
		memcpy(fSellPrice,p->m_fSellPrice,sizeof(float)*3);	//������1,2,3
		memcpy(fSellVolume,p->m_fSellVolume,sizeof(float)*3);	//������1,2,3

		fBuyPrice4;			//�����4
		fBuyVolume4;		//������4
		fSellPrice4;		//������4
		fSellVolume4;		//������4

		fBuyPrice5;			//�����5
		fBuyVolume5;		//������5
		fSellPrice5;		//������5
		fSellVolume5;		//������5
		*/
	}

	void resetItem(RCV_REPORT_STRUCTExV3* p)
	{
		tmTime = QDateTime::fromTime_t(p->m_time);
		wMarket = p->m_wMarket;
		qsCode = QString::fromLocal8Bit(p->m_szLabel);
		qsName = QString::fromLocal8Bit(p->m_szName);

		//ֱ�ӿ���ʣ�������float����
		memcpy(&fLastClose,&p->m_fLastClose,sizeof(float)*27);
	}
};

class CDataEngine : public QObject
{
	Q_OBJECT
public:
	CDataEngine(void);
	~CDataEngine(void);
	static CDataEngine* getDataEngine()
	{
		if(m_pDataEngine == NULL)
			m_pDataEngine = new CDataEngine;
		return m_pDataEngine;
	}

public:
	//���³�ʼ�����й�Ʊ����
	void initAllReport();

	//��ȡ������������
	QList<qRcvReportData*> getBaseMarket()
	{
		return m_mapBaseMarket.values();
	}
	qRcvReportData* getBaseMarket(const QString& qsCode)
	{
		if(m_mapBaseMarket.contains(qsCode))
		{
			return m_mapBaseMarket[qsCode];
		}
		return NULL;
	}
	void setBaseMarket(qRcvReportData* p)
	{
		m_mapBaseMarket[p->qsCode] = p;
	}

	//������������
	bool appendHistory(const QString& code, const qRcvHistoryData& p)
	{
		if(!m_mapBaseMarket.contains(code))
			return false;
		qRcvReportData* pData = m_mapBaseMarket[code];
		pData->mapHistorys.insert(p.time,p);

		return true;
	}

private:
	QMap<QString,qRcvReportData*> m_mapBaseMarket;

private:
	static CDataEngine* m_pDataEngine;
};

