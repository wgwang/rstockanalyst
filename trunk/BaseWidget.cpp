/************************************************************************/
/* �ļ����ƣ�BaseWidget.cpp
/* ����ʱ�䣺2012-11-16 10:59
/*
/* ��    �������л��ƴ��ڵĻ��֧࣬�ֲ�ֵȲ���
/************************************************************************/
#include "StdAfx.h"
#include "BaseWidget.h"
#include "KLineWidget.h"
#include "MarketTrendWidget.h"
#include "ColorBlockWidget.h"
#include "StockInfoWidget.h"

CBaseWidget* CBaseWidget::createBaseWidget( CBaseWidget* parent/*=0*/, WidgetType type/*=Basic*/ )
{
	switch(type)
	{
	case Basic:				//����ͼ
		return new CBaseWidget(parent);
		break;
	case KLine:					//K��ͼ
		return new CKLineWidget(parent);
		break;
	case MarketTrend:			//�г�����ͼ
		return new CMarketTrendWidget(parent);
		break;
	case ColorBlock:
		return new CColorBlockWidget(parent);
		break;
	case StockInfo:
		return new CStockInfoWidget(parent);
		break;
	}

	return new CBaseWidget(parent);
}

CBaseWidget::CBaseWidget( CBaseWidget* parent /*= 0*/, WidgetType type /*= Basic*/ )
	: QWidget(parent)
	, m_pParent(parent)
	, m_type(type)
{
	setMouseTracking(true);
	initMenu();
	//��Splitter���뵽Layout�У�����ʹ�������������
	QVBoxLayout* pLayout = new QVBoxLayout();
	pLayout->setMargin(0);		//���Ĵ˴�������Panel�ı߿��С
	m_pSplitter = new QSplitter(Qt::Vertical,this);
	m_pSplitter->setHandleWidth(1);
	m_pSplitter->setFrameShadow(QFrame::Sunken);
	m_pSplitter->setOrientation(Qt::Horizontal);
	m_pSplitter->setMouseTracking(true);
	pLayout->addWidget(m_pSplitter);
	setLayout(pLayout);

	if(m_pParent == 0 && m_type==Basic)
	{
		//ȷ�����пɲ������Ĵ��ڶ����и����ڵ�
		m_pSplitter->addWidget(new CBaseWidget(this));
	}

	setFocusPolicy(Qt::WheelFocus);
}


CBaseWidget::~CBaseWidget(void)
{
	delete m_pActRelate;
	delete m_pMenu;
	delete m_pSplitter;
}

void CBaseWidget::initMenu()
{
	m_pMenu = new QMenu(tr("���ư���˵�"));
	{
		//���ð�������
		QMenu* pMenuType = m_pMenu->addMenu(tr("���ð�������"));
		{
			pMenuType->addAction(tr("��������"),this,SLOT(onResetWidget()))->setData(CBaseWidget::Basic);
			pMenuType->addAction(tr("K��ͼ"),this,SLOT(onResetWidget()))->setData(CBaseWidget::KLine);
			pMenuType->addAction(tr("�г�����ͼ"),this,SLOT(onResetWidget()))->setData(CBaseWidget::MarketTrend);
			pMenuType->addAction(tr("ɫ��ͼ"),this,SLOT(onResetWidget()))->setData(CBaseWidget::ColorBlock);
			pMenuType->addAction(tr("������Ϣ"),this,SLOT(onResetWidget()))->setData(CBaseWidget::StockInfo);
		}
		m_pMenu->addSeparator();
	}

	{
		//���ò��뷽ʽ
		m_pMenu->addAction(tr("�����"),this,SLOT(onLeftInsert()));
		m_pMenu->addAction(tr("�Ҳ���"),this,SLOT(onRightInsert()));
		m_pMenu->addAction(tr("�ϲ���"),this,SLOT(onTopInsert()));
		m_pMenu->addAction(tr("�²���"),this,SLOT(onBottomInsert()));
		m_pMenu->addSeparator();
		m_pMenu->addAction(tr("ɾ���ô���"),this,SLOT(deleteLater()));
		m_pMenu->addSeparator();
		m_pActRelate = new QAction(tr("���������ڹ���"),m_pMenu);
		m_pMenu->addAction(m_pActRelate);
		m_pActRelate->setCheckable(true);
		m_pActRelate->setChecked(true);
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

QList<CBaseWidget*> CBaseWidget::getChildren()
{
	QList<CBaseWidget*> list;
	for(int i=0;i<m_pSplitter->count();++i)
	{
		list.push_back(reinterpret_cast<CBaseWidget*>(m_pSplitter->widget(i)));
	}
	return list;
}

void CBaseWidget::clearChildren()
{
	while(m_pSplitter->count()>0)
	{
		CBaseWidget* pPanel = static_cast<CBaseWidget*>(m_pSplitter->widget(0));
		if(pPanel)
			delete pPanel;
	}
}

int CBaseWidget::getSize()
{
	CBaseWidget* pParent = getParent();
	if(!pParent)
		return 100;
	QSplitter* pParentSplitter = pParent->getSplitter();
	if(!pParentSplitter)
		return 100;

	int iTotal = 0;
	int iSize = 0;
	QList<int> sizes = pParentSplitter->sizes();
	if(sizes.size()<2)
		return 100;

	for(int i=0;i<sizes.size();++i)
	{
		if(pParentSplitter->widget(i)==this)
			iSize = sizes[i];
		iTotal = iTotal + sizes[i];
	}

	return (iSize*100)/iTotal;
}

int CBaseWidget::getWidgetIndex( CBaseWidget* widget ) const
{
	return m_pSplitter->indexOf(widget);
}

void CBaseWidget::replaceWidget( int index, CBaseWidget* widget )
{
	QList<int> sizes = m_pSplitter->sizes();
	CBaseWidget* pWidgetPre = reinterpret_cast<CBaseWidget*>(m_pSplitter->widget(index));
	pWidgetPre->setParent(NULL);
	pWidgetPre->deleteLater();
	m_pSplitter->insertWidget(index,widget);
	QApplication::flush();
	m_pSplitter->setSizes(sizes);
}

bool CBaseWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	//��ȡWidget��Ϣ
	if(!eleWidget.isElement())
		return false;

	//��ȡ����
	QDomElement eleName = eleWidget.firstChildElement("name");
	if(eleName.isElement())
		setWidgetName(eleName.text());

	//��ȡ�ؼ������з�ʽ
	QDomElement eleAlign = eleWidget.firstChildElement("align");
	if(eleAlign.isElement())
		m_pSplitter->setOrientation(static_cast<Qt::Orientation>(eleAlign.text().toInt()));

	//��ȡ������ʽ
	QDomElement eleRelate = eleWidget.firstChildElement("relate");
	if(eleRelate.isElement())
		m_pActRelate->setChecked(eleRelate.text().toInt());

	QList<int> sizes;
	QDomElement eleChild = eleWidget.firstChildElement("widget");
	while(eleChild.isElement())
	{
		QDomElement eleChildSize = eleChild.firstChildElement("size");
		if(eleChildSize.isElement())
			sizes.push_back(eleChildSize.text().toInt());

		QDomElement eleChildType = eleChild.firstChildElement("type");
		WidgetType typeChild = Basic;
		if(eleChildType.isElement())
			typeChild = static_cast<WidgetType>(eleChildType.text().toInt());

		CBaseWidget* pWidget = createBaseWidget(this,typeChild);
		m_pSplitter->addWidget(pWidget);
		pWidget->loadPanelInfo(eleChild);

		eleChild = eleChild.nextSiblingElement("widget");
	}
	m_pSplitter->setSizes(sizes);
	return true;
}

bool CBaseWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	//����Panel��Ϣ
	if(!eleWidget.isElement())
		return false;

	//��ȡ����
	QDomElement eleName = doc.createElement("name");
	eleName.appendChild(doc.createTextNode(getWidgetName()));
	eleWidget.appendChild(eleName);

	//��ȡ�ؼ�����
	QDomElement eleType = doc.createElement("type");
	eleType.appendChild(doc.createTextNode(QString("%1").arg(m_type)));
	eleWidget.appendChild(eleType);

	//��ȡ������ʽ
	QDomElement eleRelate = doc.createElement("relate");
	eleRelate.appendChild(doc.createTextNode(QString("%1").arg(isRelate())));
	eleWidget.appendChild(eleRelate);

	//��ȡ�ؼ���Splitter�����з�ʽ
	QDomElement eleAlgin = doc.createElement("align");
	eleAlgin.appendChild(doc.createTextNode(QString("%1").arg(m_pSplitter->orientation())));
	eleWidget.appendChild(eleAlgin);

	//��ȡ�ؼ��ڸ������еĴ�С
	QDomElement eleSize = doc.createElement("size");
	eleSize.appendChild(doc.createTextNode(QString("%1").arg(getSize())));
	eleWidget.appendChild(eleSize);

	//������Panel
	QList<CBaseWidget*> listChildren = getChildren();
	foreach(CBaseWidget* pChild,listChildren)
	{
		QDomElement eleChild = doc.createElement("widget");
		eleWidget.appendChild(eleChild);
		pChild->savePanelInfo(doc,eleChild);
	}

	return true;
}

void CBaseWidget::setStockCode( const QString& code )
{
	QList<CBaseWidget*> children = getChildren();
	foreach(CBaseWidget* p,children)
	{
		if(p->isRelate())
			p->setStockCode(code);
	}
}

void CBaseWidget::setBlock( const QString& block )
{
	QList<CBaseWidget*> children = getChildren();
	foreach(CBaseWidget* p,children)
	{
		if(p->isRelate())
			p->setBlock(block);
	}
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
		m_pParent->realignSplitter();
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
		m_pParent->realignSplitter();
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
		m_pParent->realignSplitter();
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
		m_pParent->realignSplitter();
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

void CBaseWidget::onResetWidget()
{
	QAction* pAct = reinterpret_cast<QAction*>(sender());

	WidgetType type = static_cast<WidgetType>(pAct->data().toInt());	//��ȡ��������
	int iIndex = m_pParent->getWidgetIndex(this);						//��ȡ��ǰ�������ڵ�����
	if(iIndex>=0)
	{
		m_pParent->replaceWidget(iIndex,createBaseWidget(m_pParent,type));	//�滻����
	}
}
