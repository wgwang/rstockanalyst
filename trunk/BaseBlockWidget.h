/************************************************************************/
/* �ļ����ƣ�BaseBlockWidget.h
/* ����ʱ�䣺2013-01-14 13:38
/*
/* ��    ��������ɫ��ͼ�ĳ��󣬼̳���CCoordXBaseWidget
/*           �����Ե�ʹ�ò�ͬ��ɫ������ʾ��Ʊ���������ƣ��Ӷ�����ͬ�������У�
/*           չ�ָ������Ϣ��
/************************************************************************/

#ifndef BASE_BLOCK_WIDGET_H
#define BASE_BLOCK_WIDGET_H
#include "CoordXBaseWidget.h"

class CBaseBlockWidget : public CCoordXBaseWidget
{
	Q_OBJECT
public:
	enum BlockMode
	{
		BlockCircle = 1,		//Բ��
		BlockRect,				//����
	};
public:
	CBaseBlockWidget(CBaseWidget* parent = 0, RWidgetType type = WidgetBasic);
	~CBaseBlockWidget(void);

public:
	//���ظ�K��ͼ��������Ϣ
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//�����K��ͼ��������Ϣ
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

	//��������
	virtual void updateData();

protected:
	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu();

protected:
	void updateTimesH();								//���µ�ǰ�ĺ���������
	void drawCoordX(QPainter& p,const QRect& rtCoordX,	//����X������
		float fGridSize);

	//��ȡ���ݶ�ά��ͨ��������ǰ�����ڡ�
	QMap<time_t,RStockData*>* getColorBlockMap(CStockInfoItem* pItem);

protected slots:
	void setColorMode(const QString& mode);					//������ɫģʽ

	void onSetCircle();										//���õ�ǰ����ʾ����
	void onSetColorMode();									//���������ɫģʽ
	void onSetBlockMode();									//���õ�ǰ����ʾģʽ
	void onSetBlockSize();									//����ɫ��Ĵ�С

protected:
	QMap<time_t,int> m_mapTimes;			//��ǰ��Ҫ��ʾ������ʱ�䣨�������꣩

protected:
	QMenu* m_pMenuCustom;					//�Զ���˵�
	QMenu* m_pMenuCircle;					//�������ò˵�
	QMenu* m_pMenuColorMode;				//��ɫģʽ�˵�
	QMenu* m_pMenuBlockMode;				//��ʾģʽ�˵�

	QString m_qsColorMode;					//��ǰ��ɫģʽ
protected:
	BlockMode m_typeBlock;					//block��ʾ��״
	int m_iCBHeight;						//����ɫ��ĸ߶�
	int m_iCBWidth;							//����ɫ��Ŀ��
};

#endif	//BASE_BLOCK_WIDGET_H