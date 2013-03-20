#ifndef COORDX_BASE_WIDGET_H
#define	COORDX_BASE_WIDGET_H
#include "RStockFunc.h"
#include "BaseWidget.h"

class CCoordXBaseWidget : public CBaseWidget
{
public:
	CCoordXBaseWidget(CBaseWidget* parent = 0, RWidgetType type = WidgetBasic);
	~CCoordXBaseWidget(void);

protected:
	void updateTimesH();								//���µ�ǰ�ĺ���������
	void drawCoordX(QPainter& p,const QRect& rtCoordX,	//����X������
		float fGridSize);

protected:
	RStockCircle m_typeCircle;				//��ͼ����ʾ����
	QMap<time_t,int> m_mapTimes;			//��ǰ��Ҫ��ʾ������ʱ�䣨�������꣩
	lua_State* m_pL;
};

#endif	//COORDX_BASE_WIDGET_H