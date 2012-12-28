#include "StdAfx.h"
#include "ColorBlockWidget.h"
#include "DataEngine.h"
#include "ColorManager.h"
#define	RCB_OFFSET_Y	2
#define RCB_OFFSET_LEFT	50

/* ��ĳ���ӵ����ݽ���ת�� (qRcvMinuteData*) -> (stColorBlockItem) */
bool getColorBlockItemByMin(stColorBlockItem& item, qRcvFenBiData* pMin)
{
	if(!pMin)
		return false;

	item.tmTime = pMin->tmTime;
	item.fAmount = pMin->fAmount;
	item.fPrice = pMin->fPrice;
	item.fVolume = pMin->fVolume;
	return true;
}

/*ͨ���������ݻ�ȡָ�������ڵ�����*/
bool getColorBlockItemByMins(stColorBlockItem& item, const QList<qRcvFenBiData*>& list, qRcvFenBiData* pLastFenbi)
{
	if(list.size()<1)
		return false;

//	qRcvFenBiData* pBegin = list.first();
	qRcvFenBiData* pLast = list.last();
	item.tmTime = pLast->tmTime;
	item.fPrice = pLast->fPrice;

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

	return true;
}

int getColorBlockMinItems(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvFenBiData*>& minutes, int iSize = 1)
{
	QList<qRcvFenBiData*> listMins;
	time_t tmT = 0;
	qRcvFenBiData* pLastFenbi = 0;
	foreach(qRcvFenBiData* p, minutes)
	{
		if((p->tmTime-tmT)>(iSize))
		{
			stColorBlockItem item;
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
		stColorBlockItem item;
		if(getColorBlockItemByMins(item,listMins,pLastFenbi))
			pMap->insert(item.tmTime,item);
		listMins.clear();
	}


	return pMap->size();
}

/* ��ĳ������ݽ���ת�� (qRcvHistoryData*) -> (stColorBlockItem) */
bool getColorBlockItemByDay(stColorBlockItem& item,const qRcvHistoryData* pHistory)
{
	if(!pHistory)
		return false;
	item.tmTime = pHistory->time;
	item.fPrice = pHistory->fClose;
	item.fAmount = pHistory->fAmount;
	item.fVolume = pHistory->fVolume;
	return true;
}
/*ͨ���������ݻ�ȡһ�������ڵ�����*/
bool getColorBlockItemByDays(stColorBlockItem& item,const QList<qRcvHistoryData*>& list)
{
	if(list.size()<1)
		return false;

//	qRcvHistoryData* pBegin = list.first();
	qRcvHistoryData* pLast = list.last();
	item.tmTime = pLast->time;
	item.fPrice = pLast->fClose;

	item.fAmount = 0;
	item.fVolume = 0;
	foreach(qRcvHistoryData* p,list)
	{
		item.fAmount+=p->fAmount;
		item.fVolume+=p->fVolume;
	}
	return true;
}

int getColorBlockDayItems(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvHistoryData*>& historys)
{
	foreach(qRcvHistoryData* p,historys)
	{
		stColorBlockItem item;
		if(getColorBlockItemByDay(item,p))
			pMap->insert(item.tmTime,item);
	}

	return pMap->size();
}

int getColorBlockWeekItems(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvHistoryData*>& historys)
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
			iCurYear = tmDate.year();
			iCurWeek = tmDate.weekNumber();
			if(tmDate.dayOfWeek()==1)
			{
				stColorBlockItem item;
				getColorBlockItemByDays(item,weekHis);
				pMap->insert(item.tmTime,item);
				weekHis.clear();
			}
		}
		else if(tmDate.weekNumber()!=iCurWeek)
		{
			iCurWeek = tmDate.weekNumber();

			stColorBlockItem item;
			getColorBlockItemByDays(item,weekHis);
			pMap->insert(item.tmTime,item);
			weekHis.clear();
		}
		weekHis.push_back(pHistory);
	}

	return pMap->size();
}

int getColorBlockMonthItems(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvHistoryData*>& historys)
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
				stColorBlockItem item;
				getColorBlockItemByDays(item,monthHis);
				pMap->insert(item.tmTime,item);
				monthHis.clear();
			}
		}
		else if(tmDate.month()!=iCurMonth)
		{
			iCurMonth = tmDate.month();

			stColorBlockItem item;
			getColorBlockItemByDays(item,monthHis);
			pMap->insert(item.tmTime,item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	return pMap->size();
}

int getColorBlockMonth3Items(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvHistoryData*>& historys)
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
				stColorBlockItem item;
				getColorBlockItemByDays(item,monthHis);
				pMap->insert(item.tmTime,item);
				monthHis.clear();
			}
		}
		else if(((tmDate.month()-1)/3)!=((iCurMonth-1)/3))
		{
			iCurMonth = tmDate.month();

			stColorBlockItem item;
			getColorBlockItemByDays(item,monthHis);
			pMap->insert(item.tmTime,item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	return pMap->size();
}

int getColorBlockYearItems(QMap<time_t,stColorBlockItem>* pMap,const QList<qRcvHistoryData*>& historys)
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
				stColorBlockItem item;
				getColorBlockItemByDays(item,monthHis);
				pMap->insert(item.tmTime,item);
				monthHis.clear();
			}
		}
		monthHis.push_back(pHistory);
	}

	return pMap->size();
}


//�����ʱ���ݵĺ�����ʱ��
int getTimeMapByMin(QMap<time_t,int>& mapTimes,time_t& tmBegin, time_t& tmEnd, int iSize = 60/*second*/)
{
	if(tmBegin>tmEnd)
		return 0;

	time_t tmCur = tmEnd;
	while(tmCur>=tmBegin)
	{
//		QString qsTime = QDateTime::fromTime_t(tmCur).toString();
		mapTimes.insert(tmCur,mapTimes.size());
		tmCur = tmCur-iSize;
	}
	return 1;
}


CColorBlockWidget::CColorBlockWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::ColorBlock)
	, m_pMenuCustom(0)
	, m_pMenuColorMode(0)
	, m_pMenuBlockMode(0)
	, m_pMenuBlockList(0)
	, m_iTitleHeight(16)
	, m_iBottomHeight(16)
	, m_iCBHeight(16)
	, m_iCBWidth(16)
	, showStockIndex(0)
	, m_pSelectedStock(0)
	, m_qsColorMode("")
	, m_typeBlock(BlockCircle)
	, m_typeCircle(Day)
{
	//��ʼ���˵�
	m_pMenuCustom = new QMenu(tr("ɫ��ͼ�˵�"));

	{
		//���õ�ǰK��ͼ����ʾ����
		m_pMenuCircle = m_pMenuCustom->addMenu(tr("��������"));
		m_pMenuCircle->addAction(tr("10���ʱͼ"),this,SLOT(onSetCircle()))->setData(Sec10);
		m_pMenuCircle->addAction(tr("30���ʱͼ"),this,SLOT(onSetCircle()))->setData(Sec30);
		m_pMenuCircle->addAction(tr("1���ӷ�ʱͼ"),this,SLOT(onSetCircle()))->setData(Min1);
		m_pMenuCircle->addAction(tr("5���ӷ�ʱͼ"),this,SLOT(onSetCircle()))->setData(Min5);
		m_pMenuCircle->addAction(tr("15���ӷ�ʱͼ"),this,SLOT(onSetCircle()))->setData(Min15);
		m_pMenuCircle->addAction(tr("30���ӷ�ʱͼ"),this,SLOT(onSetCircle()))->setData(Min30);
		m_pMenuCircle->addAction(tr("60���ӷ�ʱͼ"),this,SLOT(onSetCircle()))->setData(Min60);

		m_pMenuCircle->addAction(tr("����ͼ"),this,SLOT(onSetCircle()))->setData(Day);
		m_pMenuCircle->addAction(tr("����ͼ"),this,SLOT(onSetCircle()))->setData(Week);
		m_pMenuCircle->addAction(tr("����ͼ"),this,SLOT(onSetCircle()))->setData(Month);
		m_pMenuCircle->addAction(tr("����ͼ"),this,SLOT(onSetCircle()))->setData(Month3);
		m_pMenuCircle->addAction(tr("����ͼ"),this,SLOT(onSetCircle()))->setData(Year);
	}
	{
		//��ɫ��ʾģʽ�˵�
		m_pMenuColorMode = m_pMenuCustom->addMenu("������ɫģʽ");
		//������ʾ��ʽ��Բ��/���飩
		m_pMenuBlockMode = m_pMenuCustom->addMenu("������ʾ��״");
		m_pMenuBlockMode->addAction("Բ��",this,SLOT(onSetBlockMode()))->setData(BlockCircle);
		m_pMenuBlockMode->addAction("����",this,SLOT(onSetBlockMode()))->setData(BlockRect);
		//����ɫ��Ĵ�С
		m_pMenuCustom->addAction(tr("����ɫ���С"),this,SLOT(onSetBlockSize()));

		//���õ�ǰ��ʾ�İ��
		m_pMenuBlockList = m_pMenuCustom->addMenu(tr("���õ�ǰ���"));
	}

	m_pMenuCustom->addSeparator();
}

CColorBlockWidget::~CColorBlockWidget(void)
{
	clearTmpData();
	delete m_pMenuCustom;
}

bool CColorBlockWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	//��ǰ��ʾ������
	if(eleWidget.hasAttribute("circle"))
	{
		m_typeCircle = static_cast<ColorBlockCircle>(eleWidget.attribute("circle").toInt());
	}
	//ɫ��Ŀ��
	if(eleWidget.hasAttribute("CBWidth"))
	{
		m_iCBWidth = eleWidget.attribute("CBWidth").toInt();
	}
	//ɫ��ĸ߶�
	if(eleWidget.hasAttribute("CBHeight"))
	{
		m_iCBHeight = eleWidget.attribute("CBHeight").toInt();
	}

	//��ǰ�İ������
	QDomElement eleBlock = eleWidget.firstChildElement("block");
	if(eleBlock.isElement())
	{
		setBlock(eleBlock.text());
	}

	//��ǰ����ɫģʽ
	QDomElement eleColorMode = eleWidget.firstChildElement("color");
	if(eleColorMode.isElement())
	{
		m_qsColorMode = eleColorMode.text();
	}


	//��ǰ����ʾ��״ģʽ
	QDomElement eleBlockMode = eleWidget.firstChildElement("mode");
	if(eleBlockMode.isElement())
	{
		m_typeBlock = static_cast<CColorBlockWidget::BlockMode>(eleBlockMode.text().toInt());
	}


	return true;
}

bool CColorBlockWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;


	//��ʾ������
	eleWidget.setAttribute("circle",m_typeCircle);
	eleWidget.setAttribute("CBWidth",m_iCBWidth);
	eleWidget.setAttribute("CBHeight",m_iCBHeight);

	//��ǰ�İ������
	QDomElement eleBlock = doc.createElement("block");
	eleBlock.appendChild(doc.createTextNode(m_qsBlock));
	eleWidget.appendChild(eleBlock);

	//��ǰ����ɫģʽ
	QDomElement eleColorMode = doc.createElement("color");
	eleColorMode.appendChild(doc.createTextNode(m_qsColorMode));
	eleWidget.appendChild(eleColorMode);

	//��ǰ����ʾ��״ģʽ
	QDomElement eleBlockMode = doc.createElement("mode");
	eleBlockMode.appendChild(doc.createTextNode(QString("%1").arg(m_typeBlock)));
	eleWidget.appendChild(eleBlockMode);


	return true;
}

void CColorBlockWidget::setBlock( const QString& block )
{
	clearTmpData();

	m_listStocks = CDataEngine::getDataEngine()->getStocksByBlock(block);
	showStockIndex = 0;
	for(int i=0;i<m_listStocks.size();++i)
	{
		CStockInfoItem* pItem = m_listStocks[i];

		m_mapStockIndex[pItem] = i;
		//�������»���
		connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateStock(const QString&)));
		connect(pItem,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateStock(const QString&)));
	}

	m_qsBlock = block;

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
		QMap<time_t,stColorBlockItem>* pMap = mapStockColorBlocks[pItem];
		mapStockColorBlocks[pItem] = 0;
		delete pMap;
		mapStockColorBlocks[pItem] = getColorBlockMap(pItem);
		updateTimesH();
	}
	update(rectOfStock(pItem));
}

void CColorBlockWidget::setColorMode( const QString& mode )
{
	m_qsColorMode = mode;
	//QList<QAction*> listActs = m_pMenuColorMode->actions();
	//foreach(QAction* pAct,listActs)
	//{
	//	pAct->setChecked((pAct->data().toString() == mode) ? true : false);
	//}

	update();
}

void CColorBlockWidget::onSetCircle()
{
	//���õ�ǰ����ʾ����
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	m_typeCircle = static_cast<ColorBlockCircle>(pAct->data().toInt());
	setBlock(m_qsBlock);
}

void CColorBlockWidget::onSetColorMode()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	setColorMode(pAct->data().toString());
}

void CColorBlockWidget::onSetBlockMode()
{
	//���õ�ǰ����ʾ����
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	m_typeBlock = static_cast<BlockMode>(pAct->data().toInt());
	update();
}

void CColorBlockWidget::onSetBlockSize()
{
	//��������ɫ���С�ĶԻ�����������ɫ��Ĵ�С
	QDialog dlg(this);
	QGridLayout layout(&dlg);
	QLabel label1(tr("���"),&dlg);
	QLabel label2(tr("�߶�"),&dlg);
	QLineEdit editW(&dlg);
	QLineEdit editH(&dlg);
	QPushButton btnOk(&dlg);
	dlg.setLayout(&layout);
	layout.addWidget(&label1,0,0,1,1);
	layout.addWidget(&label2,1,0,1,1);
	layout.addWidget(&editW,0,1,1,1);
	layout.addWidget(&editH,1,1,1,1);
	layout.addWidget(&btnOk,2,0,1,2);
	btnOk.setText(tr("ȷ��"));
	QIntValidator intValidator(1,100);
	editW.setValidator(&intValidator);
	editH.setValidator(&intValidator);
	editW.setText(QString("%1").arg(m_iCBWidth));
	editH.setText(QString("%1").arg(m_iCBHeight));
	dlg.setWindowTitle(tr("ɫ���С����"));
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	int iW = editW.text().toInt();
	int iH = editH.text().toInt();
	if((iW>0&&iH>0))
	{
		if(iW!=m_iCBWidth||iH!=m_iCBHeight)
		{
			m_iCBHeight = iH;
			m_iCBWidth = iW;
			update();
		}
	}
	else
	{
		QMessageBox::information(this,tr("��ʾ"),tr("����ɫ���Сʧ�ܣ�����������룡"));
	}
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

	QMap<CStockInfoItem*,QMap<time_t,stColorBlockItem>*>::iterator iter = mapStockColorBlocks.begin();
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

void CColorBlockWidget::updateTimesH()
{
	//���µ�ǰ�ĺ��������ݣ��Ӻ���ǰ����ʱ��
	m_mapTimes.clear();

//	int iCount = 1024;				//����1024��ʱ��
	if(m_typeCircle<Day)
	{
		time_t tmLast = ((QDateTime::currentDateTime().toTime_t()/(3600*24))*3600*24)+3600*(9-8)+60*30;
		time_t tmCurrent = (QDateTime::currentDateTime().toTime_t()+m_typeCircle)/m_typeCircle*m_typeCircle;//���϶Է���ȡ��
		//time_t tmNoon1 = ((QDateTime::currentDateTime().toTime_t()/(3600*24))*3600*24)+3600*(11-8)+60*30;
		//time_t tmNoon2 = ((QDateTime::currentDateTime().toTime_t()/(3600*24))*3600*24)+3600*(13-8);

		if((tmCurrent%(3600*24))>3600*7)
		{
			tmCurrent = (tmCurrent/(3600*24))*3600*24 + 3600*7 + m_typeCircle*2;		//3������(���һ������)
		}
		/*�����Ϻ����¶������������*/
		//if(tmCurrent>tmNoon1)
		//{
		//	time_t tmBegin = tmLast-m_typeCircle;
		//	time_t tmEnd = tmNoon1+m_typeCircle;
		//	getTimeMapByMin(m_mapTimes,tmBegin,tmEnd,m_typeCircle);
		//}
		//if(tmCurrent>tmNoon2)
		{
			time_t tmBegin = tmLast-m_typeCircle;
			//time_t tmEnd = tmCurrent+m_typeCircle*2;
			getTimeMapByMin(m_mapTimes,tmBegin,tmCurrent,m_typeCircle);
		}
	}
	else
	{
		if(m_typeCircle == Day)
		{
		}
		else if(m_typeCircle == DayN)
		{
			//Ŀǰδʹ��
		}
		else if(m_typeCircle == Week)
		{
		}
		else if(m_typeCircle == Month)
		{
		}
		else if(m_typeCircle == Month3)
		{
		}
		else if(m_typeCircle == Year)
		{
		}
	}
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
	stColorBlockItem item = hitTestCBItem(e->pos());
	if(item.fPrice<0.1 || pStock==0)
	{
		QToolTip::hideText();
		return CBaseWidget::mouseMoveEvent(e);
	}

	QString qsTooltip;		//Tips
	if(m_typeCircle<Day)
	{
		qsTooltip = QString("��Ʊ����:%1\r\nʱ��:%2\r\n�۸�:%3")
			.arg(pStock->getCode()).arg(QDateTime::fromTime_t(item.tmTime).toString("HH:mm:ss"))
			.arg(item.fPrice);
	}
	else
	{
		qsTooltip = QString("��Ʊ����:%1\r\nʱ��:%2\r\n�۸�:%3")
			.arg(pStock->getCode()).arg(QDateTime::fromTime_t(item.tmTime).toString("yyyy/MM/dd"))
			.arg(item.fPrice);
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
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	{
		//���õ�ǰѡ�е�����ģʽ
		QList<QAction*> listAct = m_pMenuCircle->actions();
		foreach(QAction* pAct,listAct)
		{
			pAct->setCheckable(true);
			pAct->setChecked(pAct->data().toInt() == m_typeCircle);
		}
	}

	{
		//��ӵ�ǰ���е�֧�ֵ���ɫģʽ�˵�
		m_pMenuColorMode->clear();

		QStringList listColors = CColorManager::getBlockColorList();
		foreach(const QString& clr,listColors)
		{
			QAction* pAct = m_pMenuColorMode->addAction(clr,this,SLOT(onSetColorMode()));
			pAct->setData(clr);
			pAct->setCheckable(true);
			if(clr == m_qsColorMode)
				pAct->setChecked(true);
		}
	}
	{
		QList<QAction*> listAct = m_pMenuBlockMode->actions();
		foreach(QAction* pAct,listAct)
		{
			pAct->setCheckable(true);
			pAct->setChecked(pAct->data().toInt() == m_typeBlock);
		}
	}

	{
		//�������а��Ĳ˵�
		m_pMenuBlockList->clear();
		QList<QString> list = CDataEngine::getDataEngine()->getStockBlocks();
		foreach(const QString& block,list)
		{
			QAction* pAct = m_pMenuBlockList->addAction(block,this,SLOT(onSetCurrentBlock()));
			pAct->setData(block);
			if(m_qsBlock == block)
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
	p.drawText(rtHeader,Qt::AlignLeft|Qt::AlignVCenter,m_qsBlock);
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
	float fBeginX = rtBottom.right()-RCB_OFFSET_Y;
	float fEndX = rtBottom.left()+RCB_OFFSET_LEFT;
	float fCBWidth = fBeginX-fEndX;
	if(fCBWidth<0)
		return;

	QList<time_t> listTimes = m_mapTimes.keys();
	float fCurX = fBeginX;
	float fLastX = fCurX;
	int iCount = listTimes.size()-1;

	int iTimeCount = 0;				//ֻ����������ʱ�����ɫ��������ɫ�����ڲ鿴��
	while(fCurX>fEndX && iCount>=0)
	{
		if(m_typeCircle<Day)
		{
			if((fLastX-fCurX)>30)
			{
				p.setPen( iTimeCount%2 ? QColor(255,0,0) : QColor(0,255,255));
				p.drawLine(fCurX,rtBottom.top(),fCurX,rtBottom.top()+2);
				p.drawText(fCurX-14,rtBottom.top()+2,30,rtBottom.height()-2,
					Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("hh:mm"));
				fLastX = fCurX;
				++iTimeCount;
			}
		}

		--iCount;
		fCurX = fCurX-m_iCBWidth;
	}
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

	QMap<time_t,stColorBlockItem>* pMapCBs = mapStockColorBlocks[pItem];
	QMap<time_t,stColorBlockItem>::iterator iter = pMapCBs->begin();
	float fLastPrice = pItem->getCurrentReport()->fLastClose;
	while(iter!=pMapCBs->end())
	{
		if(m_typeCircle<Day)
		{
			time_t tmCur = iter.key()/(m_typeCircle)*(m_typeCircle);
//			QString qsTime = QDateTime::fromTime_t(tmCur).toString();
			if(m_mapTimes.contains(tmCur))
			{
				float fCurX = fBeginX - ((m_mapTimes[tmCur])*m_iCBWidth);
				if(fCurX>=fEndX)
				{
					//���������ٷֱ�
					float f = (iter->fPrice - fLastPrice)/fLastPrice*10.0;
					QRect rtB = QRect(fCurX,rtCB.top(),m_iCBWidth,m_iCBHeight);

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
			}
		}
		fLastPrice = iter->fPrice;
		++iter;
	}

	//int iCBCount = rtCB.width()/m_iCBHeight;
	//QList<qRcvHistoryData*> list = pItem->getLastHistory(iCBCount+1);

	//p.setPen(QColor(255,255,255));
	//p.drawText(QRect(rtCB.left(),rtCB.top(),50,m_iCBHeight),Qt::AlignCenter,pItem->getCode());

	//int iCurX = rtCB.left()+50;
	//for(int i=1;i<list.size();++i)
	//{
	//	QRect rtB = QRect(iCurX,rtCB.top(),m_iCBHeight,m_iCBHeight);
	//	switch(m_typeBlock)
	//	{
	//	case BlockRect:
	//		{
	//			rtB.adjust(2,2,-2,-2);
	//			float f = (list[i]->fClose - list[i-1]->fClose)/(list[i-1]->fClose);

	//			p.fillRect(rtB,CColorManager::getBlockColor(m_qsColorMode,f));
	//		}
	//		break;
	//	case BlockCircle:
	//		{
	//		//	rtB.adjust(2,2,-2,-2);
	//			QPainterPath path;
	//			path.addEllipse(rtB);
	//			float f = (list[i]->fClose - list[i-1]->fClose)/(list[i-1]->fClose);

	//			p.fillPath(path,CColorManager::getBlockColor(m_qsColorMode,f));
	//		}
	//		break;
	//	}

	//	iCurX = iCurX+m_iCBHeight;
	//}
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

stColorBlockItem CColorBlockWidget::hitTestCBItem( const QPoint& ptPoint ) const
{
	CStockInfoItem* pItem = hitTestStock(ptPoint);
	if(pItem && mapStockColorBlocks.contains(pItem))
	{
		QMap<time_t,stColorBlockItem>* pMap = mapStockColorBlocks[pItem];
		QMap<time_t,int>::iterator iter = m_mapTimes.end();
		if(iter!=m_mapTimes.begin())
		{
			--iter;
			int iIndex = (m_rtClient.right() - RCB_OFFSET_Y - ptPoint.x())/m_iCBWidth + 1;
			time_t tmLast = iter.key();
			time_t tmCur = tmLast - (m_typeCircle*iIndex);

			QMap<time_t,stColorBlockItem>::iterator iterCB = pMap->end();
			if(iterCB==pMap->begin())
				return stColorBlockItem();

			do
			{
				--iterCB;
				time_t tmAAA = iterCB.key()/(m_typeCircle)*(m_typeCircle);
				//QString qsTime1 = QDateTime::fromTime_t(tmCur).toString("hh:mm:ss");
				//QString qsTime2 = QDateTime::fromTime_t(tmAAA).toString("hh:mm:ss");
				if(tmAAA == tmCur)
					return iterCB.value();
				if(tmAAA<tmCur)
					break;
			}
			while (iterCB!=pMap->begin());
		}
	}
	return stColorBlockItem();
}

QMap<time_t,stColorBlockItem>* CColorBlockWidget::getColorBlockMap(CStockInfoItem* pItem)
{
	QMap<time_t,stColorBlockItem>* pMap = new QMap<time_t,stColorBlockItem>();
	if(m_typeCircle<CColorBlockWidget::Day)
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