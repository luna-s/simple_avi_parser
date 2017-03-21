#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>

#pragma pack(push, 1)
typedef struct _Group
{
	DWORD riff;
	DWORD size;
	DWORD type;
} Group;

typedef struct _Chunk
{
	DWORD fourCC;
	DWORD size;
	//BYTE *data;
} Chunk;

typedef struct _List
{
	DWORD list;
	DWORD size;
	DWORD fourCC;
	//BYTE *data;
} List;

typedef struct _MainAVIHeader
{
	DWORD microsec_per_frame;
	DWORD maxbytes_per_sec;
	DWORD padding_Granularity;
	DWORD flags;
	DWORD total_flames;
	DWORD initial_frames;
	DWORD streams;
	DWORD suggested_buffer_size;
	DWORD width;
	DWORD height;
	DWORD reserved[4];
} MainAVIHeader;

//typedef struct _RECT
//{
//	LONG left;
//	LONG top;
//	LONG right;
//	LONG bottom;
//} RECT;

typedef struct _AVIStreamHeader
{
	DWORD fcc_type;	// 'vids', 'auds', 'txts'
	DWORD fcc_handler;
	DWORD flags;
	WORD priority;
	WORD language;
	DWORD Initial_frames;
	DWORD scale;
	DWORD rate;
	DWORD start;
	DWORD length;
	DWORD suggested_buffer_size;
	DWORD quality;
	DWORD sample_size;
	RECT frame;
} AVIStreamHeader;

#pragma pack(pop)

void main()
{
	FILE *file = NULL;
	int file_size;
	int n;
	char *avi = NULL;
	char *p = NULL;

	file = fopen("flame.avi", "rb");
	if (!file)
		return;

	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	Group group;
	if (fread(&group, 1, sizeof(Group), file) < 1)
		goto ERR;
	
	// little endian
	if ((group.riff != 'FFIR') || (group.type != ' IVA'))
		goto ERR;


	avi = (char *)malloc(group.size - 4);
	if (!avi)
		goto ERR;
	n = group.size - 4;
	p = avi;
	memset(avi, 0x00, n);
	n = fread(avi, 1, n, file);
	while (n > 0) {
		unsigned int fourcc = *((unsigned int*)(p));
		int chunk_size = *((int*)(p+4));
		switch (fourcc) {
			case 'TSIL':
				printf("LIST(%d bytes) %c%c%c%c\n", chunk_size, p[0], p[1], p[2], p[3]);
				p += 12; n -= 12;
				break;

			default:
				printf("%c%c%c%c(%d bytes)\n", p[0], p[1], p[2], p[3], chunk_size);
				if (chunk_size & 1)
					chunk_size += 1;
				p += 8 + chunk_size; n -= 8 + chunk_size;
				break;
		}
	}

	if (avi)
		free(avi);
	if (file)
		fclose(file);

	return;
ERR:
	if (avi)
		free(avi);
	if (file)
		fclose(file);
	return;
}


