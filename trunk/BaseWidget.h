/************************************************************************/
/* �ļ����ƣ�BaseWidget.h
/* ����ʱ�䣺2012-11-16 10:59
/*
/* ��    �������л��ƴ��ڵĻ��֧࣬�ֲ�ֵȲ���
/************************************************************************/
#ifndef BASE_WIDGET_H
#define BASE_WIDGET_H
#include <QtGui>

class CBaseWidget : public QWidget
{
	Q_OBJECT
public:
	CBaseWidget(QWidget* parent = 0);
	~CBaseWidget(void);
};


#endif	//BASE_WIDGET_H