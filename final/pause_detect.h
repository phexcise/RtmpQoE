#include"ffplay.h"
#include"stdafx.h"
#include"final.h"
#include"finalDlg.h"
#include"afxdialogex.h"
#include"resource.h"//ֱ�ӹ�ϵ���ܷ���öԻ����еĸ�����Դ

typedef struct PacketQueue;
typedef struct VideoPicture;
typedef struct VideoState;
void pause_detect(VideoState* is,LPVOID wnd);