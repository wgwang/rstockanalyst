/************************************************************************/
/* �ļ����ƣ�KLineWidget.h
/* ����ʱ�䣺2012-11-08 10:59
/*
/* ��    ����������ʾK��ͼ�Ĵ��ڣ��̳���CBaseWidget
/************************************************************************/

#ifndef K_LINE_WIDGET_H
#define K_LINE_WIDGET_H
#include <QtGui>
#include "CoordXBaseWidget.h"
#include "StockInfoItem.h"
#include "BaseLiner.h"


class CKLineWidget : public CCoordXBaseWidget
{
	Q_OBJECT
public:
	CKLineWidget(CBaseWidget* parent = 0);
	~CKLineWidget(void);

public:
	//���ظ�K��ͼ��������Ϣ
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//�����K��ͼ��������Ϣ
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

	//��������
	virtual void updateData();

public slots:
	virtual void setStockCode(const QString& code);
	void updateKLine(const QString& code);

protected:
	virtual void paintEvent(QPaintEvent* e);				//�����¼�
	virtual void mouseMoveEvent(QMouseEvent* e);			//����ƶ��¼�
	virtual void leaveEvent(QEvent* e);						//����Ƴ��¼�
	virtual void mousePressEvent(QMouseEvent* e);			//������¼�
	virtual void mouseDoubleClickEvent(QMouseEvent *e);		//���˫���¼�
	virtual void keyPressEvent(QKeyEvent* e);				//���̰����¼������ڴ����ݼ�����

	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu();

protected slots:
	void onSetStockCode();								//�����Ի������û��ֶ������Ʊ����
	void onSetExpression();								//�����Ի������û��ֶ�������ʽ
	void onClickedAddShow();							//������ʾ����
	void onClickedSubShow();							//������ʾ����
	void onAddDeputy();									//���Ӹ�ͼ
	void onAddVolume();									//�Ƿ���ʾ����ͼ
	void onRemoveDeputy();								//ɾ����ͼ
	void onSetSizes();									//��������ͼ����ʾ����

private:
	void drawTitle(QPainter& p,const QRect& rtTitle);	//����ͷ��
	void drawShowBtns(QPainter& p,const QRect& rtBtns);	//�������½ǵ�������ť
private:
	void resetTmpData();					//���¼������ݡ�
	void clearTmpData();					//���������д������ڴ档

private:
	CStockInfoItem* m_pStockItem;			//��ǰK��ͼ�Ĺ�Ʊ����ָ��
	QMap<time_t,RStockData*>* m_mapData;	//����������ʾ������
	int m_iShowCount;						//��Ҫ��ʾ�����ݸ��������ȣ�������ӦС��listItems��size��

	int m_iCurExp;							//��ǰѡ�еı��ʽ
	bool m_bShowMax;						//�Ƿ������ʾ��Ŀǰֻ�Ը�ͼ��Ч��

	int m_iTitleHeight;						//ͷ���߶�
	int m_iCoorYWidth;						//Y������Ŀ��
	int m_iCoorXHeight;						//X������ĸ߶�
	float m_fItemWidth;						//����Item�Ŀ��
	int m_iMainLinerHeight;					//��ͼ�ĸ߶�

	QVector<int> m_vSizes;					//��ʾ����(�ܺ�Ϊ100)
	QVector<QString> m_vExps;				//��ʾ�ı��ʽ

	QRect m_rtAddShow;						//������ʾ�����İ�ť����
	QRect m_rtSubShow;						//������ʾ�����İ�ť����
};


#endif	//K_LINE_WIDGET_H