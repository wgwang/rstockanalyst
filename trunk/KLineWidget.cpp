#include "StdAfx.h"
#include "KLineWidget.h"
#include "DataEngine.h"


CKLineWidget::CKLineWidget( QWidget* parent /*= 0*/ )
	: QWidget(parent)
{

}

CKLineWidget::~CKLineWidget(void)
{

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
	}
}

void CKLineWidget::updateKLine( const QString& code )
{
	if(m_pStockItem&&m_pStockItem->getCode()!=code)
		return;

	//����K��ͼ
	update();
}

void CKLineWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	p.drawLine(0,0,100,100);
}
