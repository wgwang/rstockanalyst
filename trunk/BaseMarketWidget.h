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
	void treeItemClicked(const QModelIndex& index);

private:
	//��֤A��
	CBaseMarketTreeModel* m_pModelSHA;
	QTreeView* m_pViewSHA;

	//����ָ��
	CBaseMarketTreeModel* m_pModelSZ;
	QTreeView* m_pViewSZ;
};

