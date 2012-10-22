#ifndef STOCK_INFO_ITEM_H
#define STOCK_INFO_ITEM_H
#include <QtCore>


struct qRcvHistoryData
{
	time_t	time;			//UCT
	float	fOpen;			//����
	float	fHigh;			//���
	float	fLow;				//���
	float	fClose;			//����
	float	fVolume;			//��
	float	fAmount;			//��
	WORD	wAdvance;			//����,��������Ч
	WORD	wDecline;			//����,��������Ч

	qRcvHistoryData()
	{

	}
	qRcvHistoryData(RCV_HISTORY_STRUCTEx* p)
	{
		memcpy(&time,&p->m_time,sizeof(qRcvHistoryData));
	}
};

struct qRcvReportData
{
	time_t	tmTime;			//�ɽ�ʱ��
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

	qRcvReportData(RCV_REPORT_STRUCTExV3* p)
	{
		tmTime = p->m_time;
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
		tmTime = p->m_time;
		wMarket = p->m_wMarket;
		qsCode = QString::fromLocal8Bit(p->m_szLabel);
		qsName = QString::fromLocal8Bit(p->m_szName);

		//ֱ�ӿ���ʣ�������float����
		memcpy(&fLastClose,&p->m_fLastClose,sizeof(float)*27);
	}
};


struct qRcvBaseInfoData
{
	QString qsCode;
	WORD wMarket;

	float fZgb;                    //�ܹɱ�(���)			0
	float fGjg;                    //���ҹ�(���)
	float fFqrfrg;                  //�����˷��˹�(���)
	float fFrg;                    //���˹�(���)
	float fZgg;                   //ְ����(���)
	float fGzAg;                  //����A��(���)			5
	float fLtAg;                  //��ͨA��(���)
	float fHg;                    //�ȹ�(���)
	float fBg;                    //B��(���)
	float fZpg;                   //ת���(���)
	float fZzc;                   //���ʲ�(��Ԫ)			10
	float fLdzc;                  //�����ʲ�(��Ԫ)
	float fCqtz;                  //����Ͷ��(��Ԫ)
	float fGdzc;                  //�̶��ʲ�(��Ԫ)
	float fWxzc;                 //�����ʲ�(��Ԫ)
	float fLdfz;                  //������ծ(��Ԫ)		15
	float fCqfz;                 //���ڸ�ծ(��Ԫ)
	float fGgqy;                 //�ɶ�Ȩ��(��Ԫ)
	float fZbgjj;                 //�ʱ�������(��Ԫ)
	float fYygjj;                 //ӯ�๫����(��Ԫ)
	float fMgjz;                 //ÿ�ɾ�ֵ(Ԫ)			20
	float fGdqybl;               //�ɶ�Ȩ�����(%)
	float fMggjj;                //ÿ�ɹ�����(Ԫ)
	float fZyywsr;               //��Ӫҵ������(��Ԫ)
	float fZyywlr;               //��Ӫҵ������(��Ԫ)
	float fQtywlr;               //����ҵ������(��Ԫ)		25
	float fLrze;                 //�����ܶ�(��Ԫ)
	float fJlr;                  //������(��Ԫ)
	float fWfplr;               //δ��������(��Ԫ)
	float fMgsy;                //ÿ������(Ԫ)
	float fJzcsyl;                //���ʲ�������(%)		30
	float fMgwfplr;              //ÿ��δ��������(Ԫ)
	float fMgjzc;                //ÿ�ɾ��ʲ�(Ԫ)			32

	qRcvBaseInfoData()
	{
		qsCode = "";
		wMarket = 0x0000;
		memset(&fZgb,0,sizeof(float)*33);
	}

	qRcvBaseInfoData( float* fVal )
	{
		fVal = fVal+1;
		fZgb = fVal[0];
		fGjg = fVal[1];
		fFqrfrg = fVal[2];
		fFrg = fVal[3];
		fBg = fVal[4];

		fHg = fVal[5];
		fLtAg = fVal[6];
		fZgg = fVal[7];

		fZzc = fVal[9]/10;

		fLdzc = fVal[10]/10;
		fGdzc = fVal[11]/10;
		fWxzc = fVal[12]/10;
		fCqtz = fVal[13]/10;
		fLdfz = fVal[14]/10;

		fCqfz = fVal[15]/10;
		fZbgjj = fVal[16]/10;
		fMggjj = fVal[17];
		fGgqy = fVal[18]/10;
		fZyywsr = fVal[19]/10;

		fZyywlr = fVal[20]/10;		//?
		fQtywlr = fVal[21]/10;
		fZyywlr = fVal[22]/10;		//?

		fLrze = fVal[27]/10;

		fJlr = fVal[29]/10;

		fWfplr = fVal[30]/10;
		fMgwfplr  = fVal[31];
		fMgsy = fVal[32];
		fMgjzc = fVal[33];

		fGdqybl  = fVal[35];
		fJzcsyl = fVal[36];
	}
};


class CStockInfoItem : public QObject
{
	Q_OBJECT
public:
	CStockInfoItem(const QString& code, WORD market);
	~CStockInfoItem(void);

public:
	//����Report����
	void appendReport(qRcvReportData* p);

	//������������
	QList<qRcvHistoryData*> getHistoryList();
	void appendHistorys(const QList<qRcvHistoryData*>& list);

public:
	/*�������ֶΣ�ֻ��*/
	QString getCode() const;		//��Ʊ����
	WORD getMarket() const;			//��Ʊ�г�
	QString getName() const;		//��Ʊ����

private:
	QString qsCode;
	WORD wMarket;

private:
	QMap<time_t,qRcvReportData*> mapReports;
	QMap<time_t,qRcvHistoryData*> mapHistorys;
};

#endif	//STOCK_INFO_ITEM_H