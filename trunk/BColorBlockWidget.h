#ifndef B_COLOR_BLOCK_WIDGET_H
#define	B_COLOR_BLOCK_WIDGET_H

#include "CoordXBaseWidget.h"
#include "BlockInfoItem.h"

class CBColorBlockWidget : public CCoordXBaseWidget
{
	Q_OBJECT
public:
	CBColorBlockWidget(CBaseWidget* parent = 0);
	~CBColorBlockWidget(void);


	/*���ڻ��Ʋ����ĳ�Ա����*/
private:
	int m_iTitleHeight;						//ͷ���߶�
	int m_iCBHeight;						//����ɫ��ĸ߶�
	int m_iCBWidth;							//����ɫ��Ŀ��
	int m_iBottomHeight;					//�ײ��ĸ߶�
	int showStockIndex;						//��ǰ��ʾ����ʼλ�ã��У�

	QRect m_rtHeader;						//ͷ��Header����
	QRect m_rtClient;						//ʵ��ɫ���������
	QRect m_rtBottom;						//�ײ�������������������Ϣ
};

#endif // !B_COLOR_BLOCK_WIDGET_H
