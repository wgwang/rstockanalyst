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

class CColorManager
{
public:
	static void initAll();
private:
	static void initCommonColor();
	static void initBlockColors();

public:
	/*ͨ��ֵfVal����ȡ����iType������Ӧ��ֵ*/
	static QColor getBlockColor(int iType,float fVal);

public:
	static QVector<QColor> CommonColor;		//���õ���ɫ��
	static QMap<QString,QVector<QColor>> BlockColors;	//ɫ����ɫ���ࡣ��ͨ���ٷֱ�������ȡ��ɫ����������
};


#endif	//COLOR_MANAGER_H