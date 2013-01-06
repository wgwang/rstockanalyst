#include "StdAfx.h"
#include "BaseLiner.h"
#include "ColorManager.h"


CBaseLiner::CBaseLiner( QScriptEngine* pEngine,const QString& exp,const QString& title /*= "" */ )
	: m_pEngine(pEngine)
	, m_qsExp(exp)
	, m_qsTitle(title)
	, m_clrLine(QColor(255,255,255))
{
	updateData();
	if(m_qsTitle.isEmpty())
		m_qsTitle = m_qsExp;
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
	for(int i=(m_vals.size()-1);(i>=0&&c<=iCount);--i)
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


	//���û�����ɫ
	p.setPen(m_clrLine);

	float fItemWidth = rtClient.width()/iShowCount;			//���п��

	int iIndex = m_vals.size()-1;
	float fBeginX = rtClient.right()-(fItemWidth/2);
	float fHighMax = fMaxPrice-fMinPrice;

	int iCount = 0;
	QPointF ptBegin(0.0,0.0);
	QPointF ptEnd(0.0,0.0);
	while(iCount<iShowCount&&iIndex>=0)
	{
		float fHighY = rtClient.center().y();
		if(fHighMax > 0)
			fHighY = rtClient.bottom() - (((m_vals[iIndex]-fMinPrice)/fHighMax)*rtClient.height());
		if(ptBegin.isNull())
		{
			ptBegin = QPointF(fBeginX,fHighY);
		}
		else if(ptEnd.isNull())
		{
			ptEnd = ptBegin;
			ptBegin = QPointF(fBeginX,fHighY);
			p.drawLine(ptBegin,ptEnd);
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
	: CBaseLiner(pEngine,"", "K��ͼ"),
	m_typeKLine(CKLineLiner::Normal)
{

}

CKLineLiner::~CKLineLiner( void )
{

}

void CKLineLiner::updateData()
{
	//����K�����õ�������
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


	//���û�����ɫ
	p.setPen(QColor(255,0,0));

	float fItemWidth = rtClient.width()/iShowCount;			//���п��

	int iIndex = m_vOpen.size()-1;
	float fBeginX = rtClient.right()-fItemWidth;
	int iCount = 0;

	while(iCount<iShowCount&&iIndex>=0)
	{
		if(m_typeKLine == FenShi)
			drawFenShi(iIndex,p,QRectF(fBeginX,rtClient.top(),fItemWidth,rtClient.height()));
		else
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

	if(m_vClose[iIndex]>=m_vOpen[iIndex])
	{
		//���������ƺ�ɫɫ��
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
		//���ͣ�������ɫɫ��
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

	p.drawLine(rtItem.center().x(),rtItem.bottom()-fHighY,rtItem.center().x(),rtItem.bottom()-fLowY);		//����߼۵���ͼ۵���
}

void CKLineLiner::drawFenShi( const int& iIndex,QPainter& p,const QRectF& rtItem )
{
	float fPrice1 = m_vClose.size()>(iIndex+2) ? m_vClose[iIndex+1] : -1;
	float fPrice2 = m_vClose[iIndex];
	float fHighMax = fMaxPrice-fMinPrice;

	if(fPrice1>=0)
	{
		float fY1 = ((fPrice1-fMinPrice)/fHighMax)*rtItem.height();
		float fY2 = ((fPrice2-fMinPrice)/fHighMax)*rtItem.height();
		p.drawLine(rtItem.center().x()+rtItem.width(),rtItem.bottom()-fY1,rtItem.center().x(),rtItem.bottom()-fY2);		//����߼۵���ͼ۵���
	}
}



CVolumeLiner::CVolumeLiner( QScriptEngine* pEngine )
	: CBaseLiner(pEngine,"", "����ͼ")
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
//	return CBaseLiner::updateData();
}

void CVolumeLiner::getMinAndMax( float& fMin,float& fMax,int iCount )
{
	return CBaseLiner::getMinAndMax(fMin,fMax,iCount);
}

void CVolumeLiner::Draw( QPainter& p,const QRectF& rtClient,int iShowCount )
{
	p.setPen(QColor(127,0,0));
	p.drawRect(rtClient);

	//���û�����ɫ
	p.setPen(QColor(255,0,0));

	float fItemWidth = rtClient.width()/iShowCount;			//���п��

	int iIndex = m_vOpen.size()-1;
	float fBeginX = rtClient.right()-fItemWidth;
	int iCount = 0;
	float fAbsValue = (fMaxPrice - fMinPrice);
	while(iCount<iShowCount&&iIndex>=0)
	{
		QRectF rtItem = QRectF(fBeginX,rtClient.top(),fItemWidth,rtClient.height());
		float fVal = ((m_vals[iIndex]-fMinPrice)/fAbsValue)*rtItem.height();

		if(m_vClose[iIndex]>=m_vOpen[iIndex])
		{
			//���������ƺ�ɫɫ��
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
			//���ͣ�������ɫɫ��
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


CMultiLiner::CMultiLiner( MultiLinerType type,QScriptEngine* pEngine,const QString& exp )
	: m_type(type)
	, m_pEngine(pEngine)
{
	setExpression(exp);
}

CMultiLiner::~CMultiLiner( void )
{
	foreach(CBaseLiner* p,m_listLiner)
		delete p;
	m_listLiner.clear();
}

void CMultiLiner::updateData()
{
	for(int i=0;i<m_listLiner.size();++i)
	{
		m_listLiner[i]->setLineColor(CColorManager::CommonColor[(i%(CColorManager::CommonColor.size()))]);
		m_listLiner[i]->updateData();
	}
}

void CMultiLiner::Draw( QPainter& p, const QRectF& rtClient, int iShowCount )
{
	m_rtClient = rtClient;
	if(!rtClient.isValid())
		return;
	fMinPrice = FLT_MAX;
	fMaxPrice = FLT_MIN;

	//��ȡ���ֵ/��Сֵ
	foreach(CBaseLiner* pLiner,m_listLiner)
	{
		pLiner->getMinAndMax(fMinPrice,fMaxPrice,iShowCount);
	}
	if(fMaxPrice<fMinPrice)
		return;

	if(fMaxPrice<214748364)
	{
		//ȡ��
		fMinPrice = ((int)((float)(fMinPrice*10.0)))/10.0;
		fMaxPrice = ((int)((float)(fMaxPrice*10.0 + 1.0)))/10.0;
	}

	//����������Y������
	drawCoordY(p,QRectF(rtClient.right()+2,rtClient.top(),50,rtClient.height()),fMinPrice,fMaxPrice);

	//���Ƹ�����Liner
	foreach(CBaseLiner* pLiner,m_listLiner)
	{
		pLiner->setMaxPrice(fMaxPrice);
		pLiner->setMinPrice(fMinPrice);
		pLiner->Draw(p,rtClient,iShowCount);
	}

	//���Ƹ�Liner��ɫ��title
	int iTextHeight = p.fontMetrics().height();
	int iIndex = 0;
	foreach(CBaseLiner* pLiner,m_listLiner)
	{
		if(typeid(*pLiner)!=typeid(CBaseLiner))
			continue;
		p.setPen(pLiner->getLineColor());
		QRect rtText(rtClient.left(),rtClient.top()+iTextHeight*iIndex,rtClient.width(),iTextHeight);
		p.drawText(rtText,Qt::AlignLeft|Qt::AlignTop,pLiner->getTitle());
		++iIndex;
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
	else if(m_type == VolumeLine)
	{
		m_listLiner.push_back(new CVolumeLiner(m_pEngine));
	}

	QStringList listExp = exp.split("\n");
	foreach(const QString& str,listExp)
	{
		QString e = str.trimmed();
		if(!e.isEmpty())
		{
			CBaseLiner* pLiner = new CBaseLiner(m_pEngine,str);
			m_listLiner.push_back(pLiner);
		}
	}
	updateData();
}

float CMultiLiner::getValueByY( int y )
{
	if(m_rtClient.top()>=y||m_rtClient.bottom()<=y)
		return float();
	float fPersent = float(m_rtClient.bottom()-y)/float(m_rtClient.height());

	return (fMaxPrice-fMinPrice)*fPersent+fMinPrice;
}

void CMultiLiner::setKLineType(CKLineLiner::KLineType t)
{
	if(m_type == MainKLine)
	{
		CKLineLiner* pK = reinterpret_cast<CKLineLiner*>(m_listLiner.first());
		pK->setShowType(t);
	}
}

void CMultiLiner::drawCoordY( QPainter& p,const QRectF& rtClient,float fMinPrice,float fMaxPrice )
{
	if(!rtClient.isValid())
		return;
	if(fMaxPrice<=fMinPrice)
		return;
	//����Y������
	//���û�����ɫ
	p.setPen(QColor(255,0,0));

	//Y���꣨�۸�
	p.drawLine(rtClient.topLeft(),rtClient.bottomLeft());			//����
	float iPower = 1.0;		//�Ŵ����

	float iBeginPrice = ((float)(fMinPrice*10.0));
	float iEndPrice = ((float)(fMaxPrice*10.0));

	float fGridHeight = rtClient.height()/((iEndPrice-iBeginPrice)/iPower);
	if(fGridHeight>50)
	{
		iPower = iPower/10.0;
		iBeginPrice = iBeginPrice*10;
		iEndPrice = iEndPrice*10;
		fGridHeight = rtClient.height()/(iEndPrice-iBeginPrice);
	}
	else
	{
		while(fGridHeight<1)
		{
			iPower = iPower*10.0;
			iBeginPrice = iBeginPrice/10.0;
			iEndPrice = iEndPrice/10.0;
			fGridHeight = rtClient.height()/(iEndPrice-iBeginPrice);
		}
	}
	if(fGridHeight<=0.0)
		return;

	int iGridSize = 1;
	while((fGridHeight*iGridSize)<10)
			++iGridSize;
	fGridHeight = fGridHeight*iGridSize;

	float fY = rtClient.bottom();
	float fX = rtClient.left();
	float fLast = fY+31;

	int iGridCount = 0;
	for (int i=(iBeginPrice+iGridSize); i<iEndPrice; i=i+iGridSize)
	{
		fY = fY-fGridHeight;
		if((fLast-fY)>30)
		{
			p.drawLine(fX,fY,fX+4,fY);
			p.setPen(QColor(0,255,255));
			float fVal = float(float(i)/10*iPower);
			if(fVal>10000.0)
				p.drawText(fX+7,fY+4,QString("%1").arg(fVal,0,'g',2));
			else
				p.drawText(fX+7,fY+4,QString("%1").arg(fVal,0,'f',2));
			p.setPen(QColor(255,0,0));
			fLast = fY;
		}
		else
		{
			p.drawLine(fX,fY,fX+2,fY);
		}
		//if(iGridCount%10 == 0)
		//{
		//	p.drawLine(fX,fY,fX+4,fY);
		//	p.setPen(QColor(0,255,255));
		//	p.drawText(fX+7,fY+4,QString("%1").arg(float(float(i)/10),0,'f',2));
		//	p.setPen(QColor(255,0,0));
		//}
		//else if(iGridCount%5 == 0)
		//	p.drawLine(fX,fY,fX+4,fY);
		//else
		//	p.drawLine(fX,fY,fX+2,fY);
		++iGridCount;
	}
}
