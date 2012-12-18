#include "StdAfx.h"
#include "DataEngine.h"
#include "STKDRV.h"
#include <time.h>
#include <QApplication>
#include <QtXml>

CDataEngine* CDataEngine::m_pDataEngine = NULL;

time_t CDataEngine::m_tmCurrentDay = NULL;
time_t* CDataEngine::m_tmLast5Day = new time_t[5];

void CDataEngine::importData()
{
	QString qsDir = qApp->applicationDirPath();
	{
		//����F10 ����
		/*����б������ݣ��ȵ��뱾������*/
		QString qsBaseInfo = qsDir+"/data/baseinfo.rsqfin";
		if(QFile::exists(qsBaseInfo))
		{
			qDebug()<<"Import F10 data...";
			int iCount = importBaseInfo(qsBaseInfo);
			if(iCount<1)
			{
				QFile::remove(qsBaseInfo);
				qDebug()<<"Import F10 data from "<<qsBaseInfo<<" error!";
			}
			else
				qDebug()<<iCount<<" F10 data had been imported.";
		}

		//�����������F10���ݣ������ٴε���������F10 ���ݡ�
		QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\StockDrv",QSettings::NativeFormat);
		QString qsF10File = QFileInfo(settings.value("driver").toString()).absolutePath() + "/��������.fin";
		if(QFile::exists(qsF10File))
		{
			qDebug()<<"Import F10 Data from "<<qsF10File;
			CDataEngine::importBaseInfoFromFinFile(qsF10File);	
		}
	}
	{
		//�������һ���Reports����
		QString qsReportFile = QString("%1/data/reports").arg(qsDir);
		qDebug()<<"Import reports data from "<<qsReportFile;
		int iCount = importReportsInfo(qsReportFile);
		if(iCount<1)
		{
			QFile::remove(qsReportFile);
			qDebug()<<"Import reports data from "<<qsReportFile<<" error!";
		}
		else
			qDebug()<<iCount<<" reports data had been imported.";
	}
	{
		//���뵱��ķ�������
		QString qsMinFile = QString("%1/data/minutes/%2").arg(qsDir).arg(QDate::currentDate().toString("yyyyMMdd"));
		qDebug()<<"Import minutes data from "<<qsMinFile;
		int iCount = importMinutesData(qsMinFile);
		if(iCount<1)
		{
			QFile::remove(qsMinFile);
			qDebug()<<"Import minutes data from "<<qsMinFile<<" error!";
		}
		else
			qDebug()<<iCount<<" minutes data had been imported.";
	}
	{
		//���뵱��ķֱ�����
		QString qsFenBiPath = QString("%1/data/FenBi/%2/").arg(qsDir).arg(QDate::currentDate().toString("yyyyMMdd"));

		qDebug()<<"Import FenBi data from "<<qsFenBiPath;
		int iCount = importFenBisData(qsFenBiPath);
		if(iCount<1)
		{
			qDebug()<<"Import FenBi data from "<<qsFenBiPath<<" error!";
		}
		else
			qDebug()<<iCount<<" FenBi data had been imported.";
	}
}

void CDataEngine::exportData()
{
	QString qsDir = qApp->applicationDirPath();
	QString qsBaseInfo = qsDir+"/data/baseinfo.rsqfin";
	{
		QDir().mkpath(qsDir+"/data");
		qDebug()<<"Export F10 data...";
		int iCount = exportBaseInfo(qsBaseInfo);
		qDebug()<<iCount<<" F10 data had been exported.";
	}

	{
		//�������һ���Reports����
		QString qsReportDir = qsDir + "/data";
		if(!QDir().exists(qsReportDir))
			QDir().mkpath(qsReportDir);
		QString qsReportFile = QString("%1/reports").arg(qsReportDir);
		qDebug()<<"Export reports data to "<<qsReportFile;
		int iCount = exportReportsInfo(qsReportFile);
		qDebug()<<iCount<<" reports data had been exported.";
	}

	{
		//��������ķ�ʱ����
		QString qsMinDir = QString("%1/data/minutes")
			.arg(qsDir);
		QDir().mkpath(qsMinDir);
		qDebug()<<"Export minutes data to "<<qsMinDir;
		int iCount = exportMinutesData(QString("%1/%2").arg(qsMinDir).arg(QDate::currentDate().toString("yyyyMMdd")));
		qDebug()<<iCount<<" minutes data had been exported.";
	}
}

int CDataEngine::importBaseInfoFromFinFile( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return 0;

	int iFlag;
	file.read((char*)&iFlag,4);
	int iTotal;
	file.read((char*)&iTotal,4);

	int iCout = 0;

	while(true)
	{
		WORD wMarket;
		if(file.read((char*)&wMarket,2)!=2) break;
		if(!file.seek(file.pos()+2)) break;

		char chCode[STKLABEL_LEN];
		if(file.read(chCode,STKLABEL_LEN)!=STKLABEL_LEN) break;


		float fVal[38];
		if(file.read((char*)fVal,sizeof(float)*38)!=(sizeof(float)*38)) break;

		qRcvBaseInfoData d(fVal);
		d.wMarket = wMarket;
		memcpy(d.code,chCode,STKLABEL_LEN);

		QString qsCode = QString::fromLocal8Bit(chCode);

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
		if(pItem)
		{
			pItem->setBaseInfo(d);
		}
		else
		{
			CStockInfoItem* pItem = new CStockInfoItem(d);
			CDataEngine::getDataEngine()->setStockInfoItem(pItem);
		}

		++iCout;
	}

	return iCout;
}

int CDataEngine::importBaseInfo( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return -1;

	QDataStream out(&file);

	int iCount = 0;
	while(true)
	{
		qRcvBaseInfoData baseInfo;
		int iSize = out.readRawData((char*)&baseInfo,sizeof(qRcvBaseInfoData));
		if(iSize!=sizeof(qRcvBaseInfoData))
			break;


		float fLast5Volume = 0.0;
		if(out.readRawData((char*)&fLast5Volume,sizeof(float))!=sizeof(float))
			break;

		QString qsCode = QString::fromLocal8Bit(baseInfo.code);
		if(qsCode.isEmpty())
			return -1;

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(qsCode);
		if(pItem)
		{
			pItem->setBaseInfo(baseInfo);
			pItem->setLast5Volume(fLast5Volume);
		}
		else
		{
			CStockInfoItem* pItem = new CStockInfoItem(baseInfo);
			pItem->setLast5Volume(fLast5Volume);
			CDataEngine::getDataEngine()->setStockInfoItem(pItem);
		}

		++iCount;
	}

	file.close();
	return iCount;
}

int CDataEngine::importReportsInfo( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return -1;

	QDataStream in(&file);

	int iCount = 0;
	while(true)
	{
		qRcvReportData* pReport = new qRcvReportData;
		quint32 _t;
		in>>_t>>pReport->wMarket>>pReport->qsCode>>pReport->qsName;
		pReport->tmTime = _t;

		int iSize = in.readRawData((char*)&pReport->fLastClose,sizeof(float)*27);
		if(iSize!=(sizeof(float)*27))
			break;

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(pReport->qsCode);
		if(pItem==NULL)
		{
			pItem = new CStockInfoItem(pReport->qsCode,pReport->wMarket);
			CDataEngine::getDataEngine()->setStockInfoItem(pItem);
		}
		pItem->setReport(pReport);

		++iCount;
	}

	file.close();
	return iCount;
}

int CDataEngine::importMinutesData( const QString& qsFile )
{
	QFile file(qsFile);
	if(!file.open(QFile::ReadOnly))
		return -1;

	int iCount = 0;
	while(true)
	{
		char chCode[STKLABEL_LEN];
		int iSize;
		if(file.read(chCode,STKLABEL_LEN)!=STKLABEL_LEN)
			break;
		if(file.read((char*)&iSize,sizeof(iSize))!=sizeof(iSize))
			break;

		char* pMinData = new char[iSize];
		if(file.read(pMinData,iSize)!=iSize)
		{
			delete pMinData;
			break;
		}

		CStockInfoItem* pItem = CDataEngine::getDataEngine()->getStockInfoItem(QString::fromLocal8Bit(chCode));
		if(!pItem)
		{
			delete pMinData;
			continue;
		}

		int iIndex = 0;
		QList<qRcvMinuteData*> listMins;
	//	QString qsMin;
		while(iIndex<iSize)
		{
			qRcvMinuteData* pData = new qRcvMinuteData();
			memcpy(pData,&pMinData[iIndex],sizeof(qRcvMinuteData));
			listMins.push_back(pData);
		//	qsMin+= QString("%1\r\n").arg(QDateTime::fromTime_t(pData->tmTime).toString("hh:mm:ss"));

			iIndex += sizeof(qRcvMinuteData);
		}
		pItem->appendMinutes(listMins);
		delete pMinData;

		++iCount;
	}

	file.close();
	return iCount;
}

int CDataEngine::importFenBisData( const QString& qsPath )
{

	return 0;
}


int CDataEngine::exportBaseInfo( const QString& qsFile )
{
	if(QFile::exists(qsFile))
		QFile::remove(qsFile);
	if(QFile::exists(qsFile))
		return -1;

	QFile file(qsFile);
	if(!file.open(QFile::WriteOnly))
		return -1;

	QDataStream out(&file);

	QList<CStockInfoItem*> listItem = CDataEngine::getDataEngine()->getStockInfoList();
	int iCount = 0;

	foreach( CStockInfoItem* pItem, listItem)
	{
		qRcvBaseInfoData* pBaseInfo = pItem->getBaseInfo();

		int iSize = out.writeRawData((char*)pBaseInfo,sizeof(qRcvBaseInfoData));
		if(iSize!=sizeof(qRcvBaseInfoData))
			break;
		float fLast5Volume = pItem->getLast5Volume();
		if(out.writeRawData((char*)&fLast5Volume,sizeof(float))!=sizeof(float))
			break;
		++iCount;
	}

	file.close();
	return iCount;
}

int CDataEngine::exportReportsInfo( const QString& qsFile )
{
	if(QFile::exists(qsFile))
		QFile::remove(qsFile);
	if(QFile::exists(qsFile))
		return -1;

	QFile file(qsFile);
	if(!file.open(QFile::WriteOnly))
		return -1;

	QDataStream out(&file);

	QList<CStockInfoItem*> listItem = CDataEngine::getDataEngine()->getStockInfoList();
	int iCount = 0;

	foreach( CStockInfoItem* pItem, listItem)
	{
		//���浱�����е�Reports
		qRcvReportData* pReport = pItem->getCurrentReport();
		if(pReport&&(!pReport->qsCode.isEmpty()))
		{
			out<<pReport->tmTime<<pReport->wMarket<<pReport->qsCode<<pReport->qsName;
			//ֱ�ӿ���ʣ�������float����
			if(out.writeRawData((char*)&pReport->fLastClose,sizeof(float)*27)!=(sizeof(float)*27))
				break;
			//		int iSize = out.writeRawData((char*)pBaseInfo,sizeof(qRcvBaseInfoData));
			//		if(iSize!=sizeof(qRcvBaseInfoData))
			//			break;
			++iCount;
		}
	}

	file.close();
	return iCount;
}

int CDataEngine::exportMinutesData( const QString& qsFile )
{
	QList<CStockInfoItem*> listItem = CDataEngine::getDataEngine()->getStockInfoList();
	int iCount = 0;

	if(QFile::exists(qsFile))
		QFile::remove(qsFile);
	if(QFile::exists(qsFile))
		return -1;

	QFile file(qsFile);
	if(!file.open(QFile::WriteOnly))
		return -1;

	foreach(CStockInfoItem* pItem,listItem)
	{
		//���浱�����еķ�������
		QList<qRcvMinuteData*> listMins= pItem->getMinuteList();
		int iSize = listMins.size()*sizeof(qRcvMinuteData);
		char* pMinData = new char[iSize];
		for(int i = 0; i<listMins.size(); ++i)
		{
			qRcvMinuteData* pData = listMins[i];
			memcpy(pMinData+i*sizeof(qRcvMinuteData),pData,sizeof(qRcvMinuteData));
		}
		//foreach(qRcvMinuteData* pData,listMins)
		//{
		//	minData.append(pData->tmTime);
		//	minData.append(pData->fPrice);
		//	minData.append(pData->fVolume);
		//	minData.append(pData->fAmount);
		//	minData<<pData->tmTime<<pData->fPrice<<pData->fVolume<<pData->fAmount;
		//	out<<pData->tmTime<<pData->fPrice<<pData->fVolume<<pData->fAmount;
		//}
		char chCode[STKLABEL_LEN];
		memset(chCode,0,STKLABEL_LEN);
		QByteArray arrCode = pItem->getCode().toLocal8Bit();
		memcpy(chCode,arrCode.data(),arrCode.size());
		file.write(chCode,STKLABEL_LEN);
		file.write((char*)&iSize,sizeof(int));

		file.write(pMinData,iSize);
		file.flush();

		delete pMinData;

		++iCount;
	}

	file.close();
	return iCount;
}


bool CDataEngine::isStockOpenDay( time_t tmDay )
{
	QDate tmDate = QDateTime::fromTime_t(tmDay).date();
	int iWeek = tmDate.dayOfWeek();
	if((iWeek == 6)||(iWeek == 7))
		return false;

	return true;
}

time_t* CDataEngine::getLast5DayTime()
{
	time_t tmCur = time(NULL);
	tmCur = tmCur-(tmCur%(3600*24));
	tmCur = tmCur-(3600*R_TIME_ZONE);

	if(tmCur==m_tmCurrentDay)
		return m_tmLast5Day;

	m_tmCurrentDay = tmCur;

	for(int i=0;i<5;++i)
	{
		tmCur = tmCur-(3600*24);
		while(!isStockOpenDay(tmCur))
		{
			tmCur = tmCur-(3600*24);
		}
		m_tmLast5Day[i] = tmCur;
	}
	QDateTime tmDataTime = QDateTime::fromTime_t(tmCur);

	return m_tmLast5Day;
}

time_t CDataEngine::getOpenSeconds()
{
	return 3600*4;
}

time_t CDataEngine::getOpenSeconds( time_t tmTime )
{
	time_t tmEnd2 = m_tmCurrentDay+3600*15;
	if(tmTime>tmEnd2)
		return 3600*4;

	time_t tmBegin2 = m_tmCurrentDay+3600*13;
	if(tmTime>tmBegin2)
		return 3600*2+(tmTime-tmBegin2);

	time_t tmEnd1 = m_tmCurrentDay+1800*23;
	if(tmTime>tmEnd1)
		return 3600*2;

	time_t tmBegin1 = m_tmCurrentDay + 1800*19;
	if(tmTime>tmBegin1)
		return tmTime-tmBegin1;


	return 0;
}



CDataEngine::CDataEngine(void)
{
	getLast5DayTime();

	m_qsHistroyDir = qApp->applicationDirPath()+"/data/history/";
	QDir().mkpath(m_qsHistroyDir);

	m_qsBlocksDir = qApp->applicationDirPath()+"/data/blocks/";
	QDir().mkpath(m_qsBlocksDir);

	m_qsCommonBlocks = qApp->applicationDirPath()+"/config/CommonBlocks.xml";
	initCommonBlocks();

	m_qsNewsDir = qApp->applicationDirPath()+"/data/news/";
	QDir().mkpath(m_qsNewsDir);

	m_qsF10Dir = qApp->applicationDirPath()+"/data/F10/";
	QDir().mkpath(m_qsF10Dir);

	m_qsFenBiDir = qApp->applicationDirPath()+"/data/FenBi/";
	QDir().mkpath(m_qsFenBiDir);
}

CDataEngine::~CDataEngine(void)
{
}


void CDataEngine::initCommonBlocks()
{
	m_listCommonBlocks.clear();

	QDomDocument doc("CommonBlocks");
	QFile file(m_qsCommonBlocks);
	if (!file.open(QIODevice::ReadOnly))
		return;
	if (!doc.setContent(&file)) {
		file.close();
		return;
	}
	file.close();

	QDomElement eleRoot = doc.documentElement();
	if(eleRoot.isNull())
		return;
	QDomElement eleNode = eleRoot.firstChildElement("block");
	while(!eleNode.isNull())
	{
		QString qsName = eleNode.attribute("name");
		if(!(qsName.isEmpty()||isBlockInCommon(qsName)))
		{
			QString qsRegexp = eleNode.text();
			if(!qsRegexp.isEmpty())
			{
				QRegExp r(qsRegexp);
				if(eleNode.hasAttribute("RegType"))
				{
					r.setPatternSyntax(static_cast<QRegExp::PatternSyntax>(eleNode.attribute("RegType").toInt()));
				}
				m_listCommonBlocks.push_back(QPair<QString,QRegExp>(qsName,r));
			}
		}
		eleNode = eleNode.nextSiblingElement("block");
	}
}

QList<QString> CDataEngine::getStockBlocks()
{
	QList<QString> listBlocks;
	QList<QPair<QString,QRegExp>>::iterator iter = m_listCommonBlocks.begin();
	while(iter!=m_listCommonBlocks.end())
	{
		listBlocks.push_back(iter->first);
		++iter;
	}

	QDir dir(m_qsBlocksDir);
	QStringList listEntity = dir.entryList(QDir::Files);
	foreach(const QString& e,listEntity)
	{
		if(!isBlockInCommon(e))
		{
			listBlocks.push_back(e);
		}
	}

	return listBlocks;
}

bool CDataEngine::isHadBlock( const QString& block )
{
	if(isBlockInCommon(block))
		return true;

	return QFile::exists(m_qsBlocksDir+block);
}

QList<CStockInfoItem*> CDataEngine::getStocksByMarket( WORD wMarket )
{
	QList<CStockInfoItem*> listStocks;
	QMap<QString,CStockInfoItem*>::iterator iter = m_mapStockInfos.begin();
	while(iter!=m_mapStockInfos.end())
	{
		if((*iter)->getMarket() == wMarket)
			listStocks.push_back(iter.value());
		++iter;
	}

	return listStocks;
}

QList<CStockInfoItem*> CDataEngine::getStocksByBlock( const QString& block )
{
	QList<CStockInfoItem*> listStocks;
	if(isBlockInCommon(block))
	{
		QRegExp r = getRegexpByBlock(block);
		QMap<QString,CStockInfoItem*>::iterator iter = m_mapStockInfos.begin();
		while(iter!=m_mapStockInfos.end())
		{
			if(r.exactMatch((*iter)->getCode()))
			{
				listStocks.push_back(*iter);
			}
			++iter;
		}
	}
	else
	{
		QFile file(m_qsBlocksDir+block);
		if(file.open(QFile::ReadOnly))
		{
			while(!file.atEnd())
			{
				QString code = file.readLine();
				code = code.trimmed();
				if((!code.isEmpty())&&(m_mapStockInfos.contains(code)))
				{
					listStocks.push_back(m_mapStockInfos[code]);
				}
			}
			file.close();
		}
	}

	return listStocks;
}

bool CDataEngine::appendStocksToBlock( const QString& block,QList<CStockInfoItem*> list )
{
	QStringList listCodes;
	foreach(CStockInfoItem* pItem,list)
		listCodes.push_back(pItem->getCode());

	return appendStocksToBlock(block,listCodes);
}

bool CDataEngine::appendStocksToBlock( const QString& block,QList<QString> list )
{
	if(isBlockInCommon(block))
		return false;
	QFile file(m_qsBlocksDir+block);
	if(!file.open(QFile::Append|QFile::WriteOnly))
		return false;
	foreach(const QString& e,list)
	{
		file.write(QString(e+"\r\n").toAscii());
	}
	file.close();

	return true;
}

bool CDataEngine::removeStocksFromBlock( const QString& block,QList<CStockInfoItem*> list )
{
	QStringList listCodes;
	foreach(CStockInfoItem* pItem,list)
		listCodes.push_back(pItem->getCode());

	return removeStocksFromBlock(block,listCodes);
}

bool CDataEngine::removeStocksFromBlock( const QString& block,QList<QString> list )
{
	if(isBlockInCommon(block))
		return false;

	QMap<QString,QString> mapStocks;
	{
		//��ȡ�ļ��еĹ�Ʊ����
		QFile file(m_qsBlocksDir+block);
		if(!file.open(QFile::ReadOnly))
			return false;

		while(!file.atEnd())
		{
			QString code = file.readLine();
			code = code.trimmed();
			if((!code.isEmpty())&&(m_mapStockInfos.contains(code)))
			{
				mapStocks[code] = code;
			}
		}
		file.close();
	}
	{
		//ɾ����Ӧ�Ĺ�Ʊ����
		foreach(const QString& e,list)
		{
			mapStocks.remove(e);
		}
	}
	{
		//����д��ȥ
		QFile file(m_qsBlocksDir+block);
		if(!file.open(QFile::Truncate|QFile::WriteOnly))
			return false;

		QMap<QString,QString>::iterator iter = mapStocks.begin();
		while(iter!=mapStocks.end())
		{
			file.write(QString(iter.value()+"\r\n").toAscii());
			++iter;
		}
		file.close();
	}

	return true;
}


void CDataEngine::appendNews( const QString& title, const QString& content )
{
	QString qsNewsPath = m_qsNewsDir + title;

	QDir().mkpath(QFileInfo(qsNewsPath).absolutePath());
	QFile file(qsNewsPath);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return;
	file.write(content.toAscii());
	file.close();
}

void CDataEngine::appendF10( const QString& title, const QString& content )
{
	QString qsF10Path = m_qsF10Dir + title;

	QDir().mkpath(QFileInfo(qsF10Path).absolutePath());
	QFile file(qsF10Path);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return;
	file.write(content.toAscii());
	file.close();
}


QList<CStockInfoItem*> CDataEngine::getStockInfoList()
{
	return m_mapStockInfos.values();
}

CStockInfoItem* CDataEngine::getStockInfoItem( const QString& qsCode )
{
	if(m_mapStockInfos.contains(qsCode))
	{
		return m_mapStockInfos[qsCode];
	}
	return NULL;
}

void CDataEngine::setStockInfoItem( CStockInfoItem* p )
{
	m_mapStockInfos[p->getCode()] = p;
}

CDataEngine* CDataEngine::getDataEngine()
{
	if(m_pDataEngine == NULL)
		m_pDataEngine = new CDataEngine;
	return m_pDataEngine;
}

bool CDataEngine::exportHistoryData( const QString& qsCode, const QList<qRcvHistoryData*>& list )
{
	/*���ݿ����
	QSqlDatabase db = QSqlDatabase::database("HISTORY");
	if(!db.isOpen())
		return false;
	db.transaction();
	QSqlQuery q(db);

	foreach(qRcvHistoryData* pData, list)
	{
		QString qsQuery = QString("replace into [History] ('code','time','open','high','low','close','volume','amount') \
								  values ('%1',%2,%3,%4,%5,%6,%7,%8)")
								  .arg(qsCode).arg(pData->time).arg(pData->fOpen).arg(pData->fHigh)
								  .arg(pData->fLow).arg(pData->fClose).arg(pData->fVolume).arg(pData->fAmount);
		q.exec(qsQuery);
	}
	db.commit();
	return true;
	*/

	QString qsDayData = QString("%1%2").arg(m_qsHistroyDir).arg(qsCode);

	QFile file(qsDayData);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return false;

	QDataStream out(&file);

	foreach(qRcvHistoryData* pData, list)
	{
		int iSize = out.writeRawData((char*)pData,sizeof(qRcvHistoryData));
		if(iSize!=sizeof(qRcvHistoryData))
		{
			file.close();
			return false;
		}
	}

	file.close();
	return true;
}

QList<qRcvHistoryData*> CDataEngine::getHistoryList( const QString& code )
{
	QList<qRcvHistoryData*> list;

	QString qsDayData = QString("%1%2").arg(m_qsHistroyDir).arg(code);
	QFile file(qsDayData);
	if(!file.open(QFile::ReadOnly))
		return list;

	QDataStream inStream(&file);
	while(!inStream.atEnd())
	{
		qRcvHistoryData* pData = new qRcvHistoryData;
		int iSize = inStream.readRawData(reinterpret_cast<char*>(pData),sizeof(qRcvHistoryData));
		if(iSize!=sizeof(qRcvHistoryData))
		{
			delete pData;
			break;
		}
		list.push_back(pData);
	}

	file.close();
	return list;
}

QList<qRcvHistoryData*> CDataEngine::getHistoryList( const QString& code, int count )
{
	QList<qRcvHistoryData*> list;

	QString qsDayData = QString("%1%2").arg(m_qsHistroyDir).arg(code);
	QFile file(qsDayData);
	if(!file.open(QFile::ReadOnly))
		return list;
	int iDataSize = sizeof(qRcvHistoryData);

	int iPos = file.size()-iDataSize;
	int iCount = 0;
	while(iPos>=0&&iCount<count)
	{
		++iCount;
		file.seek(iPos);
		qRcvHistoryData* pData = new qRcvHistoryData;
		int iSize = file.read(reinterpret_cast<char*>(pData),iDataSize);
		if(iSize!=iDataSize)
		{
			delete pData;
			break;
		}
		list.push_front(pData);
		iPos = iPos-iDataSize;
	}

	file.close();
	return list;
}

bool CDataEngine::exportFenBiData( const QString& qsCode, const long& lDate, const QList<qRcvFenBiData*>& list )
{
	QString qsPath = QString("%1%2/").arg(m_qsFenBiDir).arg(lDate);
	QDir().mkpath(qsPath);
	QString qsFenBiData = QString("%1%2").arg(qsPath).arg(qsCode);

	QFile file(qsFenBiData);
	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return false;

	QDataStream out(&file);

	foreach(qRcvFenBiData* pData, list)
	{
		//int iSize = file.write((char*)pData,sizeof(qRcvFenBiData));
		//if(iSize!=sizeof(qRcvFenBiData))
		//{
		//	file.close();
		//	return false;
		//}
		QString qsLine = QString("time:%1	Price:%2	Buy1:%3	Sell1:%4\r\n")
			.arg(pData->tmTime).arg(pData->fPrice)
			.arg(pData->fBuyPrice[0]).arg(pData->fSellPrice[1]);
		file.write(qsLine.toLocal8Bit());
	}

	file.close();
	return true;
}


bool CDataEngine::isBlockInCommon( const QString& block )
{
	QList<QPair<QString,QRegExp>>::iterator iter = m_listCommonBlocks.begin();
	while(iter!=m_listCommonBlocks.end())
	{
		if(iter->first==block)
			return true;
		++iter;
	}

	return false;
}

QRegExp CDataEngine::getRegexpByBlock( const QString& block )
{
	QList<QPair<QString,QRegExp>>::iterator iter = m_listCommonBlocks.begin();
	while(iter!=m_listCommonBlocks.end())
	{
		if(iter->first==block)
			return iter->second;
		++iter;
	}

	return QRegExp("*");
}
