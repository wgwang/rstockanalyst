/****************************************************************************
**
**
****************************************************************************/

#include <QtCore>
#include <QtGui>
#include "BaseMarketTreeModel.h"
#include "DataEngine.h"

CBaseMarketTreeModel::CBaseMarketTreeModel( WORD wMarket,QObject *parent /*= 0*/ )
    : QAbstractTableModel(parent)
	, m_wMarket(wMarket)
{
    QList<QVariant> rootData;
    m_listHeader << tr("����") << tr("����") << tr("����")
		<< tr("�Ƿ�") << tr("����") << tr("�����ʣ��ֲ") << tr("ǰ��") << tr("��")
		<< tr("���") << tr("���") << tr("����") << tr("����") << tr("�ܶ�ֲ֣�")
		<< tr("����") << tr("����") << tr("�ǵ�") << tr("���") << tr("����") << tr("��ӯ��")
		<< tr("��ͨ��ֵ") << tr("����") << tr("����") << tr("ί����") << tr("ί����")
		<< tr("ί��") << tr("ί��");
}


CBaseMarketTreeModel::~CBaseMarketTreeModel()
{

}


int CBaseMarketTreeModel::columnCount(const QModelIndex &parent) const
{
	if(parent.isValid())
		return 0;

	return m_listHeader.size();
}


QVariant CBaseMarketTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

	if(role == Qt::DisplayRole)
	{
		CStockInfoItem* itemData = m_listItems.at(index.row());

		switch(index.column())
		{
		case 0:
			{
				//������
				return index.row()+1;
			}
			break;
		case 1:
			{
				//��Ʊ����
				return itemData->getCode();
			}
			break;
		case 2:
			{
				//��Ʊ����
				return itemData->getName();
			}
			break;
		case 3:
			{
				//�Ƿ�
				return itemData->getIncrease();
			}
			break;
		case 4:
			{
				//����
				return itemData->getVolumeRatio();
			}
			break;
		case 5:
			{
				//������
				return itemData->getTurnRatio();
			}
			break;
		case 6:
			{
				//ǰ��
				return itemData->getLastClose();
			}
			break;
		case 7:
			{
				//��
				return itemData->getOpenPrice();
			}
			break;
		case 8:
			{
				//���
				return itemData->getHighPrice();
			}
			break;
		case 9:
			{
				//���
				return itemData->getLowPrice();
			}
			break;
		case 10:
			{
				//����
				return itemData->getNewPrice();
			}
			break;
		case 11:
			{
				//����
				return itemData->getTotalVolume();
			}
			break;
		case 12:
			{
				//�ܶ�ֲ֣�
				return itemData->getTotalAmount();
			}
			break;
		case 13:
			{
				//����
				return itemData->getNowVolume();
			}
			break;
		case 14:
			{
				//����
				return itemData->getIncSpeed();
			}
			break;
		case 15:
			{
				//�ǵ�
				return itemData->getPriceFluctuate();
			}
			break;
		case 16:
			{
				//���
				return itemData->getAmplitude();
			}
			break;
		case 17:
			{
				//����
				return itemData->getAvePrice();
			}
			break;
		case 18:
			{
				//��ӯ��
				return itemData->getPERatio();
			}
			break;
		case 19:
			{
				//��ͨ��ֵ
				return itemData->getLTSZ();
			}
			break;
		case 20:
			{
				//����
				return itemData->getSellVOL();
			}
			break;
		case 21:
			{
				//����
				return itemData->getBuyVOL();
			}
			break;
		case 22:
			{
				//ί����
				return itemData->getBIDVOL();
			}
			break;
		case 23:
			{
				//ί����
				return itemData->getASKVOL();
			}
			break;
		case 24:
			{
				//ί��
				return itemData->getCommRatio();
			}
			break;
		case 25:
			{
				//ί��
				return itemData->getCommSent();
			}
			break;
		default:
			{
				return QString("NULL");
			}
			break;
		}
	}
	else if(role == Qt::UserRole)
	{
		CStockInfoItem* itemData = m_listItems.at(index.row());
		return QVariant(reinterpret_cast<unsigned int>(itemData));
	}
	else if(role == Qt::TextAlignmentRole)
	{
		int iColumn = index.column();
		if(iColumn==1||iColumn==2)
			return Qt::AlignCenter;
		return static_cast<int>(Qt::AlignRight|Qt::AlignVCenter);
	}
	else if(role == Qt::TextColorRole)
	{
		return QColor(255,255,0);
	}

	return QVariant();
}


QVariant CBaseMarketTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return m_listHeader[section];
	else if(role == Qt::TextAlignmentRole)
	{
		return Qt::AlignCenter;
	}
    return QVariant();
}

int CBaseMarketTreeModel::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid())
		return 0;

    return m_listItems.size();
}

int CBaseMarketTreeModel::appendStockItem( CStockInfoItem* pItem )
{
	//�Ƿ�Ϊ��Model��ʾ���г� ���� �ж��Ƿ�Ϊ�ظ����
	beginInsertRows (QModelIndex(),m_listItems.size(),m_listItems.size());
	m_listItems.append(pItem);
	m_mapTable[pItem->getCode()] = m_listItems.size()-1;
	endInsertRows();

	return m_listItems.size()-1;
}

void CBaseMarketTreeModel::clearReports()
{
	beginRemoveRows(QModelIndex(),0,m_listItems.size()-1);
	m_listItems.clear();
	m_mapTable.clear();
	endRemoveRows();
}

void CBaseMarketTreeModel::updateStockItem( const QString& qsCode )
{
	if(m_wMarket == SZ_MARKET_EX && qsCode.at(0)!='3')
		return;
	if(m_wMarket == SH_MARKET_EX && qsCode.at(0)!='6')
		return;

	CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
	if((!pItem)||(pItem->getMarket()!=m_wMarket))
		return;

	int iIndex = 0;
	if(!m_mapTable.contains(pItem->getCode()))
		iIndex = appendStockItem(pItem);
	else
		iIndex = m_mapTable[qsCode];

	emit dataChanged(createIndex(iIndex,0),createIndex(iIndex,m_listHeader.count()));
}

/*
void CBaseMarketTreeModel::resetReports()
{
	QList<qRcvReportData*> listReports = CDataEngine::getDataEngine()->getBaseMarket();
	foreach(qRcvReportData* pReport,listReports)
	{
		if(pReport->wMarket==m_wMarket)
		{
			appendReport(pReport);
		}
	}

	reset();
}
*/