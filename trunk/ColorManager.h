/************************************************************************/
/* �ļ����ƣ�ColorManager.h
/* ����ʱ�䣺2012-11-30 14:38
/*
/* ��    ������ɫ������
/*           ���ڸ��ֳ��õ���ɫ����
/************************************************************************/

#ifndef COLOR_MANAGER_H
#define COLOR_MANAGER_H
#include <QtCore>
#include <QColor>

#define	COLOR_BLOCK_SIZE	21

class CColorManager
{
public:
	static void initAll();
	static void reloadBlockColors();		//���¼���ɫ����ɫ��
private:
	static void initCommonColor();
	static void initBlockColors();

public:
	/*��ȡ��ǰ��֧�ֵ�ɫ����ɫ�б�*/
	static QStringList getBlockColorList();
	/*ͨ��ֵfVal����ȡ����mode������Ӧ��ֵ*/
	static QColor getBlockColor(const QString& mode,float fVal);
	/*ͨ������ֵ����������Ϊmode�Ķ�Ӧ��ɫֵ*/
	static QColor getBlockColor(const QString& mode,int index);

public:
	static QVector<QColor> CommonColor;		//���õ���ɫ��

private:
	static QVector<QColor> DefaultColor;	//Ĭ��ɫ���
	static QMap<QString,QVector<QColor>> BlockColors;	//ɫ����ɫ���ࡣ��ͨ���ٷֱ�������ȡ��ɫ����������
};


#endif	//COLOR_MANAGER_H