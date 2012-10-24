#ifndef DATA_ENGINE_H
#define DATA_ENGINE_H

#include "StockInfoItem.h"

#define	R_TIME_ZONE	8

class CDataEngine : public QObject
{
	Q_OBJECT
public:
	CDataEngine(void);
	~CDataEngine(void);
	static CDataEngine* getDataEngine();

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


	/*���ݵ�������*/
	/*���������������ݣ�F10����*/
	static int exportBaseInfo(const QString& qsFile);
	/*����Reports����*/
	static int exportReportsInfo(const QString& qsFile);


public:
	static time_t* getLast5DayTime();			//��ȡ���5��Ŀ�������
	static bool isStockOpenDay(time_t tmDay);	//�ж�tmDay�Ƿ���
	static time_t getOpenSeconds();				//��ȡÿ��Ŀ���ʱ�䣨�룩��һ��Ϊ4Сʱ
	static time_t getOpenSeconds(time_t tmTime);//��ȡ���������tmTime�Ŀ���ʱ�䣨�룩

public:
	//��ȡ������������
	QList<CStockInfoItem*> getStockInfoList();
	CStockInfoItem* getStockInfoItem(const QString& qsCode);
	void setStockInfoItem(CStockInfoItem* p);

signals:
	void stockInfoChanged(const QString&);

private:
	QMap<QString,CStockInfoItem*> m_mapStockInfos;

private:
	static CDataEngine* m_pDataEngine;
	static time_t m_tmCurrentDay;
	static time_t* m_tmLast5Day;
};


#endif	//DATA_ENGINE_H