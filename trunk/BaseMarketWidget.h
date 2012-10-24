#pragma once
#include <QtGui>
#include "BaseMarketTreeView.h"

//ר�����ڻ���������TabWidget ��ͷ����
class CTabButton : public QLabel
{
	Q_OBJECT
public:
	CTabButton(const QString& text,QWidget* parent = 0);
	~CTabButton(){}

public:
	void setBtnDown(bool bDown = true){ bButtonDown = bDown;}
protected:
	virtual void paintEvent(QPaintEvent* e);
	virtual void mousePressEvent(QMouseEvent *ev);

signals:
	void clicked();

private:
	bool bButtonDown;
};

//ר�����ڻ���������TabWidget ��ͷ���࣬���CTabButtonʹ��
class CTabButtonList : public QWidget
{
	Q_OBJECT
public:
	CTabButtonList(QWidget* parent = 0);
	~CTabButtonList(){}

public:
	void addTabButton(const QString& text,QWidget* widget);

protected slots:
	void tabBtnClicked();

private:
	QMap<CTabButton*,QWidget*> mapButtons;
	CTabButton* pDownBtn;
	QHBoxLayout* pLayout;
};


//��ʾ��������Ĵ��ڡ�
class CBaseMarketWidget : public QScrollArea
{
	Q_OBJECT
public:
	CBaseMarketWidget();
	~CBaseMarketWidget(void);


public slots:
	void treeItemClicked(const QModelIndex& index);

private:
	//��֤A��
	CBaseMarketTreeView* m_pViewSHA;
	//����ָ��
	CBaseMarketTreeView* m_pViewSZ;
};

