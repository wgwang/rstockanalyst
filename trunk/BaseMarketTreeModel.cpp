/****************************************************************************
**
**
****************************************************************************/

#include <QtCore>
#include <QtGui>
#include "BaseMarketTreeModel.h"
#include "DataEngine.h"

CBaseMarketTreeModel::CBaseMarketTreeModel(QObject *parent)
    : QAbstractTableModel(parent)
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

    if (role != Qt::DisplayRole)
        return QVariant();

	if(index.row()>m_listItems.size())
		return QVariant();

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
			//����
			return QString(tr("�ֳɽ�����/��(��ȥ5��������ƽ��ÿ���ӳɽ���)�������ۼƿ��� ʱ��(��)"));
		}
		break;
	case 5:
		{
			//�����ʣ��ֲ
			return QVariant();
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

void CBaseMarketTreeModel::appendReport( qRcvReportData* data )
{
	beginInsertRows (QModelIndex(),m_listItems.size(),m_listItems.size());
	m_listItems.append(data);
	endInsertRows();
}

void CBaseMarketTreeModel::clearReports()
{
	beginRemoveRows(QModelIndex(),0,m_listItems.size()-1);
	m_listItems.clear();
	endRemoveRows();
}
