#pragma once
#include <QtGui>
#include "BaseMarketWidget.h"

class CMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	CMainWindow();
	~CMainWindow();


public:
	bool setupStockDrv();

protected slots:
	void onAddTemplate();			//��Ӱ���

protected:
	virtual bool winEvent( MSG* message, long* result );


protected:
	virtual long OnStockDrvMsg(WPARAM wParam,LPARAM lParam);

private:
	QTabWidget* m_pMainWidget;
	CBaseMarketWidget* m_pBaseMarketWidget;
	QMenuBar* m_pMenuBar;
};

