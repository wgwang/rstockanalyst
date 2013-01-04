#include <QApplication>
#include <iostream>
#include <QtGui>
#include "STKDRV.h"
#include "MainWindow.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "KLineWidget.h"
#include "SplashDlg.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));

	CSplashDlg splash;
	splash.show();

	splash.showMessage(QObject::tr("������ʽ��"),10);
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

	splash.showMessage(QObject::tr("��װ����"),20);
	CSTKDRV::InitStockDrv();

	CMainWindow::getMainWindow()->hide();



	splash.showMessage(QObject::tr("��ʼ������..."),40);
	app.processEvents();
	CDataEngine::importData();				//��ʼ������


	//��ʼ��������
	CMainWindow::getMainWindow()->initTemplates();	//��ʼ��ģ��

	splash.showMessage(QObject::tr("��������..."),99);
	CMainWindow::getMainWindow()->showMaximized();

	//�����������ݽӿ�
	splash.showMessage(QObject::tr("�����������ݽӿ�"),30);
	app.processEvents();
//	if(!CMainWindow::getMainWindow()->setupStockDrv())
	{
//		return app.exit();
	}

	splash.hide();

	app.exec();
	splash.show();

	splash.showMessage(QObject::tr("���ڱ�������..."),10);
	CMainWindow::getMainWindow()->saveTemplates();	//��������ģ��
	splash.showMessage(QObject::tr("���ڵ�������..."),20);

	CDataEngine::exportData();				//��������
	splash.showMessage(QObject::tr("�����������..."),99);


	//�ͷ���Դ
	delete CMainWindow::getMainWindow();
	CDataEngine::releaseDataEngine();

	return 1;
}
