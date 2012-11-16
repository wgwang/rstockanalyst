/************************************************************************/
/* �ļ����ƣ�BaseWidget.h
/* ����ʱ�䣺2012-11-16 10:59
/*
/* ��    �������л��ƴ��ڵĻ��֧࣬�ֲ�ֵȲ���
/************************************************************************/
#ifndef BASE_WIDGET_H
#define BASE_WIDGET_H
#include <QtGui>
#include <QtXml>

class CBaseWidget : public QWidget
{
	Q_OBJECT
public:
	enum WidgetType
	{
		Basic = 0,
		KLine,
	};
public:
	CBaseWidget(CBaseWidget* parent = 0, WidgetType type = CBaseWidget::Basic);
	~CBaseWidget(void);

	void initMenu();				//��ʼ���˵���
	void realignSplitter();			//���¶�Splitter���в���
public:
	/*Widget�Ļ�������*/
	//����
	void setWidgetName(const QString& qsTitle){m_qsName = qsTitle;}
	QString getWidgetName(){return m_qsName;}


	//��ȡ�ô����е�Splitterָ��
	QSplitter* getSplitter(){return m_pSplitter;}
	//��ȡ������
	CBaseWidget* getParent(){return m_pParent;}
	//�������ø�����
	void resetParent(CBaseWidget* parent);

	//��ȡwidget���ڵ�����
	int getWidgetIndex(CBaseWidget* widget) const;
	//���µ�widget �滻֮ǰ����index�Ĵ���
	void replaceWidget(int index, CBaseWidget* widget);

	//�麯���������ؼ����Զ���˵���
	virtual QMenu* getCustomMenu(){ return 0; }
	//���ظ�Widget��������Ϣ
	virtual bool loadPanelInfo(const QDomElement& elePanel);
	//�����Widget��������Ϣ
	virtual bool savePanelInfo(QDomDocument& doc,QDomElement& elePanel);

protected slots:
	/*�Ҽ��˵�����*/
	void onLeftInsert();			//�����
	void onRightInsert();			//�Ҳ���
	void onTopInsert();				//�ϲ���
	void onBottomInsert();			//�²���

	/*�Ҽ��˵������ò�ͬ�İ���*/
	void onSetNormalWidget();		//����Ϊԭʼ�Ĵ��ڣ���ʵ������
	void onSetKLineWidget();		//����ΪK��ͼ

protected:
	virtual void paintEvent(QPaintEvent* e);
	virtual void contextMenuEvent(QContextMenuEvent* e);

protected:
	CBaseWidget* m_pParent;			//������ָ��
	QMenu* m_pMenu;					//�Ҽ��˵�
	QSplitter* m_pSplitter;			//�ָ���
	QString m_qsName;
	WidgetType m_type;
};


#endif	//BASE_WIDGET_H