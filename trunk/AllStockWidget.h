/************************************************************************/
/* �ļ����ƣ�AllStockWidget.h
/* ����ʱ�䣺2013-07-05 14:07
/*
/* ��    �����������Թ�Ʊ�Ŀ���Ԥ����ʾ
/************************************************************************/

#ifndef All_STOCK_WIDGET_H
#define All_STOCK_WIDGET_H
#include "BaseWidget.h"

class CAllStockWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CAllStockWidget(CBaseWidget* parent = 0, RWidgetType type = WidgetAllStock);
	~CAllStockWidget(void);

protected:
	virtual void paintEvent(QPaintEvent*);

	void drawStock(CStockInfoItem* pStock,const QRect& rtStock,QPainter& p);

private:
	int m_iItemWidth;				//�����ڵ���
	int m_iItemHeight;				//�����ڵ�߶�
	int m_iStockWidth;				//������Ʊ�Ŀ��
	int m_iStockHeight;				//������Ʊ�ĸ߶�

	QVector<uint> m_vColors;		//��ɫ��21����
private:
	QList<CStockInfoItem*> m_listStocks;		//���еĹ�Ʊ
};

#endif	//All_STOCK_WIDGET_H