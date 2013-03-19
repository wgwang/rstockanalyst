#include "StdAfx.h"
#include "ColorBlockWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"
#include "BlockInfoItem.h"
#include "RLuaEx.h"


#define	RCB_OFFSET_Y	2
#define RCB_OFFSET_LEFT	50

/* ��ĳ���ӵ����ݽ���ת�� (qRcvMinuteData*) -> (stColorBlockItem) */
bool getColorBlockItemByMin(RStockData& item, qRcvFenBiData* pMin)
{
	if(!pMin)
		return false;

	item.tmTime = pMin->tmTime;
	item.fOpen = pMin->fPrice;
	item.fHigh = pMin->fPrice;
	item.fLow = pMin->fPrice;
	item.fClose = pMin->fPrice;
	item.fAmount = pMin->fAmount;
	item.fVolume = pMin->fVolume;
	return true;
}

/*ͨ���������ݻ�ȡָ�������ڵ�����*/
bool getColorBlockItemByMins(RStockData& item, const QList<qRcvFenBiData*>& list, qRcvFenBiData* pLastFenbi)
{
	if(list.size()<1)
		return false;

//	qRcvFenBiData* pBegin = list.first();
	qRcvFenBiData* pLast = list.last();
	qRcvFenBiData* pFirst = list.first();
	item.tmTime = pFirst->tmTime;
	item.fOpen = pFirst->fPrice;
	item.fClose = pLast->fPrice;

	if(pLastFenbi)
	{
		item.fAmount = pLast->fAmount-pLastFenbi->fAmount;
		item.fVolume = pLast->fVolume-pLastFenbi->fVolume;
	}
	else
	{
		item.fAmount = pLast->fAmount;
		item.fVolume = pLast->fVolume;
	}

	foreach(qRcvFenBiData* p,list)
	{
		if(item.fLow>p->fPrice)
			item.fLow = p->fPrice;
		if(item.fHigh<p->fPrice)
			item.fHigh = p->fPrice;
	}
	return true;
}

int getColorBlockMinItems(QMap<time_t,RStockData>* pMap,const QList<qRcvFenBiData*>& minutes, int iSize = 1)
{
	QList<qRcvFenBiData*> listMins;
	time_t tmT = 0;
	qRcvFenBiData* pLastFenbi = 0;
	foreach(qRcvFenBiData* p, minutes)
	{
		if((p->tmTime-tmT)>(iSize))
		{
			RStockData item;
			if(getColorBlockItemByMins(item,listMins,pLastFenbi))
			{
//				QString qsTime = QDateTime::fromTime_t(item.tmTime).toString("hh:mm:ss");
				pMap->insert(item.tmTime,item);
			}
			if(listMins.size()>0)
				pLastFenbi = listMins.last();
			listMins.clear();
			tmT = p->tmTime;
		}

		listMins.push_back(p);
	}

	
	{
		//��������
		RStockData item;
		if(getColorBlockItemByMins(item,listMins,pLastFenbi))
			pMap->insert(item.tmTime,item);
		listMins.clear();
	}


	return pMap->size();
}

/* ��ĳ������ݽ���ת�� (qRcvHistoryData*) -> (stColorBlockItem) */
bool getColorBlockItemByDay(RStockData& item,const qRcvHistoryData* pHistory)
{
	if(!pHistory)
		return false;

	memcpy(&item,&pHistory,sizeof(tagRStockData));
	return true;
}
/*ͨ���������ݻ�ȡһ�������ڵ�����*/
bool getColorBlockItemByDays(RStockData& item,const QList<qRcvHistoryData*>& list)
{
	if(list.size()<1)
		return false;

	qRcvHistoryData* pFirst = list.first();
	qRcvHistoryData* pLast = list.last();
	item.tmTime = pFirst->time;
	item.fOpen = pFirst->fOpen;
	item.fClose = pLast->fClose;


	item.fLow = 0;
	item.fHigh = 0;
	item.fAmount = 0;
	item.fVolume = 0;
	foreach(qRcvHistoryData* p,list)
	{
		if(item.fLow>p->fLow)
			item.fLow = p->fLow;
		if(item.fHigh<p->fHigh)
			item.fHigh = p->fHigh;
		item.fAmount+=p->fAmount;
		item.fVolume+=p->fVolume;
	}
	return true;
}

int getColorBlockDayItems(QMap<time_t,RStockData>* pMap,const QList<qRcvHistoryData*>& historys)
{
	foreach(qRcvHistoryData* p,historys)
	{
		RStockData item;
		if(getColorBlockItemByDay(item,p))
			pMap->insert(item.tmTime,item);
	}

	return pMap->size();
}

int getColorBlockWeekItems(QMap<time_t,RStockData>* pMap,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	int iCurWeek = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
		iCurWeek = tmDate.weekNumber();
	}


	QList<qRcvHistoryData*> weekHis;		//�����ڽ��й����Ժ����������
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			if(tmDate.weekNumber(&iCurYear)!=iCurWeek)
			{
				RStockData item;
				getColorBlockItemByDays(item,weekHis);
				pMap->insert(item.tmTime,item);
				weekHis.clear();
			}
			iCurYear = tmDate.year();
			iCurWeek = tmDate.weekNumber();
		}
		else if(tmDate.weekNumber()!=iCurWeek)
		{
			iCurWeek = tmDate.weekNumber();

			RStockData item;
			getColorBlockItemByDays(item,weekHis);
			pMap->insert(item.tmTime,item);
			weekHis.clear();
		}
		weekHis.push_back(pHistory);
	}

	if(weekHis.size()>0)
	{
		//���ʣ���Ҳ����
		RStockData item;
		getColorBlockItemByDays(item,weekHis);
		pMap->insert(item.tmTime,item);
		weekHis.clear();
	}
	return pMap->size();
}

int getColorBlockMonthItems(QMap<time_t,RStockData>* pMap,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	int iCurMonth = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
		iCurMonth = tmDate.month();
	}

	QList<qRcvHistoryData*> monthHis;		//�����ڽ��й����Ժ����������
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			iCurYear = tmDate.year();
			iCurMonth = tmDate.month();
			{
				RStockData item;
				getColorBlockItemByDays(item,monthHis);
				pMap->insert(item.tmTime,item);
				monthHis.clear();
			}
		}
		else if(tmDate.month()!=iCurMonth)
		{
			iCurMonth = tmDate.month();

			RStockData item;
			getColorBlockItemByDays(item,monthHis);
			pMap->insert(item.tmTime,item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//���ʣ���Ҳ����
		RStockData item;
		getColorBlockItemByDays(item,monthHis);
		pMap->insert(item.tmTime,item);
		monthHis.clear();
	}
	return pMap->size();
}

int getColorBlockMonth3Items(QMap<time_t,RStockData>* pMap,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	int iCurMonth = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
		iCurMonth = tmDate.month();
	}

	QList<qRcvHistoryData*> monthHis;		//�����ڽ��й����Ժ����������
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			iCurYear = tmDate.year();
			iCurMonth = tmDate.month();
			{
				RStockData item;
				getColorBlockItemByDays(item,monthHis);
				pMap->insert(item.tmTime,item);
				monthHis.clear();
			}
		}
		else if(((tmDate.month()-1)/3)!=((iCurMonth-1)/3))
		{
			iCurMonth = tmDate.month();

			RStockData item;
			getColorBlockItemByDays(item,monthHis);
			pMap->insert(item.tmTime,item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//���ʣ���Ҳ����
		RStockData item;
		getColorBlockItemByDays(item,monthHis);
		pMap->insert(item.tmTime,item);
		monthHis.clear();
	}
	return pMap->size();
}

int getColorBlockYearItems(QMap<time_t,RStockData>* pMap,const QList<qRcvHistoryData*>& historys)
{
	if(historys.size()<1)
		return 0;
	int iCurYear = 0;
	{
		//first day's week and year.
		QDate tmDate = QDateTime::fromTime_t(historys.first()->time).date();
		iCurYear = tmDate.year();
	}

	QList<qRcvHistoryData*> monthHis;		//�����ڽ��й����Ժ����������
	for(int i=0;i<historys.size();++i)
	{
		qRcvHistoryData* pHistory = historys[i];
		QDate tmDate = QDateTime::fromTime_t(pHistory->time).date();
		if(tmDate.year()!=iCurYear)
		{
			iCurYear = tmDate.year();
			{
				RStockData item;
				getColorBlockItemByDays(item,monthHis);
				pMap->insert(item.tmTime,item);
				monthHis.clear();
			}
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//���ʣ���Ҳ����
		RStockData item;
		getColorBlockItemByDays(item,monthHis);
		pMap->insert(item.tmTime,item);
		monthHis.clear();
	}
	return pMap->size();
}


CColorBlockWidget::CColorBlockWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseBlockWidget(parent,CBaseWidget::StockColorBlock)
	, m_pMenuBlockList(0)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, showStockIndex(0)
	, m_pSelectedStock(0)
	, m_pBlock(0)
{
	//���õ�ǰ��ʾ�İ��
	m_pMenuBlockList = m_pMenuCustom->addMenu(tr("���õ�ǰ���"));
}

CColorBlockWidget::~CColorBlockWidget(void)
{
	clearTmpData();
}

bool CColorBlockWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseBlockWidget::loadPanelInfo(eleWidget))
		return false;

	//��ǰ�İ������
	QDomElement eleBlock = eleWidget.firstChildElement("block");
	if(eleBlock.isElement())
	{
		setBlock(eleBlock.text());
	}

	return true;
}

bool CColorBlockWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseBlockWidget::savePanelInfo(doc,eleWidget))
		return false;

	//��ǰ�İ������
	QDomElement eleBlock = doc.createElement("block");
	eleBlock.appendChild(doc.createTextNode(m_pBlock->getBlockName()));
	eleWidget.appendChild(eleBlock);

	return true;
}

void CColorBlockWidget::updateData()
{
	setBlock(m_pBlock->getBlockName());
	return CBaseBlockWidget::updateData();
}

void CColorBlockWidget::setBlock( const QString& block )
{
	clearTmpData();
	CBlockInfoItem* pBlock = CDataEngine::getDataEngine()->getStockBlock(block);
	if(!pBlock)
		return;

	m_listStocks = pBlock->getStockList();
	showStockIndex = 0;
	for(int i=0;i<m_listStocks.size();++i)
	{
		CStockInfoItem* pItem = m_listStocks[i];

		m_mapStockIndex[pItem] = i;
		//�������»���
		connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateStock(const QString&)));
		connect(pItem,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateStock(const QString&)));
	}

	m_pBlock = pBlock;

	if(m_listStocks.size()>0)
	{
		clickedStock(m_listStocks.first());
	}

	updateTimesH();			//���º������ʱ������
	update();

	return CBaseWidget::setBlock(block);
}

void CColorBlockWidget::updateStock( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);

	if(mapStockColorBlocks.contains(pItem))
	{
		//��������
		QMap<time_t,RStockData>* pMap = mapStockColorBlocks[pItem];
		mapStockColorBlocks[pItem] = 0;
		delete pMap;
		mapStockColorBlocks[pItem] = getColorBlockMap(pItem);
		updateTimesH();
	}
	update(rectOfStock(pItem));
}


void CColorBlockWidget::onSetCurrentBlock()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	setBlock(pAct->data().toString());
}

void CColorBlockWidget::updateColorBlockData()
{
	QList<CStockInfoItem*> listShowItems;
	int iClientHeight = this->rect().height();

	//��ȡ��ǰ��Ҫ��ʾ�Ĺ�Ʊ�б�
	int iIndex = showStockIndex;
	while (iIndex<m_listStocks.size())
	{
		if((iIndex-showStockIndex)*m_iCBHeight<iClientHeight)
		{
			listShowItems.push_back(m_listStocks[iIndex]);
		}
		else
		{
			break;
		}
		++iIndex;
	}

	/*
	//��map��ɾ������Ҫ��ʾ�Ĺ�Ʊ
	QMap<CStockInfoItem*,QMap<time_t,stColorBlockItem>*>::iterator iter = mapStockColorBlocks.begin();
	while (iter!=mapStockColorBlocks.end())
	{
		if(!listShowItems.contains(iter.key()))
		{
			delete iter.value();
			mapStockColorBlocks.remove(iter.key());
		}

		++iter;
	}*/


	
	//����Ҫ��ʾ��map��û�еĹ�Ʊ���뵽map��
	foreach(CStockInfoItem* p,listShowItems)
	{
		if(!mapStockColorBlocks.contains(p))
		{
			mapStockColorBlocks[p] = getColorBlockMap(p);
		}
	}
}

void CColorBlockWidget::clearTmpData()
{
	foreach(CStockInfoItem* p,m_listStocks)
	{
		//�Ƴ����к� updateStock������ �ź�/��
		disconnect(p,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateStock(const QString&)));
		disconnect(p,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateStock(const QString&)));
	}
	m_pSelectedStock = 0;
	m_listStocks.clear();

	QMap<CStockInfoItem*,QMap<time_t,RStockData>*>::iterator iter = mapStockColorBlocks.begin();
	while(iter!=mapStockColorBlocks.end())
	{
		delete iter.value();
		++iter;
	}

	mapStockColorBlocks.clear();
	m_mapStockIndex.clear();
}

void CColorBlockWidget::clickedStock( CStockInfoItem* pItem )
{
	if(pItem == m_pSelectedStock)
		return;
	int iShowCount = m_rtClient.height()/m_iCBHeight;
	int iRow = m_mapStockIndex[pItem];
	if((iRow<showStockIndex)||(iRow>showStockIndex+iShowCount))
	{
		showStockIndex = iRow;
		update(m_rtClient);
	}

	CStockInfoItem* pPreSelectedStock = m_pSelectedStock;
	m_pSelectedStock = pItem;
	update(rectOfStock(pPreSelectedStock));
	update(rectOfStock(m_pSelectedStock));
}

void CColorBlockWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	m_rtHeader = QRect(rtClient.left(),rtClient.top(),rtClient.width(),m_iTitleHeight);
	m_rtClient = QRect(rtClient.left(),rtClient.top()+m_iTitleHeight,rtClient.width(),rtClient.height()-m_iTitleHeight-m_iBottomHeight);
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);

	drawHeader(p,m_rtHeader);
	drawClient(p,m_rtClient);
	drawBottom(p,m_rtBottom);
}

void CColorBlockWidget::mouseMoveEvent( QMouseEvent* e )
{
	CStockInfoItem* pStock = hitTestStock(e->pos());
	RStockData item = hitTestCBItem(e->pos());
	if(item.fClose<0.1 || pStock==0)
	{
		QToolTip::hideText();
		return CBaseWidget::mouseMoveEvent(e);
	}

	QString qsTooltip;		//Tips
	if(m_typeCircle<Day)
	{
		qsTooltip = QString("��Ʊ����:%1\r\nʱ��:%2\r\n�۸�:%3")
			.arg(pStock->getName()).arg(QDateTime::fromTime_t(item.tmTime).toString("hh:mm:ss"))
			.arg(item.fClose);
	}
	else
	{
		QString qsTime;
		QDate dtTmp = QDateTime::fromTime_t(item.tmTime).date();
		if(m_typeCircle == Week)
			qsTime = QString("%1 %2").arg(dtTmp.year()).arg(dtTmp.weekNumber());
		else if(m_typeCircle == Month)
			qsTime = dtTmp.toString("yyyy/MM");
		else if(m_typeCircle == Month3)
			qsTime = dtTmp.toString("yyyy/MM");
		else if(m_typeCircle == Year)
			qsTime = dtTmp.toString("yyyy");
		else
			qsTime = dtTmp.toString("yyyy/MM/dd");

		qsTooltip = QString("��Ʊ����:%1\r\nʱ��:%2\r\n�۸�:%3")
			.arg(pStock->getName()).arg(qsTime)
			.arg(item.fClose);
	}

	QToolTip::showText(e->globalPos(),qsTooltip,this);

	return CBaseWidget::mouseMoveEvent(e);
}

void CColorBlockWidget::mousePressEvent( QMouseEvent* e )
{
	e->accept();
	QPoint ptCur = e->pos();
	if(m_rtHeader.contains(ptCur))
	{

	}
	else if(m_rtClient.contains(ptCur))
	{
		int iCurIndex = showStockIndex+(ptCur.y()-m_rtClient.top())/m_iCBHeight;
		if(iCurIndex>=0&&iCurIndex<m_listStocks.size())
		{
			clickedStock(m_listStocks[iCurIndex]);
		}
	}
	else if(m_rtBottom.contains(ptCur))
	{

	}
}

void CColorBlockWidget::wheelEvent( QWheelEvent* e )
{
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;
	int iIndex = showStockIndex-numSteps*5;
	if(iIndex<0) {iIndex = 0;}
	if(iIndex>=0&&iIndex<m_listStocks.size())
	{
		e->accept();
		showStockIndex = iIndex;
		update();
	}
	return CBaseWidget::wheelEvent(e);
}

void CColorBlockWidget::keyPressEvent( QKeyEvent* e )
{
	if(Qt::Key_Left == e->key())
	{
		e->accept();
		return;
	}
	else if(Qt::Key_Right == e->key())
	{
		e->accept();
		return;
	}
	else if(Qt::Key_Down == e->key())
	{
		int iCurIndex = m_mapStockIndex[m_pSelectedStock];
		if(m_listStocks.size()>(iCurIndex+1))
		{
			CStockInfoItem* pItem = m_listStocks[iCurIndex+1];
			int iRow = m_mapStockIndex[pItem];
			int iShowCount = m_rtClient.height()/m_iCBHeight;
			if(iShowCount<1)
				return;
			if((iRow-showStockIndex)>=iShowCount)
			{
				showStockIndex = iRow-iShowCount+1;
				update(m_rtClient);
			}
			clickedStock(m_listStocks[iCurIndex+1]);
		}
		e->accept();
	}
	else if(Qt::Key_Up == e->key())
	{
		int iCurIndex = m_mapStockIndex[m_pSelectedStock];
		if(iCurIndex>0)
		{
			CStockInfoItem* pItem = m_listStocks[iCurIndex-1];
			int iRow = m_mapStockIndex[pItem];
			if(iRow<showStockIndex)
			{
				showStockIndex = iRow;
				update(m_rtClient);
			}
			clickedStock(pItem);
		}
		e->accept();
	}
	else if(Qt::Key_PageDown == e->key())
	{
		int iShowCount = m_rtClient.height()/m_iCBHeight;
		if(iShowCount<1)
			return;
		if((showStockIndex+iShowCount)<m_listStocks.size())
		{
			showStockIndex = showStockIndex+iShowCount;
			update(m_rtClient);
		}
		e->accept();
	}
	else if(Qt::Key_PageUp == e->key())
	{
		int iShowCount = m_rtClient.height()/m_iCBHeight;
		if(iShowCount<1)
			return;
		showStockIndex = (showStockIndex-iShowCount)>0 ? (showStockIndex-iShowCount) : 0;
		update(m_rtClient);

		e->accept();
	}

	return CBaseWidget::keyPressEvent(e);
}

QMenu* CColorBlockWidget::getCustomMenu()
{
	CBaseBlockWidget::getCustomMenu();
	{
		//�������а��Ĳ˵�
		m_pMenuBlockList->clear();
		QList<CBlockInfoItem*> list = CDataEngine::getDataEngine()->getStockBlocks();
		foreach(const CBlockInfoItem* block,list)
		{
			QAction* pAct = m_pMenuBlockList->addAction(block->getBlockName(),this,SLOT(onSetCurrentBlock()));
			pAct->setData(block->getBlockName());
			if(m_pBlock == block)
			{
				pAct->setCheckable(true);
				pAct->setChecked(true);
			}
		}
	}

	return m_pMenuCustom;
}

void CColorBlockWidget::drawHeader( QPainter& p,const QRect& rtHeader )
{
	p.fillRect(rtHeader,QColor(0,0,0));
	p.setPen(QColor(255,0,0));
	QRect rtCoord = rtHeader.adjusted(0,0,-1,-1);
	p.drawRect(rtCoord);

	p.setPen(QColor(255,255,255));
	if(!m_pBlock)
		return;
	p.drawText(rtHeader,Qt::AlignLeft|Qt::AlignVCenter,m_pBlock->getBlockName());
}

void CColorBlockWidget::drawClient( QPainter& p,const QRect& rtClient )
{
	p.fillRect(rtClient,QColor(0,0,0));

	updateColorBlockData();
	int iCurY = rtClient.top();
	int iIndex = showStockIndex;
	for(;iCurY<rtClient.bottom();iCurY=(iCurY+m_iCBHeight))
	{
		if(iIndex<0||iIndex>=m_listStocks.size())
			break;

		drawStock(p,QRect(rtClient.left(),iCurY,rtClient.width(),m_iCBHeight),m_listStocks[iIndex]);
		++iIndex;
	}
}

void CColorBlockWidget::drawBottom( QPainter& p,const QRect& rtBottom )
{
	p.fillRect(rtBottom,QColor(0,0,0));

	QRectF rtColors = QRectF(rtBottom.left(),rtBottom.top(),50,rtBottom.height());
	float fColorsWidth = rtColors.width()-5;
	float fColorWidth = fColorsWidth/COLOR_BLOCK_SIZE;
	for(int i=0;i<COLOR_BLOCK_SIZE;++i)
	{
		p.fillRect(QRectF(rtBottom.left()+i*fColorWidth,rtBottom.top(),fColorWidth,rtBottom.height()),
			CColorManager::getBlockColor(m_qsColorMode,i));
	}

	//����������ƺ�����
	drawCoordX(p,QRect(rtBottom.left()+RCB_OFFSET_LEFT,rtBottom.top(),
		rtBottom.width()-RCB_OFFSET_Y-RCB_OFFSET_LEFT,rtBottom.height()),
		m_iCBWidth);
}

void CColorBlockWidget::drawStock( QPainter& p,const QRect& rtCB,CStockInfoItem* pItem )
{
	if(pItem == m_pSelectedStock)
	{
		p.fillRect(rtCB,QColor(50,50,50));
	}

	{
		//�������ı�ʶ��Ϣ������������ƣ�
		p.setPen(QColor(255,255,255));
		QString qsText = pItem->getName();
		if(qsText.isEmpty())
			qsText = pItem->getCode();
		p.drawText(QRect(rtCB.left(),rtCB.top(),RCB_OFFSET_LEFT,m_iCBHeight),Qt::AlignCenter,qsText);
	}

	//����������ƺ�����
	float fBeginX = rtCB.right()-RCB_OFFSET_Y;
	float fEndX = rtCB.left()+RCB_OFFSET_LEFT;
	float fCBWidth = fBeginX-fEndX;
	if(fCBWidth<0)
		return;
	if(!mapStockColorBlocks.contains(pItem))
		return;

	QMap<time_t,RStockData>* pMapCBs = mapStockColorBlocks[pItem];
	RCalcInfo calc;
	RDrawInfo draw;
	{
		calc.dwVersion = RSTOCK_VER;
		calc.emCircle = m_typeCircle;
		calc.mapData = pMapCBs;
		calc.mapDataEx = NULL;
		calc.pItem = pItem;

		draw.dwVersion = RSTOCK_VER;
		draw.pPainter = &p;
		draw.rtClient = rtCB;
	}
	lua_pushlightuserdata(m_pL,&calc);
	lua_setglobal(m_pL,"_calc");

	lua_pushlightuserdata(m_pL,&draw);
	lua_setglobal(m_pL,"_draw");

	luaL_dostring(m_pL,"return HIGH()");
//	QString sss = lua_tostring(m_pL,-1);


	QMap<time_t,RStockData>::iterator iter = pMapCBs->begin();
	float fLastPrice = pItem->getCurrentReport()->fLastClose;
	while(iter!=pMapCBs->end())
	{
		float f = FLOAT_NAN;
		QRect rtB;
		if(m_typeCircle<Day)
		{
			time_t tmCur = iter.key()/(m_typeCircle)*(m_typeCircle);		//����ȡ��
			if(m_mapTimes.contains(tmCur))
			{
				float fCurX = fBeginX - ((m_mapTimes[tmCur])*m_iCBWidth);
				if(fCurX>=fEndX)
				{
					//���������ٷֱ�
					f = (iter->fClose - fLastPrice)/fLastPrice*10.0;
					rtB = QRect(fCurX,rtCB.top(),m_iCBWidth,m_iCBHeight);
				}
			}
		}
		else
		{
			time_t tmCur = iter.key();
			QMap<time_t,int>::iterator iterTime = m_mapTimes.upperBound(tmCur);
			if(iterTime!=m_mapTimes.begin())
			{
				--iterTime;
				float fCurX = fBeginX - ((iterTime.value())*m_iCBWidth);
				if(fCurX>=fEndX)
				{
					//���������ٷֱ�
					f = (iter->fClose - fLastPrice)/fLastPrice;
					if(m_typeCircle>DayN)
						f = f/10.0;							//�������ߵģ���Ա���������С
					rtB = QRect(fCurX,rtCB.top(),m_iCBWidth,m_iCBHeight);
				}
			}
		}
		if(f!=FLOAT_NAN)
		{
			switch(m_typeBlock)
			{
			case BlockRect:
				{
					rtB.adjust(1,1,-1,-1);
					p.fillRect(rtB,CColorManager::getBlockColor(m_qsColorMode,f));
				}
				break;
			case BlockCircle:
				{
					QPainterPath path;
					path.addEllipse(rtB);
					p.fillPath(path,CColorManager::getBlockColor(m_qsColorMode,f));
				}
				break;
			}
		}
		fLastPrice = iter->fClose;
		++iter;
	}
}

QRect CColorBlockWidget::rectOfStock( CStockInfoItem* pItem )
{
	if(m_mapStockIndex.contains(pItem))
	{
		int iRow = m_mapStockIndex[pItem];
		return QRect(m_rtClient.left(),(m_rtClient.top()+(iRow-showStockIndex)*m_iCBHeight),m_rtClient.width(),m_iCBHeight);
	}

	return QRect();
}

CStockInfoItem* CColorBlockWidget::hitTestStock( const QPoint& ptPoint ) const
{
	int iRow = (ptPoint.y()-m_rtClient.top())/m_iCBHeight + showStockIndex;
	if(iRow<0||iRow>=m_listStocks.size())
		return 0;

	return m_listStocks[iRow];
}

RStockData CColorBlockWidget::hitTestCBItem( const QPoint& ptPoint ) const
{
	CStockInfoItem* pItem = hitTestStock(ptPoint);
	if(pItem && mapStockColorBlocks.contains(pItem))
	{
		QMap<time_t,RStockData>* pMap = mapStockColorBlocks[pItem];
		QMap<time_t,int>::iterator iterLastTime = m_mapTimes.end();
		if(iterLastTime!=m_mapTimes.begin())
		{
			--iterLastTime;								//ȡʱ�����ϵ����һ��ʱ���
			int iIndex = (m_rtClient.right() - RCB_OFFSET_Y - ptPoint.x())/m_iCBWidth + 1;
			time_t tmLast = iterLastTime.key();			//ʱ�����ϵ����һ��ʱ���
			time_t tmCur = tmLast;
			if (m_typeCircle<Day)
			{
				tmCur = tmCur - (m_typeCircle*iIndex);
			}
			else
			{
				time_t tmDefault = 0;
				tmCur = m_mapTimes.key(iIndex,tmDefault);
				if(tmCur==tmDefault)
					return RStockData();
			}

			//��ȡ����Ľڵ�
			QMap<time_t,RStockData>::iterator iterCB = pMap->lowerBound(tmCur);
			if(m_typeCircle<Day)
			{
				if(iterCB==pMap->end())
					return RStockData();
				if(iterCB.key()/m_typeCircle*m_typeCircle != (tmCur))
					return RStockData();
			}
			else
			{
				if(iterCB==pMap->begin())
					return RStockData();
				if(m_typeCircle==Day)
				{
					return (pMap->find(tmCur)!=pMap->end()) ? pMap->find(tmCur).value() : RStockData();
				}
				else if(m_typeCircle == Week)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					int iTheYear = dtKey.year();
					return dtKey.weekNumber(&iTheYear)==dtCur.weekNumber(&iTheYear) ? iterCB.value() : RStockData();
				}
				else if(m_typeCircle == Month)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					return ((dtKey.year()==dtKey.year())&&(dtKey.month()==dtCur.month())) ? iterCB.value() : RStockData();
				}
				else if(m_typeCircle == Month3)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					return ((dtKey.year()==dtKey.year())&&(((dtKey.month()-1)/3)==((dtCur.month()-1)/3))) ? iterCB.value() : RStockData();
				}
				else if(m_typeCircle == Year)
				{
					QDate dtKey = QDateTime::fromTime_t(iterCB.key()).date();
					QDate dtCur = QDateTime::fromTime_t(tmCur).date();
					return (dtKey.year()==dtKey.year()) ? iterCB.value() : RStockData();
				}
			}
			return iterCB.value();
		}
	}
	return RStockData();
}

QMap<time_t,RStockData>* CColorBlockWidget::getColorBlockMap(CStockInfoItem* pItem)
{
	QMap<time_t,RStockData>* pMap = new QMap<time_t,RStockData>();
	if(m_typeCircle < Day)
	{
		//��ȡ�������ݣ����м���
		QList<qRcvFenBiData*> FenBis = pItem->getFenBiList();
		getColorBlockMinItems(pMap,FenBis,m_typeCircle);
	}
	else
	{
		//��ȡ��������
		QList<qRcvHistoryData*> historys = pItem->getHistoryList();
		qRcvReportData* pLastReport = pItem->getCurrentReport();
		bool bAppendLast = true;
		if(historys.size()>0 && pLastReport)
		{
			qRcvHistoryData* pLastHistory = historys.last();
			if(QDateTime::fromTime_t(pLastHistory->time).date() == QDateTime::fromTime_t(pLastReport->tmTime).date())
				bAppendLast = false;
		}
		if(pLastReport&&bAppendLast)
		{
			historys.push_back(new qRcvHistoryData(pLastReport));
		}
		if(m_typeCircle == Day)
		{
			getColorBlockDayItems(pMap,historys);
		}
		else if(m_typeCircle == DayN)
		{
			//Ŀǰδʹ��
			//	getLinerItem(listItems,historys,3);
		}
		else if(m_typeCircle == Week)
		{
			getColorBlockWeekItems(pMap,historys);
		}
		else if(m_typeCircle == Month)
		{
			getColorBlockMonthItems(pMap,historys);
		}
		else if(m_typeCircle == Month3)
		{
			getColorBlockMonth3Items(pMap,historys);
		}
		else if(m_typeCircle == Year)
		{
			getColorBlockYearItems(pMap,historys);
		}

		{
			//�����ȡ����������
			foreach(qRcvHistoryData* p,historys)
				delete p;
			historys.clear();
		}
	}

	return pMap;
}