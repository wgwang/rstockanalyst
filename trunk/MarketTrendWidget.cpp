#include "StdAfx.h"
#include "MarketTrendWidget.h"
#include "DataEngine.h"
#include "MainWindow.h"

#define	GetColorByFloat(x)	(((x)==0.0) ? QColor(191,191,191) : (((x)>0.0) ? QColor(255,80,80) : QColor(0,255,255)))


CMarketTrendWidget::CMarketTrendWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::MarketTrend)
	, m_iHeaderHeight(20)
	, m_iStockHeight(16)
	, m_iBottomHeight(18)
	, m_rtHeader(0,0,0,0)
	, m_rtClient(0,0,0,0)
	, m_rtBottom(0,0,0,0)
	, showHeaderIndex(0)
	, showStockIndex(0)
	, showBlockIndex(0)
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
	m_listItemWidth.push_back(50);
	while(m_listItemWidth.size()<m_listHeader.size())
	{
		m_listItemWidth.push_back(80);
	}
	m_listItemWidth[5] = 100;

	m_pMenuCustom = new QMenu(tr("�г�����ͼ�˵�"));
	m_pMenuCustom->addAction(tr("ˢ��"),this,SLOT(onRefresh()));
	m_pMenuToBlock = new QMenu(tr("��ӵ����"));
	m_pMenuCustom->addMenu(m_pMenuToBlock);
	m_pMenuCustom->addAction(tr("ɾ��ѡ�й�"),this,SLOT(onRemoveStock()));
	m_pMenuCustom->addSeparator();

	onRefresh();
//	setMinimumHeight(m_iHeaderHeight+m_iStockHeight+m_iBottomHeight);
//	setMinimumWidth(200);
}

CMarketTrendWidget::~CMarketTrendWidget(void)
{
	clearTmpData();
	delete m_pMenuCustom;
}

bool CMarketTrendWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	return true;
}

bool CMarketTrendWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;

	return true;
}

void CMarketTrendWidget::setStocks( const QList<CStockInfoItem*>& list )
{
	clearTmpData();
	m_listStocks = list;
	showHeaderIndex = 0;
	showStockIndex = 0;
	for(int i=0;i<m_listStocks.size();++i)
	{
		m_mapStockIndex[m_listStocks[i]] = i;
	}
	foreach(CStockInfoItem* pItem,m_listStocks)
	{
		connect(pItem,SIGNAL(stockInfoItemChanged(const QString&)),this,SLOT(stockInfoChanged(const QString&)));
	}

	if(m_listStocks.size()>0)
	{
		clickedStock(m_listStocks.first());
	}
}

void CMarketTrendWidget::stockInfoChanged( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);
	update(rectOfStock(pItem));
}

void CMarketTrendWidget::onRefresh()
{
	m_listBlocks.clear();
	QStringList listBlocks = CDataEngine::getDataEngine()->getStockBlocks();
	if(listBlocks.size()>0)
	{
		foreach(const QString& b,listBlocks)
		{
			m_listBlocks.push_back(QPair<QString,QRect>(b,QRect()));
		}
		updateBlockRect();
		if(m_qsSelectedBlock.isEmpty())
			clickedBlock(listBlocks.first());
		else
			clickedBlock(m_qsSelectedBlock);
	}
}

void CMarketTrendWidget::onAddToBlock()
{
	if(!m_pSelectedStock)
		return;
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	if(!pAct)
		return;
	QString block = pAct->data().toString();
	if(block.isEmpty())
		return;
	CDataEngine::getDataEngine()->appendStocksToBlock(block,QStringList()<<m_pSelectedStock->getCode());
}

void CMarketTrendWidget::onAddToNewBlock()
{
	if(!m_pSelectedStock)
		return;
	QDialog dlg(this);
	QVBoxLayout layout(this);
	QLineEdit edit(this);
	QPushButton btnOk(this);
	dlg.setLayout(&layout);
	layout.addWidget(&edit);
	layout.addWidget(&btnOk);
	btnOk.setText(tr("ȷ��"));
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	QString block = edit.text();
	if(CDataEngine::getDataEngine()->isHadBlock(block))
		return;
	CDataEngine::getDataEngine()->appendStocksToBlock(block,QStringList()<<m_pSelectedStock->getCode());
}

void CMarketTrendWidget::onRemoveStock()
{
	if(m_pSelectedStock)
	{
		if(CDataEngine::getDataEngine()->removeStocksFromBlock(m_qsSelectedBlock,QStringList()<<m_pSelectedStock->getCode()))
			clickedBlock(m_qsSelectedBlock);
	}
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
	if(pItem == m_pSelectedStock)
		return;

	CStockInfoItem* pPreSelectedStock = m_pSelectedStock;
	m_pSelectedStock = pItem;
	update(rectOfStock(pPreSelectedStock));
	update(rectOfStock(m_pSelectedStock));
	CMainWindow::getMainWindow()->clickedStock(pItem->getCode());
}

void CMarketTrendWidget::offsetShowHeaderIndex( int offset )
{
	int iIndex = showHeaderIndex+offset;
	if(iIndex>=0&&iIndex<m_listHeader.size()-3)
	{
		showHeaderIndex = iIndex;
		update();
	}
}

void CMarketTrendWidget::clickedBlock( const QString& block )
{
	if(m_qsSelectedBlock == block)
		return;

	if(!block.isEmpty())
	{
		setStocks(CDataEngine::getDataEngine()->getStocksByBlock(block));
		CMainWindow::getMainWindow()->clickedBlock(block);
	}
	else
	{
		setStocks(CDataEngine::getDataEngine()->getStockInfoList());
	}
	m_qsSelectedBlock = block;
	update();
}

void CMarketTrendWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	drawHeaders(p);
	drawStocks(p);
	drawBottom(p);
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
		if(m_rtPreIndex.contains(ptCur))
		{
			offsetShowHeaderIndex(-1);
		}
		else if(m_rtNextIndex.contains(ptCur))
		{
			offsetShowHeaderIndex(1);
		}
		else
		{
			QList<QPair<QString,QRect>>::iterator iter = m_listBlocks.begin();
			while(iter!=m_listBlocks.end())
			{
				if(iter->second.contains(ptCur))
				{
					clickedBlock(iter->first);
					break;
				}
				++iter;
			}
		}
	}
}

void CMarketTrendWidget::wheelEvent( QWheelEvent* e )
{
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;
	int iIndex = showStockIndex-numSteps*5;
	if(iIndex>=0&&iIndex<m_listStocks.size())
	{
		showStockIndex = iIndex;
		update();
	}
}

QMenu* CMarketTrendWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	m_pMenuToBlock->clear();
	QList<QString> list = CDataEngine::getDataEngine()->getStockBlocks();
	foreach(const QString& block,list)
	{
		if(block == m_qsSelectedBlock)
			continue;
		QAction* pAct = m_pMenuToBlock->addAction(block,this,SLOT(onAddToBlock()));
		pAct->setData(block);
	}
	m_pMenuToBlock->addSeparator();
	m_pMenuToBlock->addAction(tr("�½����"),this,SLOT(onAddToNewBlock()));

	return m_pMenuCustom;
}

void CMarketTrendWidget::updateDrawRect()
{
	QRect rtClient = this->rect();
	m_rtHeader = QRect(rtClient.topLeft(),QSize(rtClient.width(),m_iHeaderHeight));
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);
	m_rtClient = QRect(m_rtHeader.left(),m_rtHeader.bottom()+2,rtClient.width(),rtClient.height()-m_iBottomHeight-m_iHeaderHeight-2);

	//����ǰ���λ��
	m_rtNextIndex = QRect(m_rtBottom.right()-m_iBottomHeight,m_rtBottom.top(),m_iBottomHeight,m_iBottomHeight);
	m_rtPreIndex = QRect(m_rtBottom.right()-m_iBottomHeight*2,m_rtBottom.top(),m_iBottomHeight,m_iBottomHeight);
	updateBlockRect();
}

void CMarketTrendWidget::updateBlockRect()
{
	//���������ÿ�
	for(int i=0;i<showBlockIndex;++i)
	{
		if(i<m_listBlocks.size())
			m_listBlocks[i].second = QRect();
	}
	//�����������Ĵ�С
	QFont f;
	QFontMetrics m(f);
	int iCurX = m_rtBottom.left();
	for (int i=showBlockIndex;i<m_listBlocks.size();++i)
	{
		int iWidth = m.width(m_listBlocks[i].first) + 16;
		QRect rtEntity = QRect(iCurX,m_rtBottom.top(),iWidth,m_rtBottom.height());
		m_listBlocks[i].second = rtEntity;

		iCurX = iCurX+iWidth;
	}
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

void CMarketTrendWidget::drawBottom( QPainter& p )
{
	p.fillRect(m_rtBottom,QColor(0,0,0));

	p.setPen(QColor(255,255,255));

	QList<QPair<QString,QRect>>::iterator iter = m_listBlocks.begin();
	while(iter!=m_listBlocks.end())
	{
		QRect rtBlock = iter->second;
		if(rtBlock.isValid())
		{
			p.drawRect(rtBlock);
			if(iter->first==m_qsSelectedBlock)
				p.fillRect(rtBlock,QColor(127,127,127));
			p.drawText(rtBlock,Qt::AlignCenter,iter->first);
		}

		++iter;
	}
	drawBottomBtn(p);
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

void CMarketTrendWidget::drawBottomBtn( QPainter& p )
{
	if(m_rtPreIndex.isValid())
	{
		QRect rtBtn = m_rtPreIndex.adjusted(1,1,0,-1);
		p.fillRect(rtBtn,QColor(200,200,200));
		QPainterPath path;
		path.moveTo(rtBtn.center().x()-m_iBottomHeight/2+4,rtBtn.center().y());
		path.lineTo(rtBtn.right()-4,rtBtn.top()+4);
		path.lineTo(rtBtn.right()-4,rtBtn.bottom()-4);
		p.fillPath(path,QColor(0,0,0));
	}
	if(m_rtNextIndex.isValid())
	{
		QRect rtBtn = m_rtNextIndex.adjusted(1,1,0,-1);
		p.fillRect(rtBtn,QColor(200,200,200));
		QPainterPath path;
		path.moveTo(rtBtn.center().x()+m_iBottomHeight/2-4,rtBtn.center().y());
		path.lineTo(rtBtn.left()+4,rtBtn.top()+4);
		path.lineTo(rtBtn.left()+4,rtBtn.bottom()-4);
		p.fillPath(path,QColor(0,0,0));
	}
}
