/************************************************************************/
/* �ļ����ƣ�KLineWidget.h
/* ����ʱ�䣺2012-11-08 10:59
/*
/* ��    ����������ʾK��ͼ�Ĵ���
/************************************************************************/

#ifndef K_LINE_WIDGET_H
#define K_LINE_WIDGET_H
#include <QtGui>
#include "StockInfoItem.h"

class CKLineWidget : QWidget
{
	Q_OBJECT
public:
	CKLineWidget(QWidget* parent = 0);
	~CKLineWidget(void);

public slots:
	void setStockCode(const QString& code);

private:
	CStockInfoItem* m_pStockItem;	//��ǰK��ͼ�Ĺ�Ʊ����ָ��
};


#endif	//K_LINE_WIDGET_H