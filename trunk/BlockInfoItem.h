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

public:
	QString getBlockName() const{ return blockName; }			//��ȡ�������
	QList<CStockInfoItem*> getStockList();						//��ȡ����б�

	bool appendStocks(QList<CStockInfoItem*> list);		//��������ӹ�Ʊ
	bool appendStocks(QList<QString> list);				//��������ӹ�Ʊ
	bool removeStocks(QList<CStockInfoItem*> list);		//�Ӱ����ɾ����Ʊ
	bool removeStocks(QList<QString> list);				//�Ӱ����ɾ����Ʊ

protected slots:
	void stockFenbiChanged(const QString& _code);	//ĳֻ��Ʊ�ķֱ����ݷ����ı�
	void stockHistoryChanged(const QString& _code);	//ĳֻ��Ʊ����ʷ���ݷ����ı�

private:
	void addStock(CStockInfoItem* p);				//��ӹ�Ʊ
	void removeStock(CStockInfoItem* p);			//ɾ����Ʊ
	void clearTmpData();							//������ʱ�ڴ档
private:
	QString blockName;
	QString blockFilePath;						//������ݵĴ洢·��
	QList<CStockInfoItem*> stocksInBlock;
};

#endif	//BLOCK_INFO_ITEM_H