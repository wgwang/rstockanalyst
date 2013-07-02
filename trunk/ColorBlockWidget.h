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
	//��ʾ���ͣ���
	enum RShowType
	{
		ShowIncrease = 1,	//�Ƿ�
		ShowTurnRatio,		//������
		ShowVolumeRatio,	//����
	};

public:
	CColorBlockWidget(CBaseWidget* parent = 0);
	~CColorBlockWidget(void);

public:
	//���ظ�K��ͼ��������Ϣ
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//�����K��ͼ��������Ϣ
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);


	//����ǰ���ڴ�
	virtual void clearTmpData();

	//���µ�ǰ������ʽ
	virtual void updateSortMode(bool bSelFirst);

protected:
	//���µ�ǰ�ĺ���������
	virtual void updateTimesH();

public:
	//ͨ������keyword��ȡ��Ҫ�ڰ�����������ʾ������
	virtual void getKeyWizData(const QString& keyword,QList<KeyWizData*>& listRet);
	//���̾��鴰��ȷ�Ϻ󴥷�
	virtual void keyWizEntered(KeyWizData* pData);

public slots:
	virtual void setBlock(const QString& block);

protected slots:
	void onSetCurrentBlock();								//���õ�ǰ��ʾ�İ��
	void onSetShowType();									//������ʾ����
	void onSetExpression();									//���õ�ǰ��ʾ�ı��ʽ
	virtual void updateColorBlockData();					//���µ�ǰ��Ҫ��ʾ������

protected:
	void setShowType(RShowType _t);

private:
	void clickedStock(CStockInfoItem* pItem);	//�������Ʊʱ����

protected:
	virtual void paintEvent(QPaintEvent* e);				//�����¼�
	virtual void mouseMoveEvent(QMouseEvent* e);			//����ƶ��¼�
	virtual void mousePressEvent(QMouseEvent* e);			//������¼�
	virtual void wheelEvent(QWheelEvent* e);				//����м������¼�
	virtual void keyPressEvent(QKeyEvent* e);				//���̲���

	//����ɫ��
	//vColor:ȡ0-21
	//vHeight:�ٷֱ�0-100%;
	//vWidth:�ٷֱ�0%-100%;
	void drawColocBlock(QPainter& p,int iY,
		QVector<float>& vColor,QVector<float>& vHeight,QVector<float>& vWidth);

	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu();

	virtual void onBlockClicked(CBlockInfoItem* pBlock,int iCmd);
	/*��ػ�ͼ����*/
private:
	void drawHeader(QPainter& p,const QRect& rtHeader);			//����ͷ����Ϣ
	void drawClient(QPainter& p,const QRect& rtClient);			//����������
	void drawBottom(QPainter& p,const QRect& rtBottom);			//���Ƶײ�����

	void drawStock(QPainter& p,const QRect& rtCB,CStockInfoItem* pItem);	//���Ƶ�ֻ��Ʊ

	QRect rectOfStock(CStockInfoItem* pItem);					//��ȡĳֻ��Ʊ��ʾ��λ��
	CStockInfoItem* hitTestStock(const QPoint& ptPoint) const;		//����ĳ����ָ��Ĺ�Ʊ��Ϣ
	RStockData* hitTestCBItem(const QPoint& ptPoint) const;//����ĳ����ָ���ɫ����Ϣ

private:
	QMenu* m_pMenuShowType;					//���ƽ����е���ʾ����

	QList<RWidgetOpData> m_listShowOp;		//��ʾ�����б�

	CBlockInfoItem* m_pBlock;				//��ǰ�İ������
	QList<CStockInfoItem*> m_listStocks;	//��ǰ��ʾ�����й�Ʊ�б�

	QMap<CStockInfoItem*,int> m_mapStockIndex;	//�������ٲ���ĳֻ��Ʊ���ڵ�����
	CStockInfoItem* m_pSelectedStock;			//��ǰѡ�еĹ�Ʊ

	/*���ڻ��Ʋ����ĳ�Ա����*/
private:
	int m_iTitleHeight;						//ͷ���߶�
	int m_iBottomHeight;					//�ײ��ĸ߶�
	int showStockIndex;						//��ǰ��ʾ����ʼλ�ã��У�

	QRect m_rtHeader;						//ͷ��Header����
	QRect m_rtBottom;						//�ײ�������������������Ϣ

	QTimer m_timerUpdateUI;					//������µ�timer

	QString m_qsExpColor;					//��ɫ���ʽ
	QString m_qsExpHeight;					//�߶ȱ��ʽ
	QString m_qsExpWidth;					//��ȱ��ʽ

	//��������ʽ���ڵľ���
	QMap<int,QRect> m_mapSorts;

	bool m_bShowIncrease;					//��ʾ�Ƿ�����ɫ��
	bool m_bShowTurnRatio;					//��ʾ�����ʣ��߶ȣ�
	bool m_bShowVolumeRatio;				//��ʾ���ȣ���ȣ�
};

#endif	//COLOR_BLOCK_WIDGET_H