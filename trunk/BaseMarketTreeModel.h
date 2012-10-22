/****************************************************************************
**
**
****************************************************************************/

#ifndef BASE_MARKET_TREEMODEL_H
#define BASE_MARKET_TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QtCore>
#include "DataEngine.h"

class CBaseMarketTreeModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CBaseMarketTreeModel(WORD wMarket,QObject *parent = 0);
    ~CBaseMarketTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

public slots:
	void clearReports();
	bool appendReport(qRcvReportData* data);
	void updateBaseMarket(const QString& qsCode);
//	void resetReports();					//���¸������е�����

private:
	WORD m_wMarket;							//��Model��ʾ�Ĺ�Ʊ�г�����
	QStringList m_listHeader;
	QList<qRcvReportData*> m_listItems;
	QMap<QString,int> m_mapTable;			//��Ʊ�����index���ձ�������ٲ���
};

#endif	//BASE_MARKET_TREEMODEL_H
