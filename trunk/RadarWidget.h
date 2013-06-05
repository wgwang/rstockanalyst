/************************************************************************/
/* �ļ����ƣ�RadarWidget.h
/* ����ʱ�䣺2013-06-05 10:30
/*
/* ��    ����������ʾ�г��״��⣬�̳���CBaseWidget
/************************************************************************/
#ifndef RADAR_WIDGET_H
#define RADAR_WIDGET_H
#include "BaseWidget.h"
#include "StockInfoItem.h"

class CRadarWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CRadarWidget(CBaseWidget* parent = 0);
	~CRadarWidget(void);

public:
	//���ظ�K��ͼ��������Ϣ
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//�����K��ͼ��������Ϣ
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

public:
	//ͨ������keyword��ȡ��Ҫ�ڰ�����������ʾ������
	virtual void getKeyWizData(const QString& keyword,QList<KeyWizData*>& listRet);
	//���̾��鴰��ȷ�Ϻ󴥷�
	virtual void keyWizEntered(KeyWizData* pData);

public slots:
	virtual void setStockCode(const QString& only);

protected:
	void setStockItem(CStockInfoItem* pItem);

protected:
	virtual void paintEvent(QPaintEvent* e);				//�����¼�
	virtual void keyPressEvent(QKeyEvent* e);				//���̲���

	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu();

private:
	QMenu* m_pMenuCustom;					//�Զ���˵�

private:

};

#endif	//RADAR_WIDGET_H