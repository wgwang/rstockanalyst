#include "StdAfx.h"
#include "BaseLiner.h"

void getMinAndMax(float& fMin,float& fMax,const QList<stLinerItem*>& list,int iCount)
{
	fMin = 999999999;
	fMax = -999999999;

	int c = 0;
	for(int i=(list.size()-1);(i>0&&c<=iCount);--i)
	{
		stLinerItem* p = list[i];
		if(p->fHigh>fMax)
			fMax = p->fHigh;
		if(p->fLow<fMin)
			fMin = p->fLow;

		++c;
	}
}

CBaseLiner::CBaseLiner(void)
{
}

CBaseLiner::~CBaseLiner(void)
{
}

void CBaseLiner::Draw( QPainter& p,const QList<stLinerItem*>& d,const QRectF& rtClient, int iShowCount  )
{

}



CKLineLiner::CKLineLiner( void )
{

}

CKLineLiner::~CKLineLiner( void )
{

}

void CKLineLiner::Draw( QPainter& p,const QList<stLinerItem*>& d,const QRectF& rtClient, int iShowCount )
{
	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);
	float fMinPrice,fMaxPrice;
	getMinAndMax(fMinPrice,fMaxPrice,d,iShowCount);
	drawCoordY(p,QRectF(rtClient.right()+2,rtClient.top(),50,rtClient.height()),fMinPrice,fMaxPrice);

	//void CKLineWidget::drawCoordY( QPainter& p,const QRectF& rtClient )
	//{
	//	//���û�����ɫ
	//	p.setPen(QColor(255,0,0));

	//	//Y���꣨�۸�
	//	p.drawLine(rtClient.topRight(),rtClient.bottomRight());			//����

	//	int iBeginPrice = fMinPrice*10;
	//	int iEndPrice = fMaxPrice*10;
	//	float fGridHeight = rtClient.height()/(iEndPrice-iBeginPrice);
	//	int iGridSize = 1;
	//	while((fGridHeight*iGridSize)<10)
	//		++iGridSize;
	//	fGridHeight = fGridHeight*iGridSize;

	//	float fY = rtClient.bottom();
	//	float fX = rtClient.right();

	//	int iGridCount = 0;
	//	for (int i=(iBeginPrice+iGridSize); i<iEndPrice; i=i+iGridSize)
	//	{
	//		fY = fY-fGridHeight;
	//		if(iGridCount%10 == 0)
	//		{
	//			p.drawLine(fX,fY,fX+4,fY);
	//			p.setPen(QColor(0,255,255));
	//			p.drawText(fX+7,fY+4,QString("%1").arg(float(float(i)/10),0,'f',2));
	//			p.setPen(QColor(255,0,0));
	//		}
	//		else if(iGridCount%5 == 0)
	//			p.drawLine(fX,fY,fX+4,fY);
	//		else
	//			p.drawLine(fX,fY,fX+2,fY);
	//		++iGridCount;
	//	}
	//}

	//void CKLineWidget::drawKGrids( QPainter& p,const QRectF& rtClient )
	//{
	//	p.setLayoutDirection(Qt::LeftToRight);
	//	//���û�����ɫ
	//	p.setPen(QColor(255,0,0));

	//	/*X���꣨ʱ�䣩*/
	//	p.drawLine(rtClient.bottomLeft(),rtClient.bottomRight());		//��

	//	int iCount = listHistory.size();				//�ܸ���
	//	fItemWidth = rtClient.width()/iCount;			//���п��

	//	int iIndex = 0;
	//	int iMonth = 0;
	//	int iYear = 0;
	//	foreach(qRcvHistoryData* pHistory, listHistory)
	//	{
	//		p.setPen(QColor(255,0,0));
	//		float fBeginX=rtClient.left() + fItemWidth*iIndex;
	//		QDateTime date = QDateTime::fromTime_t(pHistory->time);
	//		if(date.date().month()!=iMonth)
	//		{
	//			float fX = fBeginX+(fItemWidth/2);
	//			p.drawLine(fX,rtClient.bottom(),fX,rtClient.bottom()+2);
	//			if(date.date().year()!=iYear)
	//			{
	//				iYear = date.date().year();
	//				iMonth = date.date().month();
	//				p.drawText(fBeginX,rtClient.bottom()+2,40,50,Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(iYear));
	//			}
	//			else
	//			{
	//				iMonth = date.date().month();
	//				p.drawText(fBeginX,rtClient.bottom()+2,40,50,Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(iMonth));
	//			}
	//		}

	//		drawKGrid(pHistory,p,QRectF(fBeginX,rtClient.top(),fItemWidth,rtClient.bottom()));
	//		++iIndex;
	//	}
	//}

	//void CKLineWidget::drawKGrid( qRcvHistoryData* pHistory,QPainter& p,const QRectF& rtClient )
	//{
	//	float fHighMax = fMaxPrice-fMinPrice;
	//	float fHighY = ((pHistory->fHigh-fMinPrice)/fHighMax)*rtClient.height();
	//	float fLowY = ((pHistory->fLow-fMinPrice)/fHighMax)*rtClient.height();
	//	float fOpenY = ((pHistory->fOpen-fMinPrice)/fHighMax)*rtClient.height();
	//	float fCloseY = ((pHistory->fClose-fMinPrice)/fHighMax)*rtClient.height();

	//	if(pHistory->fClose>pHistory->fOpen)
	//	{
	//		//���������ƺ�ɫɫ��
	//		p.setPen(QColor(255,0,0));
	//		if(int(rtClient.width())%2==0)
	//		{
	//			QRectF rt = QRectF(rtClient.left()+0.5,rtClient.bottom()-fCloseY,rtClient.width()-1.0,fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
	//			p.fillRect(rt,QColor(255,0,0));
	//		}
	//		else
	//		{
	//			QRectF rt = QRectF(rtClient.left(),rtClient.bottom()-fCloseY,rtClient.width(),fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
	//			p.fillRect(rt,QColor(255,0,0));
	//		}
	//	}
	//	else
	//	{
	//		//���ͣ�������ɫɫ��
	//		p.setPen(QColor(0,255,255));
	//		if(int(rtClient.width())%2==0)
	//		{
	//			QRectF rt = QRectF(rtClient.left()+0.5,rtClient.bottom()-fOpenY,rtClient.width()-1.0,fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
	//			p.fillRect(rt,QColor(0,255,255));
	//		}
	//		else
	//		{
	//			QRectF rt = QRectF(rtClient.left(),rtClient.bottom()-fOpenY,rtClient.width(),fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
	//			p.fillRect(rt,QColor(0,255,255));
	//		}
	//	}

	//	p.drawLine(rtClient.center().x(),rtClient.bottom()-fHighY,rtClient.center().x(),rtClient.bottom()-fLowY);		//����߼۵���ͼ۵���
	//}
}

void CKLineLiner::drawCoordY( QPainter& p,const QRectF& rtClient,float fMinPrice,float fMaxPrice )
{
	//����Y������
	//���û�����ɫ
	p.setPen(QColor(255,0,0));

	//Y���꣨�۸�
	p.drawLine(rtClient.topLeft(),rtClient.bottomLeft());			//����

	int iBeginPrice = fMinPrice*10;
	int iEndPrice = fMaxPrice*10;
	float fGridHeight = rtClient.height()/(iEndPrice-iBeginPrice);
	int iGridSize = 1;
	while((fGridHeight*iGridSize)<10)
		++iGridSize;
	fGridHeight = fGridHeight*iGridSize;

	float fY = rtClient.bottom();
	float fX = rtClient.left();

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



CMultiLiner::CMultiLiner( MultiLinerType type )
	: m_type(type)
{

}

CMultiLiner::~CMultiLiner( void )
{

}

void CMultiLiner::Draw( QPainter& p, const QList<stLinerItem*>& d,const QRectF& rtClient, int iShowCount )
{
	foreach(CBaseLiner* pLiner,m_listLiner)
	{
		pLiner->Draw(p,d,rtClient,iShowCount);
	}
}

void CMultiLiner::setExpression( const QString& exp )
{
	foreach(CBaseLiner* p,m_listLiner)
		delete p;
	m_listLiner.clear();

	if(m_type==Main)
	{
		m_listLiner.push_back(new CKLineLiner());
	}
}
