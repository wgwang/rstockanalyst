#include "StdAfx.h"
#include "MainWindow.h"
#include "STKDRV.h"
#include "DataEngine.h"
#include "KLineWidget.h"
#include "BlockColorSettingDlg.h"
#include "FuncHelper.h"
#include "SplashDlg.h"
#include "WatcherSettingDlg.h"
#include "ConfigSettings.h"
#include "AllStockDialog.h"

#define RSTOCK_ANALYST_MAINMSG (WM_USER+1)
#define	EXPORT_TIME_TAG		"ExportTime"
#define CURRENT_TEMP_TAG	"CurrentTemp"

CMainWindow* CMainWindow::m_pMainWindow = 0;

CMainWindow* CMainWindow::getMainWindow()
{
	if(m_pMainWindow == 0)
		m_pMainWindow = new CMainWindow();

	return m_pMainWindow;
}


CMainWindow::CMainWindow()
	: QMainWindow()
	, m_pBlockMenuWidget(0)
	, m_iBlockMenuCmd(0)
	, m_bExportClose(true)
	, m_tmExportClose(15,5)
{
	setWindowIcon(QIcon(":/res/icon.png"));
	m_pTabWidget = new QTabWidget();
	setCentralWidget(m_pTabWidget);
	m_qsTemplateDir = qApp->applicationDirPath()+"/config/template/";
	QDir().mkpath(m_qsTemplateDir);
	{
		//初始化Menu
		m_pMenuBar = new QMenuBar(this);
		//版面管理
		QMenu* pMenuTemplate = m_pMenuBar->addMenu(tr("版面管理"));
		pMenuTemplate->addAction(tr("添加版面"),this,SLOT(onAddTemplate()));
		pMenuTemplate->addAction(tr("保存所有"),this,SLOT(onSaveTemplate()));
		pMenuTemplate->addAction(tr("删除当前版面"),this,SLOT(onRemoveTemplate()));
		setMenuBar(m_pMenuBar);

		//设置
		QMenu* pMenuSettings = m_pMenuBar->addMenu(tr("设置"));
		pMenuSettings->addAction(tr("设置色块颜色"),CBlockColorSettingDlg::getDialog(),SLOT(exec()));
		pMenuSettings->addAction(tr("监视雷达设置"),this,SLOT(onWatcherSetting()));
		pMenuSettings->addAction(tr("收盘后作业设置"),this,SLOT(onMarketCloseSetting()));
		pMenuSettings->addAction(tr("立即进行收盘作业"),this,SLOT(onMarketClose()));
		pMenuSettings->addAction(tr("显示所有的股票"),this,SLOT(onShowAllStocks()));

		//帮助
		QMenu* pMenuHelp = m_pMenuBar->addMenu(tr("帮助"));
		pMenuHelp->addAction(tr("函数说明"),this,SLOT(onShowFuncHelper()));
	}

	connect(&m_timerClose,SIGNAL(timeout()),this,SLOT(onMarketCloseTimer()));
	m_timerClose.start(60000);

	//定时计算板块数据
	connect(&m_timerCalcBlock,SIGNAL(timeout()),this,SLOT(onCalcBlock()));

	//获取导出时间
	QVariant tmExport = DilideCode::DConfigSettings::getConfigSettings()->getValue(EXPORT_TIME_TAG);
	if(!tmExport.toString().isEmpty())
	{
		m_tmExportClose = tmExport.toTime();
	}
}

CMainWindow::~CMainWindow()
{
	delete m_pTabWidget;
	delete m_pMenuBar;
}

bool CMainWindow::setupStockDrv()
{
	int iMode = RCV_WORK_MEMSHARE;
	if(CSTKDRV::GetStockDrvInfo(RI_V2SUPPORT,NULL))
		iMode = RCV_WORK_SENDMSG;
	if(CSTKDRV::Stock_Init(this->winId(),RSTOCK_ANALYST_MAINMSG,iMode)>0)
	{
		char szBuf[MAX_PATH];

		CSTKDRV::GetStockDrvInfo(RI_IDSTRING,szBuf);
		QString qsDrvInfo = QString::fromLocal8Bit(szBuf);

		CSTKDRV::GetStockDrvInfo(RI_VERSION,szBuf);
		qsDrvInfo += QString::fromLocal8Bit(szBuf);

		qDebug()<<"驱动程序信息——"<<qsDrvInfo;

		if(CSTKDRV::SetupReceiver(TRUE)>0)
		{
			return true;
		}
	}

	return false;
}

void CMainWindow::initTemplates()
{
	QDir dir(m_qsTemplateDir);
	QFileInfoList list = dir.entryInfoList(QDir::Files,QDir::Time);
	foreach(const QFileInfo& info,list)
	{
		QFile file(info.absoluteFilePath());
		if(!file.open(QFile::ReadOnly))
			continue;
		QDomDocument doc;
		doc.setContent(file.readAll());
		file.close();

		QDomElement eleRoot = doc.firstChildElement("ROOT");
		if(!eleRoot.isElement())
			continue;

		QString qsTitle = eleRoot.attribute("title");
		if(!getSubWindows(qsTitle))
		{
			CBaseWidget* pWidget = new CBaseWidget(0);
			QDomElement eleWidget = eleRoot.firstChildElement("widget");
			if(eleWidget.isElement())
			{
				pWidget->clearChildren();
				pWidget->loadPanelInfo(eleWidget);
			}
			m_pTabWidget->addTab(pWidget,qsTitle);
		}
	}

	//初始化当前选中的模板
	QString qsTemp = DilideCode::DConfigSettings::getConfigSettings()->getValue(CURRENT_TEMP_TAG).toString();
	if(!qsTemp.isEmpty())
	{
		for(int i=0;i<m_pTabWidget->count();++i)
		{
			if(m_pTabWidget->tabText(i) == qsTemp)
			{
				m_pTabWidget->setCurrentIndex(i);
			}
		}
	}
}

void CMainWindow::saveTemplates()
{
	for(int i=0;i<m_pTabWidget->count();++i)
	{
		CBaseWidget* pWidget = reinterpret_cast<CBaseWidget*>(m_pTabWidget->widget(i));
		QString qsTitle = m_pTabWidget->tabText(i);
		QDomDocument doc("template");
		QDomElement eleRoot = doc.createElement("ROOT");
		eleRoot.setAttribute("title",qsTitle);
		doc.appendChild(eleRoot);
		QDomElement eleWidget = doc.createElement("widget");
		eleRoot.appendChild(eleWidget);
		pWidget->savePanelInfo(doc,eleWidget);

		QFile file(m_qsTemplateDir+qsTitle+".xml");
		if(!file.open(QFile::Truncate|QFile::WriteOnly))
		{
			QMessageBox::warning(this,tr("错误"),QString("保存模板'%1'失败！").arg(qsTitle));
			continue;
		}
		file.write(doc.toByteArray());
		file.close();
	}

	//保存当前选中的模板title
	int iIndex = m_pTabWidget->currentIndex();
	if(iIndex>-1)
	{
		DilideCode::DConfigSettings::getConfigSettings()->setValue(CURRENT_TEMP_TAG,m_pTabWidget->tabText(iIndex));
	}
}

void CMainWindow::clickedStock( const QString& code )
{
	CBaseWidget* pWidget = reinterpret_cast<CBaseWidget*>(m_pTabWidget->currentWidget());
	if(pWidget)
	{
		pWidget->setStockCode(code);
	}
}

void CMainWindow::clickedBlock( const QString& block )
{
	CBaseWidget* pWidget = reinterpret_cast<CBaseWidget*>(m_pTabWidget->currentWidget());
	if(pWidget)
	{
		pWidget->setBlock(block);
	}
}

bool CMainWindow::winEvent( MSG* message, long* result )
{
	if(message->message == RSTOCK_ANALYST_MAINMSG)
	{
		*result = OnStockDrvMsg(message->wParam,message->lParam);
	}

	return QMainWindow::winEvent(message,result);
}

long CMainWindow::OnStockDrvMsg( WPARAM wParam,LPARAM lParam )
{
	RCV_DATA* pHeader;
	pHeader = (RCV_DATA*)lParam;

	//  对于处理速度慢的数据类型,最好将 pHeader->m_pData 指向的数据备份,再作处理

	/*
		补分笔和补分时其实补的都是分笔数据，只是补分时补的每分钟的最后一笔，而补分笔补的每分钟的所有分笔数据。
	*/

	switch( wParam )
	{
	case RCV_REPORT:                        // 共享数据引用方式,股票行情
		{
			qDebug()<< "####Comming reports"<<QTime::currentTime().toString()<<"####";
			if(pHeader->m_nPacketNum<1)
				break;

			RCV_REPORT_STRUCTExV3* pReport = pHeader->m_pReportV3;
			for(int i=0;i<pHeader->m_nPacketNum;++i)
			{
				pReport = (pHeader->m_pReportV3+i);

				QString qsCode = QString::fromLocal8Bit(pReport->m_szLabel);
				QString qsOnly = qsCode + CDataEngine::getMarketStr(pReport->m_wMarket);

				CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsOnly);
				if(pItem==NULL)
				{
					pItem = new CStockInfoItem(qsCode,pReport->m_wMarket);
					CDataEngine::getDataEngine()->setStockInfoItem(pItem);
				}

				pItem->setReport(pReport);
			}
		}
		break;
	case RCV_FILEDATA:                      // 共享数据引用方式,文件
		{
			switch(pHeader->m_wDataType)
			{
			case FILE_HISTORY_EX:               // 补日线数据
				{
					QTime timeBegin = QTime::currentTime();
					RCV_HISTORY_STRUCTEx* pHistory = pHeader->m_pDay;
					QString qsOnly;

					QList<qRcvHistoryData*> listHistory;
 					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pHistory = (pHeader->m_pDay+i);
						if(pHistory->m_head.m_dwHeadTag == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsOnly);
							if(pItem==NULL)
							{
								//删除指针，防止内存泄漏
								foreach(qRcvHistoryData* p,listHistory)
									delete p;
							}
							else
							{
								pItem->appendHistorys(listHistory);
							}
							qsOnly = QString::fromLocal8Bit(pHistory->m_head.m_szLabel)
								+ CDataEngine::getMarketStr(pHistory->m_head.m_wMarket);
							listHistory.clear();
						}
						else
						{
							listHistory.append(new qRcvHistoryData(pHistory));
						}
					}

					CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsOnly);
					if(pItem==NULL)
					{
						//删除指针，防止内存泄漏
						foreach(qRcvHistoryData* p,listHistory)
							delete p;
					}
					else
					{
						pItem->appendHistorys(listHistory);
					}

					qDebug()<<"Packet cout:"<<pHeader->m_nPacketNum;
					qDebug()<<"UseTime:"<<qAbs(QTime::currentTime().msecsTo(timeBegin))<<"m secs";
				}
				break;

			case FILE_MINUTE_EX:                // 补分钟线数据
				{
					qDebug()<<"Minute Packet cout:"<<pHeader->m_nPacketNum;

					RCV_MINUTE_STRUCTEx* pMinute = pHeader->m_pMinute;
					QString qsOnly;

					QList<qRcvFenBiData*> listFenBis;
					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pMinute = (pHeader->m_pMinute+i);

						if(pMinute->m_time == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsOnly);
							if(pItem==NULL)
							{
								//删除指针，防止内存泄漏
								foreach(qRcvFenBiData* p,listFenBis)
									delete p;
							}
							else
							{
								pItem->appendFenBis(listFenBis);
								pItem->recalcMinData();
								pItem->recalc5MinData();
								m_timerCalcBlock.start(30000);
							}
							qsOnly = QString::fromLocal8Bit(pMinute->m_head.m_szLabel)
								+ CDataEngine::getMarketStr(pMinute->m_head.m_wMarket);
							listFenBis.clear();
						}
						else
						{
							listFenBis.append(new qRcvFenBiData(pMinute));
						}
					}
				}
				break;

			case FILE_BASE_EX:                  // 钱龙兼容基本资料文件,m_szFileName仅包含文件名
				{
					QString qsF10Title = QString::fromLocal8Bit(pHeader->m_File.m_szFileName);
					QString qsContent = QString::fromLocal8Bit((char*)pHeader->m_pData,pHeader->m_File.m_dwLen);
					qDebug()<<"Comming F10 "<<qsF10Title;
					CDataEngine::getDataEngine()->appendF10(qsF10Title,qsContent);
				}
				break;

			case FILE_NEWS_EX:                  // 新闻类,其类型由m_szFileName中子目录名来定
				{
					QString qsNewsTitle = QString::fromLocal8Bit(pHeader->m_File.m_szFileName);
					QString qsContent = QString::fromLocal8Bit((char*)pHeader->m_pData,pHeader->m_File.m_dwLen);
					qDebug()<<"Comming News "<< qsNewsTitle;
					CDataEngine::getDataEngine()->appendNews(qsNewsTitle,qsContent);
				}
				break;
			case FILE_POWER_EX:
				{
					qDebug()<<"Power Packet cout:"<<pHeader->m_nPacketNum;

					RCV_POWER_STRUCTEx* pPower = pHeader->m_pPower;
					QString qsOnly;

					QList<qRcvPowerData*> listPower;
					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pPower = (pHeader->m_pPower+i);

						if(pPower->m_time == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsOnly);
							if(pItem==NULL)
							{
								//删除指针，防止内存泄漏
								foreach(qRcvPowerData* p,listPower)
									delete p;
							}
							else
							{
								pItem->appendPowers(listPower);
							}
							qsOnly = QString::fromLocal8Bit(pPower->m_head.m_szLabel)
								+ CDataEngine::getMarketStr(pPower->m_head.m_wMarket);
							listPower.clear();
						}
						else
						{
							listPower.append(new qRcvPowerData(pPower));
						}
					}
				}
				break;
			case FILE_5MINUTE_EX:
				{
					//历史5分钟数据
					qDebug()<<"5 Minute Packet cout:"<<pHeader->m_nPacketNum;
					RCV_HISTORY_STRUCTEx* pMinute5 = pHeader->m_pDay;
					QString qsOnly;

					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pMinute5 = (pHeader->m_pDay+i);

						if(pMinute5->m_time == EKE_HEAD_TAG)
						{
							//数据头
							qsOnly = QString::fromLocal8Bit(pMinute5->m_head.m_szLabel)
								+ CDataEngine::getMarketStr(pMinute5->m_head.m_wMarket);
						}
						else
						{
							//实体数据
						}
					}
				}
				break;
			default:
				{
					int i = 0;
				}
				break;
			}
		}
		break;
	case RCV_FENBIDATA:
		{
			RCV_FENBI* pFb = reinterpret_cast<RCV_FENBI*>(lParam);

			qDebug()<< "FenBi Packet count:"<<pFb->m_nCount;
			QDateTime tmNow = QDateTime::currentDateTime();
			QString qsOnly = QString::fromLocal8Bit(pFb->m_szLabel) + CDataEngine::getMarketStr(pFb->m_wMarket);
			int iCount = 0;
			RCV_FENBI_STRUCTEx* p = pFb->m_Data;
			QList<qRcvFenBiData*> listFenBis;
			CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsOnly);
			if(pItem)
			{
				while(iCount<pFb->m_nCount)
				{
					//数据处理
					listFenBis.append(new qRcvFenBiData(p,pFb->m_lDate));

					++p;
					++iCount;
				}
				pItem->appendFenBis(listFenBis);
				pItem->recalcMinData();
				pItem->recalc5MinData();
				m_timerCalcBlock.start(30000);
			}
			qDebug()<<"Using Time: "<<tmNow.msecsTo(QDateTime::currentDateTime());
		}
		break;

	default:
		return 0;                           // unknown data
	}
	return 1;
}

void CMainWindow::onAddTemplate()
{
	QDialog dlg(this);
	QVBoxLayout layout;
	dlg.setLayout(&layout);
	QLineEdit edit;
	layout.addWidget(&edit);
	QPushButton btnOk(tr("确定"));
	layout.addWidget(&btnOk);
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted == dlg.exec())
	{
		QString qsName = edit.text().trimmed();
		if(!getSubWindows(qsName))
		{
			m_pTabWidget->addTab(new CBaseWidget(0),qsName);
//			m_pMdiArea->setActiveSubWindow(pSubWin);
		}
		else
		{
			QMessageBox::information(this,tr("提示"),tr("该模板名称已经存在！"));
		}
	}
}

void CMainWindow::onSaveTemplate()
{
	saveTemplates();
}

void CMainWindow::onRemoveTemplate()
{
	int iCurIndex = m_pTabWidget->currentIndex();
	QString qsText = m_pTabWidget->tabText(iCurIndex);
	if(QFile::remove(m_qsTemplateDir+qsText+".xml"))
	{
		QWidget* pWidget = m_pTabWidget->widget(iCurIndex);
		m_pTabWidget->removeTab(iCurIndex);
		delete pWidget;
	}
}

void CMainWindow::onMarketCloseSetting()
{
	QDialog dlg(this);
	QVBoxLayout layout;
	dlg.setLayout(&layout);
	QTimeEdit edit;
	//初始值15:05:00
	edit.setTime(m_tmExportClose);
	layout.addWidget(&edit);
	QPushButton btnOk(tr("确定"));
	layout.addWidget(&btnOk);
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted == dlg.exec())
	{
		m_tmExportClose = edit.time();
		DilideCode::DConfigSettings::getConfigSettings()->setValue(EXPORT_TIME_TAG,m_tmExportClose);
	}
}

void CMainWindow::onMarketClose()
{
	CSplashDlg::getSplashDlg()->showMessage("正在进行收盘后数据整理...");
	CDataEngine::exportCloseData();
	CSplashDlg::getSplashDlg()->hide();
}

void CMainWindow::onMarketCloseTimer()
{
	QDateTime tmDataEngine = QDateTime::fromTime_t(CDataEngine::getCurrentTime());
	QDateTime tmCurrent = QDateTime::currentDateTime();
	if(tmCurrent.date() == tmDataEngine.date())
	{
		//15:05后导出收盘后的数据
		if(tmCurrent.time()>m_tmExportClose)
		{
			if(!m_bExportClose)
			{
				onMarketClose();
				m_bExportClose = true;
			}
		}
		else if(tmCurrent.time().hour()>12&&tmCurrent.time().hour()<15)
		{
			m_bExportClose = false;
		}
	}
}

void CMainWindow::onShowFuncHelper()
{
	CFuncHelper dlg(this);
	dlg.exec();
}

void CMainWindow::onWatcherSetting()
{
	CWatcherSettingDlg dlg(this);
	dlg.exec();
}


CBaseWidget* CMainWindow::getSubWindows( const QString& title )
{
	for(int i=0;i<m_pTabWidget->count();++i)
	{
		if(m_pTabWidget->tabText(i) == title)
			return reinterpret_cast<CBaseWidget*>(m_pTabWidget->widget(i));
	}

	return 0;
}

void CMainWindow::initBlockMenus()
{
	m_menuBlocks.setTitle("板块菜单");
	QList<CBlockInfoItem*> listBlocks = CDataEngine::getDataEngine()->getTopLevelBlocks();
	foreach(CBlockInfoItem* _block,listBlocks)
	{
		QAction* pActBlock = 0;
		if(_block->hasBlocks())
		{
			QMenu* pMenuBlock = new QMenu(_block->getBlockName());
			pActBlock = m_menuBlocks.addMenu(pMenuBlock);
			CreateSubBlockMenu(pMenuBlock,_block);
		}
		else
		{
			pActBlock = m_menuBlocks.addAction(_block->getBlockName());
		}
		pActBlock->setData((uint)_block);
		connect(pActBlock,SIGNAL(triggered()),this,SLOT(onBlockActClicked()));
		mapBlockMenus[_block] = pActBlock;
	}
}

void CMainWindow::onBlockActClicked()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	CBlockInfoItem* pBlock = reinterpret_cast<CBlockInfoItem*>(pAct->data().toUInt());
	if(m_pBlockMenuWidget)
	{
		m_pBlockMenuWidget->onBlockClicked(pBlock,m_iBlockMenuCmd);
	}
}

void CMainWindow::CreateSubBlockMenu( QMenu* pMenuParent,CBlockInfoItem* pBlockParent )
{
	QList<CBlockInfoItem*> listBlocks = pBlockParent->getBlockList();
	foreach(CBlockInfoItem* _block,listBlocks)
	{
		QAction* pActBlock = 0;
		if(_block->hasBlocks())
		{
			QMenu* pMenuBlock = new QMenu(_block->getBlockName());
			pActBlock = pMenuParent->addMenu(pMenuBlock);
			CreateSubBlockMenu(pMenuBlock,_block);
		}
		else
		{
			pActBlock = pMenuParent->addAction(_block->getBlockName());
		}
		pActBlock->setData((uint)_block);
		connect(pActBlock,SIGNAL(triggered()),this,SLOT(onBlockActClicked()));
		mapBlockMenus[_block] = pActBlock;
	}
}

void CMainWindow::onShowAllStocks()
{
	CAllStockDialog dlg;
	dlg.exec();
}

void CMainWindow::onCalcBlock()
{
	CSplashDlg::getSplashDlg()->showMessage(tr("正在重新计算板块数据..."),50);
	QList<CBlockInfoItem*> listBlocks = CDataEngine::getDataEngine()->getStockBlocks();
	foreach(CBlockInfoItem* pItem,listBlocks)
	{
		pItem->recalc5MinData();
		pItem->recalcMinData();
	}
	m_timerCalcBlock.stop();
	CSplashDlg::getSplashDlg()->hide();
}
