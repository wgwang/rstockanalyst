/************************************************************************/
/* �ļ����ƣ�ColorBlockWidget.h
/* ����ʱ�䣺2012-11-19 10:59
/*
/* ��    ����������ʾ��ֻ��Ʊ�����ƣ��̳���CBaseWidget
/*           �����Ե�ʹ�ò�ͬ��ɫ������ʾ��Ʊ���������ƣ��Ӷ�����ͬ�������У�
/*           չ�ָ������Ϣ��
/************************************************************************/

#ifndef COLOR_BLOCK_WIDGET_H
#define COLOR_BLOCK_WIDGET_H
#include "BaseWidget.h"
#include "StockInfoItem.h"

class CColorBlockWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CColorBlockWidget(CBaseWidget* parent = 0);
	~CColorBlockWidget(void);

public:
	//���ظ�K��ͼ��������Ϣ
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//�����K��ͼ��������Ϣ
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);


public slots:
	virtual void setBlock(const QString& block);

protected:
	virtual void paintEvent(QPaintEvent* e);				//�����¼�

	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu();

	/*��ػ�ͼ����*/
private:
	void drawHeader(QPainter& p,const QRect& rtHeader);			//����ͷ����Ϣ
	void drawClient(QPainter& p,const QRect& rtClient);			//����������
	void drawBottom(QPainter& p,const QRect& rtBottom);			//���Ƶײ�����

	void drawStock(QPainter& p,const QRect& rtCB,CStockInfoItem* pItem);	//���Ƶ�ֻ��Ʊ

private:
	QMenu* m_pMenuCustom;					//�Զ���˵�
	QString m_qsBlock;						//��ǰ�İ������
	QList<CStockInfoItem*> m_listStocks;	//��ǰ��ʾ�����й�Ʊ�б�

	/*���ڻ��Ʋ����ĳ�Ա����*/
private:
	int m_iCBHeight;						//����ɫ��ĸ߶�
	int showStockIndex;
};

#endif	//COLOR_BLOCK_WIDGET_H