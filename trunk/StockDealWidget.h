#ifndef STOCK_DEAL_WIDGET_H
#define STOCK_DEAL_WIDGET_H
#include "CoordXBaseWidget.h"

enum RDealWidgetType
{
	DealKind = 1,
};

class CStockDealWidget : public CCoordXBaseWidget
{
	Q_OBJECT
public:
	CStockDealWidget(CBaseWidget* parent = 0);
	~CStockDealWidget(void);


protected:
	//���õ�ǰ��ʾ�Ĺ�Ʊ
	virtual void setStockCode(const QString& code);

	//�������ݣ���ʱʹ��
	virtual void updateData();

	//��ͼ
	virtual void paintEvent(QPaintEvent* e);

protected:
	void drawTitle(QPainter& p);
	void drawClient(QPainter& p);
	void drawBottom(QPainter& p);

private:
	int m_iTitleHeight;		//����߶�
	int m_iBottomHeight;	//�ײ��߶�

	int m_iItemWidth;		//����ɫ��Ŀ��
	int m_iItemHeight;	//����ɫ��ĸ߶�

	QRect m_rtTitle;		//��������
	QRect m_rtClient;		//��ͼ����
	QRect m_rtBottom;		//�ײ�����

	RDealWidgetType m_typeWidget; //��ǰ���ڵ���ʾ����

private:
	CStockInfoItem* m_pCurStock;			//��ǰ�Ĺ�Ʊ
	QList<qRcvFenBiData*> m_listFenbi;		//��ǰ��Ʊ�ķֱ�����
	QMap<time_t,int> m_mapTimes;			//��ǰ��Ҫ��ʾ������ʱ�䣨�������꣩
};


#endif	//STOCK_DEAL_WIDGET_H