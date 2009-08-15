// mktheme.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

typedef struct
{
	int x,y,w,h;
	unsigned int size;
} image;

#define NUMFILES 14

int main(int argc, char* argv[])
{
	if ((argc != 2) && (argc != 3))
	{
		if (argc < 2)
		{
			printf("ERROR: Too few arguments.\n\n");
		}
		else if (argc > 3)
		{
			printf("ERROR: Too many arguements.\n\n");
		}
		printf("Usage:\nmktheme <coordinates file> [output file name]\n\nPress any key to continue...");
		getch();
		return 0;
	}
	
	FILE* fc;
	fc = fopen(argv[1], "r");
	
	if (fc == NULL)
	{
		printf("Error opening coordinates file %s\n\n", argv[1]);
		return 0;
	}
	
	char* filenames[] = {
		"background.png",
		"buttons.png",
		"footer_changemode.png",
		"footer_pressselect.png",
		"mask.png",
		"msg_notmatch.png",
		"msg_passwordchanged.png",
		"msg_passwordincorrect.png",
		"msg_passwordok.png",
		"title_confirmpassword.png",
		"title_newpassword.png",
		"title_oldpassword.png",
		"title_password.png",
		"title_requirepassword.png"
	};
	
	int exists[NUMFILES] = {0};
	image images[NUMFILES];
	unsigned int i,c=0,x,y,w,h;
	char name[30];
	
	while(fscanf(fc,"%s%d%d%d%d", name, &x, &y, &w, &h) != EOF)
	{
		int found = 0;
		for (i = 0; i < NUMFILES; i++)
		{
			if (stricmp(name, filenames[i]) == 0)
			{
				if (exists[i] == 1)
				{
					printf("ERROR: %s is already defined.\n\n", name);
					fclose(fc);
					return 0;
				}
				images[i].x = x;
				images[i].y = y;
				images[i].w = w;
				images[i].h = h;
				exists[i] = 1;
				found = 1;
				break;
			}
		}
		
		if (found == 0)
		{
			printf("ERROR: %s is an illegal image name.\n\n", name);
			fclose(fc);
			return 0;
		}
		
		c++;
	}
	
	fclose(fc);
	
	if (c < NUMFILES)
	{
		printf("ERROR: Only %d out of %d image files have been defined.\n\n", c, NUMFILES);
		return 0;
	}
	
	char* buffer;
	size_t result;
	
	FILE* ft;
	if (argc == 3)
	{
		ft = fopen(argv[2], "wb");
	}
	else
	{
		ft = fopen("lockdown.thm", "wb");
	}
	
	if (ft == NULL)
	{
		printf("Error opening output file.\n\n");
		return 0;
	}
	
	for (i = 0; i < NUMFILES; i++)
	{
		FILE* fp;
		fp = fopen(filenames[i], "rb");

		if (fp == NULL)
		{
			printf("Error opening %s\n\n", filenames[i]);
			fclose(fp);
			fclose(ft);
			return 0;
		}
		
		fseek(fp, 0, SEEK_END);
		images[i].size = ftell(fp);
		rewind(fp);
		
		buffer = (char*) malloc (sizeof(char)*images[i].size);
		result = fread(buffer, 1, images[i].size, fp);
		
		if (result != images[i].size)
		{
			printf("ERROR: Read error.\n\n");
			fclose(ft);
			fclose(fp);
			return 0;
		}
		
		fclose(fp);
		
		fwrite(&images[i], sizeof(image), 1, ft);
		fwrite(buffer, 1, images[i].size, ft);
		
		free(buffer);
	}
	
	fclose(ft);
	printf("Theme file written.\n\n");
	return 0;
}
