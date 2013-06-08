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
	void updateUI();							//���µ�ǰ��Ҫ��ʾ������

private:
	void clickedBlock(CBlockInfoItem* pItem);				//�������Ʊʱ����

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

	void drawBlock(QPainter& p,const QRect& rtCB,CBlockInfoItem* pItem);	//���Ƶ�ֻ��Ʊ

	QRect rectOfBlock(CBlockInfoItem* pItem);					//��ȡĳֻ��Ʊ��ʾ��λ��
	CBlockInfoItem* hitTestBlock(const QPoint& ptPoint) const;	//����ĳ����ָ��Ĺ�Ʊ��Ϣ
	qRcvFenBiData* hitTestCBItem(const QPoint& ptPoint) const;	//����ĳ����ָ���ɫ����Ϣ

private:
	QList<CBlockInfoItem*> m_listBlocks;		//��ǰ��ʾ�����й�Ʊ�б�
	QMap<CBlockInfoItem*,int> m_mapBlockIndex;	//�������ٲ���ĳֻ��Ʊ���ڵ�����

	//����������ʾ��ColorBlock���ݣ�Ϊ�˿��ٲ��Һͼ���
	QMap<CBlockInfoItem*,QMap<time_t,qRcvFenBiData*>*> mapBlockColorBlocks;

	CBlockInfoItem* m_pSelectedBlock;				//ѡ�е�block
	CBlockInfoItem* m_pCurBlock;					//��ǰ��ʾ��block

	/*���ڻ��Ʋ����ĳ�Ա����*/
private:
	int m_iTitleHeight;						//ͷ���߶�
	int m_iBottomHeight;					//�ײ��ĸ߶�
	int showStockIndex;						//��ǰ��ʾ����ʼλ�ã��У�

	QRect m_rtHeader;						//ͷ��Header����
	QRect m_rtBottom;						//�ײ�������������������Ϣ

	QTimer m_timerUpdateUI;					//������µ�timer

	QVector<QRgb> m_clrTable;				//��ɫ��12��
};

#endif // !B_COLOR_BLOCK_WIDGET_H
