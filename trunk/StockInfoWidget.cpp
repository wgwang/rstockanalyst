/************************************************************************/
/* �ļ����ƣ�StockInfoWidget.cpp
/* ����ʱ�䣺2012-12-13 17:55
/*
/* ��    ����������ʾ��ֻ��Ʊ��������Ϣ���̳���CBaseWidget
/************************************************************************/

#include "StdAfx.h"
#include "StockInfoWidget.h"

CStockInfoWidget::CStockInfoWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent, CBaseWidget::StockInfo)
{
	m_pMenuCustom = new QMenu(tr("������Ϣ�˵�"));
}

CStockInfoWidget::~CStockInfoWidget(void)
{
	delete m_pMenuCustom;
}

void CStockInfoWidget::paintEvent( QPaintEvent* e )
{
	QPainter p(this);
	p.fillRect(this->rect(),QColor(255,0,0));
}

QMenu* CStockInfoWidget::getCustomMenu()
{
	QAction* pAction = m_pMenu->menuAction();
	if(!m_pMenuCustom->actionGeometry(pAction).isValid())
		m_pMenuCustom->addMenu(m_pMenu);

	return m_pMenuCustom;
}
