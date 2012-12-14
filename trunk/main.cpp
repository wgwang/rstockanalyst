#include <QApplication>
#include <iostream>
#include <QtGui>
#include "STKDRV.h"
#include "MainWindow.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "KLineWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));

	QFile file(":/res/qss/FeiHu.qss");
	if(file.open(QFile::ReadOnly))
	{
		QString qsStyleSheet = file.readAll();
		app.setStyleSheet(qsStyleSheet);
	}

	//��ʼ����ɫ��
	CColorManager::initAll();
	//��ʼ��js�ű�
	CKLineWidget::initJSScript();

	CSTKDRV::InitStockDrv();

	CMainWindow::getMainWindow()->showMaximized();


	//�����������ݽӿ�
	if(!CMainWindow::getMainWindow()->setupStockDrv())
	{
		return app.exit();
	}
	

	CDataEngine::importData();				//��ʼ������

	CMainWindow::getMainWindow()->initTemplates();	//��ʼ��ģ��
	app.exec();
	CMainWindow::getMainWindow()->saveTemplates();	//��������ģ��

	CDataEngine::exportData();				//��������


	return 1;
}
