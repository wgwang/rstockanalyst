#ifndef COORDX_BASE_WIDGET_H
#define	COORDX_BASE_WIDGET_H
#include "BaseWidget.h"

class CCoordXBaseWidget : public CBaseWidget
{
public:
	enum CoordXCircle		//ɫ��ͼ������
	{
		FenShi = 1,				//��ʱģʽ��ֱ�����ߣ�
		Sec10 = 10,						//10��
		Sec30 = 30,						//30��
		Min1 = 60,					//1����
		Min5 = 5*60,					//5����
		Min15 = 15*60,					//15����
		Min30 = 30*60,					//30����
		Min60 = 60*60,					//60����
		MinN,					//N����
		Day,					//����
		DayN,					//N����
		Week,					//����
		Month,					//����
		Month3,					//����
		Year,					//����
	};

public:
	CCoordXBaseWidget(CBaseWidget* parent = 0, WidgetType type = CBaseWidget::Basic);
	~CCoordXBaseWidget(void);

protected:
	void updateTimesH();								//���µ�ǰ�ĺ���������
	void drawCoordX(QPainter& p,const QRect& rtCoordX,	//����X������
		int iGridSize);

protected:
	CoordXCircle m_typeCircle;				//��ͼ����ʾ����
	QMap<time_t,int> m_mapTimes;			//��ǰ��Ҫ��ʾ������ʱ�䣨�������꣩
};

#endif	//COORDX_BASE_WIDGET_H