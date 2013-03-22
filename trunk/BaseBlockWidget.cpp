/************************************************************************/
/* �ļ����ƣ�BaseBlockWidget.h
/* ����ʱ�䣺2013-01-14 13:38
/*
/* ��    ��������ɫ��ͼ�ĳ��󣬼̳���CCoordXBaseWidget
/*           �����Ե�ʹ�ò�ͬ��ɫ������ʾ��Ʊ���������ƣ��Ӷ�����ͬ�������У�
/*           չ�ָ������Ϣ��
/************************************************************************/

#include "StdAfx.h"
#include "BaseBlockWidget.h"
#include "ColorManager.h"
#include "DataEngine.h"

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

int getDayMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	time_t tmNow = (_tmEnd+3600*24);			//������һ������
	while(tmNow>=_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		tmNow -= 3600*24;
	}

	return 1;
}

int getWeekMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addDays(8-dtNow.dayOfWeek());	//�����һ������
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		tmNow -= (3600*24*7);
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getMonthMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addMonths(1);					//�����һ������
	dtNow = QDate(dtNow.year(),dtNow.month(),1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addMonths(-1);
		dtNow = QDate(dtNow.year(),dtNow.month(),1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getMonth3MapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();
	dtNow = dtNow.addMonths(3);					//�����һ������
	dtNow = QDate(dtNow.year(),(dtNow.month()-1)/3*3,1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addMonths(-3);
		dtNow = QDate(dtNow.year(),(dtNow.month()-1)/3*3,1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getYearMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addYears(1);					//�����һ������
	dtNow = QDate(dtNow.year(),1,1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addYears(-1);
		dtNow = QDate(dtNow.year(),1,1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

/*ͨ���������ݻ�ȡָ�������ڵ�����*/
RStockData* getColorBlockItemByMins(const QList<qRcvFenBiData*>& list, qRcvFenBiData* pLastFenbi)
{
	if(list.size()<1)
		return NULL;

	RStockData* pData = new RStockData;
	//	qRcvFenBiData* pBegin = list.first();
	qRcvFenBiData* pLast = list.last();
	qRcvFenBiData* pFirst = list.first();
	pData->tmTime = pFirst->tmTime;
	pData->fOpen = pFirst->fPrice;
	pData->fClose = pLast->fPrice;

	if(pLastFenbi)
	{
		pData->fAmount = pLast->fAmount-pLastFenbi->fAmount;
		pData->fVolume = pLast->fVolume-pLastFenbi->fVolume;
	}
	else
	{
		pData->fAmount = pLast->fAmount;
		pData->fVolume = pLast->fVolume;
	}

	foreach(qRcvFenBiData* p,list)
	{
		if(pData->fLow>p->fPrice)
			pData->fLow = p->fPrice;
		if(pData->fHigh<p->fPrice)
			pData->fHigh = p->fPrice;
	}
	return pData;
}

/*ͨ���������ݻ�ȡһ�������ڵ�����*/
RStockData* getColorBlockItemByDays(const QList<qRcvHistoryData*>& list)
{
	if(list.size()<1)
		return NULL;
	RStockData* pData = new RStockData();

	qRcvHistoryData* pFirst = list.first();
	qRcvHistoryData* pLast = list.last();
	pData->tmTime = pFirst->time;
	pData->fOpen = pFirst->fOpen;
	pData->fClose = pLast->fClose;


	pData->fLow = 0;
	pData->fHigh = 0;
	pData->fAmount = 0;
	pData->fVolume = 0;
	foreach(qRcvHistoryData* p,list)
	{
		if(pData->fLow>p->fLow)
			pData->fLow = p->fLow;
		if(pData->fHigh<p->fHigh)
			pData->fHigh = p->fHigh;
		pData->fAmount+=p->fAmount;
		pData->fVolume+=p->fVolume;
	}
	return pData;
}

/*���ֱ����ݰ���ʱ��������*/
QMap<time_t,RStockData*>* getColorBlockItems(QMap<time_t,int>& mapTimes, const QList<qRcvFenBiData*>& minutes)
{
	QMap<time_t,RStockData*>* pMap = new QMap<time_t,RStockData*>();
	if(mapTimes.size()<1)
	{
		return pMap;
	}

	QMap<time_t,int>::iterator iter = mapTimes.begin();

	time_t tmBegin = iter.key();
	if(mapTimes.size()<2)
	{
		QList<qRcvFenBiData*> listFenBi;
		foreach(qRcvFenBiData* pFenBi,minutes)
		{
			if(pFenBi->tmTime>tmBegin)
				listFenBi.push_back(pFenBi);
		}
		pMap->insert(tmBegin,getColorBlockItemByMins(listFenBi,NULL));
		return pMap;
	}

	time_t tmEnd = (iter+1).key();

	QList<qRcvFenBiData*> listPer;
	qRcvFenBiData* pLastFenBi = NULL;
	foreach(qRcvFenBiData* pFenBi,minutes)
	{
		if(pFenBi->tmTime<tmBegin)
		{
			pLastFenBi = pFenBi;
			continue;
		}
		else if(pFenBi->tmTime>=tmBegin&&pFenBi->tmTime<=tmEnd)
		{
			listPer.push_back(pFenBi);
		}
		else
		{
			if(listPer.size()>0)
				pLastFenBi = listPer.last();
			pMap->insert(tmBegin,getColorBlockItemByMins(listPer,NULL));
			listPer.clear();
			
			++iter;
			while (iter!=mapTimes.end())
			{
				tmBegin = iter.key();
				if(iter!=mapTimes.end())
					tmEnd = (iter+1).key();
				else
					tmEnd = tmBegin+3600*24*1000;		//��1000���ʱ��
				
				if(pFenBi->tmTime>=tmBegin&&pFenBi->tmTime<=tmEnd)
				{
					listPer.push_back(pFenBi);
					break;
				}
				else
				{
					pMap->insert(tmBegin,NULL);
				}
				++iter;
			}
			
			if(iter==mapTimes.end())
				break;
		}
	}
	while(iter!=mapTimes.end())
	{
		pMap->insert(iter.key(),NULL);
		++iter;
	}

	if(pMap->size()!=mapTimes.size())
	{
		qDebug()<<"FenBi Map is not enouph";
	}

	return pMap;
}

/*�����߰���ʱ��������*/
QMap<time_t,RStockData*>* getColorBlockItems(QMap<time_t,int>& mapTimes, const QList<qRcvHistoryData*>& minutes)
{
	QMap<time_t,RStockData*>* pMap = new QMap<time_t,RStockData*>();
	if(mapTimes.size()<1)
		return pMap;

	QMap<time_t,int>::iterator iter = mapTimes.begin();

	time_t tmBegin = iter.key();
	if(mapTimes.size()<2)
	{
		QList<qRcvHistoryData*> listFenBi;
		foreach(qRcvHistoryData* pFenBi,minutes)
		{
			if(pFenBi->time>tmBegin)
				listFenBi.push_back(pFenBi);
		}
		pMap->insert(tmBegin,getColorBlockItemByDays(listFenBi));
		return pMap;
	}

	time_t tmEnd = (iter+1).key();

	QList<qRcvHistoryData*> listPer;
	qRcvHistoryData* pLastFenBi = NULL;
	foreach(qRcvHistoryData* pFenBi,minutes)
	{
		if(pFenBi->time<tmBegin)
		{
			pLastFenBi = pFenBi;
			continue;
		}
		else if(pFenBi->time>=tmBegin&&pFenBi->time<=tmEnd)
			listPer.push_back(pFenBi);
		else
		{
			if(listPer.size()>0)
				pLastFenBi = listPer.last();
			pMap->insert(tmBegin,getColorBlockItemByDays(listPer));
			listPer.clear();
			
			++iter;
			while (iter!=mapTimes.end())
			{
				tmBegin = iter.key();
				if(iter!=mapTimes.end())
					tmEnd = (iter+1).key();
				else
					tmEnd = tmBegin+3600*24*1000;		//��1000���ʱ��
				
				if(pFenBi->time>=tmBegin&&pFenBi->time<=tmEnd)
				{
					listPer.push_back(pFenBi);
					break;
				}
				else
				{
					pMap->insert(tmBegin,NULL);
				}
				++iter;
			}
			
			if(iter==mapTimes.end())
				break;
		}
	}
	while(iter!=mapTimes.end())
	{
		pMap->insert(iter.key(),NULL);
		++iter;
	}
	
	if(pMap->size()!=mapTimes.size())
	{
		qDebug()<<"Day Map is not enouph";
	}
	return pMap;
}


CBaseBlockWidget::CBaseBlockWidget( CBaseWidget* parent /*= 0*/, RWidgetType type /*= CBaseWidget::Basic*/ )
	: CCoordXBaseWidget(parent,type)
	, m_typeBlock(BlockCircle)
	, m_pMenuCustom(0)
	, m_pMenuColorMode(0)
	, m_pMenuBlockMode(0)
	, m_iCBHeight(16)
	, m_iCBWidth(16)
	, m_qsColorMode("")
{
	m_typeCircle = Min1;				//���ó�ʼ��ʾ����Ϊ1����
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

	}

	m_pMenuCustom->addSeparator();
}


CBaseBlockWidget::~CBaseBlockWidget(void)
{
	delete m_pMenuCustom;
}

bool CBaseBlockWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	//��ǰ��ʾ������
	if(eleWidget.hasAttribute("circle"))
	{
		m_typeCircle = static_cast<RStockCircle>(eleWidget.attribute("circle").toInt());
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
		m_typeBlock = static_cast<CBaseBlockWidget::BlockMode>(eleBlockMode.text().toInt());
	}
	
	return true;
}

bool CBaseBlockWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;


	//��ʾ������
	eleWidget.setAttribute("circle",m_typeCircle);
	eleWidget.setAttribute("CBWidth",m_iCBWidth);
	eleWidget.setAttribute("CBHeight",m_iCBHeight);

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

void CBaseBlockWidget::updateData()
{

}


void CBaseBlockWidget::setColorMode( const QString& mode )
{
	m_qsColorMode = mode;
	//QList<QAction*> listActs = m_pMenuColorMode->actions();
	//foreach(QAction* pAct,listActs)
	//{
	//	pAct->setChecked((pAct->data().toString() == mode) ? true : false);
	//}

	update();
}

void CBaseBlockWidget::onSetCircle()
{
	//���õ�ǰ����ʾ����
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	m_typeCircle = static_cast<RStockCircle>(pAct->data().toInt());
	updateData();
}

void CBaseBlockWidget::onSetColorMode()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	setColorMode(pAct->data().toString());
}

void CBaseBlockWidget::onSetBlockMode()
{
	//���õ�ǰ����ʾ����
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	m_typeBlock = static_cast<BlockMode>(pAct->data().toInt());
	update();
}

void CBaseBlockWidget::onSetBlockSize()
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

QMenu* CBaseBlockWidget::getCustomMenu()
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


	return m_pMenuCustom;
}


void CBaseBlockWidget::updateTimesH()
{
	//���µ�ǰ�ĺ��������ݣ��Ӻ���ǰ����ʱ��
	m_mapTimes.clear();

	//	int iCount = 1024;				//����1024��ʱ��
	if(m_typeCircle<Day)
	{
		time_t tmCur = CDataEngine::getCurrentTime();
		time_t tmLast = ((tmCur/(3600*24))*3600*24)+3600*(9-8)+60*25;	//9��25����
		time_t tmCurrent = (tmCur+m_typeCircle*2)/m_typeCircle*m_typeCircle;//���϶Է���ȡ��
		time_t tmNoon1 = ((tmCur/(3600*24))*3600*24)+3600*(11-8)+60*30;
		time_t tmNoon2 = ((tmCur/(3600*24))*3600*24)+3600*(13-8);


		if((tmCurrent%(3600*24))>3600*7)
		{
			tmCurrent = (tmCurrent/(3600*24))*3600*24 + 3600*7 + m_typeCircle;		//3������(���һ������)
		}
		/*�����Ϻ����¶����һ������*/
		if(tmCurrent>tmNoon2)
		{
			time_t tmBegin = tmNoon2-m_typeCircle;
			//time_t tmEnd = tmCurrent+m_typeCircle*2;
			getTimeMapByMin(m_mapTimes,tmBegin,tmCurrent,m_typeCircle);
		}

		if(tmCurrent>tmNoon1)
		{
			time_t tmBegin = tmLast-m_typeCircle;
			time_t tmEnd = tmNoon1+m_typeCircle;
			getTimeMapByMin(m_mapTimes,tmBegin,tmEnd,m_typeCircle);
		}
		else if(tmCurrent>tmLast)
		{
			time_t tmBegin = tmLast-m_typeCircle;
			time_t tmEnd = tmCurrent;
			getTimeMapByMin(m_mapTimes,tmBegin,tmEnd,m_typeCircle);
		}
	}
	else
	{
		time_t tmBegin = QDateTime(QDate(2000,1,1)).toTime_t();
		time_t tmEnd = (CDataEngine::getCurrentTime()/(3600*24))*3600*24 - 8*3600;

		if(m_typeCircle == Day)
		{
			getDayMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == DayN)
		{
			//Ŀǰδʹ��
		}
		else if(m_typeCircle == Week)
		{
			getWeekMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == Month)
		{
			getMonthMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == Month3)
		{
			getMonth3MapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == Year)
		{
			getYearMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
	}

	qDebug()<<m_mapTimes.size();
}

void CBaseBlockWidget::drawCoordX(QPainter& p,const QRect& rtCoordX)	//����X������
{
	m_mapShowTimes.clear();
	//����������ƺ�����
	int fBeginX = rtCoordX.right();
	int fEndX = rtCoordX.left();
	int fCBWidth = fBeginX-fEndX;
	if(fCBWidth<0)
		return;

	QList<time_t> listTimes = m_mapTimes.keys();
	int fCurX = fBeginX;
	int fLastX = fCurX;
	int iCount = listTimes.size()-1;

	int iTimeCount = 0;				//ֻ����������ʱ�����ɫ��������ɫ�����ڲ鿴��
	while(fCurX>fEndX && iCount>=0)
	{
		m_mapShowTimes[listTimes[iCount]] = fCurX;
		if(m_typeCircle<Day)
		{
			if((fLastX-fCurX)>30)
			{
				p.setPen( iTimeCount%2 ? QColor(255,0,0) : QColor(0,255,255));
				p.drawLine(fCurX,rtCoordX.top(),fCurX,rtCoordX.top()+2);
				p.drawText(fCurX-14,rtCoordX.top()+2,30,rtCoordX.height()-2,
					Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("hh:mm"));
				fLastX = fCurX;
				++iTimeCount;
			}
		}
		else
		{
			if((fLastX-fCurX)>80)
			{
				p.setPen( iTimeCount%2 ? QColor(255,0,0) : QColor(0,255,255));
				p.drawLine(fCurX,rtCoordX.top(),fCurX,rtCoordX.top()+2);
				switch(m_typeCircle)
				{
				case Week:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QString("%1 %2").arg(dtmTmp.date().year()).arg(dtmTmp.date().weekNumber()));
					}
					break;
				case Month:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy/MM"));
					}
					break;
				case Month3:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy/MM"));
					}
					break;
				case Year:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy"));
					}
					break;
				default:
					p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
						Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy/MM/dd"));
					break;
				}
				fLastX = fCurX;
				++iTimeCount;
			}
		}

		--iCount;
		fCurX = fCurX- m_iCBWidth;
	}
	return;
}

void CBaseBlockWidget::drawColocBlock(QPainter& p,int iY,QVector<float>& vValue)
{
	int nTimes = 1;
	if(m_typeCircle<=Min60)
		nTimes = 10;
	else if(m_typeCircle<=Week)
		nTimes = 1;
	else
		nTimes = 0.1;

	QMap<time_t,int>::iterator iter = m_mapShowTimes.begin();
	while(iter!=m_mapShowTimes.end())
	{
		QRect rtCB = QRect(iter.value(),iY,m_iCBWidth,m_iCBHeight);
		if(m_mapTimes.contains(iter.key()))
		{
			float f = ((vValue[m_mapTimes[iter.key()]])*nTimes);
			switch(m_typeBlock)
			{
			case BlockRect:
				{
					rtCB.adjust(1,1,-1,-1);
					p.fillRect(rtCB,CColorManager::getBlockColor(m_qsColorMode,f));
				}
				break;
			case BlockCircle:
				{
					QPainterPath path;
					path.addEllipse(rtCB);
					p.fillPath(path,CColorManager::getBlockColor(m_qsColorMode,f));
				}
				break;
			}
		}
		++iter;
	}
	/*
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
	}*/
}

QMap<time_t,RStockData*>* CBaseBlockWidget::getColorBlockMap( CStockInfoItem* pItem )
{
	QMap<time_t,RStockData*>* pMap = NULL;
	if(m_typeCircle < Day)
	{
		//��ȡ�������ݣ����м���
		QList<qRcvFenBiData*> FenBis = pItem->getFenBiList();
		pMap = getColorBlockItems(m_mapTimes,FenBis);
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
		pMap = getColorBlockItems(m_mapTimes,historys);
		{
			//�����ȡ����������
			foreach(qRcvHistoryData* p,historys)
				delete p;
			historys.clear();
		}
	}

	return pMap;
}
