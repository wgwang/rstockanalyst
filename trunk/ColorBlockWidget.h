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
#include "BaseBlockWidget.h"
#include "BlockInfoItem.h"

class CColorBlockWidget : public CBaseBlockWidget
{
	Q_OBJECT
public:
	CColorBlockWidget(CBaseWidget* parent = 0);
	~CColorBlockWidget(void);

public:
	//���ظ�K��ͼ��������Ϣ
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//�����K��ͼ��������Ϣ
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

	//��������
	virtual void updateData();

public slots:
	virtual void setBlock(const QString& block);
	virtual void updateStock(const QString& code);			//����ĳֻ��Ʊ����ʾ

protected slots:
	void onSetCurrentBlock();								//���õ�ǰ��ʾ�İ��
	void updateColorBlockData();							//���µ�ǰ��Ҫ��ʾ������

private:
	void clearTmpData();						//���������д������ڴ档
	void clickedStock(CStockInfoItem* pItem);	//�������Ʊʱ����

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
	RStockData hitTestCBItem(const QPoint& ptPoint) const;//����ĳ����ָ���ɫ����Ϣ

	//��ȡ���ݶ�ά��ͨ��������ǰ�����ڡ�
	QMap<time_t,RStockData>* getColorBlockMap(CStockInfoItem* pItem);

private:
	QMenu* m_pMenuBlockList;				//���а����Ϣ����ǰѡ�еİ���ϴ򹴣�

	CBlockInfoItem* m_pBlock;						//��ǰ�İ������
	QList<CStockInfoItem*> m_listStocks;	//��ǰ��ʾ�����й�Ʊ�б�

	QMap<CStockInfoItem*,int> m_mapStockIndex;	//�������ٲ���ĳֻ��Ʊ���ڵ�����
	QMap<CStockInfoItem*,QMap<time_t,RStockData>*> mapStockColorBlocks;	//��ǰ��ʾ��ColorBlock����
	CStockInfoItem* m_pSelectedStock;			//��ǰѡ�еĹ�Ʊ

	/*���ڻ��Ʋ����ĳ�Ա����*/
private:
	int m_iTitleHeight;						//ͷ���߶�
	int m_iBottomHeight;					//�ײ��ĸ߶�
	int showStockIndex;						//��ǰ��ʾ����ʼλ�ã��У�

	QRect m_rtHeader;						//ͷ��Header����
	QRect m_rtClient;						//ʵ��ɫ���������
	QRect m_rtBottom;						//�ײ�������������������Ϣ
};

#endif	//COLOR_BLOCK_WIDGET_H