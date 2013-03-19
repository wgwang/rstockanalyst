#include "StdAfx.h"
#include "KLineWidget.h"
#include "DataEngine.h"

#define	KLINE_BORDER	2

/*
��stLinerItem����ת���Ĵ��룬����Ч�����⣬ȡ���˸�ת��
QScriptValue linerItem2ScriptValue(QScriptEngine *engine, const stLinerItem &s)
{
	QScriptValue obj = engine->newObject();
	obj.setProperty("time", engine->newDate(QDateTime::fromTime_t(s.time)));
	obj.setProperty("open", s.fOpen);
	obj.setProperty("high", s.fHigh);
	obj.setProperty("low", s.fLow);
	obj.setProperty("close", s.fClose);
	obj.setProperty("volume", s.fVolume);
	obj.setProperty("amount", s.fAmount);
	obj.setProperty("advance", s.wAdvance);
	obj.setProperty("decline", s.wDecline);
	return obj;
}

void scriptValue2LinerItem(const QScriptValue &obj, stLinerItem &s)
{
	s.time = obj.property("time").toDateTime().toTime_t();
	s.fOpen = obj.property("open").toNumber();
	s.fHigh = obj.property("high").toNumber();
	s.fLow = obj.property("low").toNumber();
	s.fClose = obj.property("close").toNumber();
	s.fVolume = obj.property("volume").toNumber();
	s.fAmount = obj.property("amount").toNumber();
	s.wAdvance = obj.property("advance").toUInt16();
	s.wDecline = obj.property("decline").toUInt16();
}

QScriptValue createLinerItem(QScriptContext *, QScriptEngine *engine)
{
	stLinerItem s;
	return engine->toScriptValue(s);
}
*/

/* ��ĳ���ӵ����ݽ���ת�� (qRcvMinuteData*) -> (stLinerItem) */
bool getLinerItemByMin(stLinerItem& item, qRcvFenBiData* pMin)
{
	if(!pMin)
		return false;

	item.time = pMin->tmTime;
	item.fOpen = pMin->fPrice;
	item.fClose = pMin->fPrice;
	item.fLow = pMin->fPrice;
	item.fHigh = pMin->fPrice;
	item.fAmount = pMin->fAmount;
	item.fVolume = pMin->fVolume;
	return true;
}

/*ͨ���������ݻ�ȡָ�������ڵ�����*/
bool getLinerItemByMins(stLinerItem& item, const QList<qRcvFenBiData*>& list, qRcvFenBiData* pLastFenbi)
{
	if(list.size()<1)
		return false;

	qRcvFenBiData* pBegin = list.first();
	qRcvFenBiData* pLast = list.last();
	item.time = pLast->tmTime;
	item.fOpen = pBegin->fPrice;
	item.fClose = pLast->fPrice;

	item.fLow = pBegin->fPrice;
	item.fHigh = pBegin->fPrice;

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

int getLinerMinItems(QVector<stLinerItem>& listItems,const QList<qRcvFenBiData*>& minutes, int iSize = 1)
{
	QList<qRcvFenBiData*> listMins;
	time_t tmT = 0;
	qRcvFenBiData* pLastFenbi = 0;
	foreach(qRcvFenBiData* p, minutes)
	{
		if((p->tmTime-tmT)>=(iSize))
		{
			stLinerItem item;
			if(getLinerItemByMins(item,listMins,pLastFenbi))
				listItems.push_back(item);
			if(listMins.size()>0)
				pLastFenbi = listMins.last();
			listMins.clear();
			tmT = p->tmTime;
		}

		listMins.push_back(p);
	}

	{
		//���һ������
		stLinerItem item;
		if(getLinerItemByMins(item,listMins,pLastFenbi))
			listItems.push_back(item);
		listMins.clear();
	}
	return listItems.size();
}

/* ��ĳ������ݽ���ת�� (qRcvHistoryData*) -> (stLinerItem) */
bool getLinerItemByDay(stLinerItem& item,const qRcvHistoryData* pHistory)
{
	if(!pHistory)
		return false;
	item.time = pHistory->time;
	item.fOpen = pHistory->fOpen;
	item.fClose = pHistory->fClose;
	item.fHigh = pHistory->fHigh;
	item.fLow = pHistory->fLow;
	item.fAmount = pHistory->fAmount;
	item.fVolume = pHistory->fVolume;
	item.wAdvance = pHistory->wAdvance;
	item.wDecline = pHistory->wDecline;
	return true;
}
/*ͨ���������ݻ�ȡһ�������ڵ�����*/
bool getLinerItemByDays(stLinerItem& item,const QList<qRcvHistoryData*>& list)
{
	if(list.size()<1)
		return false;

	qRcvHistoryData* pBegin = list.first();
	qRcvHistoryData* pLast = list.last();
	item.time = pBegin->time;
	item.fOpen = pBegin->fOpen;
	item.fClose = pLast->fClose;

	item.fLow = pBegin->fLow;
	item.fHigh = pBegin->fHigh;
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
//	item.wAdvance = pHistory->wAdvance;
//	item.wDecline = pHistory->wDecline;
	return true;
}

int getLinerDayItems(QVector<stLinerItem>& listItems,const QList<qRcvHistoryData*>& historys)
{
	foreach(qRcvHistoryData* p,historys)
	{
		stLinerItem item;
		if(getLinerItemByDay(item,p))
			listItems.push_back(item);
	}

	return listItems.size();
}

int getLinerWeekItems(QVector<stLinerItem>& listItems,const QList<qRcvHistoryData*>& historys)
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
				stLinerItem item;
				getLinerItemByDays(item,weekHis);
				listItems.push_back(item);
				weekHis.clear();
			}
			iCurYear = tmDate.year();
			iCurWeek = tmDate.weekNumber();
		}
		else if(tmDate.weekNumber()!=iCurWeek)
		{
			iCurWeek = tmDate.weekNumber();

			stLinerItem item;
			getLinerItemByDays(item,weekHis);
			listItems.push_back(item);
			weekHis.clear();
		}
		weekHis.push_back(pHistory);
	}

	if(weekHis.size()>0)
	{
		//���ʣ���Ҳ����
		stLinerItem item;
		getLinerItemByDays(item,weekHis);
		listItems.push_back(item);
		weekHis.clear();
	}

	return listItems.size();
}

int getLinerMonthItems(QVector<stLinerItem>& listItems,const QList<qRcvHistoryData*>& historys)
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
				stLinerItem item;
				getLinerItemByDays(item,monthHis);
				listItems.push_back(item);
				monthHis.clear();
			}
		}
		else if(tmDate.month()!=iCurMonth)
		{
			iCurMonth = tmDate.month();

			stLinerItem item;
			getLinerItemByDays(item,monthHis);
			listItems.push_back(item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//���ʣ���Ҳ����
		stLinerItem item;
		getLinerItemByDays(item,monthHis);
		listItems.push_back(item);
		monthHis.clear();
	}
	return listItems.size();
}

int getLinerMonth3Items(QVector<stLinerItem>& listItems,const QList<qRcvHistoryData*>& historys)
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
				stLinerItem item;
				getLinerItemByDays(item,monthHis);
				listItems.push_back(item);
				monthHis.clear();
			}
		}
		else if(((tmDate.month()-1)/3)!=((iCurMonth-1)/3))
		{
			iCurMonth = tmDate.month();

			stLinerItem item;
			getLinerItemByDays(item,monthHis);
			listItems.push_back(item);
			monthHis.clear();
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//���ʣ���Ҳ����
		stLinerItem item;
		getLinerItemByDays(item,monthHis);
		listItems.push_back(item);
		monthHis.clear();
	}
	return listItems.size();
}

int getLinerYearItems(QVector<stLinerItem>& listItems,const QList<qRcvHistoryData*>& historys)
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
				stLinerItem item;
				getLinerItemByDays(item,monthHis);
				listItems.push_back(item);
				monthHis.clear();
			}
		}
		monthHis.push_back(pHistory);
	}

	if(monthHis.size()>0)
	{
		//���ʣ���Ҳ����
		stLinerItem item;
		getLinerItemByDays(item,monthHis);
		listItems.push_back(item);
		monthHis.clear();
	}
	return listItems.size();
}

QString CKLineWidget::g_qsScript;
bool CKLineWidget::initJSScript()
{
	QFile file(qApp->applicationDirPath()+"/RStockLiners.js");
	if(!file.open(QFile::ReadOnly))
	{
		qDebug()<<"load script error.";
		return false;
	}
	g_qsScript = file.readAll();
	qDebug()<<"loaded script from:"<<file.fileName();
	file.close();
	return true;
}

CKLineWidget::CKLineWidget( CBaseWidget* parent /*= 0*/ )
	: CCoordXBaseWidget(parent,CBaseWidget::KLine)
	, m_pMenuCustom(0)
	, m_pMenuCircle(0)
	, m_pActShowMain(0)
	, m_pStockItem(0)
	, m_iShowCount(100)
	, m_pLinerMain(0)
	, m_pCurrentLiner(0)
	, m_bShowMax(false)
	, m_iTitleHeight(16)
	, m_iCoorYWidth(50)
	, m_iCoorXHeight(16)
	, m_iMainLinerHeight(200)
	, m_pScriptEngine(0)
{

	m_typeCircle = Day;
	{
		//��ʼ���ű�������
		m_pScriptEngine = new QScriptEngine;
		/*
		qScriptRegisterMetaType<stLinerItem>(m_pScriptEngine, linerItem2ScriptValue, scriptValue2LinerItem);
		QScriptValue ctor = m_pScriptEngine->newFunction(createLinerItem);
		m_pScriptEngine->globalObject().setProperty("stLinerItem", ctor);
		qScriptRegisterSequenceMetaType<QVector<stLinerItem>>(m_pScriptEngine);
		*/
		qScriptRegisterSequenceMetaType<QVector<float>>(m_pScriptEngine);

		//����Ԥ�ýű�
		m_pScriptEngine->evaluate(QScriptProgram(g_qsScript));
	}

	m_pLinerMain = new CMultiLiner(CMultiLiner::MainKLine,m_pScriptEngine,"");
	m_vSizes.push_back(60);

	//���ò˵�
	m_pMenuCustom = new QMenu("K��ͼ����");
	m_pMenuCustom->addAction(tr("���ù�Ʊ����"),this,SLOT(onSetStockCode()));
	{
		//���õ�ǰK��ͼ����ʾ����
		m_pMenuCircle = m_pMenuCustom->addMenu(tr("��������"));
		m_pMenuCircle->addAction(tr("��ʱͼ"),this,SLOT(onSetCircle()))->setData(FenShi);
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
		m_pMenuCustom->addSeparator();
		QMenu* pMenuDeputy = m_pMenuCustom->addMenu(tr("��Ӹ�ͼ"));
		pMenuDeputy->addAction(tr("��ͨ��ͼ"),this,SLOT(onAddDeputy()));
		pMenuDeputy->addAction(tr("����ͼ"),this,SLOT(onAddVolume()));

	}
	m_pMenuCustom->addAction(tr("ɾ����ͼ"),this,SLOT(onRemoveDeputy()));
	m_pMenuCustom->addAction(tr("���ñ��ʽ"),this,SLOT(onSetExpression()));
	{
		m_pActShowMain = m_pMenuCustom->addAction(tr("��ʾ��ͼ"));
		m_pActShowMain->setCheckable(true);
		m_pActShowMain->setChecked(true);
		connect(m_pActShowMain,SIGNAL(toggled(bool)),this,SLOT(onShowMainChanged(bool)));
	}
	m_pMenuCustom->addAction(tr("��������ͼ����ʾ����"),this,SLOT(onSetSizes()));

//	setMinimumSize(200,200);
}

CKLineWidget::~CKLineWidget(void)
{
	clearTmpData();
	delete m_pLinerMain;
	foreach(CMultiLiner* p,m_listLiners)
		delete p;
	m_listLiners.clear();
	delete m_pScriptEngine;

	delete m_pMenuCustom;
}

bool CKLineWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	m_vSizes.clear();

	//�Ƿ���ʾ��ͼ
	if(eleWidget.hasAttribute("mainliner"))
	{
		m_pActShowMain->setChecked(eleWidget.attribute("mainliner").toInt());
	}

	//��ǰ��ʾ������
	if(eleWidget.hasAttribute("circle"))
	{
		m_typeCircle = static_cast<RStockCircle>(eleWidget.attribute("circle").toInt());
	}

	QDomElement eleLiners = eleWidget.firstChildElement("liners");
	if(eleLiners.isElement())
	{
		//�������е�Liner
		{
			//������ͼ����Ϣ
			QDomElement eleMainLiner = eleLiners.firstChildElement("MainLiner");
			if(eleMainLiner.isElement())
			{
				m_vSizes.push_back(eleMainLiner.attribute("size").toInt());

				QDomElement eleExp = eleMainLiner.firstChildElement("exp");
				if(eleExp.isElement())
				{
					m_pLinerMain->setExpression(eleExp.text());
				}
			}
		}
		{
			//�������еĸ�ͼ��Ϣ
			QDomElement eleLiner = eleLiners.firstChildElement("Liner");
			while(eleLiner.isElement())
			{
				CMultiLiner::MultiLinerType t = CMultiLiner::Deputy;
				{
					//��ȡ�ø�ͼ������
					bool bOk = false;
					int iType = eleLiner.attribute("type").toInt(&bOk);
					if(bOk)
						t = static_cast<CMultiLiner::MultiLinerType>(iType);
				}
				CMultiLiner* pLiner = new CMultiLiner(t,m_pScriptEngine,"");

				m_vSizes.push_back(eleLiner.attribute("size").toInt());
				QDomElement eleExp = eleLiner.firstChildElement("exp");
				if(eleExp.isElement())
				{
					pLiner->setExpression(eleExp.text());
				}
				m_listLiners.push_back(pLiner);

				eleLiner = eleLiner.nextSiblingElement("Liner");
			}
		}
	}

	QDomElement eleCode = eleWidget.firstChildElement("code");
	if(eleCode.isElement())
		setStockCode(eleCode.text());

	return true;
}

bool CKLineWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;
	if(m_pStockItem)
	{
		//��ǰ�Ĺ�Ʊֵ
		QDomElement eleCode = doc.createElement("code");
		eleCode.appendChild(doc.createTextNode(m_pStockItem->getCode()));
		eleWidget.appendChild(eleCode);

		//�Ƿ���ʾ��ͼ
		eleWidget.setAttribute("mainliner",m_pActShowMain->isChecked());

		//��ʾ������
		eleWidget.setAttribute("circle",m_typeCircle);

		QDomElement eleLiners = doc.createElement("liners");
		eleWidget.appendChild(eleLiners);
		{
			//�������е�Liner
			{
				//������ͼ����Ϣ
				QDomElement eleMainLiner = doc.createElement("MainLiner");
				eleLiners.appendChild(eleMainLiner);
				int iSize = 1;
				if(m_vSizes.size()>0)
					iSize = m_vSizes[0];
				eleMainLiner.setAttribute("size",iSize);

				QDomElement eleExp = doc.createElement("exp");
				eleExp.appendChild(doc.createTextNode(m_pLinerMain->getExpression()));
				eleMainLiner.appendChild(eleExp);
			}
			{
				//�������еĸ�ͼ��Ϣ
				for(int i=0;i<m_listLiners.size();++i)
				{
					CMultiLiner* pLiner = m_listLiners[i];
					QDomElement eleLiner = doc.createElement("Liner");
					eleLiners.appendChild(eleLiner);
					int iSize = 1;
					if(m_vSizes.size()>(i+1))
						iSize = m_vSizes[i+1];
					eleLiner.setAttribute("size",iSize);
					eleLiner.setAttribute("type",pLiner->getType());

					QDomElement eleExp = doc.createElement("exp");
					eleExp.appendChild(doc.createTextNode(pLiner->getExpression()));
					eleLiner.appendChild(eleExp);
				}
			}
		}
	}

	return true;
}

void CKLineWidget::setStockCode( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);
	if(pItem)
	{
		if(m_pStockItem)
		{
			//�Ƴ����к� updateKLine������ �ź�/��
			disconnect(m_pStockItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
			disconnect(m_pStockItem,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
//			disconnect(m_pStockItem,SIGNAL(stockItemReportChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
		}

		m_pStockItem = pItem;

		//�������»���
		connect(pItem,SIGNAL(stockItemHistoryChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
		connect(pItem,SIGNAL(stockItemFenBiChanged(const QString&)),this,SLOT(updateKLine(const QString&)));
//		connect(pItem,SIGNAL(stockItemReportChanged(const QString&)),this,SLOT(updateKLine(const QString&)));

		//����K��ͼ
		updateKLine(code);
	}

	return CBaseWidget::setStockCode(code);
}

void CKLineWidget::updateKLine( const QString& code )
{
	if(m_pStockItem&&m_pStockItem->getCode()!=code)
		return;

	resetTmpData();
}

void CKLineWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRect rtClient = this->rect();
	p.fillRect(rtClient,QColor(0,0,0));
	if(!m_pStockItem)
		return;

	/*��ͷ��*/
	QRect rtTitle = QRect(rtClient.left(),rtClient.top(),rtClient.width(),m_iTitleHeight);
	drawTitle(p,rtTitle);

	/*��X������*/
	QRect rtCoordX = QRect(rtClient.left(),rtClient.bottom()-m_iCoorXHeight+1,rtClient.width()-m_iCoorYWidth,m_iCoorXHeight);
	drawCoordX(p,rtCoordX);
	/*�����½ǵ�������ť*/
	QRect rtShowBtns = QRect(rtClient.right()-m_iCoorYWidth,rtClient.bottom()-m_iCoorXHeight,m_iCoorYWidth,m_iCoorXHeight);
	drawShowBtns(p,rtShowBtns);

	rtClient.adjust(3,m_iTitleHeight,-m_iCoorYWidth-2,-m_iCoorXHeight);			//�ı�Ϊ�ɻ�ͼ����Ĵ�С


	QPoint ptCurMouse = this->mapFromGlobal(QCursor::pos());
	CMultiLiner* pCurLiner = 0;		//��ǰ�������λ�õ�MultiLiner

	int iCurY = rtClient.top();		//��ǰ���Ƶ���λ��
	int iSizeIndex = 0;
	/*������ͼ*/
	if(m_pActShowMain->isChecked())
	{
		if(m_vSizes.size()<1)
			m_vSizes.push_back(60);
		int iTotal = 0;					//�����ܺ�
		for (int i=iSizeIndex;i<m_vSizes.size();++i)
			iTotal += m_vSizes[i];
		int iTotalHeight = rtClient.bottom()-iCurY;

		int iMainHeight = (float)((float)m_vSizes[iSizeIndex]/float(iTotal))*iTotalHeight + 0.5;
		m_pLinerMain->Draw(p,QRectF(rtClient.left(),rtClient.top(),rtClient.width(),iMainHeight),m_iShowCount);
		if(m_pLinerMain->getRect().contains(ptCurMouse))
			pCurLiner = m_pLinerMain;
		iCurY += iMainHeight;
	}
	else
	{
		m_pLinerMain->Draw(p,QRectF(),0);
	}
	++iSizeIndex;

	//���Ʒָ���
	if(m_listLiners.size()>0)
	{
		p.setPen(QColor(255,255,255));
		if(m_listLiners.contains(m_pCurrentLiner)&&m_bShowMax)
		{
			p.drawLine(this->rect().left(),iCurY,this->rect().right(),iCurY);
			foreach(CMultiLiner* pLiner,m_listLiners)
				pLiner->Draw(p,QRectF(),0);
			m_pCurrentLiner->Draw(p,QRectF(rtClient.left(),iCurY,rtClient.width(),rtClient.bottom()-iCurY),m_iShowCount);
			if(m_pCurrentLiner->getRect().contains(ptCurMouse))
				pCurLiner = m_pCurrentLiner;
		}
		else
		{
			for(int i=0;i<m_listLiners.size();++i)
			{
				int iTotal = 0;					//�����ܺ�
				for (int j=iSizeIndex;j<m_vSizes.size();++j)
					iTotal += m_vSizes[j];
				int iTotalHeight = rtClient.bottom()-iCurY;
				int iHeight = (float)((float)m_vSizes[iSizeIndex]/float(iTotal))*iTotalHeight + 0.5;

				p.drawLine(this->rect().left(),iCurY,this->rect().right(),iCurY);
				m_listLiners[i]->Draw(p,QRectF(rtClient.left(),iCurY,rtClient.width(),iHeight),m_iShowCount);
				if(m_listLiners[i]->getRect().contains(ptCurMouse))
					pCurLiner = m_listLiners[i];
				iCurY += iHeight;
				++iSizeIndex;
			}
		}
	}

	if(pCurLiner)
	{
		//���Ƹ�����
		QPen pen(QColor(255,0,0));
		pen.setStyle(Qt::DotLine);
		p.setPen(pen);
		p.drawLine(ptCurMouse,QPoint(rtClient.right(),ptCurMouse.y()));
		p.drawLine(ptCurMouse,QPoint(ptCurMouse.x(),rtClient.bottom()));

		/*����y���ֵ*/
		float fV = pCurLiner->getValueByY(ptCurMouse.y());
		{
			if(fV>10000.0)
				p.drawText(QRect(rtClient.right()+10,ptCurMouse.y()-5,50,20),Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(fV,0,'g',2));
			else
				p.drawText(QRect(rtClient.right()+10,ptCurMouse.y()-5,50,20),Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(fV,0,'f',2));				
		}

		/*����x���ֵ*/
		QRectF rtCurLiner = pCurLiner->getRect();
		int iAbsIndex = (rtCurLiner.right() - ptCurMouse.x())/fItemWidth;
		int iIndex = listItems.size()-iAbsIndex-1;
		if(iIndex>=0&&iIndex<listItems.size())
		{
			p.setPen(QColor(0,255,255));
			if(m_typeCircle<Day)
			{
				QTime tmTime = QDateTime::fromTime_t(listItems[iIndex].time).time();
				p.drawText(QRect(ptCurMouse.x(),rtClient.bottom(),100,20),Qt::AlignCenter,tmTime.toString("HH:mm:ss"));
			}
			else
			{
				QDate tmDate = QDateTime::fromTime_t(listItems[iIndex].time).date();
				p.drawText(QRect(ptCurMouse.x()-50,rtClient.bottom(),100,20),Qt::AlignCenter,tmDate.toString("yyyy/MM/dd"));
			}
		}
	}
}

void CKLineWidget::mouseMoveEvent( QMouseEvent* e )
{
	if(!((qApp->mouseButtons())&Qt::LeftButton))
	{
		update();
		QToolTip::hideText();
		return CBaseWidget::mouseMoveEvent(e);
	}
	if(!m_pStockItem)
	{
		return QToolTip::hideText();
	}
	QRectF rtClient = this->rect();
	rtClient.adjust(3,m_iTitleHeight,-m_iCoorYWidth-2,-m_iCoorXHeight);			//�ı�Ϊ�ɻ�ͼ����Ĵ�С

	float fEnd = rtClient.right();
	int iLastN = (fEnd - e->posF().x())/fItemWidth;
	if(iLastN<0||iLastN>=m_iShowCount)
		return QToolTip::hideText();

	int iIndex = listItems.size()-iLastN-1;
	if(iIndex<0)
		return QToolTip::hideText();

	QString qsTooltip;		//Tips
	stLinerItem item = listItems[iIndex];
	if(m_typeCircle<Day)
	{
		qsTooltip = QString("��Ʊ����:%1\r\nʱ��:%2\r\n��߼�:%3\r\n��ͼ�:%4\r\n�ɽ���:%5\r\n�ɽ���:%6")
			.arg(m_pStockItem->getCode()).arg(QDateTime::fromTime_t(item.time).toString("HH:mm:ss"))
			.arg(item.fHigh).arg(item.fLow).arg(item.fVolume).arg(item.fAmount);
	}
	else
	{
		qsTooltip = QString("��Ʊ����:%1\r\nʱ��:%2\r\n��߼�:%3\r\n��ͼ�:%4\r\n���̼�:%5\r\n���̼�:%6\r\n�ɽ���:%7\r\n�ɽ���:%8")
			.arg(m_pStockItem->getCode()).arg(QDateTime::fromTime_t(item.time).toString("yyyy/MM/dd"))
			.arg(item.fHigh).arg(item.fLow).arg(item.fOpen).arg(item.fClose)
			.arg(item.fVolume).arg(item.fAmount);
	}

	QToolTip::showText(e->globalPos(),qsTooltip,this);
	return CBaseWidget::mouseMoveEvent(e);
}

void CKLineWidget::leaveEvent( QEvent* e )
{
	update();
	return CBaseWidget::leaveEvent(e);
}

void CKLineWidget::mousePressEvent( QMouseEvent* e )
{
	QPoint ptCur = e->pos();
	if(m_pLinerMain->getRect().contains(ptCur))
	{
		m_pCurrentLiner = m_pLinerMain;
	}
	else
	{
		foreach(CMultiLiner* p, m_listLiners)
		{
			if(p->getRect().contains(ptCur))
			{
				m_pCurrentLiner = p;
				break;
			}
		}
	}
	if(e->button()==Qt::LeftButton)
	{
		if(m_rtAddShow.contains(ptCur))
		{
			onClickedAddShow();
		}
		else if(m_rtSubShow.contains(ptCur))
		{
			onClickedSubShow();
		}
	}
}

void CKLineWidget::mouseDoubleClickEvent( QMouseEvent* e )
{
	foreach(CMultiLiner* pLiner,m_listLiners)
	{
		if(pLiner->getRect().contains(e->pos()))
		{
			m_pCurrentLiner = pLiner;
			m_bShowMax = !m_bShowMax;
			update();
		}
	}
}

void CKLineWidget::keyPressEvent(QKeyEvent* e)
{

	return CBaseWidget::keyPressEvent(e);
}

QMenu* CKLineWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	{
		QList<QAction*> listAct = m_pMenuCircle->actions();
		foreach(QAction* pAct,listAct)
		{
			pAct->setCheckable(true);
			pAct->setChecked(pAct->data().toInt() == m_typeCircle);
		}
	}

	return m_pMenuCustom;
}

void CKLineWidget::onSetStockCode()
{
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

	QString code = edit.text();
	setStockCode(code);
}

void CKLineWidget::onSetExpression()
{
	if(!m_pCurrentLiner)
		return;

	QDialog dlg(this);
	QVBoxLayout layout(this);
	QTextEdit edit(this);
	QPushButton btnOk(this);
	dlg.setLayout(&layout);
	layout.addWidget(&edit);
	layout.addWidget(&btnOk);
	btnOk.setText(tr("ȷ��"));
	edit.setText(m_pCurrentLiner->getExpression());
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	m_pCurrentLiner->setExpression(edit.toPlainText());
	if(m_pCurrentLiner == m_pLinerMain)
	{
		if(m_typeCircle == FenShi)
			m_pCurrentLiner->setKLineType(CKLineLiner::FenShi);
	}
}

void CKLineWidget::onClickedAddShow()
{
	if((m_iShowCount+10)<=listItems.size())
	{
		m_iShowCount += 10;
		update();
	}
}

void CKLineWidget::onClickedSubShow()
{
	if((m_iShowCount-10)>0)
	{
		m_iShowCount -= 10;
		update();
	}
}

void CKLineWidget::onSetCircle()
{
	//���õ�ǰ����ʾ����
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	m_typeCircle = static_cast<RStockCircle>(pAct->data().toInt());
	resetTmpData();
}

void CKLineWidget::onAddDeputy()
{
	CMultiLiner* pMultiLiner = new CMultiLiner(CMultiLiner::Deputy,m_pScriptEngine,"CLOSE;");
	m_listLiners.push_back(pMultiLiner);
	m_vSizes.push_back(20);
	update();
}

void CKLineWidget::onAddVolume()
{
	CMultiLiner* pMultiLiner = new CMultiLiner(CMultiLiner::VolumeLine,m_pScriptEngine,"");
	m_listLiners.push_back(pMultiLiner);
	m_vSizes.push_back(20);
	update();
}


void CKLineWidget::onRemoveDeputy()
{
	//ɾ����ǰѡ�еĸ�ͼ
	if(m_pCurrentLiner)
	{
		if(m_pCurrentLiner!=m_pLinerMain)
		{
			//������Ǹ�ͼ�Ļ��ٽ���ɾ������
			int iIndex = m_listLiners.indexOf(m_pCurrentLiner);
			if(iIndex>=0)
			{
				m_listLiners.removeOne(m_pCurrentLiner);
				delete m_pCurrentLiner;
				if(m_vSizes.size()>(iIndex+1))
					m_vSizes.remove(iIndex+1);

				update();
			}
		}
		m_pCurrentLiner = m_pLinerMain;
	}
}

void CKLineWidget::onShowMainChanged( bool bShow )
{
	if(bShow)
		m_pLinerMain->updateData();
	update();
}

void CKLineWidget::onSetSizes()
{
	//�������û��༭���Դ��ڱ����ĶԻ���
	QDialog dlg(this);
	QGridLayout layout(&dlg);
	QPushButton btnOk(this);
	dlg.setLayout(&layout);

	int iCount = m_listLiners.size()+1;
	QVector<QLabel*> vLabels;
	QVector<QSpinBox*> vSpins;
	for (int i=0;i<iCount;++i)
	{
		//���ó�ʼ�ı���
		QLabel* pLabel = new QLabel(&dlg);
		pLabel->setText(QString("%1").arg(i+1));
		vLabels.push_back(pLabel);
		QSpinBox* pSpin = new QSpinBox(&dlg);
		pSpin->setMinimum(1);
		pSpin->setMaximum(100);
		pSpin->setValue(m_vSizes[i]);
		vSpins.push_back(pSpin);

		layout.addWidget(pLabel,i,0);
		layout.addWidget(pSpin,i,1);
	}
	layout.addWidget(&btnOk,iCount,0,1,2);
	btnOk.setText(tr("ȷ��"));
	connect(&btnOk,SIGNAL(clicked()),&dlg,SLOT(accept()));

	if(QDialog::Accepted != dlg.exec())
		return;

	m_vSizes.clear();			//���֮ǰ�ı�������

	for (int i=0;i<iCount;++i)
	{
		m_vSizes.push_back(vSpins[i]->value());
		delete vLabels[i];
		delete vSpins[i];
	}
}

void CKLineWidget::drawTitle( QPainter& p,const QRect& rtTitle )
{
	if(!rtTitle.isValid())
		return;

	QRect rtClient = rtTitle.adjusted(2,0,-2,0);
	QString qsName = m_pStockItem->getName();
	if(qsName.isEmpty())
		qsName = m_pStockItem->getCode();

	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);

	p.setPen(QColor(255,255,255));
	p.drawText(rtClient,Qt::AlignLeft|Qt::AlignVCenter,qsName);
}

void CKLineWidget::drawCoordX( QPainter& p,const QRect& rtCoordX )
{
	if(!rtCoordX.isValid())
		return;

	//���Ƶ���
	float fTopLine = rtCoordX.top()+1;
	p.setPen(QColor(255,0,0));
	p.drawLine(rtCoordX.right(),fTopLine,rtCoordX.left(),fTopLine);


	//�����3Pix���ҿ���2Pix
	fItemWidth = float(rtCoordX.width()-2-3)/float(m_iShowCount);
	//�����������
	float fCurX = rtCoordX.right();
	float fLastX = fCurX;
	//���ƿ̶�
	if(m_typeCircle<Day)
	{
		//��ʱ���ݵĺ�����ƣ�����ƾ���ΪСʱ��
		int iCurHour = 0;
		int iCurIndex = listItems.size()-1;
		int iCount = 0;
		while(iCount<m_iShowCount)
		{
			QTime tmTime = QDateTime::fromTime_t(listItems[iCurIndex].time).time();
			if(tmTime.hour()!=iCurHour)
			{
				if((fLastX-fCurX)>16)
				{
					p.drawLine(fCurX,fTopLine,fCurX,rtCoordX.bottom()-1);
					p.drawText(fCurX+2,rtCoordX.bottom()-3,QString("%1ʱ").arg(iCurHour));
					fLastX = fCurX;
				}

				iCurHour = tmTime.hour();
			}

			fCurX -= fItemWidth;
			++iCount;
			--iCurIndex;
		}
	}
	else
	{
		//�������ݵĺ�����ƣ�ֻ����ƾ���Ϊ�·ݼ��ɣ�
		int iCurMonth = 0;
		int iCurIndex = listItems.size()-1;
		int iCount = 0;
		while(iCount<m_iShowCount)
		{
			QDate tmDate = QDateTime::fromTime_t(listItems[iCurIndex].time).date();
			if(tmDate.month()!=iCurMonth)
			{				
				if((fLastX-fCurX)>16)
				{
					p.drawLine(fCurX,fTopLine,fCurX,rtCoordX.bottom()-1);
					p.drawText(fCurX+2,rtCoordX.bottom()-3,QString("%1").arg(iCurMonth));
					fLastX = fCurX;
				}

				iCurMonth = tmDate.month();
			}

			fCurX -= fItemWidth;
			++iCount;
			--iCurIndex;
		}
	}
}

void CKLineWidget::drawShowBtns( QPainter& p,const QRect& rtBtns )
{
	p.setPen(QColor(255,0,0));
	int iBtnWidth = 16;
	int iBtnHeight = rtBtns.height()-1;
	m_rtAddShow = QRect(rtBtns.right()-2*iBtnWidth,rtBtns.top()+1,iBtnWidth,iBtnHeight);
	m_rtSubShow = QRect(rtBtns.right()-iBtnWidth,rtBtns.top()+1,iBtnWidth,iBtnHeight);
	
	p.drawRect(m_rtAddShow);
	p.drawRect(m_rtSubShow);

	p.drawText(m_rtAddShow,Qt::AlignCenter,QString("+"));
	p.drawText(m_rtSubShow,Qt::AlignCenter,QString("-"));
}

void CKLineWidget::clearTmpData()
{
	//foreach(stLinerItem* p,listItems)
	//	delete p;
	listItems.clear();
	disconnect(this,SLOT(updateKLine(const QString&)));
}

void CKLineWidget::resetTmpData()
{
	if(m_iShowCount<50)
		m_iShowCount = 100;
	clearTmpData();
	if(m_typeCircle<Day)
	{
		//��ȡ�������ݣ����м���
		QList<qRcvFenBiData*> FenBis = m_pStockItem->getFenBiList();
		if(m_typeCircle == FenShi)
		{
			m_pLinerMain->setKLineType(CKLineLiner::FenShi);
			getLinerMinItems(listItems,FenBis,m_typeCircle);
		}
		else
		{
			m_pLinerMain->setKLineType(CKLineLiner::Normal);
			getLinerMinItems(listItems,FenBis,m_typeCircle);
		}
		if(listItems.size()>0)
		{
			listItems.first().fOpen = m_pStockItem->getCurrentReport()->fLastClose;	//���õ�һ���ڵ����ʼ��Ϊ��������̼�
			if(listItems.first().fLow>m_pStockItem->getCurrentReport()->fOpen)
				listItems.first().fLow = m_pStockItem->getCurrentReport()->fOpen;
			if(listItems.first().fHigh<m_pStockItem->getCurrentReport()->fOpen)
				listItems.first().fHigh = m_pStockItem->getCurrentReport()->fOpen;
		}
	}
	else
	{
		m_pLinerMain->setKLineType(CKLineLiner::Normal);
		//��ȡ��������
		QList<qRcvHistoryData*> historys = m_pStockItem->getHistoryList();
		qRcvReportData* pLastReport = m_pStockItem->getCurrentReport();
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
			getLinerDayItems(listItems,historys);
		}
		else if(m_typeCircle == DayN)
		{
			//Ŀǰδʹ��
		//	getLinerItem(listItems,historys,3);
		}
		else if(m_typeCircle == Week)
		{
			getLinerWeekItems(listItems,historys);
		}
		else if(m_typeCircle == Month)
		{
			getLinerMonthItems(listItems,historys);
		}
		else if(m_typeCircle == Month3)
		{
			getLinerMonth3Items(listItems,historys);
		}
		else if(m_typeCircle == Year)
		{
			getLinerYearItems(listItems,historys);
		}

		{
			//�����ȡ����������
			foreach(qRcvHistoryData* p,historys)
				delete p;
			historys.clear();
		}
	}

	if(listItems.size()<m_iShowCount)
		m_iShowCount = listItems.size();


	{
		/*�����º���������õ��ű�������*/
		QVector<float> vOpen;
		QVector<float> vHigh;
		QVector<float> vLow;
		QVector<float> vClose;
		QVector<float> vVolume;
		QVector<float> vAmount;
		QVector<float> vAdvance;
		QVector<float> vDecline;
		foreach(const stLinerItem& i,listItems)
		{
			vOpen.push_back(i.fOpen);
			vHigh.push_back(i.fHigh);
			vLow.push_back(i.fLow);
			vClose.push_back(i.fClose);
			vVolume.push_back(i.fVolume);
			vAmount.push_back(i.fAmount);
			vAdvance.push_back(i.wAdvance);
			vDecline.push_back(i.wDecline);
		}

		QTime tmNow = QTime::currentTime();
		//	m_pScriptEngine->globalObject().setProperty("ITEMS",m_pScriptEngine->toScriptValue(listItems));
		//	qDebug()<<m_pScriptEngine->evaluate(QScriptProgram("CalcBaseData();")).toString();
		m_pScriptEngine->globalObject().setProperty("OPEN",m_pScriptEngine->toScriptValue(vOpen));
		m_pScriptEngine->globalObject().setProperty("HIGH",m_pScriptEngine->toScriptValue(vHigh));
		m_pScriptEngine->globalObject().setProperty("LOW",m_pScriptEngine->toScriptValue(vLow));
		m_pScriptEngine->globalObject().setProperty("CLOSE",m_pScriptEngine->toScriptValue(vClose));
		m_pScriptEngine->globalObject().setProperty("VOLUME",m_pScriptEngine->toScriptValue(vVolume));
		m_pScriptEngine->globalObject().setProperty("AMOUNT",m_pScriptEngine->toScriptValue(vAmount));
		m_pScriptEngine->globalObject().setProperty("ADVANCE",m_pScriptEngine->toScriptValue(vAdvance));
		m_pScriptEngine->globalObject().setProperty("DECLINE",m_pScriptEngine->toScriptValue(vDecline));
		qDebug()<<"set "<<m_pStockItem->getCode()<<" data to script, use ms:"<<tmNow.msecsTo(QTime::currentTime());
	}
	//���»����е�����
	if(m_pActShowMain->isChecked())
		m_pLinerMain->updateData();
	foreach(CMultiLiner* p,m_listLiners)
		p->updateData();

	//���½���
	update();
}
