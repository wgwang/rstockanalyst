#ifndef DATA_ENGINE_H
#define DATA_ENGINE_H

#include "StockInfoItem.h"
#include "BlockInfoItem.h"

#define	R_TIME_ZONE	8

class CDataEngine : public QObject
{
	Q_OBJECT
public:
	CDataEngine(void);
	~CDataEngine(void);
	static CDataEngine* getDataEngine();
	static void releaseDataEngine();

public:
	//��������ʱ�����ݵĳ�ʼ��
	static void importData();
	//�����˳�ʱ�����ݵ��Զ�����
	static void exportData();


	/*���ݵ��뺯��*/
	//F10���ݵ���,����ֵΪ������������������Ӻ�׺��Ϊ.fin���ļ����롣
	static int importBaseInfoFromFinFile(const QString& qsFile);
	//F10���ݵ��룬�ӱ�Ӧ��֧�ֵ����ݵ��룬��׺��Ϊ .rsafin
	static int importBaseInfo(const QString& qsFile);
	/*����Reports����*/
	static int importReportsInfo(const QString& qsFile);
	/*����ֱ�����*/
	static int importFenBisData(const QString& qsFile);
	/*����������*/
	static int importBlocksData(const QString& qsPath);



	/*���ݵ�������*/
	/*���������������ݣ�F10����*/
	static int exportBaseInfo(const QString& qsFile);
	/*����Reports����*/
	static int exportReportsInfo(const QString& qsFile);
	/*�����ֱ�����*/
	static int exportFenBisData(const QString& qsFile);

public:
	static bool isStockOpenDay(time_t tmDay);	//�ж�tmDay�Ƿ��У�ĳ������ڣ�����ʱ�䣩
	static bool isStockOpenTime(time_t tmMin);	//�ж�tmMin�Ƿ��У�ĳ���ʱ�䣬�������ڣ�
	static time_t getOpenSeconds();				//��ȡÿ��Ŀ���ʱ�䣨�룩��һ��Ϊ4Сʱ
	static time_t getOpenSeconds(time_t tmTime);//��ȡ���������tmTime�Ŀ���ʱ�䣨�룩
	static time_t getCurrentTime();				//��ȡ���һ��report���ݵ�ʱ��
	static void setCurrentTime(const time_t& t);//�������һ��report���ݵ�ʱ��

public:
	QString getStockBlockDir() const{ return m_qsBlocksDir; }
	QString getCommStockBlockFile() const{ return m_qsCommonBlocks; }

public:
	//��ȡ�������
	QList<CBlockInfoItem*> getStockBlocks();	//��ȡ���а���б�
	CBlockInfoItem* getStockBlock(const QString& block);		//ͨ��������ƻ�ȡ���
	bool isHadBlock(const QString& block);		//�Ƿ����ĳ���
	void setBlockInfoItem(CBlockInfoItem* _p);						//���ð������
	QList<CStockInfoItem*> getStocksByMarket(WORD wMarket);			//�����г����ͻ�ȡ��Ʊ�б�

	//��������
	void appendNews(const QString& title, const QString& content);		//�������
	//F10����
	void appendF10(const QString& title, const QString& content);		//���F10����

	//��ȡ������������
	QList<CStockInfoItem*> getStockInfoList();
	CStockInfoItem* getStockInfoItem(const QString& qsCode);
	void setStockInfoItem(CStockInfoItem* p);

	/*������������*/
	bool exportHistoryData(const QString& qsCode, const QList<qRcvHistoryData*>& list);
	bool exportHistoryData(const QString& qsCode, const QList<qRcvHistoryData*>& list, int iOffset);
	/*��ȡĳֻ��Ʊ����������*/
	QList<qRcvHistoryData*> getHistoryList(const QString& code);
	/*��ȡĳֻ��Ʊ���count������������*/
	QList<qRcvHistoryData*> getHistoryList(const QString& code, int count);

	/*�����ֱ�����*/
	bool exportFenBiData(const QString& qsCode, const long& lDate, const QList<qRcvFenBiData*>& list);

private:
	QMap<QString,CStockInfoItem*> m_mapStockInfos;
	QMap<QString,CBlockInfoItem*> m_mapBlockInfos;

private:
	static CDataEngine* m_pDataEngine;
	static time_t m_tmCurrentDay;
	static time_t m_tmCurrent;					//���һ��report���ݵ�ʱ��
	QString m_qsHistroyDir;						//�������ݴ洢��·�� AppDir/data/history/...
	QString m_qsBlocksDir;						//������ݵĴ洢·�� AppDir/data/blocks/...
	QString m_qsCommonBlocks;					//���ð���б������ļ��Ĵ洢·�� AppDir/data/CommonBlocks.xml
	QString m_qsNewsDir;						//�������ݵĴ洢·�� AppDir/data/news/...
	QString m_qsF10Dir;							//F10���ݵĴ洢·�� AppDir/data/F10/...
	QString m_qsFenBiDir;						//�ֱ����ݵĴ洢·�� AppDir/data/FenBi/Date/...
};


#endif	//DATA_ENGINE_H