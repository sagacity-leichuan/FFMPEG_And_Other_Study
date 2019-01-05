#pragma once

struct AVFrame;
struct AVPacket;
class AVCodecContext;
enum XSampleFMT
{
	X_S16 = 1,
	X_FLATP =8
};

///����Ƶ����ӿ���
class XMediaEncode
{
public:
	///�������
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;
	int channels = 2;
	int sampleRate = 44100;
	XSampleFMT inSampleFmt = X_S16;

	///�������
	int outWidth = 1280;
	int outHeight = 720;
	int bitrate = 4000000;//ѹ����ÿ����Ƶ��bitλ��С 50kB
	int fps = 25;
	int nbSample = 1024;
	XSampleFMT outSampleFmt = X_FLATP;


	//������������
	static XMediaEncode * Get(unsigned char index = 0);
	
	//��ʼ�����ظ�ʽת���������ĳ�ʼ��
	virtual bool InitScale() = 0;

	//��Ƶ�ز��������ĳ�ʼ��
	virtual bool InitResample() = 0;

	//����ֵ�������������
	virtual AVFrame *Resample(char *data) = 0;

	//����ֵ�������������
	virtual AVFrame* RGBToYUV(char *rgb) = 0;

	//��Ƶ��������ʼ��
	virtual bool InitVideoCodec() = 0;

	//��Ƶ�����ʼ��
	virtual bool InitAudioCode() = 0;

	//��Ƶ���� ����ֵ�������������
	virtual AVPacket * EncodeVideo(AVFrame* frame) = 0;

	//��Ƶ���� ����ֵ�������������
	virtual AVPacket * EncodeAudio(AVFrame* frame) = 0;

	virtual ~XMediaEncode();

	AVCodecContext *vc = 0;	//��Ƶ������������
	AVCodecContext *ac = 0; //��Ƶ������������
protected:
	XMediaEncode();
};

