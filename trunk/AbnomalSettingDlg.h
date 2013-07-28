#ifndef ABNORMAL_SETTING_DLG_H
#define ABNORMAL_SETTING_DLG_H
#include <QtGui>

enum RAbnomalType
{
	HighIncrease = 1,		//���Ƿ�
	LowIncrease,			//���Ƿ�������
	HighTurnRatio,			//�߻�����
	HighVolumeRatio,		//������
	HighAvgVolume,			//ÿ�ʾ��� ��
	HighPrice,				//�۸����ĳֵ
	LowPrice,				//�۸�С��ĳֵ
	HighLTSZ,				//����ͨ��
	LowLTSZ,				//����ͨ��
	HighMGSY,				//��ÿ������
	HighPERatio,			//����ӯ��
	HighZYYWRatio,			//����Ӫҵ�������� ��
	HighYJYZ,				//��ҵ��Ԥ��
	HighYJZZ,				//��ҵ������
	HighYJHBZZ,				//��ҵ��������������4������
	HighJZCSYL,				//���ʲ�������   >=
	HighBuyVolume,			//ί��1+2+3>=
	HighSellVolume,			//ί��1+2+3>=
	HighCommRatio,			//ί�� >=
};

class CAbnomalSettingDlg : public QDialog
{
	Q_OBJECT
public:
	CAbnomalSettingDlg(const QMap<RAbnomalType,float>& map,QWidget* parent = 0);
	~CAbnomalSettingDlg(void);

public:
	QMap<RAbnomalType,float> getAbnomalMap();
protected:
	void initDlg();

protected slots:
	void onBtnOk();
	void onBtnCancel();

private:
	QLineEdit m_editHighIncrease;
	QLineEdit m_editHighTurnRatio;
	QLineEdit m_editHighVolumeRatio;

	QCheckBox m_ckHighIncrease;
	QCheckBox m_ckHighTurnRatio;
	QCheckBox m_ckHighVolumeRatio;

private:
	QMap<RAbnomalType,float> m_mapAbnomal;	//�쳣����������
};

#endif	//ABNORMAL_SETTING_DLG_H