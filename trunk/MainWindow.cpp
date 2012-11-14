#include "StdAfx.h"
#include "MainWindow.h"
#include "STKDRV.h"
#include "BaseMarketWidget.h"
#include "DataEngine.h"
#include "KLineWidget.h"


#define RSTOCK_ANALYST_MAINMSG (WM_USER+1)

CMainWindow::CMainWindow()
	: QMainWindow()
{
	{
		//��ʼ������
		m_pMdiArea = new QMdiArea();
		m_pMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		m_pMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		m_pMdiArea->setViewMode(QMdiArea::TabbedView);
		setCentralWidget(m_pMdiArea);

		//��Ӷ���洰��
		m_pTemplateWidget = new QTabWidget(this);
		m_pSubTemplate = new CRMdiSubWindow(m_pMdiArea);
		m_pSubTemplate->setWindowTitle(tr("���Ի�����"));
		m_pSubTemplate->setWidget(m_pTemplateWidget);

		//��ӻ������鴰��
		m_pBaseMarketWidget = new CBaseMarketWidget;
		m_pSubBaseMarket = new CRMdiSubWindow(m_pMdiArea);
		m_pSubBaseMarket->setWindowTitle(tr("��������"));
		m_pSubBaseMarket->setWidget(m_pBaseMarketWidget);
		m_pSubBaseMarket->showMaximized();
	//	m_pMdiArea->setActiveSubWindow(m_pSubBaseMarket);
	}

	{
		//��ʼ��Menu
		m_pMenuBar = new QMenuBar(this);
		//��������
		QMenu* pMenuBaseMarket = m_pMenuBar->addMenu(tr("��������"));
		//�������
		QMenu* pMenuTemplate = m_pMenuBar->addMenu(tr("�������"));
		pMenuTemplate->addAction(tr("��Ӱ���"),this,SLOT(onAddTemplate()));
		setMenuBar(m_pMenuBar);

		//��ͼ�˵���������SubWindow����ʾ���
		QMenu* pMenuView = m_pMenuBar->addMenu(tr("��ͼ"));
		pMenuView->addAction(tr("��������"),this,SLOT(onActiveBaseMarket()));
		pMenuView->addAction(tr("�������"),this,SLOT(onActiveTemplate()));
	}

}

CMainWindow::~CMainWindow()
{

}

bool CMainWindow::setupStockDrv()
{
	if(CSTKDRV::Stock_Init(this->winId(),RSTOCK_ANALYST_MAINMSG,RCV_WORK_MEMSHARE)>0)
	{
		if(CSTKDRV::SetupReceiver(TRUE)>0)
		{
			return true;
		}
	}

	return false;
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

					QList<qRcvMinuteData*> listMinute;
					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pMinute = (pHeader->m_pMinute+i);

						if(pMinute->m_time == EKE_HEAD_TAG)
						{
							CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
							if(pItem==NULL)
							{
								//ɾ��ָ�룬��ֹ�ڴ�й©
								foreach(qRcvMinuteData* p,listMinute)
									delete p;
							}
							else
							{
								pItem->appendMinutes(listMinute);
							}
							qsCode = QString::fromLocal8Bit(pMinute->m_head.m_szLabel);
							listMinute.clear();
						}
						else
						{
							listMinute.append(new qRcvMinuteData(pMinute));
						}
					}
				}
				break;

			case FILE_BASE_EX:                  // Ǯ�����ݻ��������ļ�,m_szFileName�������ļ���
				{
					int i = 0;
				}
				break;

			case FILE_NEWS_EX:                  // ������,��������m_szFileName����Ŀ¼������
				{
					QString qsNewsTitle = QString::fromLocal8Bit(pHeader->m_File.m_szFileName);
					qDebug()<<"Comming News "<< qsNewsTitle;
					QString qsNewsPath = qApp->applicationDirPath() + "\\news\\" + qsNewsTitle;

					QDir().mkpath(QFileInfo(qsNewsPath).absolutePath());
					QFile file(qsNewsPath);
					if(!file.open(QFile::WriteOnly|QFile::Truncate))
						break;
					file.write((char*)pHeader->m_pData,pHeader->m_File.m_dwLen);
					file.close();
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

	default:
		return 0;                           // unknown data
	}
	return 1;
}

void CMainWindow::onActiveBaseMarket()
{
	m_pSubBaseMarket->showMaximized();
}

void CMainWindow::onActiveTemplate()
{
	m_pSubTemplate->showMaximized();
}

void CMainWindow::onAddTemplate()
{

}
