/*
** FAAD2 - Freeware Advanced Audio (AAC) Decoder including SBR decoding
** Copyright (C) 2003-2005 M. Bakker, Nero AG, http://www.nero.com
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
** Any non-GPL usage of this software or parts of this software is strictly
** forbidden.
**
** The "appropriate copyright message" mentioned in section 2c of the GPLv2
** must read: "Code from FAAD2 is copyright (c) Nero AG, www.nero.com"
**
** Commercial non-GPL licensing of this software is possible.
** For more info contact Nero AG through Mpeg4AAClicense@nero.com.
**
** $Id: audio.h,v 1.19 2007/11/01 12:33:29 menno Exp $
**/

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define MAXWAVESIZE     4294967040LU

#define OUTPUT_WAV 1
#define OUTPUT_RAW 2

typedef struct
{
    int toStdio;
    int outputFormat;
    FILE *sndfile;
    unsigned int fileType;
    unsigned long samplerate;
    unsigned int bits_per_sample;
    unsigned int channels;
    unsigned long total_samples;
    long channelMask;
} audio_file;

typedef struct {
	unsigned char decode_AAC_flag; //ָʾ��ǰ֡�Ƿ���ȷ���룬decode_AAC_flag==1Ϊ��ȷ���룬ecode_AAC_flag==0Ϊ����ʱ����

	unsigned long samplerate; // ��Ƶ�����ʣ���λΪHz
	unsigned char channels;   // ��Ƶ��������channels=1Ϊ��������channels=2Ϊ˫����
    unsigned char bits_per_sample;//ÿ������������������

	unsigned long bitrate;    // ��Ƶ���ʣ���λΪbps

	float score_second;             //һ��������Ƶ�Ŀ͹����۷���
	float score_minute;             //һ��������Ƶ�Ŀ͹����۷���
	unsigned char score_second_flag; //��Ϊÿ����һ�α�����������һ֡������ȡһ֡��������������һ���ӲŴ��һ�Σ�����øñ���ָʾ�Ƿ�����
	unsigned char score_minute_flag; //ͬ��

	/* ���²���Ϊһ����/һ�����ڸ�֡ȡƽ����Ĳ���. ������һ�뻹��һ֡�Ĳ���������.._flag�ж�.*/
	float global_gain;  
    float scale_factors_M; 
    float scale_factors_S; 
    float nonzero_SB_M; 
    float nonzero_SB_S; 
    float ms_used;
    float longwindows;
	int   bandwidth;

} AUDIO_PARAMETER_T;

int decode_AAC(unsigned char *audio_buffer, unsigned short audio_size, 
			 unsigned char *wav_buffer, unsigned int *wav_size,
			 AUDIO_PARAMETER_T *audio_parameter);

audio_file *open_audio_file(char *infile, int samplerate, int channels,
                            int outputFormat, int fileType, long channelMask);
int write_audio_file(audio_file *aufile, void *sample_buffer, int samples, int offset);
void close_audio_file(audio_file *aufile);
static int write_wav_header(audio_file *aufile);
static int write_wav_extensible_header(audio_file *aufile, long channelMask);
static int write_audio_16bit(audio_file *aufile, void *sample_buffer,
                             unsigned int samples);
static int write_audio_24bit(audio_file *aufile, void *sample_buffer,
                             unsigned int samples);
static int write_audio_32bit(audio_file *aufile, void *sample_buffer,
                             unsigned int samples);
static int write_audio_float(audio_file *aufile, void *sample_buffer,
                             unsigned int samples);


#ifdef __cplusplus
}
#endif
#endif
