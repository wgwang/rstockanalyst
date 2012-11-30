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

Q_DECLARE_METATYPE(stLinerItem)
Q_DECLARE_METATYPE(QVector<float>)
Q_DECLARE_METATYPE(QVector<stLinerItem>)

class CBaseLiner
{
public:
	CBaseLiner(QScriptEngine* pEngine,const QString& exp);
	~CBaseLiner(void);
public:
	virtual void updateData();				//������ʾ������
	virtual void getMinAndMax(float& fMin,float& fMax,int iCount);
	virtual void Draw(QPainter& p,const QRectF& rtClient,int iShowCount);

public:
	void setLineColor(const QColor& clr){ m_clrLine = clr; }
	void setMinPrice(float f){ fMinPrice=f; }
	void setMaxPrice(float f){ fMaxPrice=f; }

protected:
	QColor m_clrLine;		//������ɫ
	QRect m_rtClient;		//���Ƶ�����
	float fMaxPrice;
	float fMinPrice;
	QVector<float> m_vals;
	QString m_qsExp;			//���ʽ
	QScriptEngine* m_pEngine;	//�ű�����
};

//K��ͼ����������
class CKLineLiner : public CBaseLiner
{
public:
	CKLineLiner(QScriptEngine* pEngine);
	~CKLineLiner(void);

public:
	virtual void updateData();				//������ʾ������
	virtual void getMinAndMax(float& fMin,float& fMax,int iCount);
	virtual void Draw(QPainter& p,const QRectF& rtClient,int iShowCount);

private:
	void drawKGrid( const int& iIndex,QPainter& p,const QRectF& rtItem );

private:
	QVector<float> m_vOpen;
	QVector<float> m_vHigh;
	QVector<float> m_vLow;
	QVector<float> m_vClose;
};


class CVolumeLiner : public CBaseLiner
{
public:
	CVolumeLiner(QScriptEngine* pEngine);
	~CVolumeLiner(void);

public:
	virtual void updateData();				//������ʾ������
	virtual void getMinAndMax(float& fMin,float& fMax,int iCount);
	virtual void Draw(QPainter& p,const QRectF& rtClient,int iShowCount);

private:
	QVector<float> m_vOpen;
	QVector<float> m_vClose;
//	QVector<float> m_vVolume;
};

class CMultiLiner
{
public:
	enum MultiLinerType
	{
		MainKLine = 1,	//��ͼ(��ҪָK��ͼ)
		VolumeLine,		//��ͼ�е�������
		Deputy,			//��ͼ
	};
public:
	CMultiLiner(MultiLinerType type,QScriptEngine* pEngine,const QString& exp);
	~CMultiLiner(void);

public:
	void updateData();			//��������
	void Draw(QPainter& p, const QRectF& rtClient,int iShowCount);
	void setExpression(const QString& exp);
	QString getExpression() const { return m_qsExp; }
	QRectF getRect() const { return m_rtClient; }

private:
	void drawCoordY(QPainter& p,const QRectF& rtClient,float fMinPrice,float fMaxPrice);

private:
	QList<CBaseLiner*> m_listLiner;	//��ӵ�еĻ����б�
	MultiLinerType m_type;			//������������ͼ���Ǹ�ͼ
	QScriptEngine* m_pEngine;		//��������
	QRectF m_rtClient;				//��ǰ���ڵĴ�С
	QString m_qsExp;				//�ô����еı��ʽ
};

#endif	//BASE_LINER_H