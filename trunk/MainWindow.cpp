#include "StdAfx.h"
#include "MainWindow.h"
#include "STKDRV.h"
#include "BaseMarketWidget.h"
#include "DataEngine.h"


#define RSTOCK_ANALYST_MAINMSG (WM_USER+1)

CMainWindow::CMainWindow()
	: QMainWindow()
{
	m_pMdiArea = new QMdiArea();
	m_pMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_pMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setCentralWidget(m_pMdiArea);

	m_pBaseMarketWidget = new CBaseMarketWidget;

	m_pMdiArea->addSubWindow(m_pBaseMarketWidget);

//	m_pBaseMarketWidget->showMaximized();
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
			qDebug()<< "####Comming reports####";
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

				qRcvReportData* p = new qRcvReportData(pReport);
				pItem->appendReport(p);
			}
		}
		break;

	case RCV_FILEDATA:                      // �����������÷�ʽ,�ļ�
		{
			switch(pHeader->m_wDataType)
			{
			case FILE_HISTORY_EX:               // ����������
				{
					if(pHeader->m_nPacketNum<1)
						break;

					QTime timeBegin = QTime::currentTime();
					RCV_HISTORY_STRUCTEx* pHistory = pHeader->m_pDay;
					QString qsCode;

					QList<qRcvHistoryData*> listHistory;
 					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pHistory = (pHeader->m_pDay+i);
						if(pHistory->m_time == EKE_HEAD_TAG)
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

					qDebug()<<"Packet cout:"<<pHeader->m_nPacketNum;
					qDebug()<<"UseTime:"<<QTime::currentTime().msecsTo(timeBegin)<<"m secs";
				}
				break;

			case FILE_MINUTE_EX:                // ������������
				{
					int i = 0;
				}
				break;

			case FILE_BASE_EX:                  // Ǯ�����ݻ��������ļ�,m_szFileName�������ļ���
				{
					int i = 0;
				}
				break;

			case FILE_NEWS_EX:                  // ������,��������m_szFileName����Ŀ¼������
				{
					int i = 0;
				}
				break;
			case FILE_POWER_EX:
				{
					int i = 0;
				}
				break;
			default:
				{
					int j = 0;
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
