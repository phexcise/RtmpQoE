#define WPCAP
#define HAVE_REMOTE
#include<pcap.h>
#include<stdlib.h>


#include"stdafx.h"
#include"final.h"
#include"finalDlg.h"
#include"afxdialogex.h"
#include"resource.h"//ֱ�ӹ�ϵ���ܷ���öԻ����еĸ�����Դ
#include"AdapterList.h"

typedef struct delay_param;
delay_param* delay_qoe();
__int64 download_speed();
void get_devlist(LPVOID lpParam);