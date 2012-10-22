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
	static CDataEngine* getDataEngine()
	{
		if(m_pDataEngine == NULL)
			m_pDataEngine = new CDataEngine;
		return m_pDataEngine;
	}

public:
	/*���ݵ��뺯��*/

	//F10���ݵ���,����ֵΪ���������������
	static int importBaseInfoFromFile(const QString& qsFile);


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
	void stockInfoAdded(const QString&);

private:
	QMap<QString,CStockInfoItem*> m_mapStockInfos;

private:
	static CDataEngine* m_pDataEngine;
	static time_t m_tmCurrentDay;
	static time_t* m_tmLast5Day;
};


#endif	//DATA_ENGINE_H