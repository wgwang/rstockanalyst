#include "StdAfx.h"
#include "MarketTrendWidget.h"
#include "DataEngine.h"

#define	GetColorByFloat(x)	(((x)==0.0) ? QColor(191,191,191) : (((x)>0.0) ? QColor(255,80,80) : QColor(0,255,255)))


CMarketTrendWidget::CMarketTrendWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::MarketTrend)
	, m_iHeaderHeight(20)
	, m_iStockHeight(16)
	, m_iBottomHeight(20)
	, m_rtHeader(0,0,0,0)
	, m_rtClient(0,0,0,0)
	, m_rtBottom(0,0,0,0)
	, showHeaderIndex(0)
	, showStockIndex(0)
	, m_pSelectedStock(0)
	, m_iSortColumn(0)
	, m_sortOrder(Qt::AscendingOrder)
{
	m_listHeader << tr("����") << tr("����") << tr("����")
		<< tr("�Ƿ�") << tr("����") << tr("�����ʣ��ֲ") << tr("ǰ��") << tr("��")
		<< tr("���") << tr("���") << tr("����") << tr("����") << tr("�ܶ�ֲ֣�")
		<< tr("����") << tr("����") << tr("�ǵ�") << tr("���") << tr("����") << tr("��ӯ��")
		<< tr("��ͨ��ֵ") << tr("����") << tr("����") << tr("ί����") << tr("ί����")
		<< tr("ί��") << tr("ί��");

	/*��ʼ��������Ԫ�Ŀ��*/
	m_listItemWidth.push_back(30);
	for(int i=0;i<m_listHeader.size()-1;++i)
	{
		m_listItemWidth.push_back(80);
	}
	m_listItemWidth[5] = 100;

	m_pMenuCustom = new QMenu(tr("�г�����ͼ�˵�"));

	setStocks(CDataEngine::getDataEngine()->getStockInfoList());
	m_pSelectedStock = m_listStocks.first();
}

CMarketTrendWidget::~CMarketTrendWidget(void)
{
	clearTmpData();
	delete m_pMenuCustom;
}

void CMarketTrendWidget::setStocks( const QList<CStockInfoItem*>& list )
{
	clearTmpData();
	m_listStocks = list;
	for(int i=0;i<m_listStocks.size();++i)
	{
		m_mapStockIndex[m_listStocks[i]] = i;
	}
	foreach(CStockInfoItem* pItem,m_listStocks)
	{
		connect(pItem,SIGNAL(stockInfoItemChanged(const QString&)),this,SLOT(stockInfoChanged(const QString&)));
	}
}

void CMarketTrendWidget::stockInfoChanged( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);
	update(rectOfStock(pItem));
}

void CMarketTrendWidget::clearTmpData()
{
	m_pSelectedStock = 0;
	foreach(CStockInfoItem* pItem,m_listStocks)
	{
		disconnect(pItem,SIGNAL(stockInfoItemChanged(const QString&)),this,SLOT(stockInfoChanged(const QString&)));
	}
	m_listStocks.clear();
	m_mapStockIndex.clear();
}

void CMarketTrendWidget::clickedHeader( int column )
{
	if(column==0)
		return;
	if(column == m_iSortColumn)
	{
		m_sortOrder = (m_sortOrder==Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
	}
	else
	{
		m_iSortColumn = column;
		m_sortOrder = Qt::AscendingOrder;
	}
	if(column == 1||column ==2)
	{
		QMultiMap<QString,CStockInfoItem*> mapSort;
		foreach(CStockInfoItem* pItem,m_listStocks)
		{
			mapSort.insert(dataOfDisplay(pItem,column),pItem);
		}
		
		if(m_sortOrder==Qt::AscendingOrder)
			setStocks(mapSort.values());
		else
		{
			QList<CStockInfoItem*> list;
			QMultiMap<QString,CStockInfoItem*>::iterator iter = mapSort.begin();
			while(iter!=mapSort.end())
			{
				list.push_front(iter.value());
				++iter;
			}
			setStocks(list);
		}
	}
	else
	{
		QMultiMap<float,CStockInfoItem*> mapSort;
		foreach(CStockInfoItem* pItem,m_listStocks)
		{
			QString qsData = dataOfDisplay(pItem,column);
			qsData = qsData.replace("%","");
			mapSort.insert(qsData.toFloat(),pItem);
		}
		if(m_sortOrder==Qt::AscendingOrder)
			setStocks(mapSort.values());
		else
		{
			QList<CStockInfoItem*> list;
			QMultiMap<float,CStockInfoItem*>::iterator iter = mapSort.begin();
			while(iter!=mapSort.end())
			{
				list.push_front(iter.value());
				++iter;
			}
			setStocks(list);
		}
	}
	update(m_rtHeader);
	update(m_rtClient);
}

void CMarketTrendWidget::clickedStock( CStockInfoItem* pItem )
{
	CStockInfoItem* pPreSelectedStock = m_pSelectedStock;
	m_pSelectedStock = pItem;
	update(rectOfStock(pPreSelectedStock));
	update(rectOfStock(m_pSelectedStock));
}

void CMarketTrendWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	drawHeaders(p);
	drawStocks(p);
	p.fillRect(m_rtBottom,QColor(0,255,0));
	p.setPen(QColor(255,255,255));
	p.drawRect(m_rtBottom);
}

void CMarketTrendWidget::resizeEvent( QResizeEvent* e)
{
	updateDrawRect();
	return CBaseWidget::resizeEvent(e);
}

void CMarketTrendWidget::mousePressEvent( QMouseEvent* e )
{
	e->accept();
	QPoint ptCur = e->pos();
	if(m_rtHeader.contains(ptCur))
	{
		int iCurX = m_rtHeader.left();
		int iCurIndex = 0;
		for (;iCurIndex<3;++iCurIndex)
		{
			iCurX = iCurX+m_listItemWidth[iCurIndex];
			if(iCurX>ptCur.x())
				break;
		}
		if(iCurX>ptCur.x())
		{
			clickedHeader(iCurIndex);
			return;
		}
		iCurIndex = iCurIndex+showHeaderIndex;
		for (;iCurIndex<m_listItemWidth.size();++iCurIndex)
		{
			iCurX = iCurX+m_listItemWidth[iCurIndex];
			if(iCurX>ptCur.x())
				break;
		}
		if(iCurX>ptCur.x())
		{
			clickedHeader(iCurIndex);
			return;
		}
	}
	else if(m_rtClient.contains(ptCur))
	{
		int iCurIndex = showStockIndex+(ptCur.y()-m_rtClient.top())/m_iStockHeight;
		if(iCurIndex>=0&&iCurIndex<m_listStocks.size())
		{
			clickedStock(m_listStocks[iCurIndex]);
		}
	}
	else if(m_rtBottom.contains(ptCur))
	{

	}
}

QMenu* CMarketTrendWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	return m_pMenuCustom;
}

void CMarketTrendWidget::updateDrawRect()
{
	QRect rtClient = this->rect();
	m_rtHeader = QRect(rtClient.topLeft(),QSize(rtClient.width(),m_iHeaderHeight));
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);
	m_rtClient = QRect(m_rtHeader.left(),m_rtHeader.bottom()+2,rtClient.width(),rtClient.height()-m_iBottomHeight-m_iHeaderHeight-2);
}

QString CMarketTrendWidget::dataOfDisplay( CStockInfoItem* itemData,int column )
{
	switch(column)
	{
	case 0:
		{
			//������
			if(m_mapStockIndex.contains(itemData))
				return QString("%1").arg(m_mapStockIndex[itemData]+1);
			return QString("Error");
		}
		break;
	case 1:
		{
			//��Ʊ����
			return itemData->getCode();
		}
		break;
	case 2:
		{
			//��Ʊ����
			return itemData->getName();
		}
		break;
	case 3:
		{
			//�Ƿ�
			if(_isnan(itemData->getIncrease()))
				return QString();
			return QString("%1%").arg(itemData->getIncrease(),0,'f',2);
		}
		break;
	case 4:
		{
			//����
			if(_isnan(itemData->getVolumeRatio()))
				return QString();
			return QString("%1").arg(itemData->getVolumeRatio(),0,'f',2);
		}
		break;
	case 5:
		{
			//������
			if(_isnan(itemData->getTurnRatio()))
				return QString();
			return QString("%1%").arg(itemData->getTurnRatio(),0,'f',2);
		}
		break;
	case 6:
		{
			//ǰ��
			if(_isnan(itemData->getLastClose()))
				return QString();

			return QString("%1").arg(itemData->getLastClose(),0,'f',2);
		}
		break;
	case 7:
		{
			//��
			if(_isnan(itemData->getOpenPrice()))
				return QString();

			return QString("%1").arg(itemData->getOpenPrice(),0,'f',2);
		}
		break;
	case 8:
		{
			//���
			if(_isnan(itemData->getHighPrice()))
				return QString();

			return QString("%1").arg(itemData->getHighPrice(),0,'f',2);
		}
		break;
	case 9:
		{
			//���
			if(_isnan(itemData->getLowPrice()))
				return QString();

			return QString("%1").arg(itemData->getLowPrice(),0,'f',2);
		}
		break;
	case 10:
		{
			//����
			if(_isnan(itemData->getNewPrice()))
				return QString();

			return QString("%1").arg(itemData->getNewPrice(),0,'f',2);
		}
		break;
	case 11:
		{
			//����
			if(_isnan(itemData->getTotalVolume()))
				return QString();

			return QString("%1").arg(itemData->getTotalVolume(),0,'f',0);
		}
		break;
	case 12:
		{
			//�ܶ�ֲ֣�
			if(_isnan(itemData->getTotalAmount()))
				return QString();

			return QString("%1").arg(itemData->getTotalAmount()/10000,0,'f',0);
		}
		break;
	case 13:
		{
			//����
			if(_isnan(itemData->getNowVolume()))
				return QString();

			return QString("%1").arg(itemData->getNowVolume(),0,'f',0);
		}
		break;
	case 14:
		{
			//����
			if(_isnan(itemData->getIncSpeed()))
				return QString();

			return QString("%1%").arg(itemData->getIncSpeed()*100,0,'f',2);
		}
		break;
	case 15:
		{
			//�ǵ�
			if(_isnan(itemData->getPriceFluctuate()))
				return QString();

			return QString("%1%").arg(itemData->getPriceFluctuate(),0,'f',2);
		}
		break;
	case 16:
		{
			//���
			if(_isnan(itemData->getAmplitude()))
				return QString();

			return QString("%1%").arg(itemData->getAmplitude()*100,0,'f',2);
		}
		break;
	case 17:
		{
			//����
			if(_isnan(itemData->getAvePrice()))
				return QString();

			return QString("%1").arg(itemData->getAvePrice(),0,'f',2);
		}
		break;
	case 18:
		{
			//��ӯ��
			if(_isnan(itemData->getPERatio()))
				return QString();
			return QString("%1").arg(itemData->getPERatio(),0,'f',2);
		}
		break;
	case 19:
		{
			//��ͨ��ֵ
			if(_isnan(itemData->getLTSZ()))
				return QString();
			return QString("%1").arg(itemData->getLTSZ(),0,'f',0);
		}
		break;
	case 20:
		{
			//����
			return static_cast<int>(itemData->getSellVOL());
		}
		break;
	case 21:
		{
			//����
			return static_cast<int>(itemData->getBuyVOL());
		}
		break;
	case 22:
		{
			//ί����
			if(_isnan(itemData->getBIDVOL()))
				return QString();

			return QString("%1").arg(itemData->getBIDVOL(),0,'f',0);
		}
		break;
	case 23:
		{
			//ί����
			if(_isnan(itemData->getASKVOL()))
				return QString();

			return QString("%1").arg(itemData->getASKVOL(),0,'f',0);
		}
		break;
	case 24:
		{
			//ί��
			if(_isnan(itemData->getCommRatio()))
				return QString();

			return QString("%1%").arg(itemData->getCommRatio(),0,'f',2);
		}
		break;
	case 25:
		{
			//ί��
			if(_isnan(itemData->getCommSent()))
				return QString();

			return QString("%1").arg(itemData->getCommSent(),0,'f',0);
		}
		break;
	default:
		{
			return QString("NULL");
		}
		break;
	}
}

QColor CMarketTrendWidget::dataOfColor( CStockInfoItem* itemData,int column )
{
	if(column == 1)
		return QColor(127,255,191);
	else if(column == 2)
		return QColor(255,255,0);
	else if(column == 3 || column == 15)
	{
		//�Ƿ�
		return GetColorByFloat(itemData->getIncrease());
	}
	else if(column == 7)
	{
		//��
		float f = itemData->getOpenPrice()-itemData->getLastClose();
		return GetColorByFloat(f);
	}
	else if(column == 8)
	{
		//���
		float f = itemData->getHighPrice()-itemData->getLastClose();
		return GetColorByFloat(f);
	}
	else if(column == 9)
	{
		//���
		float f = itemData->getLowPrice()-itemData->getLastClose();
		return GetColorByFloat(f);
	}
	else if(column == 10)
	{
		float f = itemData->getNewPrice()-itemData->getLastClose();
		return GetColorByFloat(f);
	}
	else if(column == 17)
	{
		float f = itemData->getAvePrice()-itemData->getLastClose();
		return GetColorByFloat(f);
	}
	else if(column == 20 || column == 22)
		return QColor(255,80,80);
	else if(column == 21 || column == 23)
		return QColor(0,255,255);
	else if(column == 24)
	{
		float f = itemData->getCommRatio();
		return GetColorByFloat(f);
	}
	else if(column == 25)
	{
		float f = itemData->getCommSent();
		return GetColorByFloat(f);
	}

	return QColor(191,191,191);
}

QRect CMarketTrendWidget::rectOfStock( CStockInfoItem* pItem )
{
	if(m_mapStockIndex.contains(pItem))
	{
		int iRow = m_mapStockIndex[pItem];
		return QRect(m_rtClient.left(),(m_rtClient.top()+(iRow-showStockIndex)*m_iStockHeight),m_rtClient.width(),m_iStockHeight);
	}

	return QRect();
}

void CMarketTrendWidget::drawHeaders( QPainter& p )
{
	p.setPen(QColor(255,255,255));
	p.fillRect(m_rtHeader,QColor(0,0,0));
	int iCurX = m_rtHeader.left();
	int iCurIndex = 0;
	for (;iCurIndex<3;++iCurIndex)
	{
		QRect rtItem = QRect(iCurX,m_rtHeader.top(),m_listItemWidth[iCurIndex],m_rtHeader.height());
		p.drawRect(rtItem);
		if((m_iSortColumn == iCurIndex)&&iCurIndex!=0)
		{
			if(m_sortOrder == Qt::AscendingOrder)
			{
				p.drawText(rtItem,Qt::AlignCenter,QString("��%1").arg(m_listHeader[iCurIndex]));
			}
			else
			{
				p.drawText(rtItem,Qt::AlignCenter,QString("��%1").arg(m_listHeader[iCurIndex]));
			}
		}
		else
		{
			p.drawText(rtItem,Qt::AlignCenter,m_listHeader[iCurIndex]);
		}
		iCurX = iCurX+m_listItemWidth[iCurIndex];
	}

	iCurIndex = iCurIndex+showHeaderIndex;
	while(iCurX<m_rtHeader.right()&&iCurIndex<m_listHeader.size())
	{
		QRect rtItem = QRect(iCurX,m_rtHeader.top(),m_listItemWidth[iCurIndex],m_rtHeader.height());
		p.drawRect(rtItem);
		if((m_iSortColumn == iCurIndex))
		{
			if(m_sortOrder == Qt::AscendingOrder)
			{
				p.drawText(rtItem,Qt::AlignCenter,QString("��%1").arg(m_listHeader[iCurIndex]));
			}
			else
			{
				p.drawText(rtItem,Qt::AlignCenter,QString("��%1").arg(m_listHeader[iCurIndex]));
			}
		}
		else
		{
			p.drawText(rtItem,Qt::AlignCenter,m_listHeader[iCurIndex]);
		}
		iCurX = iCurX+m_listItemWidth[iCurIndex];
		++iCurIndex;
	}
}

void CMarketTrendWidget::drawStocks( QPainter& p )
{
	p.fillRect(m_rtClient,QColor(0,0,0));
	int iCurRow = showStockIndex;
	int iCurY = m_rtClient.top();
	while(iCurY<m_rtClient.bottom()&&iCurRow<m_listStocks.size())
	{
		QRect rtStock = QRect(m_rtClient.left(),iCurY,m_rtClient.width(),m_iStockHeight);
		CStockInfoItem* pStock = m_listStocks[iCurRow];

		if(m_pSelectedStock == pStock)
		{
			p.fillRect(rtStock,QColor(50,50,50));
		}
		drawStock(p,rtStock,pStock);

		++iCurRow;
		iCurY = iCurY + m_iStockHeight;
	}
}

void CMarketTrendWidget::drawStock( QPainter& p,const QRect& rtStock,CStockInfoItem* pItem )
{
	int iCurX = rtStock.left();
	int iCurIndex = 0;
	for (;iCurIndex<3;++iCurIndex)
	{
		QRect rtItem = QRect(iCurX,rtStock.top(),m_listItemWidth[iCurIndex],rtStock.height());
		p.setPen(QColor(127,0,0));
		p.drawRect(rtItem);
		p.setPen(dataOfColor(pItem,iCurIndex));
		p.drawText(rtItem,Qt::AlignCenter,dataOfDisplay(pItem,iCurIndex));
		iCurX = iCurX+m_listItemWidth[iCurIndex];
	}

	iCurIndex = iCurIndex+showHeaderIndex;
	while(iCurX<m_rtHeader.right()&&iCurIndex<m_listHeader.size())
	{
		QRect rtItem = QRect(iCurX,rtStock.top(),m_listItemWidth[iCurIndex],rtStock.height());
		p.setPen(QColor(127,0,0));
		p.drawRect(rtItem);
		p.setPen(dataOfColor(pItem,iCurIndex));
		p.drawText(rtItem,Qt::AlignCenter,dataOfDisplay(pItem,iCurIndex));
		iCurX = iCurX+m_listItemWidth[iCurIndex];
		++iCurIndex;
	}
}
