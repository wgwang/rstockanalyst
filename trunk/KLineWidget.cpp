#include "StdAfx.h"
#include "KLineWidget.h"
#include "DataEngine.h"

#define	KLINE_BORDER	2

CKLineWidget::CKLineWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::KLine)
	, m_pStockItem(0)
	, fKGridWidth(0)
{
//	setMinimumSize(200,200);
	setMouseTracking(true);
	setStockCode(QString("600000"));
	m_pMenuCustom = new QMenu("K��ͼ����");
}

CKLineWidget::~CKLineWidget(void)
{
	clearTmpData();
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

	//����K��ͼ
	update();
}

void CKLineWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	QRectF rtClient = this->rect();
	p.fillRect(rtClient,QColor(0,0,0));
	if(!m_pStockItem)
		return;

	rtClient.adjust(KLINE_BORDER,KLINE_BORDER,-50,-15);
	if(!rtClient.isValid())
		return;

	/*��������*/
	drawCoordY(p,rtClient);
	drawKGrids(p,rtClient);
}

void CKLineWidget::mouseMoveEvent( QMouseEvent* e )
{
	if(!m_pStockItem)
	{
		QToolTip::hideText();
		return;
	}
	QRectF rtClient = this->rect();
	rtClient.adjust(KLINE_BORDER,KLINE_BORDER,-50,-15);

	float fBegin = rtClient.left();

	int iIndex = (e->posF().x() - fBegin)/fKGridWidth;
	if(iIndex>=0&&iIndex<listHistory.size())
	{
		qRcvHistoryData* pHistory = listHistory[iIndex];
		QString qsTooltip = QString("��Ʊ����:%1\r\nʱ��:%2\r\n��߼�:%3\r\n��ͼ�:%4\r\n���̼�:%5\r\n���̼�:%6\r\n�ɽ���:%7\r\n�ɽ���:%8")
			.arg(m_pStockItem->getCode()).arg(QDateTime::fromTime_t(pHistory->time).toString("yyyy/MM/dd"))
			.arg(pHistory->fHigh).arg(pHistory->fLow).arg(pHistory->fOpen).arg(pHistory->fClose)
			.arg(pHistory->fVolume).arg(pHistory->fAmount);

		QToolTip::showText(e->globalPos(),qsTooltip,this);
	}
	else
	{
		QToolTip::hideText();
	}
}

QMenu* CKLineWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	return m_pMenuCustom;
}

void CKLineWidget::drawCoordY( QPainter& p,const QRectF& rtClient )
{
	//���û�����ɫ
	p.setPen(QColor(255,0,0));

	//Y���꣨�۸�
	p.drawLine(rtClient.topRight(),rtClient.bottomRight());			//����

	int iBeginPrice = fMinPrice*10;
	int iEndPrice = fMaxPrice*10;
	float fGridHeight = rtClient.height()/(iEndPrice-iBeginPrice);
	int iGridSize = 1;
	while((fGridHeight*iGridSize)<10)
		++iGridSize;
	fGridHeight = fGridHeight*iGridSize;

	float fY = rtClient.bottom();
	float fX = rtClient.right();

	int iGridCount = 0;
	for (int i=(iBeginPrice+iGridSize); i<iEndPrice; i=i+iGridSize)
	{
		fY = fY-fGridHeight;
		if(iGridCount%10 == 0)
		{
			p.drawLine(fX,fY,fX+4,fY);
			p.setPen(QColor(0,255,255));
			p.drawText(fX+7,fY+4,QString("%1").arg(float(float(i)/10),0,'f',2));
			p.setPen(QColor(255,0,0));
		}
		else if(iGridCount%5 == 0)
			p.drawLine(fX,fY,fX+4,fY);
		else
			p.drawLine(fX,fY,fX+2,fY);
		++iGridCount;
	}
}

void CKLineWidget::drawKGrids( QPainter& p,const QRectF& rtClient )
{
	p.setLayoutDirection(Qt::LeftToRight);
	//���û�����ɫ
	p.setPen(QColor(255,0,0));

	/*X���꣨ʱ�䣩*/
	p.drawLine(rtClient.bottomLeft(),rtClient.bottomRight());		//��

	int iCount = listHistory.size();				//�ܸ���
	fKGridWidth = rtClient.width()/iCount;			//���п��

	int iIndex = 0;
	int iMonth = 0;
	int iYear = 0;
	foreach(qRcvHistoryData* pHistory, listHistory)
	{
		p.setPen(QColor(255,0,0));
		float fBeginX=rtClient.left() + fKGridWidth*iIndex;
		QDateTime date = QDateTime::fromTime_t(pHistory->time);
		if(date.date().month()!=iMonth)
		{
			float fX = fBeginX+(fKGridWidth/2);
			p.drawLine(fX,rtClient.bottom(),fX,rtClient.bottom()+2);
			if(date.date().year()!=iYear)
			{
				iYear = date.date().year();
				iMonth = date.date().month();
				p.drawText(fBeginX,rtClient.bottom()+2,40,50,Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(iYear));
			}
			else
			{
				iMonth = date.date().month();
				p.drawText(fBeginX,rtClient.bottom()+2,40,50,Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(iMonth));
			}
		}

		drawKGrid(pHistory,p,QRectF(fBeginX,rtClient.top(),fKGridWidth,rtClient.bottom()));
		++iIndex;
	}
}

void CKLineWidget::drawKGrid( qRcvHistoryData* pHistory,QPainter& p,const QRectF& rtClient )
{
	float fHighMax = fMaxPrice-fMinPrice;
	float fHighY = ((pHistory->fHigh-fMinPrice)/fHighMax)*rtClient.height();
	float fLowY = ((pHistory->fLow-fMinPrice)/fHighMax)*rtClient.height();
	float fOpenY = ((pHistory->fOpen-fMinPrice)/fHighMax)*rtClient.height();
	float fCloseY = ((pHistory->fClose-fMinPrice)/fHighMax)*rtClient.height();

	if(pHistory->fClose>pHistory->fOpen)
	{
		//���������ƺ�ɫɫ��
		p.setPen(QColor(255,0,0));
		if(int(rtClient.width())%2==0)
		{
			QRectF rt = QRectF(rtClient.left()+0.5,rtClient.bottom()-fCloseY,rtClient.width()-1.0,fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
			p.fillRect(rt,QColor(255,0,0));
		}
		else
		{
			QRectF rt = QRectF(rtClient.left(),rtClient.bottom()-fCloseY,rtClient.width(),fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
			p.fillRect(rt,QColor(255,0,0));
		}
	}
	else
	{
		//���ͣ�������ɫɫ��
		p.setPen(QColor(0,255,255));
		if(int(rtClient.width())%2==0)
		{
			QRectF rt = QRectF(rtClient.left()+0.5,rtClient.bottom()-fOpenY,rtClient.width()-1.0,fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
			p.fillRect(rt,QColor(0,255,255));
		}
		else
		{
			QRectF rt = QRectF(rtClient.left(),rtClient.bottom()-fOpenY,rtClient.width(),fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
			p.fillRect(rt,QColor(0,255,255));
		}
	}

	p.drawLine(rtClient.center().x(),rtClient.bottom()-fHighY,rtClient.center().x(),rtClient.bottom()-fLowY);		//����߼۵���ͼ۵���
}

void CKLineWidget::clearTmpData()
{
	foreach(qRcvHistoryData* p,listHistory)
		delete p;
	listHistory.clear();
}

void CKLineWidget::resetTmpData()
{
	clearTmpData();
	listHistory = m_pStockItem->getLastHistory(100);
	if(listHistory.size()<1)
		return;
	//��ʼʱ��ͽ���ʱ��
	tmBegin = listHistory.first()->time;
	tmEnd = listHistory.last()->time;

	//������߼ۺ���ͼ�
	float fMin=FLT_MAX;
	float fMax=0;
	foreach(qRcvHistoryData* p,listHistory)
	{ 
		if(p->fLow<fMin&&p->fLow>0){fMin = p->fLow;}
		if(p->fHigh>fMax){fMax = p->fHigh;}
	}
	if(fMin<fMax)
	{
		fMaxPrice = float(int(fMax*100)/10)/float(10) + 0.1;
		fMinPrice = float(int(fMin*100)/10)/float(10);
	}
}
