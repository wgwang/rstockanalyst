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
#include "RadarWatcher.h"

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

	//��������signals
	void reconnectSignals();

public:
	//ͨ������keyword��ȡ��Ҫ�ڰ�����������ʾ������
	virtual void getKeyWizData(const QString& keyword,QList<KeyWizData*>& listRet);
	//���̾��鴰��ȷ�Ϻ󴥷�
	virtual void keyWizEntered(KeyWizData* pData);

public slots:
	virtual void setStockCode(const QString& only);

protected slots:
	//�µ����ݵ���
	void onRadarAlert(RRadarData* pRadar);
	//�Ƴ�ĳ�������״�
	void onWatcherDelete(CRadarWatcher* pWatcher);

protected:
	void setStockItem(CStockInfoItem* pItem);

protected:
	virtual void paintEvent(QPaintEvent* e);				//�����¼�
	virtual void keyPressEvent(QKeyEvent* e);				//���̲���
	virtual void mouseMoveEvent(QMouseEvent* e);			//����ƶ��¼�
	virtual void mousePressEvent(QMouseEvent* e);			//������¼�

	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu();

private:
	void drawTitle(QPainter& p);
	void drawClient(QPainter& p);
	void clickedRadar(RRadarData* pData);

	//����ĳ������Ӧ������
	RRadarData* testRadarData(const QPoint& ptClicked);
private:
	//��ͼ��ر���
	int m_iItemHeight;						//�����״����ݵ���ʾ�߶�
	int m_iTitleHeight;						//����ĸ߶�
	QRect m_rtClient;						//����ʾ���ľ�������
	QRect m_rtTitle;						//������ʾ����

	int m_iShowIndex;						//��ǰ��ʾ����ʼλ��

private:
	QMenu* m_pMenuCustom;					//�Զ���˵�
	QList<RRadarData*> m_listRadars;		//��ʾ������
	QMap<RRadarData*,int> m_mapRadarsIndex;	//�������ٲ���ĳֻ�״��������ڵ�����
	RRadarData* m_pSelRadar;				//��ǰѡ�е�����

private:

};

#endif	//RADAR_WIDGET_H