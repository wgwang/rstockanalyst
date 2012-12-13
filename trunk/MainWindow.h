#pragma once
#include <QtGui>
#include "BaseWidget.h"

class CMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	CMainWindow();
	~CMainWindow();
	static CMainWindow* getMainWindow();


public:
	bool setupStockDrv();
	void initTemplates();			//��ʼ������ģ��
	void saveTemplates();			//��������ģ��

public:
	void clickedStock(const QString& code);
	void clickedBlock(const QString& block);

protected slots:
	void onAddTemplate();			//��Ӱ���
	void onSaveTemplate();			//�������а���
	void onRemoveTemplate();		//ɾ������

	void onSetBlockColor();			//����ɫ�����ɫģʽ

protected:
	virtual bool winEvent( MSG* message, long* result );


protected:
	virtual long OnStockDrvMsg(WPARAM wParam,LPARAM lParam);

private:
	CBaseWidget* getSubWindows(const QString& title);

private:
	static CMainWindow* m_pMainWindow;
	QTabWidget* m_pTabWidget;
	QMenuBar* m_pMenuBar;
	QString m_qsTemplateDir;		//��������ļ����ڵ�Ŀ¼
};

