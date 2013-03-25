#ifndef COORDX_BASE_WIDGET_H
#define	COORDX_BASE_WIDGET_H
#include "RStockFunc.h"
#include "BaseWidget.h"

class CCoordXBaseWidget : public CBaseWidget
{
	Q_OBJECT
public:
	CCoordXBaseWidget(CBaseWidget* parent = 0, RWidgetType type = WidgetBasic);
	~CCoordXBaseWidget(void);

protected:
	//���ظ�K��ͼ��������Ϣ
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//�����K��ͼ��������Ϣ
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);
	//��������
	virtual void updateData();

protected:
	void updateTimesH();												//���µ�ǰ�ĺ���������
	void updateShowTimes(const QRectF& rtCoordX,float fItemWidth);			//������Ҫ���л��Ƶ�ʱ����
	void drawCoordX(QPainter& p,const QRectF& rtCoordX,float fItemWidth);	//����X������
	//��ȡ���ݶ�ά��ͨ��������ǰ�����ڡ�
	QMap<time_t,RStockData*>* getColorBlockMap(CStockInfoItem* pItem);

protected slots:
	void onSetCircle();										//���õ�ǰ����ʾ����

protected:
	QMenu* m_pMenuCustom;					//�Զ���˵�
	QMenu* m_pMenuCircle;					//�������ò˵�
	RStockCircle m_typeCircle;				//��ͼ����ʾ����
	lua_State* m_pL;

protected:
	QMap<time_t,int> m_mapTimes;			//��ǰ��Ҫ��ʾ������ʱ�䣨�������꣩
	QMap<time_t,float> m_mapShowTimes;		//��ǰ�Ѿ����Ƶ�ʱ��
};

#endif	//COORDX_BASE_WIDGET_H