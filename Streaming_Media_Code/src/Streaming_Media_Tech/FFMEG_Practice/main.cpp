/*****************************FILE INFOMATION***********************************
**
**  Project       : FFmpegSDK
**  Description   : ����FFMpegSDK��
*******************************************************************************/

#include "pch.h"
#include <iostream>
using namespace std;

#define NO_USE_CODEC  0   // 0 ��ʾ��ʹ��codec  1  ��ʾʹ��codec

int64_t lastReadPacktTime;

static int interrupt_cb(void *ctx)
{
	int  timeout = 3;
	if (av_gettime() - lastReadPacktTime > timeout * 1000 * 1000)
	{
		return -1;
	}
	return 0;
}

int XError(int errNum)
{
	char buf[1024] = { 0 };
	av_strerror(errNum, buf, sizeof(buf));
	cout << buf << endl;
	return  errNum;
}

static double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

//ʹ���ļ�������RTMP
void test000()
{
	char *inUrl = "test.flv";
	char *outUrl = "rtmp://192.168.10.181/live";
	//��ʼ�����з�װ�ͽ��װ flv mp4 mov mp3
	av_register_all();

	//��ʼ�������
	avformat_network_init();

	//���ļ������װ
	//�����ʽ�����ģ���װ
	AVFormatContext *ictx = NULL;
	lastReadPacktTime = av_gettime();
	ictx = avformat_alloc_context();
	ictx->interrupt_callback.callback = interrupt_cb;
	int ret = avformat_open_input(&ictx, inUrl, 0, 0);
	if (ret != 0)
	{
		XError(ret);
		return;
	}

	//��ȡ����Ƶ����Ϣ
	ret = avformat_find_stream_info(ictx, 0);
	if (ret != 0)
	{
		XError(ret);
		return;
	}

	//��ӡ������
	av_dump_format(ictx, 0, inUrl, 0);
	cout << "open file" << inUrl << "Success!!!" << endl;
	///////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////
	//�����
	//���������
	AVFormatContext *octx = NULL;
	ret = avformat_alloc_output_context2(&octx, 0, "flv", outUrl);
	if (ret != 0)
	{
		XError(ret);
		return;
	}
	cout << "octx create success!!!" << endl;

	//���������
	//���������AVStream
	for (unsigned int i = 0; i < ictx->nb_streams; i++)
	{
		//���������
#if NO_USE_CODEC
		AVStream *out = avformat_new_stream(octx, ictx->streams[i]->codec->codec);
#else
		AVCodec *codec00 = avcodec_find_decoder(ictx->streams[i]->codecpar->codec_id);
		AVStream *out = avformat_new_stream(octx, codec00);
#endif
		if (!out)
		{
			XError(ret);
			return;
		}
		//����������Ϣ,����MP4
		//re = avcodec_copy_context(out->codec, ictx->streams[i]->codec);
		ret = avcodec_parameters_copy(out->codecpar, ictx->streams[i]->codecpar);
#if NO_USE_CODEC
		out->codec->codec_tag = 0;
#else
		out->codecpar->codec_tag = 0;
#endif
	}
	av_dump_format(octx, 0, outUrl, 1);

	//rtmp����

	//��io
	ret = avio_open(&octx->pb, outUrl, AVIO_FLAG_WRITE);
	if (!octx->pb)
	{
		XError(ret);
		return;
	}

	//д��ͷ��Ϣ
	ret = avformat_write_header(octx, 0);
	if (ret < 0)
	{
		XError(ret);
		return;
	}
	cout << "avformat_write_header " << ret << endl;
	AVPacket pkt;
	long long startTime = av_gettime();
	for (;;)
	{
		ret = av_read_frame(ictx, &pkt);
		lastReadPacktTime = av_gettime();
		if (ret != 0)
		{
			XError(0);
			break;
		}
		cout << pkt.pts << " " << flush;
		//����ת��pts dts
		AVRational itime = ictx->streams[pkt.stream_index]->time_base;
		AVRational otime = octx->streams[pkt.stream_index]->time_base;
		pkt.pts = av_rescale_q_rnd(pkt.pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q_rnd(pkt.duration, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.pos = -1;

		//��Ƶ֡�����ٶ�
		if (ictx->streams[pkt.stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			AVRational tb = ictx->streams[pkt.stream_index]->time_base;
			//�Ѿ���ȥ��ʱ��
			long long now = av_gettime() - startTime;
			long long dts = 0;
			dts = pkt.dts * (__int64)(1000 * 1000 * r2d(tb));
			if (dts > now)
			{
				av_usleep((unsigned)(dts - now));
			}

		}

		ret = av_interleaved_write_frame(octx, &pkt);   //Ҳ������av_write_frame�����䲻��pts��dts�����Ҳ����ͷ�pkt�ڲ��ռ�

														//ret = av_write_frame(octx, &pkt);
														//av_packet_unref(&pkt);
		if (ret < 0)
		{
			XError(ret);
			return;
		}
	}
	getchar();
}



int main(int argc, char *argv[])
{
	test000();
	return 0;
}
