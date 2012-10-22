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
				/*
				if(itemData->fNewPrice<=0.0 || itemData->fLastClose<=0.0)
					return QString();
				return QString("%1%").arg((itemData->fNewPrice-itemData->fLastClose)*100.0/itemData->fLastClose,0,'f',2);
				*/
				return QString();
			}
			break;
		case 4:
			{
				/*���ȼ��㣺
					���ȣ��ֳɽ�����/����ȥ5��ƽ��ÿ���ӳɽ���*�����ۼƿ���ʱ�䣨�֣��� 
					�����ȴ���1ʱ��˵������ÿ���ӵ�ƽ���ɽ���Ҫ���ڹ�ȥ5�յ�ƽ����ֵ�����ױȹ�ȥ5�ջ𱬣�
					��������С��1ʱ��˵�����ڵĳɽ��Ȳ��Ϲ�ȥ5�յ�ƽ��ˮƽ��
				*/
/*				if(itemData->mapHistorys.size()<5)
					return QVariant();

				//�ж����µ������Ƿ��ǽ��쿪�к������
				time_t tmSeconds = CDataEngine::getOpenSeconds(itemData->tmTime);
				if(tmSeconds<1)
					return QVariant();

				time_t* pLast5Day = CDataEngine::getLast5DayTime();
				float fVolume5 = 0.0;
				for(int i=0;i<5;++i)
				{
					if(!itemData->mapHistorys.contains(pLast5Day[i]))
						return QVariant();
					fVolume5 = (fVolume5 + itemData->mapHistorys.value(pLast5Day[i]).fVolume);
				}

				return (itemData->fVolume)/((fVolume5/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60));*/
				return QString();
			}
			break;
		case 5:
			{
				/*�����ʣ��ֲ
					������=ĳһ��ʱ���ڵĳɽ���/�����ܹ���*100%
					�����й����ɽ���/��ͨ�ܹ���*100%��
				*/
				return QVariant()/*itemData->fVolume/*/;
			}
			break;
		case 6:
			{
				//ǰ��
//				return QString("%1").arg(itemData->fLastClose,0,'f',2);
				return QString();
			}
			break;
		case 7:
			{
				//��
//				return QString("%1").arg(itemData->fOpen,0,'f',2);
				return QString();
			}
			break;
		case 8:
			{
				//���
//				return QString("%1").arg(itemData->fHigh,0,'f',2);
				return QString();
			}
			break;
		case 9:
			{
				//���
//				return QString("%1").arg(itemData->fLow,0,'f',2);;
				return QString();
			}
			break;
		case 10:
			{
				//����
//				return QString("%1").arg(itemData->fNewPrice,0,'f',2);;
				return QString();
			}
			break;
		case 11:
			{
				//����
//				return QString("%1").arg(itemData->fVolume,0,'f',0);
				return QString();
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