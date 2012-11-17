#include "StdAfx.h"
#include "MarketTrendWidget.h"

CMarketTrendWidget::CMarketTrendWidget( CBaseWidget* parent /*= 0*/ )
	: CBaseWidget(parent,CBaseWidget::MarketTrend)
	, m_iHeaderHeight(16)
	, m_iItemHeight(16)
	, m_iBottomHeight(20)
	, m_rtHeader(0,0,0,0)
	, m_rtClient(0,0,0,0)
	, m_rtBottom(0,0,0,0)
{
	m_listHeader << tr("����") << tr("����") << tr("����")
		<< tr("�Ƿ�") << tr("����") << tr("�����ʣ��ֲ") << tr("ǰ��") << tr("��")
		<< tr("���") << tr("���") << tr("����") << tr("����") << tr("�ܶ�ֲ֣�")
		<< tr("����") << tr("����") << tr("�ǵ�") << tr("���") << tr("����") << tr("��ӯ��")
		<< tr("��ͨ��ֵ") << tr("����") << tr("����") << tr("ί����") << tr("ί����")
		<< tr("ί��") << tr("ί��");

	m_pMenuCustom = new QMenu(tr("�г�����ͼ�˵�"));
}

CMarketTrendWidget::~CMarketTrendWidget(void)
{
}

void CMarketTrendWidget::paintEvent( QPaintEvent* e )
{
	QPainter p(this);
	drawHeaders(p);
	p.fillRect(m_rtClient,QColor(255,255,0));
	p.fillRect(m_rtBottom,QColor(255,255,255));
}

void CMarketTrendWidget::resizeEvent( QResizeEvent* e)
{
	updateDrawRect();
	return CBaseWidget::resizeEvent(e);
}

QMenu* CMarketTrendWidget::getCustomMenu()
{
	return m_pMenuCustom;
}

void CMarketTrendWidget::updateDrawRect()
{
	QRect rtClient = this->rect();
	m_rtHeader = QRect(rtClient.topLeft(),QSize(rtClient.width(),m_iHeaderHeight));
	m_rtBottom = QRect(rtClient.left(),rtClient.bottom()-m_iBottomHeight,rtClient.width(),m_iBottomHeight);
	m_rtClient = QRect(m_rtHeader.bottomLeft(),QSize(rtClient.width(),rtClient.height()-m_iBottomHeight-m_iHeaderHeight));
}

void CMarketTrendWidget::drawHeaders( QPainter& p )
{
	p.fillRect(m_rtHeader,QColor(255,0,0));
}
