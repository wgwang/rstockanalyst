#include "StdAfx.h"
#include "ColorManager.h"

QVector<QColor> CColorManager::CommonColor;

void CColorManager::initAll()
{
	initCommonColor();
}

void CColorManager::initCommonColor()
{
	//http://blog.csdn.net/daichanglin/article/details/1563299
	CommonColor.push_back(QColor(220,20,60));		//�ɺ�
	CommonColor.push_back(QColor(255,240,245));		//����ĵ���ɫ
	CommonColor.push_back(QColor(128,0,128));		//��ɫ
	CommonColor.push_back(QColor(75,0,130));		//����
	CommonColor.push_back(QColor(0,0,205));			//���е���ɫ
	CommonColor.push_back(QColor(0,0,128));			//������
	CommonColor.push_back(QColor(176,196,222));		//������
	CommonColor.push_back(QColor(135,206,235));		//����ɫ
	CommonColor.push_back(QColor(0,191,255));		//����ɫ
	CommonColor.push_back(QColor(60,179,113));		//�������ɫ
	CommonColor.push_back(QColor(0,100,0));			//����ɫ
	CommonColor.push_back(QColor(255,255,0));		//����ɫ
	CommonColor.push_back(QColor(128,128,0));		//���ɫ
	CommonColor.push_back(QColor(255,215,0));		//��ɫ
	CommonColor.push_back(QColor(222,184,135));		//��ʵ����
	CommonColor.push_back(QColor(192,192,192));		//����ɫ
}
