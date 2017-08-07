#include"stdafx.h"
#include"net_qoe.h"

typedef struct delay_param
{
	u_int dns_delay;
	u_int tcp_delay;
	u_int rtmp_delay;
}delay_param;

//���ֽ�ip��ַ
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ipaddress;

//ipv4�ײ�
typedef struct ip_header
{
	u_char ver_ihl;	//�汾+�ײ����ȹ�8bit
	u_char tos;		//��������
	u_short tlen;	//�ܳ�
	u_short identification;	//��ʶ
	u_short flags_fo;	//��־λ3bit����ƫ����13bit
	u_char ttl;		//���ʱ��
	u_char proto;	//Э��
	u_short crc;	//�ײ�У���
	ip_address saddr;	//Դ��ַ
	ip_address daddr;	//Ŀ���ַ
	u_int op_pad;	//ѡ�������
}ip_header;

//udp�ײ�
typedef struct udp_header
{
	u_short sport;	//Դ�˿�
	u_short dport;	//Ŀ��˿�
	u_short len;	//udp���ݰ�����
	u_short crc;	//У���
}udp_header;

//dns�ײ�
typedef struct dns_header
{
	u_short id;//��ʶ
	u_short flag;//��־
}dns_header;

 //����TCP�ײ�
typedef struct tcp_header            
{ 
    u_short sport;               //16λԴ�˿� 
    u_short dport;               //16λĿ�Ķ˿� 
    unsigned int seq;         //32λ���к� 
    unsigned int ack;         //32λȷ�Ϻ�
    unsigned char lenres;        //4λ�ײ�����/6λ������
    unsigned char flag;            //8λ��־λ 
    u_short win;                 //16λ���ڴ�С 
    u_short sum;                 //16λУ��� 
    u_short urp;                 //16λ��������ƫ���� 
}tcp_header;

/*rtmp�ײ�������0�� �ṹ�峤�ȵļ���ܱ�̬�գ�
typedef struct rtmp_header
{
	u_char fmt_csid;	//�����ͼ�����id��8bit
	u_short ts_bs1;		//ʱ�����bodysize��48λ
	u_int ts_bs2;
	u_char msg_tid;		//8λtype id
	u_int sid;			//stream id��32λ
}rtmp_header;*/

//���������е�rtmp body��string����
typedef struct rtmp_body_string
{
	//u_char amf_type;	//8bit amf0 ����
	//u_short str_len;	//16bit string����
	u_int str;	//���Ǳ��ṹ�峤�ȵı�̬���ݸ�������ģ�
}rtmp_body_string;

CfinalDlg* final_dlg;
AdapterList* adplist_dlg;
void get_devlist(LPVOID lpParam)
{
	adplist_dlg=(AdapterList*)lpParam;
    pcap_if_t *alldevs;
    pcap_if_t *d;
    int i=0;
    char errbuf[PCAP_ERRBUF_SIZE];
    CString index,dev_name,dev_description;
	

    /*��ȡ���ػ����豸�б� */
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL , &alldevs, errbuf) == -1)
    {
        fprintf(stderr,"Error in pcap_findalldevs_ex: %s\n", errbuf);
        exit(1);
    }
    
    /* ��ӡ�б� */
    for(d= alldevs; d != NULL; d= d->next)
    {
		//------------------------------
		index.Format("%d",++i);
		//��ȡ��ǰ��¼����
		int nIndex=adplist_dlg->m_devlist.GetItemCount();
		//���С����ݽṹ
		LV_ITEM lvitem;
		lvitem.mask=LVIF_TEXT;
		lvitem.iItem=nIndex;
		lvitem.iSubItem=0;
		//ע��vframe_index������ֱ�Ӹ�ֵ��
		//���ʹ��f_indexִ��Format!�ٸ�ֵ��
		lvitem.pszText=(char *)(LPCTSTR)index;
		//------------------------

		adplist_dlg->m_devlist.InsertItem(&lvitem);
		adplist_dlg->m_devlist.SetItemText(nIndex,1,d->name);
		if (d->description)
			adplist_dlg->m_devlist.SetItemText(nIndex,2, d->description);
		else
			adplist_dlg->m_devlist.SetItemText(nIndex,2, "No description available");
    }
    
    if (i == 0)
    {
        printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
        return;
    }

    /* ������Ҫ�豸�б��ˣ��ͷ��� */
    pcap_freealldevs(alldevs);
}

delay_param* delay_qoe()
{
	//memset(&dp,0,sizeof(delay_param));
	//dp->dns_delay=0;
	//dp->rtmp_delay=0;
	//dp->tcp_delay=0;
	pcap_if *alldevs;
	pcap_if *d;
	int inum;
	static u_int i=0;
	static u_int j=0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];

	u_int netmask;
	char packet_filter[]="port 53 or tcp";
	struct bpf_program fcode;

	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	int res;

	struct tm *ltime;
	char timestr[16];
	time_t local_tv_sec;
	time_t local_tv_usec;
	int result=0;

	ip_header *ih;
	udp_header *uh;
	dns_header *dnsh;
	tcp_header *th;
	delay_param *dp=(delay_param*)malloc(sizeof(delay_param));
	u_char *rh;
	rtmp_body_string* rtmp_body_str;

	u_int ip_len;
	u_int tcp_header_len;
	u_int tcp_body_len;

	u_short sport;
	u_short dport;

	time_t time_dnsr[10];		//dns����
	time_t time_dnsq[10];		//dns��Ӧ
	memset(time_dnsr,0,sizeof(time_dnsr));
	memset(time_dnsq,0,sizeof(time_dnsq));
	time_t time_rtmp_play=0;
	time_t time_tcp_1=0;
	time_t time_tcp_2=0;
	time_t time_tcp_3=0;

	//����豸�б�
	if(pcap_findalldevs_ex(PCAP_SRC_IF_STRING,NULL,&alldevs,errbuf)== -1)
	{
		fprintf(stderr,"wocao,err",errbuf);
		system("pause");
	}

	//��ת����ѡ�豸
	//for(d=alldevs,i=0;i<list_selected_dlg;d=d->next,i++);
	d=alldevs->next;

	//��������
	if((adhandle=pcap_open(d->name,65536,PCAP_OPENFLAG_PROMISCUOUS,1000,NULL,errbuf))==NULL)
	{
		fprintf(stderr,"\nwocao, da bu kai adapter!\n");
		pcap_freealldevs(alldevs);system("pause");
			return NULL;
	}

	//���������·��
	if(pcap_datalink(adhandle)!=DLT_EN10MB)
	{
		fprintf(stderr,"\nwocao, i need yi tai wang!\n");
		pcap_freealldevs(alldevs);system("pause");
			return NULL;
	}

	if(d->addresses!=NULL)
		//��ȡ�ӿڵ�һ����ַ������
		netmask=((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.s_addr;
	else
		//����ӿ�û�е�ַ�ͼ���һ��c�������
		netmask= 0xffffff;

	//���������
	if(pcap_compile(adhandle,&fcode,packet_filter,1,netmask)<0)
	{
		fprintf(stderr,"\nwocao,cant compile the filter1 !\n");
		pcap_freealldevs(alldevs);system("pause");
			return NULL;
	}

	//���ù�����
	if(pcap_setfilter(adhandle,&fcode)<0)
	{
		fprintf(stderr,"\nwocao,cant set the filter!\n");
		pcap_freealldevs(alldevs);system("pause");
			return NULL;
	}

	printf("\nlistening on %s....\n",d->description);

	while((res = pcap_next_ex( adhandle, &header, &pkt_data)) >= 0)
	{
		if(res == 0)
			/* Timeout elapsed */
			continue;
		
		//��ʱ���ת��Ϊ��ʶ��ĸ�ʽ
		local_tv_sec=header->ts.tv_sec;
		local_tv_usec=header->ts.tv_usec;

		//��ȡip���ݰ�ͷ����λ��
		ih=(ip_header*)(pkt_data+14);//���ǰ��ֽ����
		//��ȡudp�ײ���λ��
		ip_len=(ih->ver_ihl & 0xf)*4;

		if(ih->proto==0x11)
		{
			uh=(udp_header*)((u_char*)ih+ip_len);
			//if(uh->dport==0x3500 || uh->sport==0x3500)
			//{
				//��ȡdns�ײ���λ��
				dnsh=(dns_header*)((u_char*)uh+8);

				if(dnsh->flag==0x8081)
					{
						time_dnsr[i]=local_tv_sec*1000000+local_tv_usec;	//��λ��΢��
						if(time_dnsq[i]!=0 && time_dnsr[i]!=0)
						{	
							dp->dns_delay=time_dnsr[i]-time_dnsq[i];
							result=dp->dns_delay;
							memset(time_dnsr,0,sizeof(time_dnsr));
							memset(time_dnsq,0,sizeof(time_dnsq));
						}
						i++;
					}
				else
					{
						time_dnsq[j]=local_tv_sec*1000000+local_tv_usec;
						j++;
					}
			//}
		}
		else if(result)
		{
			th=(tcp_header*)((u_char*)ih+ip_len);//unsigned charռһ���ֽ�
			//��ȡrtmp���ݿ�ʼ��λ��
			//tcp���ݰ�ͷ������
			tcp_header_len=(((th->lenres<<4)|(th->lenres>>4)) & 0xf)*4;
			//tcp_body_len=ih->tlen-ip_len-tcp_header_len;
			//rh=(u_char*)((u_char*)th+tcp_header_len);//tcp���˰�ͷ���С����塱�ĳ���,ֻҪ��������������ֽ׶Σ����岿�־��Ǻ���Э���body
			rtmp_body_str=(rtmp_body_string*)((u_char*)th+12+tcp_header_len);

			local_tv_sec = header->ts.tv_sec;
			local_tv_usec= header->ts.tv_usec;
		
			if(th->flag==0x02)
			{
				time_tcp_1=(local_tv_sec*1000000+local_tv_usec);
			}
			if(th->flag==0x12 && time_tcp_1!=0)
			{
				time_tcp_2=(local_tv_sec*1000000+local_tv_usec);
			}
			if(th->flag==0x10 && time_tcp_1!=0 && time_tcp_2!=0 && time_tcp_3==0)
			{
				time_tcp_3=(local_tv_sec*1000000+local_tv_usec);
				dp->tcp_delay=time_tcp_3-time_tcp_1;
			}
			if(time_tcp_3!=0)
			{
				if(rtmp_body_str->str==0x70040002)	//��ʾplay��ʱ���ֵ����ʵ���ַ��������Ͻ�
				{
					time_rtmp_play=local_tv_sec*1000000+local_tv_usec;
					dp->rtmp_delay=time_rtmp_play-time_tcp_3;
					time_rtmp_play=0;
					time_tcp_1=0;
					time_tcp_2=0;
					time_tcp_3=0;
					return dp;
				}
				//printf("%.2x",rtmp_body_str->str_len);
			}
		}
	}
	if(res == -1)
	{
		//printf("Error reading the packets: %s\n", pcap_geterr(adhandle));
		system("pause");
		return NULL;
	}

	pcap_close(adhandle);  	
	free(dp);
}
//--------------------------------------------------------------------------------------------
__int64 download_speed()
{
	pcap_if *alldevs;
	pcap_if *d;
	int inum=1;
	int i=0;

	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct timeval st_ts;
	u_int netmask;
	struct bpf_program fcode;

	struct timeval *old_ts=&st_ts;
	u_int delay;
	_int64 KBps,KPps;
	struct tm* ltime;
	//char timestr[16];
	time_t local_tv_sec;

	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	
	//����豸�б�
	if(pcap_findalldevs_ex(PCAP_SRC_IF_STRING,NULL,&alldevs,errbuf)== -1)
	{
		fprintf(stderr,"wocao,err",errbuf);
		printf("wocao,err");
		system("pause");
	}

	//��ת����ѡ�豸
	//for(d=alldevs,i=0;i<list_selected_dlg;d=d->next,i++);
	d=alldevs->next;

	//��������
	if((fp=pcap_open(d->name,100,PCAP_OPENFLAG_PROMISCUOUS,1000,NULL,errbuf))==NULL)
	{
		fprintf(stderr,"wocao cant open adapter\n",errbuf);
		printf("wocao cant open adapter\n");
		system("pause");
		return -1;
	}

	if(d->addresses!=NULL)
		//��ȡ�ӿڵ�һ����ַ������
		netmask=((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.s_addr;
	else
		//����ӿ�û�е�ַ�ͼ���һ��c�������
		netmask= 0xffffff;

	//���������
	if(pcap_compile(fp,&fcode,"tcp",1,netmask)<0)
	{
		fprintf(stderr,"wocao cant compile the filter\n",errbuf);
		printf("wocao cant compile the filter\n");
		return -1;
	}

	//���ù�����
	if(pcap_setfilter(fp,&fcode)<0)
	{
		fprintf(stderr,"wocao cant set the filter\n");
		pcap_close(fp);
		return -1;
	}

	//���ӿ�����Ϊͳ��ģʽ
	if(pcap_setmode(fp,MODE_STAT)<0)
	{
		fprintf(stderr,"wocao cant set the mode\n");
		printf("wocao cant set the mode\n");
		pcap_close(fp);
		system("pause");
		return -1;
	}

	//printf("tcp traffic summary:\n");

	while((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
	{
		if(res == 0)
			/* Timeout elapsed */
			continue;

		//�Ժ��������һ�β������ӳ�ʱ�䣬���ֵͨ����������ʱ������
		delay=(header->ts.tv_sec-old_ts->tv_sec)*1000000-old_ts->tv_usec+header->ts.tv_usec;
		//��ȡÿ��ı�����
		KBps=((*(LONGLONG*)(pkt_data+8))*8*1000000)/(delay);
		//�õ�ÿ������ݰ�����
		//KPps.QuadPart=((*(LONGLONG*)(pkt_data))*8*1000000)/(delay);

		
		return KBps;

		//��ʱ���ת��Ϊ��ʶ��ĸ�ʽ
		local_tv_sec=header->ts.tv_sec;
		ltime=localtime(&local_tv_sec);
		//strftime(timestr,sizeof(timestr),"%H:%M:%S",ltime);

		//��ӡʱ���
		//printf("%s",timestr);

		//��ӡ�������
		//printf("\tBPS=%I64u",KBps.QuadPart);
		//printf("\tPPS=%I64u\n",KPps.QuadPart);

		//�洢��ǰ��ʱ���
		old_ts->tv_sec=header->ts.tv_sec;
		old_ts->tv_usec=header->ts.tv_usec;
	}


	if(res == -1)
	{
		printf("Error reading the packets: %s\n", pcap_geterr(fp));
		system("pause");
		return -1;
	}
	pcap_close(fp);
}

