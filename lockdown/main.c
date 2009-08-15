#include <pspctrl.h>
#include <pspkernel.h>
#include <pspthreadman.h>
#include <string.h>

//#define DEBUG 1

#ifdef DEBUG
#include <pspdebug.h>
#define printf pspDebugScreenPrintf
#else
#include <pspdisplay.h>
#include <pspgu.h>
#include "graphics.h"
#endif

PSP_MODULE_INFO("Lockdown", 0x800, 4, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);
PSP_HEAP_SIZE_KB(2048);

int __psp_free_heap(void);
int getInput(char []);
void setPassword(void);

typedef struct {
	char buttons[11];
	int onlyBoot;
} Config;

#ifndef DEBUG
void loadTheme(void);

#define NUMFILES 14
typedef struct
{
	int x,y,w,h;
	unsigned int size;
} imagehdr;

typedef struct
{
	imagehdr hdr;
	SceUID blockid;
	unsigned char* data;
} imagefile;

imagefile images[NUMFILES];

#define BACKGROUND 0
Image* background;
#define BUTTONS 1
Image* buttons;
#define FOOTER_CHANGEMODE 2
Image* footer_changemode;
#define FOOTER_PRESSSELECT 3
Image* footer_pressselect;
#define MASK 4
Image* mask;
#define MSG_NOTMATCH 5
Image* msg_notmatch;
#define MSG_PASSWORDCHANGED 6
Image* msg_passwordchanged;
#define MSG_PASSWORDINCORRECT 7
Image* msg_passwordincorrect;
#define MSG_PASSWORDOK 8
Image* msg_passwordok;
#define TITLE_CONFIRMPASSWORD 9
Image* title_confirmpassword;
#define TITLE_NEWPASSWORD 10
Image* title_newpassword;
#define TITLE_OLDPASSWORD 11
Image* title_oldpassword;
#define TITLE_PASSWORD 12
Image* title_password;
#define TITLE_REQUIREPASSWORD 13
Image* title_requirepassword;

Image** title;
unsigned int titleindex;
Image** footer;
unsigned int footerindex;
#endif

int selectEnabled=0;

int main_thread(SceSize args, void *argp)
{
	#ifdef DEBUG
	pspDebugScreenInit();
	printf("Free Memory: %u KB\n",sceKernelTotalFreeMemSize()/1024);
	#else
	initGraphics();
	loadTheme();
	background = loadImageFromMemory(images[BACKGROUND].data, images[BACKGROUND].hdr.size);
	sceKernelFreePartitionMemory(images[BACKGROUND].blockid);
	images[BACKGROUND].blockid = -1;
	mask = loadImageFromMemory(images[MASK].data, images[MASK].hdr.size);
	sceKernelFreePartitionMemory(images[MASK].blockid);
	images[MASK].blockid = -1;
	#endif

	int firstUse = 0;
	int recovery = 0;
	int bytesRead = 0;
	Config cfg;
	char input[11];

	if ((args == 9) && (strcmp(argp, "recovery") == 0))
	{
		recovery = 1;
	}

	SceUID fp;
	fp = sceIoOpen("flash0:/buttons.ini", PSP_O_RDONLY, 0777);

	if (fp < 0)
	{
		#ifdef DEBUG
		printf("\nflash0:/buttons.ini could not be opned. Assuming first usage.");
		#endif
		firstUse = 1;
	}
	else
	{
		bytesRead = sceIoRead(fp, &cfg, sizeof(cfg));
		sceIoClose(fp);

		if(bytesRead != sizeof(cfg))
		{
			firstUse = 1;
		}
	}

	if (firstUse)
	{
		setPassword();
	}
	else if (((args == 0) || (recovery != 0)) || !(cfg.onlyBoot))
	{
		int len;
main_password:
		#ifndef DEBUG
		footer_pressselect = loadImageFromMemory(images[FOOTER_PRESSSELECT].data, images[FOOTER_PRESSSELECT].hdr.size);
		title_password = loadImageFromMemory(images[TITLE_PASSWORD].data, images[TITLE_PASSWORD].hdr.size);
		#endif
		while(1)
		{
			selectEnabled = 1;

			#ifdef DEBUG
			printf("\nPress START to accept.\nPress SELECT to change password.\nEnter password: ");
			#else
			blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
			blitAlphaImageToScreen(0, 0, images[FOOTER_PRESSSELECT].hdr.w, images[FOOTER_PRESSSELECT].hdr.h, footer_pressselect, images[FOOTER_PRESSSELECT].hdr.x, images[FOOTER_PRESSSELECT].hdr.y);
			blitAlphaImageToScreen(0, 0, images[TITLE_PASSWORD].hdr.w, images[TITLE_PASSWORD].hdr.h, title_password, images[TITLE_PASSWORD].hdr.x, images[TITLE_PASSWORD].hdr.y);
			sceDisplayWaitVblankStart();
			flipScreen();

			title = &title_password;
			titleindex = TITLE_PASSWORD;
			footer = &footer_pressselect;
			footerindex = FOOTER_PRESSSELECT;
			#endif
			len = getInput(input);
			if(len == -1)
			{
				break;
			}
			if(strcmp(input,cfg.buttons) == 0)
			{
				#ifdef DEBUG
				printf("\nPassword OK.");
				#else
				int temp;
				blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
				blitAlphaImageToScreen(0, 0, images[FOOTER_PRESSSELECT].hdr.w, images[FOOTER_PRESSSELECT].hdr.h, footer_pressselect, images[FOOTER_PRESSSELECT].hdr.x, images[FOOTER_PRESSSELECT].hdr.y);
				blitAlphaImageToScreen(0, 0, images[TITLE_PASSWORD].hdr.w, images[TITLE_PASSWORD].hdr.h, title_password, images[TITLE_PASSWORD].hdr.x, images[TITLE_PASSWORD].hdr.y);
				msg_passwordok = loadImageFromMemory(images[MSG_PASSWORDOK].data, images[MSG_PASSWORDOK].hdr.size);
				blitAlphaImageToScreen(0, 0, images[MSG_PASSWORDOK].hdr.w, images[MSG_PASSWORDOK].hdr.h, msg_passwordok, images[MSG_PASSWORDOK].hdr.x, images[MSG_PASSWORDOK].hdr.y);
				freeImage(msg_passwordok);
				for(temp = 0; temp < len; temp++)
				{
					blitAlphaImageToScreen(0, 0, images[MASK].hdr.w, images[MASK].hdr.h, mask, images[MASK].hdr.x+(temp*images[MASK].hdr.w), images[MASK].hdr.y);
				}
				sceDisplayWaitVblankStart();
				flipScreen();
				#endif
				sceKernelDelayThread(1000*1000);
				break;
			}
			else
			{
				#ifdef DEBUG
				printf("\nIncorrect password.");
				#else
				int temp;
				blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
				blitAlphaImageToScreen(0, 0, images[FOOTER_PRESSSELECT].hdr.w, images[FOOTER_PRESSSELECT].hdr.h, footer_pressselect, images[FOOTER_PRESSSELECT].hdr.x, images[FOOTER_PRESSSELECT].hdr.y);
				blitAlphaImageToScreen(0, 0, images[TITLE_PASSWORD].hdr.w, images[TITLE_PASSWORD].hdr.h, title_password, images[TITLE_PASSWORD].hdr.x, images[TITLE_PASSWORD].hdr.y);
				msg_passwordincorrect = loadImageFromMemory(images[MSG_PASSWORDINCORRECT].data, images[MSG_PASSWORDINCORRECT].hdr.size);
				blitAlphaImageToScreen(0, 0, images[MSG_PASSWORDINCORRECT].hdr.w, images[MSG_PASSWORDINCORRECT].hdr.h, msg_passwordincorrect, images[MSG_PASSWORDINCORRECT].hdr.x, images[MSG_PASSWORDINCORRECT].hdr.y);
				freeImage(msg_passwordincorrect);
				for(temp = 0; temp < len; temp++)
				{
					blitAlphaImageToScreen(0, 0, images[MASK].hdr.w, images[MASK].hdr.h, mask, images[MASK].hdr.x+(temp*images[MASK].hdr.w), images[MASK].hdr.y);
				}
				sceDisplayWaitVblankStart();
				flipScreen();
				#endif
				sceKernelDelayThread(3000*1000);
			}
		}
		#ifndef DEBUG
		freeImage(footer_pressselect);
		freeImage(title_password);
		#endif

		if(len == -1)
		{
			#ifndef DEBUG
			footer_changemode = loadImageFromMemory(images[FOOTER_CHANGEMODE].data, images[FOOTER_CHANGEMODE].hdr.size);
			title_oldpassword = loadImageFromMemory(images[TITLE_OLDPASSWORD].data, images[TITLE_OLDPASSWORD].hdr.size);
			#endif
			while(1)
			{
				#ifdef DEBUG
				printf("\nChange password mode.\nEnter old password: ");
				#else
				blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
				blitAlphaImageToScreen(0, 0, images[FOOTER_CHANGEMODE].hdr.w, images[FOOTER_CHANGEMODE].hdr.h, footer_changemode, images[FOOTER_CHANGEMODE].hdr.x, images[FOOTER_CHANGEMODE].hdr.y);
				blitAlphaImageToScreen(0, 0, images[TITLE_OLDPASSWORD].hdr.w, images[TITLE_OLDPASSWORD].hdr.h, title_oldpassword, images[TITLE_OLDPASSWORD].hdr.x, images[TITLE_OLDPASSWORD].hdr.y);
				sceDisplayWaitVblankStart();
				flipScreen();

				title = &title_oldpassword;
				titleindex = TITLE_OLDPASSWORD;
				footer = &footer_changemode;
				footerindex = FOOTER_CHANGEMODE;
				#endif
				len = getInput(input);
				if(len == -1)
				{
					break;
				}
				if(strcmp(input,cfg.buttons) == 0)
				{
					#ifdef DEBUG
					printf("\nPassword OK.");
					#else
					int temp;
					blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
					blitAlphaImageToScreen(0, 0, images[FOOTER_CHANGEMODE].hdr.w, images[FOOTER_CHANGEMODE].hdr.h, footer_changemode, images[FOOTER_CHANGEMODE].hdr.x, images[FOOTER_CHANGEMODE].hdr.y);
					blitAlphaImageToScreen(0, 0, images[TITLE_OLDPASSWORD].hdr.w, images[TITLE_OLDPASSWORD].hdr.h, title_oldpassword, images[TITLE_OLDPASSWORD].hdr.x, images[TITLE_OLDPASSWORD].hdr.y);
					msg_passwordok = loadImageFromMemory(images[MSG_PASSWORDOK].data, images[MSG_PASSWORDOK].hdr.size);
					blitAlphaImageToScreen(0, 0, images[MSG_PASSWORDOK].hdr.w, images[MSG_PASSWORDOK].hdr.h, msg_passwordok, images[MSG_PASSWORDOK].hdr.x, images[MSG_PASSWORDOK].hdr.y);
					freeImage(msg_passwordok);
					for(temp = 0; temp < len; temp++)
					{
						blitAlphaImageToScreen(0, 0, images[MASK].hdr.w, images[MASK].hdr.h, mask, images[MASK].hdr.x+(temp*images[MASK].hdr.w), images[MASK].hdr.y);
					}
					sceDisplayWaitVblankStart();
					flipScreen();
					#endif
					sceKernelDelayThread(3000*1000);
					break;
				}
				else
				{
					#ifdef DEBUG
					printf("\nIncorrect password.");
					#else
					int temp;
					blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
					blitAlphaImageToScreen(0, 0, images[FOOTER_CHANGEMODE].hdr.w, images[FOOTER_CHANGEMODE].hdr.h, footer_changemode, images[FOOTER_CHANGEMODE].hdr.x, images[FOOTER_CHANGEMODE].hdr.y);
					blitAlphaImageToScreen(0, 0, images[TITLE_OLDPASSWORD].hdr.w, images[TITLE_OLDPASSWORD].hdr.h, title_oldpassword, images[TITLE_OLDPASSWORD].hdr.x, images[TITLE_OLDPASSWORD].hdr.y);
					msg_passwordincorrect = loadImageFromMemory(images[MSG_PASSWORDINCORRECT].data, images[MSG_PASSWORDINCORRECT].hdr.size);
					blitAlphaImageToScreen(0, 0, images[MSG_PASSWORDINCORRECT].hdr.w, images[MSG_PASSWORDINCORRECT].hdr.h, msg_passwordincorrect, images[MSG_PASSWORDINCORRECT].hdr.x, images[MSG_PASSWORDINCORRECT].hdr.y);
					freeImage(msg_passwordincorrect);
					for(temp = 0; temp < len; temp++)
					{
						blitAlphaImageToScreen(0, 0, images[MASK].hdr.w, images[MASK].hdr.h, mask, images[MASK].hdr.x+(temp*images[MASK].hdr.w), images[MASK].hdr.y);
					}
					sceDisplayWaitVblankStart();
					flipScreen();
					#endif
					sceKernelDelayThread(3000*1000);
				}
			}
			#ifndef DEBUG
			freeImage(footer_changemode);
			freeImage(title_oldpassword);
			#endif
			if(len == -1)
			{
				goto main_password;
			}
			selectEnabled = 0;
			setPassword();
		}
	}

	#ifndef DEBUG
	freeImage(background);
	freeImage(mask);
	sceGuTerm();
	int i;
	for (i = 0; i < NUMFILES; i++)
	{
		if (images[i].blockid != -1)
		{
			sceKernelFreePartitionMemory(images[i].blockid);
		}
	}
	#endif

	__psp_free_heap();
	#ifdef DEBUG
	printf("\n__psp_free_heap(): %u KB\n",sceKernelTotalFreeMemSize()/1024);
	sceKernelDelayThread(3000*1000);
	#endif

	#ifdef DEBUG
	printf("\nLoading loader.prx");
	#endif

	SceUID mod = sceKernelLoadModule("flash0:/loader.prx", 0, NULL);

	if (mod & 0x80000000)
	{
		#ifdef DEBUG
		printf("\nLoadModule failed 0x%x", mod);
		#endif
	}
	else
	{
		SceUID startmod;
		startmod = sceKernelStartModule(mod, args, argp, NULL, NULL);

		if (mod != startmod)
		{
			#ifdef DEBUG
			printf("\nStartModule failed 0x%x", startmod);
			#endif
		}
	}

	return sceKernelExitDeleteThread(0);
}

int getInput(char* input)
{
	int pos=0;
	int temp;
	int skipped=0;
	SceCtrlLatch latch;
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

	while(1)
	{
		sceCtrlReadLatch(&latch);
		temp=pos;
		if(!skipped)
		{
			skipped=1;
			continue;
		}

		if ((latch.uiMake & PSP_CTRL_SELECT) && selectEnabled)
		{
			input[pos] = '\0';
			return -1;
		}
		if (latch.uiMake & PSP_CTRL_START)
		{
			break;
		}
		if (pos >=10)
		{
			sceKernelDelayThread(10*1000);
			continue;
		}
		if (latch.uiMake & PSP_CTRL_TRIANGLE)
		{
			input[pos++] = 1;
		}
		if (latch.uiMake & PSP_CTRL_CROSS)
		{
			input[pos++] = 2;
		}
		if (latch.uiMake & PSP_CTRL_SQUARE)
		{
			input[pos++] = 3;
		}
		if (latch.uiMake & PSP_CTRL_CIRCLE)
		{
			input[pos++] = 4;
		}
		if (latch.uiMake & PSP_CTRL_UP)
		{
			input[pos++] = 5;
		}
		if (latch.uiMake & PSP_CTRL_DOWN)
		{
			input[pos++] = 6;
		}
		if (latch.uiMake & PSP_CTRL_LEFT)
		{
			input[pos++] = 7;
		}
		if (latch.uiMake & PSP_CTRL_RIGHT)
		{
			input[pos++] = 8;
		}
		if (latch.uiMake & PSP_CTRL_LTRIGGER)
		{
			input[pos++] = 9;
		}
		if (latch.uiMake & PSP_CTRL_RTRIGGER)
		{
			input[pos++] = 10;
		}

		if(pos>10)
		{
			pos=10;
		}

		#ifdef DEBUG
		if (pos > temp)
		{
			printf("*");
		}
		#else
		blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
		blitAlphaImageToScreen(0, 0, images[footerindex].hdr.w, images[footerindex].hdr.h, *footer, images[footerindex].hdr.x, images[footerindex].hdr.y);
		blitAlphaImageToScreen(0, 0, images[titleindex].hdr.w, images[titleindex].hdr.h, *title, images[titleindex].hdr.x, images[titleindex].hdr.y);
		for (temp = 0; temp < pos; temp++)
		{
			blitAlphaImageToScreen(0, 0, images[MASK].hdr.w, images[MASK].hdr.h, mask, images[MASK].hdr.x+(temp*images[MASK].hdr.w), images[MASK].hdr.y);
		}
		sceDisplayWaitVblankStart();
		flipScreen();
		#endif

		sceKernelDelayThread(10*1000);
	}

	input[pos] = '\0';

	return pos;
}

void setPassword()
{
	Config cfg;
	char input[11];
	int len;

	#ifndef DEBUG
	footer_changemode = loadImageFromMemory(images[FOOTER_CHANGEMODE].data, images[FOOTER_CHANGEMODE].hdr.size);
	#endif
	while(1)
	{
		#ifdef DEBUG
		printf("\nEnter new password: ");
		#else
		blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
		blitAlphaImageToScreen(0, 0, images[FOOTER_CHANGEMODE].hdr.w, images[FOOTER_CHANGEMODE].hdr.h, footer_changemode, images[FOOTER_CHANGEMODE].hdr.x, images[FOOTER_CHANGEMODE].hdr.y);
		title_newpassword = loadImageFromMemory(images[TITLE_NEWPASSWORD].data, images[TITLE_NEWPASSWORD].hdr.size);
		blitAlphaImageToScreen(0, 0, images[TITLE_NEWPASSWORD].hdr.w, images[TITLE_NEWPASSWORD].hdr.h, title_newpassword, images[TITLE_NEWPASSWORD].hdr.x, images[TITLE_NEWPASSWORD].hdr.y);
		sceDisplayWaitVblankStart();
		flipScreen();

		title = &title_newpassword;
		titleindex = TITLE_NEWPASSWORD;
		footer = &footer_changemode;
		footerindex = FOOTER_CHANGEMODE;
		#endif
		getInput(input);
		#ifndef DEBUG
		freeImage(title_newpassword);
		#endif

		#ifdef DEBUG
		printf("\nConfirm password: ");
		#else
		blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
		blitAlphaImageToScreen(0, 0, images[FOOTER_CHANGEMODE].hdr.w, images[FOOTER_CHANGEMODE].hdr.h, footer_changemode, images[FOOTER_CHANGEMODE].hdr.x, images[FOOTER_CHANGEMODE].hdr.y);
		title_confirmpassword = loadImageFromMemory(images[TITLE_CONFIRMPASSWORD].data, images[TITLE_CONFIRMPASSWORD].hdr.size);
		blitAlphaImageToScreen(0, 0, images[TITLE_CONFIRMPASSWORD].hdr.w, images[TITLE_CONFIRMPASSWORD].hdr.h, title_confirmpassword, images[TITLE_CONFIRMPASSWORD].hdr.x, images[TITLE_CONFIRMPASSWORD].hdr.y);
		sceDisplayWaitVblankStart();
		flipScreen();

		title = &title_confirmpassword;
		titleindex = TITLE_CONFIRMPASSWORD;
		#endif
		len = getInput(cfg.buttons);

		if((strcmp(input, cfg.buttons) == 0) && (len >= 0))
		{
			#ifdef DEBUG
			printf("\nPassword changed.");
			#else
			int temp;
			blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
			blitAlphaImageToScreen(0, 0, images[FOOTER_CHANGEMODE].hdr.w, images[FOOTER_CHANGEMODE].hdr.h, footer_changemode, images[FOOTER_CHANGEMODE].hdr.x, images[FOOTER_CHANGEMODE].hdr.y);
			blitAlphaImageToScreen(0, 0, images[TITLE_CONFIRMPASSWORD].hdr.w, images[TITLE_CONFIRMPASSWORD].hdr.h, title_confirmpassword, images[TITLE_CONFIRMPASSWORD].hdr.x, images[TITLE_CONFIRMPASSWORD].hdr.y);
			freeImage(title_confirmpassword);
			msg_passwordchanged = loadImageFromMemory(images[MSG_PASSWORDCHANGED].data, images[MSG_PASSWORDCHANGED].hdr.size);
			blitAlphaImageToScreen(0, 0, images[MSG_PASSWORDCHANGED].hdr.w, images[MSG_PASSWORDCHANGED].hdr.h, msg_passwordchanged, images[MSG_PASSWORDCHANGED].hdr.x, images[MSG_PASSWORDCHANGED].hdr.y);
			freeImage(msg_passwordchanged);
			for(temp = 0; temp < len; temp++)
			{
				blitAlphaImageToScreen(0, 0, images[MASK].hdr.w, images[MASK].hdr.h, mask, images[MASK].hdr.x+(temp*images[MASK].hdr.w), images[MASK].hdr.y);
			}
			sceDisplayWaitVblankStart();
			flipScreen();
			#endif
			sceKernelDelayThread(3000*1000);

			#ifdef DEBUG
			printf("\n\nPress X to require password only at reboot.\n\nPress O to require password always at XMB.");
			#else
			blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
			blitAlphaImageToScreen(0, 0, images[FOOTER_CHANGEMODE].hdr.w, images[FOOTER_CHANGEMODE].hdr.h, footer_changemode, images[FOOTER_CHANGEMODE].hdr.x, images[FOOTER_CHANGEMODE].hdr.y);
			title_requirepassword = loadImageFromMemory(images[TITLE_REQUIREPASSWORD].data, images[TITLE_REQUIREPASSWORD].hdr.size);
			blitAlphaImageToScreen(0, 0, images[TITLE_REQUIREPASSWORD].hdr.w, images[TITLE_REQUIREPASSWORD].hdr.h, title_requirepassword, images[TITLE_REQUIREPASSWORD].hdr.x, images[TITLE_REQUIREPASSWORD].hdr.y);
			freeImage(title_requirepassword);
			buttons = loadImageFromMemory(images[BUTTONS].data, images[BUTTONS].hdr.size);
			blitAlphaImageToScreen(0, 0, images[BUTTONS].hdr.w, images[BUTTONS].hdr.h, buttons, images[BUTTONS].hdr.x, images[BUTTONS].hdr.y);
			freeImage(buttons);
			sceDisplayWaitVblankStart();
			flipScreen();
			#endif

			SceCtrlLatch latch;
			sceCtrlSetSamplingCycle(0);
			sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

			int skipped=0;
			while(1)
			{
				sceCtrlReadLatch(&latch);

				if(!skipped)
				{
					skipped=1;
					continue;
				}
				if (latch.uiMake & PSP_CTRL_CROSS)
				{
					cfg.onlyBoot = 1;
					break;
				}
				if (latch.uiMake & PSP_CTRL_CIRCLE)
				{
					cfg.onlyBoot = 0;
					break;
				}
				sceKernelDelayThread(10*1000);
			}

			sceKernelDelayThread(2000*1000);
			break;
		}
		else
		{
			#ifdef DEBUG
			printf("\nPasswords do not match.");
			#else
			int temp;
			blitImageToScreen(0, 0, images[BACKGROUND].hdr.w, images[BACKGROUND].hdr.h, background, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
			blitAlphaImageToScreen(0, 0, images[FOOTER_CHANGEMODE].hdr.w, images[FOOTER_CHANGEMODE].hdr.h, footer_changemode, images[FOOTER_CHANGEMODE].hdr.x, images[FOOTER_CHANGEMODE].hdr.y);
			blitAlphaImageToScreen(0, 0, images[TITLE_CONFIRMPASSWORD].hdr.w, images[TITLE_CONFIRMPASSWORD].hdr.h, title_confirmpassword, images[TITLE_CONFIRMPASSWORD].hdr.x, images[TITLE_CONFIRMPASSWORD].hdr.y);
			freeImage(title_confirmpassword);
			msg_notmatch = loadImageFromMemory(images[MSG_NOTMATCH].data, images[MSG_NOTMATCH].hdr.size);
			blitAlphaImageToScreen(0, 0, images[MSG_NOTMATCH].hdr.w, images[MSG_NOTMATCH].hdr.h, msg_notmatch, images[MSG_NOTMATCH].hdr.x, images[MSG_NOTMATCH].hdr.y);
			freeImage(msg_notmatch);
			for(temp = 0; temp < len; temp++)
			{
				blitAlphaImageToScreen(0, 0, images[MASK].hdr.w, images[MASK].hdr.h, mask, images[MASK].hdr.x+(temp*images[MASK].hdr.w), images[MASK].hdr.y);
			}
			sceDisplayWaitVblankStart();
			flipScreen();
			#endif
			sceKernelDelayThread(3000*1000);
		}
	}
	#ifndef DEBUG
	freeImage(footer_changemode);
	#endif

	int result;
	result = sceIoUnassign("flash0:");

	if(result < 0)
	{
		#ifdef DEBUG
		printf("\nError in unassign flash0.");
		#endif
	}
	else
	{
		result = sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);
		if(result < 0)
		{
			#ifdef DEBUG
			printf("\nError in assigning flash0 for write.");
			#endif
		}
		else
		{
			SceUID fp;
			fp = sceIoOpen("flash0:/buttons.ini", PSP_O_WRONLY|PSP_O_TRUNC|PSP_O_CREAT, 0777);

			if(fp < 0)
			{
				#ifdef DEBUG
				printf("\nError writing flash0:/buttons.ini.");
				#endif
			}
			else
			{
				sceIoWrite(fp, &cfg, sizeof(cfg));
				sceIoClose(fp);
				#ifdef DEBUG
				printf("\nPassword written successfully.");
				#endif
			}
		}
	}
}

#ifndef DEBUG
void loadTheme()
{
	int i, bytesRead;

	SceUID fp;
	fp = sceIoOpen("ms0:/lockdown.thm", PSP_O_RDONLY, 0777);

	if (fp < 0)
	{
		fp = sceIoOpen("flash0:/lockdown.thm", PSP_O_RDONLY, 0777);

		if (fp < 0)
		{
			printTextScreen(0, 0, "Error loading flash0:/lockdown.thm.", RGB(255, 0, 0));
			printTextScreen(0, 8, "Place theme at ms0:/lockdown.thm to load from memory stick.", RGB(255, 0, 0));
			sceDisplayWaitVblankStart();
			flipScreen();
			sceKernelSleepThread();
		}
	}

	for (i = 0; i < NUMFILES; i++)
	{
		bytesRead = sceIoRead(fp, &images[i].hdr, sizeof(imagehdr));
		if (bytesRead != sizeof(imagehdr))
		{
			printTextScreen(0, 0, "Unexpected end of header.", RGB(255, 0, 0));
			sceDisplayWaitVblankStart();
			flipScreen();
			sceKernelSleepThread();
		}
		images[i].blockid = sceKernelAllocPartitionMemory(2, "block", 0, (sizeof(unsigned char) * images[i].hdr.size), NULL);
		if (images[i].blockid < 0)
		{
			printTextScreen(0, 0, "Memory allocation error.", RGB(255, 0, 0));
			sceDisplayWaitVblankStart();
			flipScreen();
			sceKernelSleepThread();
		}
		images[i].data = (unsigned char*) sceKernelGetBlockHeadAddr(images[i].blockid);
		bytesRead = sceIoRead(fp, images[i].data, images[i].hdr.size);
		if (bytesRead != images[i].hdr.size)
		{
			printTextScreen(0, 0, "Unexpected end of data.", RGB(255, 0, 0));
			sceDisplayWaitVblankStart();
			flipScreen();
			sceKernelSleepThread();
		}
	}

	sceIoClose(fp);
}
#endif

int module_start(SceSize args, void *argp)
{
	SceUID th;
	th = sceKernelCreateThread("main_thread", main_thread, 0x11, 0x1000, 0, NULL);

	if (th >= 0)
	{
		sceKernelStartThread(th, args, argp);
	}

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	__psp_free_heap();
	return 0;
}
