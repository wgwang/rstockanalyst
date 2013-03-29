#ifndef COORDX_BASE_WIDGET_H
#define	COORDX_BASE_WIDGET_H
#include "RStockFunc.h"
#include "BaseWidget.h"

typedef struct tagRStockCircleData
{
	RStockCircle circle;		//����
	QString key;				//���ٲ��Ҽ�
	QString desc;				//������Ϣ

	tagRStockCircleData(const RStockCircle& _c, const QString& _k, const QString& _d)
		: circle(_c)
		, key(_k)
		, desc(_d)
	{
	}
} RStockCircleData;

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

public:
	//ͨ������keyword��ȡ��Ҫ�ڰ�����������ʾ������
	virtual void getKeyWizData(const QString& keyword,QList<KeyWizData*>& listRet);
	//���̾��鴰��ȷ�Ϻ󴥷�
	virtual void keyWizEntered(KeyWizData* pData);

protected:
	void updateTimesH();												//���µ�ǰ�ĺ���������
	void updateShowTimes(const QRectF& rtCoordX,float fItemWidth);		//������Ҫ���л��Ƶ�ʱ����

	void drawCoordX(QPainter& p,const QRectF& rtCoordX,float fItemWidth);	//����X������
	//����Y�ᣬ��Ҫ����K��ͼ
	void drawCoordY(QPainter& p,const QRectF rtCoordY, float fMax, float fMin);

	//��ȡ���ݶ�ά��ͨ��������ǰ�����ڡ�
	QMap<time_t,RStockData*>* getColorBlockMap(CStockInfoItem* pItem);

protected slots:
	void onSetCircle();								//���õ�ǰ����ʾ����

protected:
	void setCircle(RStockCircle _c);				//���õ�ǰ����ʾ����

protected:
	QMenu* m_pMenuCustom;					//�Զ���˵�
	QMenu* m_pMenuCircle;					//�������ò˵�
	RStockCircle m_typeCircle;				//��ͼ����ʾ����
	lua_State* m_pL;

protected:
	QMap<time_t,int> m_mapTimes;			//��ǰ��Ҫ��ʾ������ʱ�䣨�������꣩
	QMap<time_t,float> m_mapShowTimes;		//��ǰ�Ѿ����Ƶ�ʱ��

	QList<RStockCircleData> m_listCircle;	//��ʾ���ڱ�
};

#endif	//COORDX_BASE_WIDGET_H