#include <pspctrl.h>
#include <pspkernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vlf.h>

PSP_MODULE_INFO("Lockdown", 0x800, 4, 1);
PSP_MAIN_THREAD_ATTR(0);

typedef struct {
	char buttons[11];
	int onlyBoot;
} Config;

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
	unsigned char* data;
} imagefile;

imagefile images[NUMFILES];
VlfPicture pics[NUMFILES];
VlfPicture masks[10];
VlfText statustext = NULL;
char st_text[128];

int selectEnabled=0;
int recovery = 0;
int done = 0;

extern SceSize vshargs;
extern void *vshargp;
extern SceUID modiop, modif, modvlf;

int getInput(char []);
void setPassword(void);

#define BACKGROUND 0
#define BUTTONS 1
#define FOOTER_CHANGEMODE 2
#define FOOTER_PRESSSELECT 3
#define MASK 4
#define MSG_NOTMATCH 5
#define MSG_PASSWORDCHANGED 6
#define MSG_PASSWORDINCORRECT 7
#define MSG_PASSWORDOK 8
#define TITLE_CONFIRMPASSWORD 9
#define TITLE_NEWPASSWORD 10
#define TITLE_OLDPASSWORD 11
#define TITLE_PASSWORD 12
#define TITLE_REQUIREPASSWORD 13

void showMask(int count)
{
	int temp = 0;

	for (temp = 0; temp < 10; temp++)
	{
		if (masks[temp] != NULL)
		{
			vlfGuiSetPictureVisibility(masks[temp], (temp < count)?1:0);
		}
	}
}

void showStatus(void)
{
	if (statustext == NULL)
	{
		statustext = vlfGuiAddText(5, 5, st_text);
	}
	else
	{
		vlfGuiSetText(statustext, st_text);
	}
}

int password_thread(SceSize args, void *argp)
{

	memset(&pics, '\0', sizeof(VlfPicture) * NUMFILES);
	memset(&masks, '\0', sizeof(VlfPicture) * 10);

	//sprintf(st_text, "%d KB Free Memory", sceKernelTotalFreeMemSize()/1024);
	//showStatus();
	loadTheme();

	//pics[BACKGROUND] = vlfGuiAddPicture(images[BACKGROUND].data, images[BACKGROUND].hdr.size, images[BACKGROUND].hdr.x, images[BACKGROUND].hdr.y);
	free(images[BACKGROUND].data);
	images[BACKGROUND].data = NULL;

	masks[0] = vlfGuiAddPicture(images[MASK].data, images[MASK].hdr.size, images[MASK].hdr.x, images[MASK].hdr.y);
	vlfGuiSetPictureVisibility(masks[0], 0);

	int temp;
	for (temp = 1; temp < 10; temp++)
	{
		masks[temp] = vlfGuiClonePicture(masks[0], 0, images[MASK].hdr.x+(temp*images[MASK].hdr.w), images[MASK].hdr.y);
		vlfGuiSetPictureVisibility(masks[temp], 0);
	}
	free(images[MASK].data);
	images[MASK].data = NULL;

	int firstUse = 0;
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
		pics[FOOTER_CHANGEMODE] = vlfGuiAddPicture(images[FOOTER_CHANGEMODE].data, images[FOOTER_CHANGEMODE].hdr.size, images[FOOTER_CHANGEMODE].hdr.x, images[FOOTER_CHANGEMODE].hdr.y);
		setPassword();
	}
	else if (((args == 0) || (recovery != 0)) || !(cfg.onlyBoot))
	{
		int len;
main_password:
		pics[FOOTER_PRESSSELECT] = vlfGuiAddPicture(images[FOOTER_PRESSSELECT].data, images[FOOTER_PRESSSELECT].hdr.size, images[FOOTER_PRESSSELECT].hdr.x, images[FOOTER_PRESSSELECT].hdr.y);
		pics[TITLE_PASSWORD] = vlfGuiAddPicture(images[TITLE_PASSWORD].data, images[TITLE_PASSWORD].hdr.size, images[TITLE_PASSWORD].hdr.x, images[TITLE_PASSWORD].hdr.y);

		while(1)
		{
			selectEnabled = 1;

			len = getInput(input);
			if(len == -1)
			{
				break;
			}
			if(strcmp(input,cfg.buttons) == 0)
			{
				pics[MSG_PASSWORDOK] = vlfGuiAddPicture(images[MSG_PASSWORDOK].data, images[MSG_PASSWORDOK].hdr.size, images[MSG_PASSWORDOK].hdr.x, images[MSG_PASSWORDOK].hdr.y);

				sceKernelDelayThread(1000*1000);
				vlfGuiRemovePicture(pics[MSG_PASSWORDOK]);
				pics[MSG_PASSWORDOK] = NULL;
				break;
			}
			else
			{
				pics[MSG_PASSWORDINCORRECT] = vlfGuiAddPicture(images[MSG_PASSWORDINCORRECT].data, images[MSG_PASSWORDINCORRECT].hdr.size, images[MSG_PASSWORDINCORRECT].hdr.x, images[MSG_PASSWORDINCORRECT].hdr.y);

				sceKernelDelayThread(3000*1000);
				vlfGuiRemovePicture(pics[MSG_PASSWORDINCORRECT]);
				pics[MSG_PASSWORDINCORRECT] = NULL;
			}
		}

		showMask(0);
		vlfGuiRemovePicture(pics[FOOTER_PRESSSELECT]);
		pics[FOOTER_PRESSSELECT] = NULL;
		vlfGuiRemovePicture(pics[TITLE_PASSWORD]);
		pics[TITLE_PASSWORD] = NULL;

		if(len == -1)
		{
			pics[FOOTER_CHANGEMODE] = vlfGuiAddPicture(images[FOOTER_CHANGEMODE].data, images[FOOTER_CHANGEMODE].hdr.size, images[FOOTER_CHANGEMODE].hdr.x, images[FOOTER_CHANGEMODE].hdr.y);
			pics[TITLE_OLDPASSWORD] = vlfGuiAddPicture(images[TITLE_OLDPASSWORD].data, images[TITLE_OLDPASSWORD].hdr.size, images[TITLE_OLDPASSWORD].hdr.x, images[TITLE_OLDPASSWORD].hdr.y);

			while(1)
			{
				len = getInput(input);
				if(len == -1)
				{
					break;
				}
				if(strcmp(input,cfg.buttons) == 0)
				{
					pics[MSG_PASSWORDOK] = vlfGuiAddPicture(images[MSG_PASSWORDOK].data, images[MSG_PASSWORDOK].hdr.size, images[MSG_PASSWORDOK].hdr.x, images[MSG_PASSWORDOK].hdr.y);

					sceKernelDelayThread(1000*1000);
					vlfGuiRemovePicture(pics[MSG_PASSWORDOK]);
					pics[MSG_PASSWORDOK] = NULL;
					break;
				}
				else
				{
					pics[MSG_PASSWORDINCORRECT] = vlfGuiAddPicture(images[MSG_PASSWORDINCORRECT].data, images[MSG_PASSWORDINCORRECT].hdr.size, images[MSG_PASSWORDINCORRECT].hdr.x, images[MSG_PASSWORDINCORRECT].hdr.y);

					sceKernelDelayThread(3000*1000);
					vlfGuiRemovePicture(pics[MSG_PASSWORDINCORRECT]);
					pics[MSG_PASSWORDINCORRECT] = NULL;
				}
			}

			showMask(0);
			vlfGuiRemovePicture(pics[TITLE_OLDPASSWORD]);
			pics[TITLE_OLDPASSWORD] = NULL;
			if(len == -1)
			{
				vlfGuiRemovePicture(pics[FOOTER_CHANGEMODE]);
				pics[FOOTER_CHANGEMODE] = NULL;
				goto main_password;
			}
			selectEnabled = 0;
			setPassword();
		}
	}

	for (temp = 0; temp < 10; temp++)
	{
		if (masks[temp] != NULL)
		{
			vlfGuiRemovePicture(masks[temp]);
			masks[temp] = NULL;
		}
	}

	//vlfGuiRemovePicture(pics[BACKGROUND]);
	//pics[BACKGROUND] = 0;

	int i;
	for (i = 0; i < NUMFILES; i++)
	{
		if (images[i].data != NULL)
			free(images[i].data);
			images[MASK].data = NULL;
	}

	done = 1;

	sceKernelStopModule(modiop, 0, NULL, NULL, NULL);
	sceKernelUnloadModule(modiop);

	sceKernelStopModule(modif, 0, NULL, NULL, NULL);
	sceKernelUnloadModule(modif);

	sceKernelStopModule(modvlf, 0, NULL, NULL, NULL);
	sceKernelUnloadModule(modvlf);

	SceUID mod = sceKernelLoadModule("flash0:/loader.prx", 0, NULL);
	sceKernelStartModule(mod, args, argp, NULL, NULL);

	return sceKernelExitDeleteThread(0);
}

int getInput(char* input)
{
	int old;
	int pos=0;
	int skipped=0;
	SceCtrlLatch latch;
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);
	showMask(0);

	while(1)
	{
		old = pos;
		sceCtrlReadLatch(&latch);

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

		if (pos > old)
		{
			showMask(pos);
		}

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

	pics[TITLE_NEWPASSWORD] = vlfGuiAddPicture(images[TITLE_NEWPASSWORD].data, images[TITLE_NEWPASSWORD].hdr.size, images[TITLE_NEWPASSWORD].hdr.x, images[TITLE_NEWPASSWORD].hdr.y);

	while(1)
	{
		vlfGuiSetPictureVisibility(pics[TITLE_NEWPASSWORD], 1);
		getInput(input);
		vlfGuiSetPictureVisibility(pics[TITLE_NEWPASSWORD], 0);

		pics[TITLE_CONFIRMPASSWORD] = vlfGuiAddPicture(images[TITLE_CONFIRMPASSWORD].data, images[TITLE_CONFIRMPASSWORD].hdr.size, images[TITLE_CONFIRMPASSWORD].hdr.x, images[TITLE_CONFIRMPASSWORD].hdr.y);

		len = getInput(cfg.buttons);

		if((strcmp(input, cfg.buttons) == 0) && (len >= 0))
		{
			vlfGuiRemovePicture(pics[TITLE_NEWPASSWORD]);
			pics[TITLE_NEWPASSWORD] = NULL;

			pics[MSG_PASSWORDCHANGED] = vlfGuiAddPicture(images[MSG_PASSWORDCHANGED].data, images[MSG_PASSWORDCHANGED].hdr.size, images[MSG_PASSWORDCHANGED].hdr.x, images[MSG_PASSWORDCHANGED].hdr.y);

			sceKernelDelayThread(3000*1000);
			showMask(0);
			vlfGuiRemovePicture(pics[MSG_PASSWORDCHANGED]);
			pics[MSG_PASSWORDCHANGED] = NULL;
			vlfGuiRemovePicture(pics[TITLE_CONFIRMPASSWORD]);
			pics[TITLE_CONFIRMPASSWORD] = NULL;

			pics[TITLE_REQUIREPASSWORD] = vlfGuiAddPicture(images[TITLE_REQUIREPASSWORD].data, images[TITLE_REQUIREPASSWORD].hdr.size, images[TITLE_REQUIREPASSWORD].hdr.x, images[TITLE_REQUIREPASSWORD].hdr.y);
			pics[BUTTONS] = vlfGuiAddPicture(images[BUTTONS].data, images[BUTTONS].hdr.size, images[BUTTONS].hdr.x, images[BUTTONS].hdr.y);

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
			vlfGuiRemovePicture(pics[TITLE_REQUIREPASSWORD]);
			pics[TITLE_REQUIREPASSWORD] = NULL;
			vlfGuiRemovePicture(pics[BUTTONS]);
			pics[BUTTONS] = NULL;
			break;
		}
		else
		{
			pics[MSG_NOTMATCH] = vlfGuiAddPicture(images[MSG_NOTMATCH].data, images[MSG_NOTMATCH].hdr.size, images[MSG_NOTMATCH].hdr.x, images[MSG_NOTMATCH].hdr.y);

			sceKernelDelayThread(3000*1000);
			vlfGuiRemovePicture(pics[MSG_NOTMATCH]);
			pics[MSG_NOTMATCH] = NULL;
			vlfGuiRemovePicture(pics[TITLE_CONFIRMPASSWORD]);
			pics[TITLE_CONFIRMPASSWORD] = NULL;
		}
	}

	vlfGuiRemovePicture(pics[FOOTER_CHANGEMODE]);
	pics[FOOTER_CHANGEMODE] = NULL;

	int result;
	result = sceIoUnassign("flash0:");

	if (result < 0)
	{
		sprintf(st_text, "Unassign flash0: failed.");
		showStatus();
		sceKernelDelayThread(1000*1000);
	}
	else
	{
		result = sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);
		if (result < 0)
		{
			sprintf(st_text, "Assign flash0: for writing failed.");
			showStatus();
			sceKernelDelayThread(1000*1000);
		}
		else
		{
			SceUID fp;
			fp = sceIoOpen("flash0:/buttons.ini", PSP_O_WRONLY|PSP_O_TRUNC|PSP_O_CREAT, 0777);

			if (fp < 0)
			{
				sprintf(st_text, "Failed to open flash0:/buttons.ini for writing.");
				showStatus();
				sceKernelDelayThread(1000*1000);
			}
			else
			{
				sceIoWrite(fp, &cfg, sizeof(cfg));
				sceIoClose(fp);
			}
		}
	}
}

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
			sprintf(st_text, "flash0:/lockdown.thm and ms0:/lockdown.thm not found.");
			showStatus();
			sceKernelDelayThread(1000*1000);
			asm("break\n");
			while (1);
		}
	}

	for (i = 0; i < NUMFILES; i++)
	{
		bytesRead = sceIoRead(fp, &images[i].hdr, sizeof(imagehdr));
		if (bytesRead != sizeof(imagehdr))
		{
			sprintf(st_text, "Premature end of header in item %d.", i);
			showStatus();
			sceKernelDelayThread(1000*1000);
			asm("break\n");
			while (1);
		}

		images[i].data = (unsigned char*) malloc(sizeof(unsigned char*) * images[i].hdr.size);
		if (!images[i].data)
		{
			sprintf(st_text, "Memory allocation error in item %d.", i);
			showStatus();
			sceKernelDelayThread(1000*1000);
			asm("break\n");
			while (1);
		}

		bytesRead = sceIoRead(fp, images[i].data, images[i].hdr.size);
		if (bytesRead != images[i].hdr.size)
		{
			sprintf(st_text, "Premature end of data in item %d.", i);
			showStatus();
			sceKernelDelayThread(1000*1000);
			asm("break\n");
			while (1);
		}
	}

	sceIoClose(fp);
}

int app_main(int argc, char *argv[])
{
	vlfGuiSystemSetup(1, 1, 1);
	vlfGuiSetBackgroundIndex(8);

	SceUID th;
	th = sceKernelCreateThread("password_thread", password_thread, 0x10, 0x4000, 0, NULL);

	if (th >= 0)
	{
		sceKernelStartThread(th, vshargs, vshargp);
	}

	while (!done)
	{
		vlfGuiDrawFrame();
	}

	sceKernelSleepThread();
   	return 0;
}
