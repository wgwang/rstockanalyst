/************************************************************************/
/* �ļ����ƣ�BaseWidget.cpp
/* ����ʱ�䣺2012-11-16 10:59
/*
/* ��    �������л��ƴ��ڵĻ��֧࣬�ֲ�ֵȲ���
/************************************************************************/
#include "StdAfx.h"
#include "BaseWidget.h"
#include "KLineWidget.h"

CBaseWidget::CBaseWidget( CBaseWidget* parent /*= 0*/, WidgetType type /*= Basic*/ )
	: QWidget(parent)
	, m_pParent(parent)
	, m_type(type)
{
	initMenu();
	//��Splitter���뵽Layout�У�����ʹ�������������
	QVBoxLayout* pLayout = new QVBoxLayout();
	pLayout->setMargin(0);		//���Ĵ˴�������Panel�ı߿��С
	m_pSplitter = new QSplitter(Qt::Vertical,this);
	m_pSplitter->setHandleWidth(1);
	m_pSplitter->setFrameShadow(QFrame::Sunken);
	m_pSplitter->setOrientation(Qt::Horizontal);
	pLayout->addWidget(m_pSplitter);
	setLayout(pLayout);

	if(m_pParent == 0 && m_type==Basic)
	{
		//ȷ�����пɲ������Ĵ��ڶ����и����ڵ�
		m_pSplitter->addWidget(new CBaseWidget(this));
	}
}


CBaseWidget::~CBaseWidget(void)
{
	delete m_pMenu;
	delete m_pSplitter;
}

void CBaseWidget::initMenu()
{
	m_pMenu = new QMenu(tr("���ư���˵�"));
	{
		//���ð�������
		QMenu* pMenuType = m_pMenu->addMenu(tr("���ð�������"));
		pMenuType->addAction(tr("��������"),this,SLOT(onSetNormalWidget()));
		pMenuType->addAction(tr("K��ͼ"),this,SLOT(onSetKLineWidget()));
		m_pMenu->addSeparator();
	}

	{
		//���ò��뷽ʽ
		m_pMenu->addAction(tr("�����"),this,SLOT(onLeftInsert()));
		m_pMenu->addAction(tr("�Ҳ���"),this,SLOT(onRightInsert()));
		m_pMenu->addAction(tr("�ϲ���"),this,SLOT(onTopInsert()));
		m_pMenu->addAction(tr("�²���"),this,SLOT(onBottomInsert()));
		m_pMenu->addSeparator();
	}
}

void CBaseWidget::realignSplitter()
{
	QList<int> listSize;
	listSize.push_back(-1);
	m_pSplitter->setSizes(listSize);
}

void CBaseWidget::resetParent( CBaseWidget* parent )
{
	m_pParent = parent;
	QWidget::setParent(parent);
}

int CBaseWidget::getWidgetIndex( CBaseWidget* widget ) const
{
	return m_pSplitter->indexOf(widget);
}

void CBaseWidget::replaceWidget( int index, CBaseWidget* widget )
{
	CBaseWidget* pWidgetPre = reinterpret_cast<CBaseWidget*>(m_pSplitter->widget(index));
	pWidgetPre->deleteLater();
	m_pSplitter->insertWidget(index,widget);
}

bool CBaseWidget::loadPanelInfo( const QDomElement& elePanel )
{

	return true;
}

bool CBaseWidget::savePanelInfo( QDomDocument& doc,QDomElement& elePanel )
{

	return true;
}

void CBaseWidget::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	p.fillRect(this->rect(),QColor(0,0,0));
	p.setPen(QColor(255,255,255));
	p.drawText(this->rect(),Qt::AlignCenter,QString(tr("�����Ҽ�ѡ���������")));
}

void CBaseWidget::contextMenuEvent( QContextMenuEvent* e )
{
	if(m_pParent)
	{
		QMenu* pMenuCustom = getCustomMenu();
		if(pMenuCustom)
			pMenuCustom->exec(e->globalPos());
		else
			m_pMenu->exec(e->globalPos());
	}
	return e->accept();
}

void CBaseWidget::onLeftInsert()
{
	//�����
	QSplitter* pSplitterParent = m_pParent->getSplitter();		//�����ڵ�Splitterָ��
	int iIndex = pSplitterParent->indexOf(this);				//���ڵ�������
	if(iIndex<0)
		return;

	if((pSplitterParent->orientation() == Qt::Horizontal) ||
		(pSplitterParent->count() == 1))
	{
		//ˮƽ�����Splitter ���� ��ֱ���򣬵�ֻ��һ���Ӵ���
		pSplitterParent->setOrientation(Qt::Horizontal);
		pSplitterParent->insertWidget(iIndex,new CBaseWidget(m_pParent));
	}
	else														//��ֱ����ģ����Ѿ������˴�ֱ����
	{
		CBaseWidget* pWidgetParent = new CBaseWidget(m_pParent);
		pSplitterParent->insertWidget(iIndex,pWidgetParent);
		resetParent(pWidgetParent);

		pWidgetParent->getSplitter()->setOrientation(Qt::Horizontal);
		pWidgetParent->getSplitter()->addWidget(new CBaseWidget(pWidgetParent));
		pWidgetParent->getSplitter()->addWidget(this);

		pWidgetParent->realignSplitter();
	}
}

void CBaseWidget::onRightInsert()
{
	//�Ҳ���
	QSplitter* pSplitterParent = m_pParent->getSplitter();		//�����ڵ�Splitterָ��
	int iIndex = pSplitterParent->indexOf(this);				//���ڵ�������
	if(iIndex<0)
		return;

	if((pSplitterParent->orientation() == Qt::Horizontal) ||
		(pSplitterParent->count() == 1))
	{
		//ˮƽ�����Splitter ���� ��ֱ���򣬵�ֻ��һ���Ӵ���
		pSplitterParent->setOrientation(Qt::Horizontal);
		pSplitterParent->insertWidget(iIndex+1,new CBaseWidget(m_pParent));
	}
	else														//��ֱ����ģ����Ѿ������˴�ֱ����
	{
		CBaseWidget* pWidgetParent = new CBaseWidget(m_pParent);
		pSplitterParent->insertWidget(iIndex,pWidgetParent);
		resetParent(pWidgetParent);

		pWidgetParent->getSplitter()->setOrientation(Qt::Horizontal);
		pWidgetParent->getSplitter()->addWidget(this);
		pWidgetParent->getSplitter()->addWidget(new CBaseWidget(pWidgetParent));

		pWidgetParent->realignSplitter();
	}
}

void CBaseWidget::onTopInsert()
{
	//�ϲ���
	QSplitter* pSplitterParent = m_pParent->getSplitter();		//�����ڵ�Splitterָ��
	int iIndex = pSplitterParent->indexOf(this);				//���ڵ�������
	if(iIndex<0)
		return;

	if((pSplitterParent->orientation() == Qt::Vertical) ||
		(pSplitterParent->count() == 1))
	{
		//ˮƽ�����Splitter ���� ��ֱ���򣬵�ֻ��һ���Ӵ���
		pSplitterParent->setOrientation(Qt::Vertical);
		pSplitterParent->insertWidget(iIndex,new CBaseWidget(m_pParent));
	}
	else														//��ֱ����ģ����Ѿ������˴�ֱ����
	{
		CBaseWidget* pWidgetParent = new CBaseWidget(m_pParent);
		pSplitterParent->insertWidget(iIndex,pWidgetParent);
		resetParent(pWidgetParent);

		pWidgetParent->getSplitter()->setOrientation(Qt::Vertical);
		pWidgetParent->getSplitter()->addWidget(new CBaseWidget(pWidgetParent));
		pWidgetParent->getSplitter()->addWidget(this);

		pWidgetParent->realignSplitter();
	}
}

void CBaseWidget::onBottomInsert()
{
	//�²���
	QSplitter* pSplitterParent = m_pParent->getSplitter();		//�����ڵ�Splitterָ��
	int iIndex = pSplitterParent->indexOf(this);				//���ڵ�������
	if(iIndex<0)
		return;

	if((pSplitterParent->orientation() == Qt::Vertical) ||
		(pSplitterParent->count() == 1))
	{
		//ˮƽ�����Splitter ���� ��ֱ���򣬵�ֻ��һ���Ӵ���
		pSplitterParent->setOrientation(Qt::Vertical);
		pSplitterParent->insertWidget(iIndex+1,new CBaseWidget(m_pParent));
	}
	else														//��ֱ����ģ����Ѿ������˴�ֱ����
	{
		CBaseWidget* pWidgetParent = new CBaseWidget(m_pParent);
		pSplitterParent->insertWidget(iIndex,pWidgetParent);
		resetParent(pWidgetParent);

		pWidgetParent->getSplitter()->setOrientation(Qt::Vertical);
		pWidgetParent->getSplitter()->addWidget(this);
		pWidgetParent->getSplitter()->addWidget(new CBaseWidget(pWidgetParent));

		pWidgetParent->realignSplitter();
	}
}

void CBaseWidget::onSetNormalWidget()
{
	int iIndex = m_pParent->getWidgetIndex(this);
	if(iIndex>=0)
	{
		m_pParent->replaceWidget(iIndex,new CBaseWidget(m_pParent));
	}
}

void CBaseWidget::onSetKLineWidget()
{
	int iIndex = m_pParent->getWidgetIndex(this);
	if(iIndex>=0)
	{
		m_pParent->replaceWidget(iIndex,new CKLineWidget(m_pParent));
	}
}
