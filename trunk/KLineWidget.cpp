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

bool getLinerItemByDays(stLinerItem* pItem,const QList<qRcvHistoryData*>& list)
{

	return true;
}

int getLinerDayItem(QVector<stLinerItem>& listItems,const QList<qRcvHistoryData*>& historys, int nDay)
{
	if(nDay==1)
	{
		foreach(qRcvHistoryData* p,historys)
		{
			stLinerItem item;
			item.time = p->time;
			item.fOpen = p->fOpen;
			item.fClose = p->fClose;
			item.fHigh = p->fHigh;
			item.fLow = p->fLow;
			item.fAmount = p->fAmount;
			item.fVolume = p->fVolume;
			item.wAdvance = p->wAdvance;
			item.wDecline = p->wDecline;
			listItems.push_back(item);
		}
	}
	else
	{

	}
	return listItems.size();
}

int getLinerWeekItem(QList<stLinerItem*>& listItems,const QList<qRcvHistoryData*>& historys)
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
				stLinerItem* pItem = new stLinerItem;
				getLinerItemByDays(pItem,weekHis);
				listItems.push_back(pItem);
				weekHis.clear();
			}
		}
		else if(tmDate.weekNumber()!=iCurWeek)
		{
			iCurWeek = tmDate.weekNumber();

			stLinerItem* pItem = new stLinerItem;
			getLinerItemByDays(pItem,weekHis);
			listItems.push_back(pItem);
			weekHis.clear();
		}
	}

	return listItems.size();
}

CKLineWidget::CKLineWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::KLine)
	, m_pMenuCustom(0)
	, m_pActShowMain(0)
	, m_typeCircle(CKLineWidget::Day)
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

		{
			//����Ԥ�ýű�
			QFile file("..\\RStockLiners.js");
			if(!file.open(QFile::ReadOnly))
				qDebug()<<"load script error.";
			QString qsScript = file.readAll();
			QScriptValue v = m_pScriptEngine->evaluate(QScriptProgram(qsScript));
			qDebug()<<"loaded script, info:"<<v.toString()<<"\r\n";
		}
	}

	m_pLinerMain = new CMultiLiner(CMultiLiner::MainKLine,m_pScriptEngine,"");
	m_vSizes.push_back(60);

	//���ò˵�
	m_pMenuCustom = new QMenu("K��ͼ����");
	m_pMenuCustom->addAction(tr("���ù�Ʊ����"),this,SLOT(onSetStockCode()));
	{
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
	setMouseTracking(true);
	setStockCode(QString("600000"));

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
		QDomElement eleCode = doc.createElement("code");
		eleCode.appendChild(doc.createTextNode(m_pStockItem->getCode()));
		eleWidget.appendChild(eleCode);
	}

	return true;
}

void CKLineWidget::setStockCode( const QString& code )
{
	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(code);
	if(pItem)
	{
		disconnect(this,SLOT(updateKLine(const QString&)));		//�Ƴ����к� updateKLine������ �ź�/��

		m_pStockItem = pItem;

		//�������»���
		connect(pItem,SIGNAL(stockInfoItemChanged(const QString&)),this,SLOT(updateKLine(const QString&)));

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


	int iCurY = rtClient.top();		//��ǰ���Ƶ���λ��
	int iSizeIndex = 0;
	/*������ͼ*/
	if(m_pActShowMain->isChecked())
	{
		int iTotal = 0;					//�����ܺ�
		for (int i=iSizeIndex;i<m_vSizes.size();++i)
			iTotal += m_vSizes[i];
		int iTotalHeight = rtClient.bottom()-iCurY;

		int iMainHeight = (float)((float)m_vSizes[iSizeIndex]/float(iTotal))*iTotalHeight + 0.5;
		m_pLinerMain->Draw(p,QRectF(rtClient.left(),rtClient.top(),rtClient.width(),iMainHeight),m_iShowCount);
		iCurY += iMainHeight;
		++iSizeIndex;
	}

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
				iCurY += iHeight;
				++iSizeIndex;
			}
		}
	}
}

void CKLineWidget::mouseMoveEvent( QMouseEvent* )
{
	if(!m_pStockItem)
	{
		QToolTip::hideText();
		return;
	}
	QRectF rtClient = this->rect();
	rtClient.adjust(KLINE_BORDER,KLINE_BORDER,-50,-15);

	float fBegin = rtClient.left();
	//int iIndex = (e->posF().x() - fBegin)/fItemWidth;
	//if(iIndex>=0&&iIndex<listHistory.size())
	//{
	//	qRcvHistoryData* pHistory = listHistory[iIndex];
	//	QString qsTooltip = QString("��Ʊ����:%1\r\nʱ��:%2\r\n��߼�:%3\r\n��ͼ�:%4\r\n���̼�:%5\r\n���̼�:%6\r\n�ɽ���:%7\r\n�ɽ���:%8")
	//		.arg(m_pStockItem->getCode()).arg(QDateTime::fromTime_t(pHistory->time).toString("yyyy/MM/dd"))
	//		.arg(pHistory->fHigh).arg(pHistory->fLow).arg(pHistory->fOpen).arg(pHistory->fClose)
	//		.arg(pHistory->fVolume).arg(pHistory->fAmount);

	//	QToolTip::showText(e->globalPos(),qsTooltip,this);
	//}
	//else
	//{
	//	QToolTip::hideText();
	//}
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

void CKLineWidget::mouseDoubleClickEvent( QMouseEvent* )
{
	if(m_pCurrentLiner&&(m_pCurrentLiner!=m_pLinerMain))
	{
		m_bShowMax = !m_bShowMax;
		update();
	}
}

QMenu* CKLineWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

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
	float fCurX = rtCoordX.right()-2;
	float fLastX = fCurX+100;
	//���ƿ̶�
	if(m_typeCircle<Day)
	{

	}
	else
	{
		int iCurMonth = 0;
		int iCurIndex = listItems.size()-1;
		int iCount = 0;
		while(iCount<m_iShowCount)
		{
			QDate tmDate = QDateTime::fromTime_t(listItems[iCurIndex].time).date();
			if(tmDate.month()!=iCurMonth)
			{
				iCurMonth = tmDate.month();
				if((fLastX-fCurX)>16)
				{
					p.drawLine(fCurX,fTopLine,fCurX,rtCoordX.bottom()-1);
					p.drawText(fCurX+2,rtCoordX.bottom()-3,QString("%1").arg(iCurMonth));
					fLastX = fCurX;
				}
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
}

void CKLineWidget::resetTmpData()
{
	clearTmpData();
	if(m_typeCircle<Day)
	{
		//��ȡ�������ݣ����м���
		QList<qRcvMinuteData*> minutes = m_pStockItem->getMinuteList();
		if(m_typeCircle == Min1)
		{

		}
	}
	else
	{
		//��ȡ��������
		QList<qRcvHistoryData*> historys = m_pStockItem->getHistoryList();
		if(m_typeCircle == Day)
		{
			getLinerDayItem(listItems,historys,1);
		}
		else if(m_typeCircle == DayN)
		{
			//Ŀǰδʹ��
		//	getLinerItem(listItems,historys,3);
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
