#pragma once
#include <QtGui>
#include "BaseMarketTreeModel.h"

class CBaseMarketWidget : public QWidget
{
	Q_OBJECT
public:
	CBaseMarketWidget();
	~CBaseMarketWidget(void);


public slots:
	void updateBaseMarket();

private:
	//��֤A��
	CBaseMarketTreeModel* m_pModelSHA;
	QTreeView* m_pViewSHA;

	//����ָ��
	CBaseMarketTreeModel* m_pModelSZ;
	QTreeView* m_pViewSZ;

	//��۹���
	CBaseMarketTreeModel* m_pModelHK;
	QTreeView* m_pViewHK;

	//����
	CBaseMarketTreeModel* m_pModelOther;
	QTreeView* m_pViewOther;
};

