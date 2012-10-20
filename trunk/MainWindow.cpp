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
	if(CSTKDRV::Stock_Init(this->winId(),RSTOCK_ANALYST_MAINMSG,RCV_WORK_SENDMSG)>0)
	{
		if(CSTKDRV::SetupReceiver(TRUE)>0)
			return true;
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
			if(pHeader->m_nPacketNum<1)
				break;

			RCV_REPORT_STRUCTExV3* pReport = pHeader->m_pReportV3;

			int iIndex = 0;
			while(iIndex<pHeader->m_nPacketNum)
			{
				if(pReport->m_cbSize<1)
					break;

				qRcvReportItem* pItem = CDataEngine::getDataEngine()->getBaseMarket(QString::fromLocal8Bit(pReport->m_szLabel));
				if(pItem==NULL)
					pItem = new qRcvReportItem(pReport);
				else
					pItem->resetItem(pReport);

				CDataEngine::getDataEngine()->setBaseMarket(pItem);

				pReport = (pReport+pReport->m_cbSize);
				++iIndex;
			}
			m_pBaseMarketWidget->updateBaseMarket();
		}
		break;

	case RCV_FILEDATA:                      // �����������÷�ʽ,�ļ�
		{
			switch(pHeader->m_wDataType)
			{
			case FILE_HISTORY_EX:               // ����������
				break;

			case FILE_MINUTE_EX:                // ������������
				break;

			case FILE_BASE_EX:                  // Ǯ�����ݻ��������ļ�,m_szFileName�������ļ���
				break;

			case FILE_NEWS_EX:                  // ������,��������m_szFileName����Ŀ¼������
				break;
			}
		}
		break;

	default:
		return 0;                           // unknown data
	}
	return 1;
}
