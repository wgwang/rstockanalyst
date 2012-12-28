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

struct stColorBlockItem
{
	time_t	tmTime;			//��ǰʱ��
	float	fPrice;			//��ǰ�۸�
	float	fVolume;		//��
	float	fAmount;		//��
	stColorBlockItem()
	{
		memset(&tmTime,0,sizeof(stColorBlockItem));
	}
};

class CColorBlockWidget : public CBaseWidget
{
	Q_OBJECT
public:
	enum ColorBlockCircle		//ɫ��ͼ������
	{
		Min1 = 1*60,					//1����
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
	enum BlockMode
	{
		BlockCircle = 1,		//Բ��
		BlockRect,				//����
	};
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
	virtual void updateStock(const QString& code);			//����ĳֻ��Ʊ����ʾ
	void setColorMode(const QString& mode);					//������ɫģʽ

protected slots:
	void onSetCircle();										//���õ�ǰ����ʾ����
	void onSetColorMode();									//���������ɫģʽ
	void onSetBlockMode();									//���õ�ǰ����ʾģʽ
	void onSetBlockSize();									//����ɫ��Ĵ�С
	void onSetCurrentBlock();								//���õ�ǰ��ʾ�İ��

	void updateColorBlockData();							//���µ�ǰ��Ҫ��ʾ������

private:
	void clearTmpData();						//���������д������ڴ档
	void clickedStock(CStockInfoItem* pItem);	//�������Ʊʱ����
	void updateTimesH();						//���µ�ǰ�ĺ���������

protected:
	virtual void paintEvent(QPaintEvent* e);				//�����¼�
	virtual void mouseMoveEvent(QMouseEvent* e);			//����ƶ��¼�
	virtual void mousePressEvent(QMouseEvent* e);			//������¼�
	virtual void wheelEvent(QWheelEvent* e);				//����м������¼�
	virtual void keyPressEvent(QKeyEvent* e);				//���̲���

	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu();

	/*��ػ�ͼ����*/
private:
	void drawHeader(QPainter& p,const QRect& rtHeader);			//����ͷ����Ϣ
	void drawClient(QPainter& p,const QRect& rtClient);			//����������
	void drawBottom(QPainter& p,const QRect& rtBottom);			//���Ƶײ�����

	void drawStock(QPainter& p,const QRect& rtCB,CStockInfoItem* pItem);	//���Ƶ�ֻ��Ʊ

	QRect rectOfStock(CStockInfoItem* pItem);					//��ȡĳֻ��Ʊ��ʾ��λ��
	CStockInfoItem* hitTestStock(const QPoint& ptPoint) const;		//����ĳ����ָ��Ĺ�Ʊ��Ϣ
	stColorBlockItem hitTestCBItem(const QPoint& ptPoint) const;//����ĳ����ָ���ɫ����Ϣ

	//��ȡ���ݶ�ά��ͨ��������ǰ�����ڡ�
	QMap<time_t,stColorBlockItem>* getColorBlockMap(CStockInfoItem* pItem);

private:
	QMenu* m_pMenuCustom;					//�Զ���˵�
	QMenu* m_pMenuCircle;					//�������ò˵�
	QMenu* m_pMenuColorMode;				//��ɫģʽ�˵�
	QMenu* m_pMenuBlockMode;				//��ʾģʽ�˵�
	QMenu* m_pMenuBlockList;				//���а����Ϣ����ǰѡ�еİ���ϴ򹴣�

	QString m_qsBlock;						//��ǰ�İ������
	QList<CStockInfoItem*> m_listStocks;	//��ǰ��ʾ�����й�Ʊ�б�

	QMap<CStockInfoItem*,int> m_mapStockIndex;	//�������ٲ���ĳֻ��Ʊ���ڵ�����
	QMap<CStockInfoItem*,QMap<time_t,stColorBlockItem>*> mapStockColorBlocks;	//��ǰ��ʾ��ColorBlock����
	CStockInfoItem* m_pSelectedStock;			//��ǰѡ�еĹ�Ʊ

	QString m_qsColorMode;						//��ǰ��ɫģʽ
	ColorBlockCircle m_typeCircle;				//��ǰ����ʾ����

	/*���ڻ��Ʋ����ĳ�Ա����*/
private:
	int m_iTitleHeight;						//ͷ���߶�
	int m_iCBHeight;						//����ɫ��ĸ߶�
	int m_iCBWidth;							//����ɫ��Ŀ��
	int m_iBottomHeight;					//�ײ��ĸ߶�
	int showStockIndex;						//��ǰ��ʾ����ʼλ�ã��У�
	BlockMode m_typeBlock;					//block��ʾ��״

	QMap<time_t,int> m_mapTimes;				//��ǰ��Ҫ��ʾ������ʱ�䣨�������꣩


	QRect m_rtHeader;						//ͷ��Header����
	QRect m_rtClient;						//ʵ��ɫ���������
	QRect m_rtBottom;						//�ײ�������������������Ϣ
};

#endif	//COLOR_BLOCK_WIDGET_H