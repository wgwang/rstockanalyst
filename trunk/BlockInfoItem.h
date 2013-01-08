/************************************************************************/
/* �ļ����ƣ�BlockInfoItem.h
/* ����ʱ�䣺2013-01-08 09:28
/*
/* ��    �������ڹ���������
/************************************************************************/
#ifndef	BLOCK_INFO_ITEM_H
#define BLOCK_INFO_ITEM_H
#include <QtCore>
#include "StockInfoItem.h"

class CBlockInfoItem : public QObject
{
	Q_OBJECT
public:
	CBlockInfoItem(const QString& _name);						//���캯����ֻ�������ƣ�Ȼ��ͨ������ȥ��ȡ�ļ��е��б�
	CBlockInfoItem(const QString& _name,const QRegExp& _exp);	//���캯����ͨ�õ��б�ͨ��������ʽȥƥ���Ʊ����
	~CBlockInfoItem(void);

private:
	QString blockName;
	QList<CStockInfoItem*> stocksInBlock;
};

#endif	//BLOCK_INFO_ITEM_H