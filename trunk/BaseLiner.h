/************************************************************************/
/* �ļ����ƣ�BaseLiner.h
/* ����ʱ�䣺2012-11-22 16:03
/*
/* ��    �������Ƹ����ߵĻ���
/*           ��Ҫ��������ͼ�е���������
/************************************************************************/

#ifndef BASE_LINER_H
#define BASE_LINER_H
#include <QtGui>
#include <QtScript>

struct DrawData
{
	time_t	time;			//UCT
	float	fOpen;			//����
	float	fHigh;			//���
	float	fLow;			//���
	float	fClose;			//����
	float	fVolume;		//��
	float	fAmount;		//��
	WORD	wAdvance;		//����,��������Ч
	WORD	wDecline;		//����,��������Ч
};


class CBaseLiner
{
public:
	CBaseLiner(void);
	~CBaseLiner(void);

	virtual void Draw(const QPainter& p,const QList<DrawData>& d);

public:
	void setLineColor(const QColor& clr){ m_clrLine = clr; }
	void setDrawRect(const QRect& rtClient){ m_rtClient = rtClient; }

private:
	QColor m_clrLine;		//������ɫ
	QRect m_rtClient;		//���Ƶ�����
};

#endif	//BASE_LINER_H