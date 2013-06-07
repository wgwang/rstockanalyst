#include "StdAfx.h"
#include "RadarWidget.h"
#include "DataEngine.h"
#include "MainWindow.h"

CRadarWidget::CRadarWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent, WidgetRadar)
	, m_iItemHeight(16)
	, m_iTitleHeight(16)
	, m_pSelRadar(0)
	, m_iShowIndex(0)
{
	m_pMenuCustom = new QMenu(tr("������Ϣ�˵�"));
	reconnectSignals();
}


CRadarWidget::~CRadarWidget(void)
{
}

bool CRadarWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	return true;
}

bool CRadarWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;

	return true;
}

void CRadarWidget::reconnectSignals()
{
	QList<CRadarWatcher*> list = CRadarWatcher::getRadarWatchers();
	foreach(CRadarWatcher* pWatcher,list)
	{
		disconnect(pWatcher,SIGNAL(radarAlert(RRadarData*)),this,SLOT(onRadarAlert(RRadarData*)));
		disconnect(pWatcher,SIGNAL(watcherDelete(CRadarWatcher*)),this,SLOT(onWatcherDelete(CRadarWatcher*)));

		connect(pWatcher,SIGNAL(radarAlert(RRadarData*)),this,SLOT(onRadarAlert(RRadarData*)));
		connect(pWatcher,SIGNAL(watcherDelete(CRadarWatcher*)),this,SLOT(onWatcherDelete(CRadarWatcher*)));
	}
}

void CRadarWidget::getKeyWizData( const QString& keyword,QList<KeyWizData*>& listRet )
{

}

void CRadarWidget::keyWizEntered( KeyWizData* pData )
{

}

void CRadarWidget::setStockCode( const QString& only )
{

}

void CRadarWidget::setStockItem( CStockInfoItem* pItem )
{

}

void CRadarWidget::paintEvent( QPaintEvent* e )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	p.fillRect(rtClient,QColor(0,0,0));

	m_rtTitle = QRect(0,0,rtClient.width(),m_iTitleHeight);
	m_rtClient = QRect(0,m_iTitleHeight,rtClient.width(),rtClient.height()-m_iTitleHeight);

	drawTitle(p);
	drawClient(p);
}

void CRadarWidget::keyPressEvent( QKeyEvent* e )
{
	int iKey = e->key();
	if(Qt::Key_Down == iKey)
	{
		if(m_pSelRadar)
		{
			int iCurIndex = m_mapRadarsIndex[m_pSelRadar];
			if(iCurIndex>0)
			{
				RRadarData* pRadar = m_listRadars[iCurIndex-1];
				int iRow = m_mapRadarsIndex[pRadar];
				if(iRow<m_iShowIndex)
				{
					m_iShowIndex = iRow;
					update(m_rtClient);
				}
				clickedRadar(pRadar);
			}
		}
		e->accept();
	}
	else if(Qt::Key_Up == iKey)
	{
		if(m_pSelRadar)
		{
			int iCurIndex = m_mapRadarsIndex[m_pSelRadar];
			{
				if(m_listRadars.size()>(iCurIndex+1))
				{
					RRadarData* pRadar = m_listRadars[iCurIndex+1];
					int iRow = m_mapRadarsIndex[pRadar];

					int iShowCount = m_rtClient.height()/m_iItemHeight;
					if(iShowCount<1)
						return;

					if((iRow-m_iShowIndex)>=iShowCount)
					{
						m_iShowIndex = iRow-iShowCount+1;
						update(m_rtClient);
					}
					clickedRadar(pRadar);
				}
			}
		}
		e->accept();
	}
	//else if(Qt::Key_PageDown == iKey)
	//{
	//	int iShowCount = m_rtClient.height()/m_iStockHeight;
	//	if(iShowCount<1)
	//		return;
	//	if((showStockIndex+iShowCount)<m_listStocks.size())
	//	{
	//		showStockIndex = showStockIndex+iShowCount;
	//		update(m_rtClient);
	//	}
	//	e->accept();
	//}
	//else if(Qt::Key_PageUp == iKey)
	//{
	//	int iShowCount = m_rtClient.height()/m_iStockHeight;
	//	if(iShowCount<1)
	//		return;
	//	showStockIndex = (showStockIndex-iShowCount)>0 ? (showStockIndex-iShowCount) : 0;
	//	update(m_rtClient);

	//	e->accept();
	//}
	//else if(Qt::Key_F10 == iKey)
	//{
	//	//F10����
	//	if(m_pSelectedStock)
	//	{
	//		if(!CDataEngine::getDataEngine()->showF10Data(m_pSelectedStock->getCode()))
	//		{
	//			//δ��F10���� do something
	//		}
	//	}
	//}
	else
	{
		return CBaseWidget::keyPressEvent(e);
	}
}

void CRadarWidget::mouseMoveEvent( QMouseEvent* e )
{

}

void CRadarWidget::mousePressEvent( QMouseEvent* e )
{
	QPoint ptCur = e->pos();
	if(m_rtClient.contains(ptCur))
	{
		RRadarData* pData = testRadarData(ptCur);
		if(pData)
		{
			clickedRadar(pData);
		}
	}
}

QMenu* CRadarWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	return m_pMenuCustom;
}

void CRadarWidget::onRadarAlert( RRadarData* pRadar )
{
	m_mapRadarsIndex[pRadar] = m_listRadars.size();
	m_listRadars.append(pRadar);
	update();
}

void CRadarWidget::onWatcherDelete( CRadarWatcher* pWatcher )
{
	m_mapRadarsIndex.clear();
	QList<RRadarData*> listData;
	foreach(RRadarData* pData,m_listRadars)
	{
		if(pData->pWatcher != pWatcher)
		{
			m_mapRadarsIndex[pData] = listData.size();
			listData.push_back(pData);
		}
	}

	m_listRadars = listData;
	m_pSelRadar = 0;
	m_iShowIndex = 0;
}

void CRadarWidget::drawTitle( QPainter& p )
{
	QRect rtTitle = m_rtTitle;
	if(!rtTitle.isValid())
		return;

	QRect rtClient = rtTitle.adjusted(2,0,-2,0);

	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);

	p.setPen(QColor(255,255,255));
	p.drawText(rtClient,Qt::AlignLeft|Qt::AlignVCenter,"�״����");
	p.setPen(QColor(233,0,0));
	p.drawRect(m_rtTitle);
}

void CRadarWidget::drawClient( QPainter& p )
{
	int iCount = m_rtClient.height()/m_iItemHeight + 1;
	if(m_listRadars.size()<iCount)
		iCount = m_listRadars.size();

	int iBeginY = m_rtClient.top();
	int iSize = m_listRadars.size()-m_iShowIndex-1;
	p.setPen(QColor(255,255,255));
	for(int i=0;i<iCount;++i)
	{
		RRadarData* pData = m_listRadars[iSize-i];
		QRect rtItem(0,iBeginY+m_iItemHeight*i,m_rtClient.width(),m_iItemHeight);

		if(pData == m_pSelRadar)
		{
			p.fillRect(rtItem,QColor(244,0,0));
		}

		QString qsShowText = QString("%1 %2").arg(pData->pStock->getName(),pData->qsDesc);
		p.drawText(rtItem,qsShowText);
	}
}

void CRadarWidget::clickedRadar( RRadarData* pData )
{
	if(!pData)
		return;

	m_pSelRadar = pData;
	update();
	if(pData->pStock)
	{
		CMainWindow::getMainWindow()->clickedStock(pData->pStock->getOnly());
	}
}

RRadarData* CRadarWidget::testRadarData( const QPoint& ptClicked )
{
	if(!m_rtClient.contains(ptClicked))
		return 0;
	int iIndex = (ptClicked.y()-m_rtClient.top()) / m_iItemHeight;
	if(iIndex<0)
		return 0;
	iIndex = m_listRadars.size() - m_iShowIndex - iIndex - 1;
	if(iIndex >= m_listRadars.size() || iIndex<0)
		return 0;

	return m_listRadars[iIndex];
}
