#ifndef B_COLOR_BLOCK_WIDGET_H
#define	B_COLOR_BLOCK_WIDGET_H

#include "BaseBlockWidget.h"
#include "BlockInfoItem.h"

class CBColorBlockWidget : public CBaseBlockWidget
{
	Q_OBJECT
public:
	CBColorBlockWidget(CBaseWidget* parent = 0);
	~CBColorBlockWidget(void);

public:
	//���ظ�K��ͼ��������Ϣ
	virtual bool loadPanelInfo(const QDomElement& eleWidget);
	//�����K��ͼ��������Ϣ
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& eleWidget);

	//��������
	virtual void updateData();
	//����ǰ���ڴ�
	virtual void clearTmpData();

	//���µ�ǰ������ʽ
	virtual void updateSortMode(bool bSelFirst);
public:
	//ͨ������keyword��ȡ��Ҫ�ڰ�����������ʾ������
	virtual void getKeyWizData(const QString& keyword,QList<KeyWizData*>& listRet);
	//���̾��鴰��ȷ�Ϻ󴥷�
	virtual void keyWizEntered(KeyWizData* pData);

public slots:
	virtual void setBlock(const QString& block);

protected slots:
	void updateColorBlockData();							//���µ�ǰ��Ҫ��ʾ������
	void updateShowMap();									//����Ҫ��ʾ�����ݣ���ɾ��֮ǰ��

private:
	void clickedBlock(CBlockInfoItem* pItem);				//�������Ʊʱ����

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

	/*��ػ�ͼ����*/
private:
	void drawHeader(QPainter& p,const QRect& rtHeader);			//����ͷ����Ϣ
	void drawClient(QPainter& p,const QRect& rtClient);			//����������
	void drawBottom(QPainter& p,const QRect& rtBottom);			//���Ƶײ�����

	void drawBlock(QPainter& p,const QRect& rtCB,CBlockInfoItem* pItem);	//���Ƶ�ֻ��Ʊ

	QRect rectOfBlock(CBlockInfoItem* pItem);					//��ȡĳֻ��Ʊ��ʾ��λ��
	CBlockInfoItem* hitTestStock(const QPoint& ptPoint) const;		//����ĳ����ָ��Ĺ�Ʊ��Ϣ
	RStockData* hitTestCBItem(const QPoint& ptPoint) const;//����ĳ����ָ���ɫ����Ϣ

private:
	CBlockInfoItem* m_pSelectedStock;

	/*���ڻ��Ʋ����ĳ�Ա����*/
private:
	int m_iTitleHeight;						//ͷ���߶�
	int m_iBottomHeight;					//�ײ��ĸ߶�
	int showStockIndex;						//��ǰ��ʾ����ʼλ�ã��У�

	QRect m_rtHeader;						//ͷ��Header����
	QRect m_rtBottom;						//�ײ�������������������Ϣ

	QTimer m_timerUpdateUI;					//������µ�timer
};

#endif // !B_COLOR_BLOCK_WIDGET_H
