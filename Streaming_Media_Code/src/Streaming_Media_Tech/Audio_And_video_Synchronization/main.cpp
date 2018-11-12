#include <QtCore/QCoreApplication>
#include <iostream>
#include <QThread>
#include "XMediaEncode.h"
#include "XRtmp.h"
#include "XAudioRecord.h"
#include "XVideoCapture.h"
using namespace std;
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	

	char *outUrl = "rtmp://192.168.10.181/live";
	int ret = 0;
	int sampleRate = 44100;
	int channels = 2;
	int sampleByte = 2;
	int nbSample = 1024;

	///�������
	XVideoCapture *xv = XVideoCapture::Get();
	if (!xv->Init(0))
	{
		cout << "open camera failed!" << endl;
		getchar();
		return -1;
	}
	cout << "open camera success!" << endl;
	xv->Start();

	///1 qt��Ƶ��ʼ¼��
	XAudioRecord *ar = XAudioRecord::Get();
	ar->sampleRate = sampleRate;
	ar->channels = channels;
	ar->sampleByte = sampleByte;
	ar->nbSamples = nbSample;
	if (!ar->Init())
	{
		cout << "XAudioRecord Init failed!" << endl;
		getchar();
		return -1;
	}
	ar->Start();
	
	///����Ƶ������
	XMediaEncode *xe = XMediaEncode::Get();

	///2 ��ʼ����ʽת��������
	///3 ��ʼ����������ݽṹ
	xe->inWidth = xv->width;
	xe->inHeight = xv->height;
	xe->outWidth = xv->width;
	xe->outHeight = xv->height;
	if (!xe->InitScale())
	{
		getchar();
		return -1;
	}
	cout << "��ʼ����Ƶ����ת�������ĳɹ�!" << endl;
	
	///2 ��Ƶ�ز��� �����ĳ�ʼ��
	xe->channels = channels;
	xe->nbSample = nbSample;
	xe->sampleRate = sampleRate;
	xe->inSampleFmt = XSampleFMT::X_S16;
	xe->outSampleFmt = XSampleFMT::X_FLATP;
	if (!xe->InitResample())
	{
		getchar();
		return -1;
	}
	///4 ��ʼ����Ƶ������
	if (!xe->InitAudioCode())
	{
		getchar();
		return -1;
	}

	///��ʼ����Ƶ������
	if (!xe->InitVideoCodec())
	{
		getchar();
		return -1;
	}


	///5 �����װ������Ƶ������
	//a ���������װ��������
	XRtmp *xr = XRtmp::Get(0);
	if (!xr->Init(outUrl))
	{
		getchar();
		return -1;
	}

	//�����Ƶ��
	int vindex = 0;
	vindex = xr->AddStream(xe->vc);
	if (vindex<0)
	{
		getchar();
		return -1;
	}

	//b �����Ƶ�� 
	int aindex = xr->AddStream(xe->ac);
	if (aindex<0)
	{
		getchar();
		return -1;
	}

	///��rtmp ���������IO
	//д���װͷ
	if (!xr->SendHead())
	{
		getchar();
		return -1;
	}
	
	ar->Clear();
	xv->Clear();
	long long beginTime = GetCurTime();

	//һ�ζ�ȡһ֡��Ƶ���ֽ���
	for (;;)
	{
		//һ�ζ�ȡһ֡��Ƶ
		XData ad = ar->Pop();
		XData vd = xv->Pop();
		if (ad.size<=0 && vd.size<=0)
		{
			QThread::msleep(1);
			continue;
		}

		//������Ƶ
		if (ad.size > 0)
		{
			ad.pts = ad.pts - beginTime;

			//�ز���Դ����
			XData pcm = xe->Resample(ad);
			ad.Drop();
			
			XData pkt = xe->EncodeAudio(pcm);
			if (pkt.size > 0)
			{
				////����
				if (xr->SendFrame(pkt,aindex))
				{
					cout << "#" << flush;
				}
			}
			
		}

		//������Ƶ
		if (vd.size > 0)
		{
			vd.pts = vd.pts - beginTime;
			XData yuv = xe->RGBToYUV(vd);
			vd.Drop();
			XData pkt = xe->EncodeVideo(yuv);
			if (pkt.size> 0)
			{
				////����
				if (xr->SendFrame(pkt,vindex))
				{
					cout << "@" << flush;
				}
			}
		}
	}

	getchar();
	return a.exec();
}
