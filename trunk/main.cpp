#include <QApplication>
#include <iostream>
#include <QtGui>
#include "STKDRV.h"
#include "MainWindow.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "KLineWidget.h"
#include "SplashDlg.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

static int pppp(lua_State* _L)
{
	int n = lua_gettop(_L);
	const char* aaa = lua_tostring(_L,0);
	lua_getglobal(_L,"QString");
	QString* p = (QString*)(lua_topointer(_L,-1));
	qDebug()<<*p;
	return 1;
	for (int i=1;i<=n;++i)
	{
		int _type = lua_type(_L,i);
		printf(lua_tostring(_L,i));
	}
	return 1;
}

int main(int argc, char *argv[])
{
	lua_CFunction pFun = &pppp;
	lua_State* L;
	L = luaL_newstate();
	QString qsTitle = "test for pointer to lua";

	lua_register(L,"print",pFun);
	lua_pushlightuserdata(L,&qsTitle);
	lua_setglobal(L,"QString");
	luaL_dostring(L,"print(\"Lua worked!!!\\r\\n\")");
//	luaL_dostring(L,"print(math.abs(-33))");
	//luaL_error(L,aaa);
	//qDebug()<<QString::fromLocal8Bit(aaa);
	lua_close(L);
	return 0;


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
	if(!CMainWindow::getMainWindow()->setupStockDrv())
	{
		return app.exit();
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
