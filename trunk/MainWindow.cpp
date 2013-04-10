#include "StdAfx.h"
#include "MainWindow.h"
#include "STKDRV.h"
#include "DataEngine.h"
#include "KLineWidget.h"
#include "BlockColorSettingDlg.h"
#include "FuncHelper.h"

#define RSTOCK_ANALYST_MAINMSG (WM_USER+1)

CMainWindow* CMainWindow::m_pMainWindow = 0;

CMainWindow* CMainWindow::getMainWindow()
{
	if(m_pMainWindow == 0)
		m_pMainWindow = new CMainWindow();

	return m_pMainWindow;
}


CMainWindow::CMainWindow()
	: QMainWindow()
{
	setWindowIcon(QIcon(":/res/icon.png"));
	m_pTabWidget = new QTabWidget();
	setCentralWidget(m_pTabWidget);
	m_qsTemplateDir = qApp->applicationDirPath()+"/config/template/";
	QDir().mkpath(m_qsTemplateDir);
	{
		//��ʼ��Menu
		m_pMenuBar = new QMenuBar(this);
		//�������
		QMenu* pMenuTemplate = m_pMenuBar->addMenu(tr("�������"));
		pMenuTemplate->addAction(tr("��Ӱ���"),this,SLOT(onAddTemplate()));
		pMenuTemplate->addAction(tr("��������"),this,SLOT(onSaveTemplate()));
		pMenuTemplate->addAction(tr("ɾ����ǰ����"),this,SLOT(onRemoveTemplate()));
		setMenuBar(m_pMenuBar);

		//����
		QMenu* pMenuSettings = m_pMenuBar->addMenu(tr("����"));
		pMenuSettings->addAction(tr("����ɫ����ɫ"),CBlockColorSettingDlg::getDialog(),SLOT(exec()));
		pMenuSettings->addAction(tr("���̺���������"),this,SLOT(onSaveDataEngine()));

		//����
		QMenu* pMenuHelp = m_pMenuBar->addMenu(tr("����"));
		pMenuHelp->addAction(tr("����˵��"),this,SLOT(onShowFuncHelper()));
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

		qDebug()<<"����������Ϣ����"<<qsDrvInfo;

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
			QMessageBox::warning(this,tr("����"),QString("����ģ��'%1'ʧ�ܣ�").arg(qsTitle));
			continue;
		}
		file.write(doc.toByteArray());
		file.close();
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

	//  ���ڴ����ٶ�������������,��ý� pHeader->m_pData ָ������ݱ���,��������

	/*
		���ֱʺͲ���ʱ��ʵ���Ķ��Ƿֱ����ݣ�ֻ�ǲ���ʱ����ÿ���ӵ����һ�ʣ������ֱʲ���ÿ���ӵ����зֱ����ݡ�
	*/

	switch( wParam )
	{
	case RCV_REPORT:                        // �����������÷�ʽ,��Ʊ����
		{
			qDebug()<< "####Comming reports"<<QTime::currentTime().toString()<<"####";
			if(pHeader->m_nPacketNum<1)
				break;

			RCV_REPORT_STRUCTExV3* pReport = pHeader->m_pReportV3;
			for(int i=0;i<pHeader->m_nPacketNum;++i)
			{
				pReport = (pHeader->m_pReportV3+i);

				QString qsCode = QString::fromLocal8Bit(pReport->m_szLabel);

				CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
				if(pItem==NULL)
				{
					pItem = new CStockInfoItem(qsCode,pReport->m_wMarket);
					CDataEngine::getDataEngine()->setStockInfoItem(pItem);
				}

				pItem->setReport(pReport);
			}
		}
		break;
	case RCV_FILEDATA:                      // �����������÷�ʽ,�ļ�
		{
			switch(pHeader->m_wDataType)
			{
			case FILE_HISTORY_EX:               // ����������
				{
					QTime timeBegin = QTime::currentTime();
					RCV_HISTORY_STRUCTEx* pHistory = pHeader->m_pDay;
					QString qsCode;

					QList<qRcvHistoryData*> listHistory;
 					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pHistory = (pHeader->m_pDay+i);
						if(pHistory->m_head.m_dwHeadTag == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
							if(pItem==NULL)
							{
								//ɾ��ָ�룬��ֹ�ڴ�й©
								foreach(qRcvHistoryData* p,listHistory)
									delete p;
							}
							else
							{
								pItem->appendHistorys(listHistory);
							}
							qsCode = QString::fromLocal8Bit(pHistory->m_head.m_szLabel);
							listHistory.clear();
						}
						else
						{
							listHistory.append(new qRcvHistoryData(pHistory));
						}
					}

					CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
					if(pItem==NULL)
					{
						//ɾ��ָ�룬��ֹ�ڴ�й©
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

			case FILE_MINUTE_EX:                // ������������
				{
					qDebug()<<"Minute Packet cout:"<<pHeader->m_nPacketNum;

					RCV_MINUTE_STRUCTEx* pMinute = pHeader->m_pMinute;
					QString qsCode;

					QList<qRcvFenBiData*> listFenBis;
					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pMinute = (pHeader->m_pMinute+i);

						if(pMinute->m_time == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
							if(pItem==NULL)
							{
								//ɾ��ָ�룬��ֹ�ڴ�й©
								foreach(qRcvFenBiData* p,listFenBis)
									delete p;
							}
							else
							{
								pItem->appendFenBis(listFenBis);
							}
							qsCode = QString::fromLocal8Bit(pMinute->m_head.m_szLabel);
							listFenBis.clear();
						}
						else
						{
							listFenBis.append(new qRcvFenBiData(pMinute));
						}
					}
				}
				break;

			case FILE_BASE_EX:                  // Ǯ�����ݻ��������ļ�,m_szFileName�������ļ���
				{
					QString qsF10Title = QString::fromLocal8Bit(pHeader->m_File.m_szFileName);
					QString qsContent = QString::fromLocal8Bit((char*)pHeader->m_pData,pHeader->m_File.m_dwLen);
					qDebug()<<"Comming F10 "<<qsF10Title;
					CDataEngine::getDataEngine()->appendF10(qsF10Title,qsContent);
				}
				break;

			case FILE_NEWS_EX:                  // ������,��������m_szFileName����Ŀ¼������
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
					QString qsCode;

					QList<qRcvPowerData*> listPower;
					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pPower = (pHeader->m_pPower+i);

						if(pPower->m_time == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
							if(pItem==NULL)
							{
								//ɾ��ָ�룬��ֹ�ڴ�й©
								foreach(qRcvPowerData* p,listPower)
									delete p;
							}
							else
							{
								pItem->appendPowers(listPower);
							}
							qsCode = QString::fromLocal8Bit(pPower->m_head.m_szLabel);
							listPower.clear();
						}
						else
						{
							listPower.append(new qRcvPowerData(pPower));
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
			QString qsCode = QString::fromLocal8Bit(pFb->m_szLabel);
			int iCount = 0;
			RCV_FENBI_STRUCTEx* p = pFb->m_Data;
			QList<qRcvFenBiData*> listFenBis;
			CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
			if(pItem)
			{
				while(iCount<pFb->m_nCount)
				{
					//���ݴ���
					listFenBis.append(new qRcvFenBiData(p,pFb->m_lDate));

					++p;
					++iCount;
				}
				pItem->appendFenBis(listFenBis);
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
	QPushButton btnOk(tr("ȷ��"));
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
			QMessageBox::information(this,tr("��ʾ"),tr("��ģ�������Ѿ����ڣ�"));
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
		m_pTabWidget->removeTab(iCurIndex);
}

void CMainWindow::onSaveDataEngine()
{
	CDataEngine::exportCloseData();
}

void CMainWindow::onShowFuncHelper()
{
	CFuncHelper dlg(this);
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
