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
	void onActiveBaseMarket();		//激活当前显示为基本行情窗口
	void onActiveTemplate();		//激活当前显示为版面窗口。
	void onAddTemplate();			//添加版面

protected:
	virtual bool winEvent( MSG* message, long* result );


protected:
	virtual long OnStockDrvMsg(WPARAM wParam,LPARAM lParam);

private:
	QMdiArea* m_pMdiArea;
	QTabWidget* m_pTemplateWidget;
	CBaseMarketWidget* m_pBaseMarketWidget;
	QMenuBar* m_pMenuBar;
};

