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

struct stLinerItem
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
	stLinerItem()
	{
		memset(&time,0,sizeof(stLinerItem));
	}
};


class CBaseLiner
{
public:
	CBaseLiner(void);
	~CBaseLiner(void);

	virtual void Draw(QPainter& p,const QList<stLinerItem*>& d,const QRectF& rtClient,int iShowCount);

public:
	void setLineColor(const QColor& clr){ m_clrLine = clr; }
	void setDrawRect(const QRect& rtClient){ m_rtClient = rtClient; }

private:
	QColor m_clrLine;		//������ɫ
	QRect m_rtClient;		//���Ƶ�����
};

//K��ͼ����������
class CKLineLiner : public CBaseLiner
{
public:
	CKLineLiner(void);
	~CKLineLiner(void);

public:
	virtual void Draw(QPainter& p,const QList<stLinerItem*>& d,const QRectF& rtClient,int iShowCount);

private:
	void drawCoordY(QPainter& p,const QRectF& rtClient,float fMinPrice,float fMaxPrice);
};

class CMultiLiner
{
public:
	enum MultiLinerType
	{
		Main = 1,		//��ͼ
		Deputy,			//��ͼ
	};
public:
	CMultiLiner(MultiLinerType type);
	~CMultiLiner(void);

public:
	void Draw(QPainter& p, const QList<stLinerItem*>& d,const QRectF& rtClient,int iShowCount);
	void setExpression(const QString& exp);

private:
	QList<CBaseLiner*> m_listLiner;	//��ӵ�еĻ����б�
	MultiLinerType m_type;			//������������ͼ���Ǹ�ͼ
};

#endif	//BASE_LINER_H