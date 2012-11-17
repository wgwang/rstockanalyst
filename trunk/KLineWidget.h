/************************************************************************/
/* �ļ����ƣ�KLineWidget.h
/* ����ʱ�䣺2012-11-08 10:59
/*
/* ��    ����������ʾK��ͼ�Ĵ��ڣ��̳���CBaseWidget
/************************************************************************/

#ifndef K_LINE_WIDGET_H
#define K_LINE_WIDGET_H
#include <QtGui>
#include "BaseWidget.h"
#include "StockInfoItem.h"

class CKLineWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CKLineWidget(CBaseWidget* parent = 0);
	~CKLineWidget(void);

public slots:
	void setStockCode(const QString& code);
	void updateKLine(const QString& code);

protected:
	virtual void paintEvent(QPaintEvent* e);				//�����¼�
	virtual void mouseMoveEvent(QMouseEvent* e);			//����ƶ��¼�

	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu();

private:
	void drawCoordY(QPainter& p,const QRectF& rtClient);	//����Y������
	void drawKGrids(QPainter& p,const QRectF& rtClient);	//����X�����������
	void drawKGrid(qRcvHistoryData* pHistory,QPainter& p,const QRectF& rtClient);		//�����������ݵĻ���
private:
	void resetTmpData();					//���¼������ݡ�
	void clearTmpData();					//���������д������ڴ档

private:
	QMenu* m_pMenuCustom;					//�Զ���˵�
	CStockInfoItem* m_pStockItem;			//��ǰK��ͼ�Ĺ�Ʊ����ָ��
	QList<qRcvHistoryData*> listHistory;	//K��ͼ���õ�����ʷ���ݡ�
	/*������*/
	float fMaxPrice;						//K��ͼ�е���ͼ�
	float fMinPrice;						//K��ͼ�е���߼�
	/*������*/
	time_t tmBegin;
	time_t tmEnd;

	float fKGridWidth;						//�����������ݵĿ��
};


#endif	//K_LINE_WIDGET_H