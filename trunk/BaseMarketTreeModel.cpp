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
		qRcvReportData* itemData = m_listItems.at(index.row());

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
				return itemData->qsCode;
			}
			break;
		case 2:
			{
				//��Ʊ����
				return itemData->qsName;
			}
			break;
		case 3:
			{
				//�Ƿ�
				if(itemData->fNewPrice<=0.0 || itemData->fLastClose<=0.0)
					return QString();
				return QString("%1%").arg((itemData->fNewPrice-itemData->fLastClose)*100.0/itemData->fLastClose,0,'f',2);
			}
			break;
		case 4:
			{
				/*���ȼ��㣺
					���ȣ��ֳɽ�����/����ȥ5��ƽ��ÿ���ӳɽ���*�����ۼƿ���ʱ�䣨�֣��� 
					�����ȴ���1ʱ��˵������ÿ���ӵ�ƽ���ɽ���Ҫ���ڹ�ȥ5�յ�ƽ����ֵ�����ױȹ�ȥ5�ջ𱬣�
					��������С��1ʱ��˵�����ڵĳɽ��Ȳ��Ϲ�ȥ5�յ�ƽ��ˮƽ��
				*/
				if(itemData->mapHistorys.size()<5)
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

				return (itemData->fVolume)/((fVolume5/((CDataEngine::getOpenSeconds()/60)*5))*(tmSeconds/60));
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
				return QString("%1").arg(itemData->fLastClose,0,'f',2);
			}
			break;
		case 7:
			{
				//��
				return QString("%1").arg(itemData->fOpen,0,'f',2);
			}
			break;
		case 8:
			{
				//���
				return QString("%1").arg(itemData->fHigh,0,'f',2);
			}
			break;
		case 9:
			{
				//���
				return QString("%1").arg(itemData->fLow,0,'f',2);;
			}
			break;
		case 10:
			{
				//����
				return QString("%1").arg(itemData->fNewPrice,0,'f',2);;
			}
			break;
		case 11:
			{
				//����
				return QString("%1").arg(itemData->fVolume,0,'f',0);
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
				return QString("%1").arg(itemData->fSellPrice[0],0,'f',2);
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
		qRcvReportData* itemData = m_listItems.at(index.row());
		return QVariant::fromValue(itemData);
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

bool CBaseMarketTreeModel::appendReport( qRcvReportData* data )
{
	//�Ƿ�Ϊ��Model��ʾ���г� ���� �ж��Ƿ�Ϊ�ظ����
	if((data->wMarket != m_wMarket)||(m_mapTable.contains(data->qsCode)))
		return false;

	beginInsertRows (QModelIndex(),m_listItems.size(),m_listItems.size());
	m_listItems.append(data);
	m_mapTable[data->qsCode] = m_listItems.size()-1;
	endInsertRows();

	return true;
}

void CBaseMarketTreeModel::clearReports()
{
	beginRemoveRows(QModelIndex(),0,m_listItems.size()-1);
	m_listItems.clear();
	m_mapTable.clear();
	endRemoveRows();
}

void CBaseMarketTreeModel::updateBaseMarket( const QString& qsCode )
{
	qRcvReportData* pReport = CDataEngine::getDataEngine()->getBaseMarket(qsCode);
	if((!pReport)||(pReport->wMarket!=m_wMarket))
		return;

	appendReport(pReport);

	int iIndex = m_mapTable[qsCode];
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