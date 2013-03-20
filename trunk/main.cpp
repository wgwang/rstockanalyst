#include <QApplication>
#include <iostream>
#include <QtGui>
#include "STKDRV.h"
#include "MainWindow.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "KLineWidget.h"
#include "SplashDlg.h"

lua_State* g_Lua = 0;
luaL_Reg* g_pFuncs = 0;
QString g_native = "";

int loadAllFunc()
{
	int iDllCount = 0;
	QMap<const char*,lua_CFunction> g_func;

	QDir dirFunc(qApp->applicationDirPath() + "/plugin/");
	QFileInfoList list = dirFunc.entryInfoList(QStringList()<<"*.dll");
	foreach (const QFileInfo& v,list)
	{
		HINSTANCE hDll = LoadLibrary(v.absoluteFilePath().toStdWString().data());
		if(hDll)
		{
			qDebug()<<"Load form "<<v.absoluteFilePath();
			QMap<const char*,lua_CFunction>* (WINAPI* pfnALlFuncs)()= NULL;
			pfnALlFuncs = \
				(QMap<const char*,lua_CFunction>*(WINAPI *)()) GetProcAddress(hDll,"ExportAllFuncs");
			if(pfnALlFuncs)
			{
				QMap<const char*,lua_CFunction>* _funcs = (*pfnALlFuncs)();

				g_func.unite(*_funcs);
				qDebug()<<"Load funcs from\""<<v.baseName()<<"\":";
				delete _funcs;
			}
			else
			{
				qDebug()<<"Load Dll Error!";
			}
			//FreeLibrary(hDll);
		}
	}

	QMap<const char*,lua_CFunction>::iterator iter = g_func.begin();
	g_pFuncs= new luaL_Reg[g_func.size()+1];
	luaL_Reg* pFunc = g_pFuncs;
	qDebug()<<"---------Funcs-----------";
	while(iter!=g_func.end())
	{
		qDebug()<<iter.key();
		pFunc->name = iter.key();
		pFunc->func = iter.value();
		//			lua_register(m_pL,iter.key().toAscii(),iter.value());
		++iter;
		++pFunc;
	}
	pFunc->name = NULL;
	pFunc->func = NULL;
	return iDllCount;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	{
		QFile file(qApp->applicationDirPath()+"/native.lua");
		if(file.open(QFile::ReadOnly))
		{
			g_native = file.readAll();
		}
	}

	g_Lua = luaL_newstate();			//ʵ����Lua
	loadAllFunc();


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
