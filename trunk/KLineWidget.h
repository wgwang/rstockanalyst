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
#include "BaseLiner.h"


class CKLineWidget : public CBaseWidget
{
	Q_OBJECT
public:
	enum KLineCircle			//K������
	{
		Min1 = 1,				//1����
		Min5,					//5����
		Min15,					//15����
		Min30,					//30����
		Min60,					//60����
		MinN,					//N����
		Day,					//����
		DayN,					//N����
		Week,					//����
		Month,					//����
		Month3,					//����
		Year,					//����
	};
public:
	CKLineWidget(CBaseWidget* parent = 0);
	~CKLineWidget(void);

public:
	//���ظ�K��ͼ��������Ϣ
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//�����K��ͼ��������Ϣ
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

public slots:
	virtual void setStockCode(const QString& code);
	void updateKLine(const QString& code);

protected:
	virtual void paintEvent(QPaintEvent* e);				//�����¼�
	virtual void mouseMoveEvent(QMouseEvent* e);			//����ƶ��¼�
	virtual void mousePressEvent(QMouseEvent* e);			//������¼�

	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu();

protected slots:
	void onSetStockCode();								//�����Ի������û��ֶ������Ʊ����
	void onClickedAddShow();							//������ʾ����
	void onClickedSubShow();							//������ʾ����

private:
	void drawTitle(QPainter& p,const QRect& rtTitle);	//����ͷ��
	void drawCoordX(QPainter& p,const QRect& rtCoordX);	//���Ƶײ���X�����ᣨʱ���ᣩ
	void drawShowBtns(QPainter& p,const QRect& rtBtns);	//�������½ǵ�������ť
private:
	void resetTmpData();					//���¼������ݡ�
	void clearTmpData();					//���������д������ڴ档

private:
	QMenu* m_pMenuCustom;					//�Զ���˵�
	KLineCircle m_typeCircle;				//��ͼ����ʾ����
	CStockInfoItem* m_pStockItem;			//��ǰK��ͼ�Ĺ�Ʊ����ָ��
	QVector<stLinerItem> listItems;			//����������ʾ������
	int m_iShowCount;						//��Ҫ��ʾ�����ݸ��������ȣ�������ӦС��listItems��size��
	CMultiLiner* m_pLinerMain;				//��ͼ��K����ͼ
	CMultiLiner* m_pLinerDeputy;			//��ǰ��󻯵ĸ�ͼ��һ�������ӦΪNULL
	QList<CMultiLiner*> m_listLiners;		//��ͼ������ �ɽ���/�ɽ����ʽָ��ͼ��


	int m_iTitleHeight;						//ͷ���߶�
	int m_iCoorYWidth;						//Y������Ŀ��
	int m_iCoorXHeight;						//X������ĸ߶�
	float fItemWidth;						//����Item�Ŀ��
	int m_iMainLinerHeight;					//��ͼ�ĸ߶�

	QRect m_rtAddShow;						//������ʾ�����İ�ť����
	QRect m_rtSubShow;						//������ʾ�����İ�ť����
};


#endif	//K_LINE_WIDGET_H