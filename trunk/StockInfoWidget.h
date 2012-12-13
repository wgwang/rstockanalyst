/************************************************************************/
/* �ļ����ƣ�StockInfoWidget.h
/* ����ʱ�䣺2012-12-13 17:55
/*
/* ��    ����������ʾ��ֻ��Ʊ��������Ϣ���̳���CBaseWidget
/************************************************************************/
#ifndef STOCK_INFO_WIDGET_H
#define STOCK_INFO_WIDGET_H
#include "BaseWidget.h"
#include "StockInfoItem.h"

class CStockInfoWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CStockInfoWidget(CBaseWidget* parent = 0);
	~CStockInfoWidget(void);

protected:
	virtual void paintEvent(QPaintEvent* e);				//�����¼�

	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu();

private:
	QMenu* m_pMenuCustom;					//�Զ���˵�
};


#endif	//STOCK_INFO_WIDGET_H