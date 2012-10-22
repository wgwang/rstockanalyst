#include "StdAfx.h"
#include "MainWindow.h"
#include "STKDRV.h"
#include "BaseMarketWidget.h"
#include "DataEngine.h"
#include <iostream>
using namespace std;

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
			cout<< "####Comming reports####"<<endl;
			if(pHeader->m_nPacketNum<1)
				break;

			RCV_REPORT_STRUCTExV3* pReport = pHeader->m_pReportV3;
			for(int i=0;i<pHeader->m_nPacketNum;++i)
			{
				pReport = (pHeader->m_pReportV3+i);

				qRcvReportData* pItem = CDataEngine::getDataEngine()->getBaseMarket(QString::fromLocal8Bit(pReport->m_szLabel));
				if(pItem==NULL)
				{
					pItem = new qRcvReportData(pReport);
				}
				else
				{
					if(pReport->m_time<=pItem->tmTime)
						continue;
					pItem->resetItem(pReport);
				}

				CDataEngine::getDataEngine()->setBaseMarket(pItem);
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

					QTime timeBein = QTime::currentTime();
					RCV_HISTORY_STRUCTEx* pHistory = pHeader->m_pDay;
					QString qsCode;
 					for(int i=0;i<pHeader->m_nPacketNum;++i)
					{
						pHistory = (pHeader->m_pDay+i);
						if(pHistory->m_time == EKE_HEAD_TAG)
						{
							CDataEngine::getDataEngine()->updateBaseMarket(qsCode);
							qsCode = QString::fromLocal8Bit(pHistory->m_head.m_szLabel);
						}
						else
						{
							if(!CDataEngine::getDataEngine()->appendHistory(qsCode,
								qRcvHistoryData(pHistory)))
							{
		//						cout<<"No Code:"<<qsCode.toLocal8Bit().data()<<endl;
							}
						}
					}
					CDataEngine::getDataEngine()->updateBaseMarket(qsCode);

					cout<<"Packet cout:"<<pHeader->m_nPacketNum<<endl;
					cout<<"UseTime:"<<QTime::currentTime().msecsTo(timeBein)<<"m secs"<<endl;
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
