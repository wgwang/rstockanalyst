#include "StdAfx.h"
#include "BaseLiner.h"


CBaseLiner::CBaseLiner( QScriptEngine* pEngine,const QString& exp )
	: m_pEngine(pEngine)
	, m_qsExp(exp)
{
	updateData();
}

CBaseLiner::~CBaseLiner(void)
{

}

void CBaseLiner::updateData()
{
	if(m_pEngine)
	{
		m_vals = qscriptvalue_cast<QVector<float>>(m_pEngine->evaluate(QScriptProgram(m_qsExp)));
	}
}

void CBaseLiner::getMinAndMax( float& fMin,float& fMax,int iCount )
{
	int c = 0;
	for(int i=(m_vals.size()-1);(i>0&&c<=iCount);--i)
	{
		if(m_vals[i]>fMax)
			fMax = m_vals[i];
		if(m_vals[i]<fMin)
			fMin = m_vals[i];

		++c;
	}
}

void CBaseLiner::Draw( QPainter& p,const QRectF& rtClient, int iShowCount  )
{
	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);


	//设置画笔颜色
	p.setPen(QColor(255,0,0));

	float fItemWidth = rtClient.width()/iShowCount;			//单列宽度

	int iIndex = m_vals.size()-1;
	float fBeginX = rtClient.right()-(fItemWidth/2);
	float fHighMax = fMaxPrice-fMinPrice;

	int iCount = 0;
	QPointF ptBegin(0.0,0.0);
	QPointF ptEnd(0.0,0.0);
	while(iCount<iShowCount&&iIndex>=0)
	{
		float fHighY = rtClient.bottom() - (((m_vals[iIndex]-fMinPrice)/fHighMax)*rtClient.height());
		if(ptBegin.isNull())
		{
			ptBegin = QPointF(fBeginX,fHighY);
		}
		else if(ptEnd.isNull())
		{
			ptEnd = ptBegin;
			ptBegin = QPointF(fBeginX,fHighY);
		}
		else
		{
			ptEnd = ptBegin;
			ptBegin = QPointF(fBeginX,fHighY);
			p.drawLine(ptBegin,ptEnd);
		}
	//	drawKGrid(pItem,p,QRectF(fBeginX,rtClient.top(),fItemWidth,rtClient.height()));

		fBeginX-=fItemWidth;
		--iIndex;
		++iCount;
	}
}




CKLineLiner::CKLineLiner( QScriptEngine* pEngine )
	: CBaseLiner(pEngine,"")
{

}

CKLineLiner::~CKLineLiner( void )
{

}

void CKLineLiner::updateData()
{
	//更新K线所用到的数据
	m_vOpen = qscriptvalue_cast<QVector<float>>(m_pEngine->evaluate("OPEN;"));
	m_vHigh = qscriptvalue_cast<QVector<float>>(m_pEngine->evaluate("HIGH;"));
	m_vLow = qscriptvalue_cast<QVector<float>>(m_pEngine->evaluate("LOW;"));
	m_vClose = qscriptvalue_cast<QVector<float>>(m_pEngine->evaluate("CLOSE;"));
}

void CKLineLiner::getMinAndMax( float& fMin,float& fMax,int iCount )
{
	int c = 0;
	for(int i=(m_vOpen.size()-1);(i>0&&c<=iCount);--i)
	{
		if(m_vHigh[i]>fMax)
			fMax = m_vHigh[i];
		if(m_vLow[i]<fMin)
			fMin = m_vLow[i];

		++c;
	}
}

void CKLineLiner::Draw( QPainter& p,const QRectF& rtClient, int iShowCount )
{
	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);


	//设置画笔颜色
	p.setPen(QColor(255,0,0));

	float fItemWidth = rtClient.width()/iShowCount;			//单列宽度

	int iIndex = m_vOpen.size()-1;
	float fBeginX = rtClient.right()-fItemWidth;
	int iCount = 0;
	while(iCount<iShowCount&&iIndex>=0)
	{
		drawKGrid(iIndex,p,QRectF(fBeginX,rtClient.top(),fItemWidth,rtClient.height()));

		fBeginX-=fItemWidth;
		--iIndex;
		++iCount;
	}
}

void CKLineLiner::drawKGrid( const int& iIndex,QPainter& p,const QRectF& rtItem )
{
	float fHighMax = fMaxPrice-fMinPrice;
	float fHighY = ((m_vHigh[iIndex]-fMinPrice)/fHighMax)*rtItem.height();
	float fLowY = ((m_vLow[iIndex]-fMinPrice)/fHighMax)*rtItem.height();
	float fOpenY = ((m_vOpen[iIndex]-fMinPrice)/fHighMax)*rtItem.height();
	float fCloseY = ((m_vClose[iIndex]-fMinPrice)/fHighMax)*rtItem.height();

	if(m_vClose[iIndex]>m_vOpen[iIndex])
	{
		//增长，绘制红色色块
		p.setPen(QColor(255,0,0));
		if(int(rtItem.width())%2==0)
		{
			QRectF rt = QRectF(rtItem.left()+0.5,rtItem.bottom()-fCloseY,rtItem.width()-1.0,fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
			p.fillRect(rt,QColor(255,0,0));
		}
		else
		{
			QRectF rt = QRectF(rtItem.left(),rtItem.bottom()-fCloseY,rtItem.width(),fCloseY==fOpenY ? 1.0 : fCloseY-fOpenY);
			p.fillRect(rt,QColor(255,0,0));
		}
	}
	else
	{
		//降低，绘制蓝色色块
		p.setPen(QColor(0,255,255));
		if(int(rtItem.width())%2==0)
		{
			QRectF rt = QRectF(rtItem.left()+0.5,rtItem.bottom()-fOpenY,rtItem.width()-1.0,fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
			p.fillRect(rt,QColor(0,255,255));
		}
		else
		{
			QRectF rt = QRectF(rtItem.left(),rtItem.bottom()-fOpenY,rtItem.width(),fOpenY==fCloseY ? 1.0 : (fOpenY-fCloseY));
			p.fillRect(rt,QColor(0,255,255));
		}
	}

	p.drawLine(rtItem.center().x(),rtItem.bottom()-fHighY,rtItem.center().x(),rtItem.bottom()-fLowY);		//画最高价到最低价的线
}



CVolumeLiner::CVolumeLiner( QScriptEngine* pEngine )
	: CBaseLiner(pEngine,"")
{

}

CVolumeLiner::~CVolumeLiner( void )
{

}

void CVolumeLiner::updateData()
{
	m_vOpen = qscriptvalue_cast<QVector<float>>(m_pEngine->evaluate("OPEN;"));
	m_vClose = qscriptvalue_cast<QVector<float>>(m_pEngine->evaluate("CLOSE;"));
	m_vals = qscriptvalue_cast<QVector<float>>(m_pEngine->evaluate("VOLUME;"));
//	m_vAmount = qscriptvalue_cast<QVector<float>>(m_pEngine->evaluate("CLOSE;"));
}

void CVolumeLiner::getMinAndMax( float& fMin,float& fMax,int iCount )
{
	return CBaseLiner::getMinAndMax(fMin,fMax,iCount);
}

void CVolumeLiner::Draw( QPainter& p,const QRectF& rtClient,int iShowCount )
{
	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);

	//设置画笔颜色
	p.setPen(QColor(255,0,0));

	float fItemWidth = rtClient.width()/iShowCount;			//单列宽度

	int iIndex = m_vOpen.size()-1;
	float fBeginX = rtClient.right()-fItemWidth;
	int iCount = 0;
	while(iCount<iShowCount&&iIndex>=0)
	{
		QRectF rtItem = QRectF(fBeginX,rtClient.top(),fItemWidth,rtClient.height());
		float fVal = ((m_vals[iIndex]-fMinPrice)/(fMaxPrice-fMinPrice))*rtItem.height();

		if(m_vClose[iIndex]>m_vOpen[iIndex])
		{
			//增长，绘制红色色块
			p.setPen(QColor(255,0,0));
			if(int(rtItem.width())%2==0)
			{
				QRectF rt = QRectF(rtItem.left()+0.5,rtItem.bottom()-fVal,rtItem.width()-1.0,fVal);
				p.fillRect(rt,QColor(255,0,0));
			}
			else
			{
				QRectF rt = QRectF(rtItem.left(),rtItem.bottom()-fVal,rtItem.width(),fVal);
				p.fillRect(rt,QColor(255,0,0));
			}
		}
		else
		{
			//降低，绘制蓝色色块
			p.setPen(QColor(0,255,255));
			if(int(rtItem.width())%2==0)
			{
				QRectF rt = QRectF(rtItem.left()+0.5,rtItem.bottom()-fVal,rtItem.width()-1.0,fVal);
				p.fillRect(rt,QColor(0,255,255));
			}
			else
			{
				QRectF rt = QRectF(rtItem.left(),rtItem.bottom()-fVal,rtItem.width(),fVal);
				p.fillRect(rt,QColor(0,255,255));
			}
		}


		fBeginX-=fItemWidth;
		--iIndex;
		++iCount;
	}
}


CMultiLiner::CMultiLiner( MultiLinerType type,QScriptEngine* pEngine )
	: m_type(type)
	, m_pEngine(pEngine)
{
	if(m_type==MainKLine)
	{
		m_listLiner.push_back(new CKLineLiner(m_pEngine));
	}
	if(m_type == VolumeLine)
	{
		m_listLiner.push_back(new CVolumeLiner(m_pEngine));
	}
}

CMultiLiner::~CMultiLiner( void )
{
	foreach(CBaseLiner* p,m_listLiner)
		delete p;
	m_listLiner.clear();
}

void CMultiLiner::updateData()
{
	foreach(CBaseLiner* p,m_listLiner)
	{
		p->updateData();
	}
}

void CMultiLiner::Draw( QPainter& p, const QRectF& rtClient, int iShowCount )
{
	m_rtClient = rtClient;
	if(!rtClient.isValid())
		return;
	float fMinPrice = 9999999.0;
	float fMaxPrice = -9999999.0;

	foreach(CBaseLiner* pLiner,m_listLiner)
	{
		pLiner->getMinAndMax(fMinPrice,fMaxPrice,iShowCount);
	}
	if(fMaxPrice<=fMinPrice)
		return;

	drawCoordY(p,QRectF(rtClient.right()+2,rtClient.top(),50,rtClient.height()),fMinPrice,fMaxPrice);

	foreach(CBaseLiner* pLiner,m_listLiner)
	{
		pLiner->setMaxPrice(fMaxPrice);
		pLiner->setMinPrice(fMinPrice);
		pLiner->Draw(p,rtClient,iShowCount);
	}
}

void CMultiLiner::setExpression( const QString& exp )
{
	m_qsExp = exp;
	foreach(CBaseLiner* p,m_listLiner)
		delete p;
	m_listLiner.clear();

	if(m_type==MainKLine)
	{
		m_listLiner.push_back(new CKLineLiner(m_pEngine));
	}

	QStringList listExp = exp.split("\n");
	foreach(const QString& str,listExp)
	{
		QString e = str.trimmed();
		if(!e.isEmpty())
		{
			CBaseLiner* pLiner = new CBaseLiner(m_pEngine,str);
			pLiner->updateData();
			m_listLiner.push_back(pLiner);
		}
	}
}

void CMultiLiner::drawCoordY( QPainter& p,const QRectF& rtClient,float fMinPrice,float fMaxPrice )
{
	if(!rtClient.isValid())
		return;
	//绘制Y坐标轴
	//设置画笔颜色
	p.setPen(QColor(255,0,0));

	//Y坐标（价格）
	p.drawLine(rtClient.topLeft(),rtClient.bottomLeft());			//主线

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