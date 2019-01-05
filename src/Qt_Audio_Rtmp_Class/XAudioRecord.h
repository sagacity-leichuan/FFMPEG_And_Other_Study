#pragma once
#include <QThread>
enum XAUDIOTYPE
{
	X_AUDIO_QT
};
struct XData
{
	char *data = NULL;
	int size = 0;
	void Drop()
	{
		if(data)
			delete data;
		data = NULL;
		size = 0;
	}
};
class XAudioRecord:public QThread
{
public:
	int channels = 2;		//������
	int sampleRate = 44100;	//������
	int sampleByte = 2;		//�����ֽڴ�С
	int nbSamples = 1024;	//һ֡��Ƶÿ��ͨ������������

	//����������ռ�
	virtual XData Pop() = 0;

	//��ʼ¼��
	virtual bool Init() = 0;

	//ֹͣ¼��
	virtual void Stop() = 0;

	static XAudioRecord *Get(XAUDIOTYPE type = X_AUDIO_QT, unsigned char index = 0);
	virtual ~XAudioRecord();
protected:
	XAudioRecord();
};

