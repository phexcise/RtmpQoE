#include"ffplay.h"
#include"stdafx.h"
#include"pause_detect.h"

int qoe_ptr_location_pre=0;
char qoe_ptr_location_pre_val=NULL;
int is_rebuff=0;
//���±���ֻ��¼���ֻ�������
//�������
static int qoe_rebuff_num=0;
//��¼ÿ�λ����ʱ��
static int qoe_rebuff_time[MAX_IOCHECK_NUM]={0};
//���±�����¼����ʱ��ε����
//������
static int qoe_checknum=0;

typedef struct PacketQueue{
  AVPacketList *first_pkt, *last_pkt;
  int nb_packets;
  int size;
  SDL_mutex *mutex;
  SDL_cond *cond;
};

typedef struct VideoPicture 
{
  SDL_Overlay *bmp;
  int width, height; /* source height & width */
  int allocated;
  double pts;
};

typedef struct VideoState
{
	AVFormatContext *pFormatCtx;
	int videostream,audiostream;
	AVDictionary *avdic;

	AVStream *audio_st;
	PacketQueue audioq;
	uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE *3)/2];
	unsigned int audio_buf_size;
	unsigned int audio_buf_index;
	AVPacket audiopkt;
	uint8_t *audio_pkt_data;
	int		audio_pkt_size;
	int     audio_hw_buf_size; 
	double	audio_clock;
	double  audio_diff_cum; /* used for AV difference average computation */
	double  audio_diff_avg_coef;
	double  audio_diff_threshold;
	int     audio_diff_avg_count;
	int av_sync_type;

	double frame_last_pts,frame_last_delay,frame_timer;
	
	AVStream *video_st;
	PacketQueue videoq;
	double video_clock;
	double video_current_pts;
	int64_t video_current_pts_time;

	VideoPicture pictq[VIDEO_PICTURE_QUEUE_SIZE];
	int pictq_size,pictq_rindex,pictq_windex;
	SDL_mutex *pictq_mutex;
	SDL_cond *pictq_cond;
	
	SDL_Thread *parse_tid;
	SDL_Thread *video_tid;
	
	char filename[1024];
	int quit;
	
	double  external_clock; /* external clock base */
	int64_t external_clock_time;
};

void pause_detect(VideoState* is,LPVOID wnd)
{
	HWND WND=(HWND)wnd;
	AVFormatContext *pFormatCtx = is->pFormatCtx;
	CString rebuff_num;
	int buffer_ptr_location=0;
	//while(1)
	//{
		//�������޺��Զ���0-------------------------
		if(qoe_checknum>=MAX_IOCHECK_NUM){
			qoe_checknum=0;
			qoe_rebuff_num=0;
			//continue;
		}
		//������ǰ״̬��������0��������ͣ��1��
		if(pFormatCtx->pb!=NULL)
		{
			//��ȡbuffer��ָ���λ��
			buffer_ptr_location=pFormatCtx->pb->buf_ptr-pFormatCtx->pb->buffer;
			//�����ǰһ�μ���λ�����,������ָ���ֵҲ��ȣ���˵��ָ��û���ƶ�����Ƶ������ͣ
			if((qoe_ptr_location_pre==buffer_ptr_location)&&(qoe_ptr_location_pre_val==*(pFormatCtx->pb->buf_ptr))){
				if(is_rebuff==0){
					//�״γ�����ͣ,������һ����ͣ����
					qoe_rebuff_num++;
					//ͬʱ�ı�Ϊ��ͣ״̬
					is_rebuff=1;
				}else{
					//�����״���ͣ����¼��ͣ��ʱ�䣨����һ����λʱ�䣩
					qoe_rebuff_time[qoe_rebuff_num]++;
				}
			}else{
				//��Ƶ������ͣ
				is_rebuff=0;
			}
		
			//�������-------------------------------
			rebuff_num.Format("%d",qoe_rebuff_num);
			SetDlgItemText(WND,IDC_PAUSE_FREQ,rebuff_num);
			//����----------
			qoe_checknum++;
			//�洢���Σ���������һ�αȽ�
			qoe_ptr_location_pre=buffer_ptr_location;
			qoe_ptr_location_pre_val=*(pFormatCtx->pb->buf_ptr);
			//continue;
		}
		//else
			//continue;
	//}
}