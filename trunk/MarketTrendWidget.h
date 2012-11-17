/************************************************************************/
/* �ļ����ƣ�MarketTrendWidget.h
/* ����ʱ�䣺2012-11-08 10:59
/*
/* ��    ����������ʾ�г�����ͼ�Ĵ��ڣ��̳���CBaseWidget
/************************************************************************/

#ifndef MARKET_TREND_WIDGET_H
#define	MARKET_TREND_WIDGET_H
#include "BaseWidget.h"

class CMarketTrendWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CMarketTrendWidget(CBaseWidget* parent = 0);
	~CMarketTrendWidget(void);

protected:
	virtual void paintEvent(QPaintEvent* e);				//�����¼�
	virtual void resizeEvent(QResizeEvent* e);				//��С�ı���¼�

	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu();

private:
	QMenu* m_pMenuCustom;					//�Զ���˵�
	QStringList m_listHeader;				//ͷ������

	/*���ڻ��Ƶĳ�Ա����*/
private:
	void updateDrawRect();					//���»�������Ĵ�С
	void drawHeaders(QPainter& p);						//����ͷ����Ϣ

	/*���ڻ��Ƶĳ�Ա����*/
private:
	QRect m_rtHeader;						//ͷ��Header����
	QRect m_rtClient;						//ʵ����������
	QRect m_rtBottom;						//�ײ��������ڻ��Ʒ��࣬����������Ϣ

	int m_iHeaderHeight;					//ͷ���ĸ߶�
	int m_iItemHeight;						//���������ĸ߶�
	int m_iBottomHeight;					//�ײ��ĸ߶�
};


#endif	//MARKET_TREND_WIDGET_H