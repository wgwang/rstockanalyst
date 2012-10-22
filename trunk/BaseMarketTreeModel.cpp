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
				return itemData->getVolumeRatio();
			}
			break;
		case 5:
			{
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
				return QVariant();
			}
			break;
		case 13:
			{
				//����
//				return QString("%1").arg(itemData->fSellPrice[0],0,'f',2);
				return QString();
			}
			break;
		case 14:
			{
				//����
				return QVariant();
			}
			break;
		case 15:
			{
				//�ǵ�
				return QVariant();
			}
			break;
		case 16:
			{
				//���
				return QVariant();
			}
			break;
		case 17:
			{
				//����
				return QVariant();
			}
			break;
		case 18:
			{
				//��ӯ��
				return QVariant();
			}
			break;
		case 19:
			{
				//��ͨ��ֵ
				return QVariant();
			}
			break;
		case 20:
			{
				//����
				return QVariant();
			}
			break;
		case 21:
			{
				//����
				return QVariant();
			}
			break;
		case 22:
			{
				//ί����
				return QVariant();
			}
			break;
		case 23:
			{
				//ί����
				return QVariant();
			}
			break;
		case 24:
			{
				//ί��
				return QVariant();
			}
			break;
		case 25:
			{
				//ί��
				return QVariant();
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

	return QVariant();
}


QVariant CBaseMarketTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return m_listHeader[section];

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