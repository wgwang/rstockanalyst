#pragma once
#include <QtGui>
#include "BaseMarketWidget.h"
#include "RMdiCtrl.h"

class CMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	CMainWindow();
	~CMainWindow();


public:
	bool setupStockDrv();

protected slots:
	void onActiveBaseMarket();		//���ǰ��ʾΪ�������鴰��
	void onActiveTemplate();		//���ǰ��ʾΪ���洰�ڡ�
	void onAddTemplate();			//��Ӱ���

protected:
	virtual bool winEvent( MSG* message, long* result );


protected:
	virtual long OnStockDrvMsg(WPARAM wParam,LPARAM lParam);

private:
	QMdiArea* m_pMdiArea;
	CRMdiSubWindow* m_pSubBaseMarket;
	CRMdiSubWindow* m_pSubTemplate;

	QTabWidget* m_pTemplateWidget;
	CBaseMarketWidget* m_pBaseMarketWidget;
	QMenuBar* m_pMenuBar;
};

