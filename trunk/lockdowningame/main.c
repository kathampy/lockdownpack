#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspthreadman.h>
#include <psppower.h>
#include <systemctrl.h>
#include <string.h>

#include "enter.c"
#include "incorrect.c"
#include "mask.c"
#include "password.c"

//#define LOG

PSP_MODULE_INFO("LockdownGame", 0x1000, 2, 0);
PSP_MAIN_THREAD_ATTR(0);

void suspendThreads(void);
void resumeThreads(void);
void drawimage(unsigned char *, unsigned int, unsigned int, unsigned int, unsigned int);
void readimage(unsigned char *, unsigned int, unsigned int, unsigned int, unsigned int);
void restoreimage(unsigned char *, unsigned int, unsigned int, unsigned int, unsigned int);
unsigned int getInput(char *);

#define MAX_THREAD	64

char buttons[11];

unsigned char mask_bg[mask_w * mask_h * 4 * 10];
unsigned char password_bg[password_w * password_h * 4];
unsigned char incorrect_bg[incorrect_w * incorrect_h * 4];

int thread_count_start, thread_count_now;
SceUID thread_buf_start[MAX_THREAD], thread_buf_now[MAX_THREAD], cbThread, wkThread;
int pmode, pwidth, pheight, bufferwidth, pixelformat;
unsigned int* vram32;
unsigned short* vram16;

unsigned char cbdisabled = 0, locked = 0;
int scePowerRegisterCallback_Patched (int, SceUID);
int scePowerUnregitserCallback_Patched (int);
int scePowerUnregisterCallback_Patched (int);
int sceKernelRegisterExitCallback_Patched (int);
int sceKernelUnregisterExitCallback (int);
SceUID cblist[16], exit_cblist[16];
int flags_resuming, flags_resume_complete;

int scePowerRegisterCallback_Patched (int slot, SceUID cbid)
{
	int i, ret = -1;
	
	if (slot == -1)
	{
		for (i = 0; i <= 15; i++)
		{
			if (cblist[i] == 0)
			{
				cblist[i] = cbid;
				ret = i;
				break;
			}
		}
	}
	else if ((slot >= 0) && (slot <= 15))
	{
		if (cblist[slot] == 0)
		{
			cblist[slot] = cbid;
			ret = 0;
		}
	}
	
	return ret;
}

int scePowerUnregitserCallback_Patched (int slot)
{
	if (cblist[slot] == 0)
	{
		return -1;
	}
	else
	{
		cblist[slot] = 0;
		return 0;
	}
}

int scePowerUnregisterCallback_Patched (int slot)
{
	return scePowerUnregitserCallback_Patched(slot);
}

int sceKernelRegisterExitCallback_Patched (int cbid)
{
	int i, ret = -1;
	for (i = 0; i <= 15; i++)
	{
		if (exit_cblist[i] == 0)
		{
			ret = sceKernelRegisterExitCallback(cbid);
			if (ret >= 0)
			{
				exit_cblist[i] = cbid;
			}
			break;
		}
	}
	
	return ret;
}


void drawimage(unsigned char * image, unsigned int srcw, unsigned int srch, unsigned int destx, unsigned int desty)
{
	#define sbpp 4
	unsigned int x, y, pos = 0;
	
	sceDisplayWaitVblankStart();
	
	for(y = desty; y < desty+srch; y++){
		for(x = destx; x < destx+srcw; x++){
			unsigned int color, offset = x + y*bufferwidth;
			unsigned char r = 0, g = 0, b = 0;
			
			switch (pixelformat) {
				case 0:	// 16-bit RGB 5:6:5
					color = vram16[offset];
					r = (color & 0x1f) << 3;
					g = ((color >> 5) & 0x3f) << 2;
					b = ((color >> 11) & 0x1f) << 3;
					r = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*r + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp]) >> 3;
					g = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*g + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp + 1]) >> 2;
					b = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*b + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp + 2]) >> 3;
					vram16[offset] = (b << 11)|(g << 5)|r;
					break;
				case 1:// 16-bit RGBA 5:5:5:1
					color = vram16[offset];
					r = (color & 0x1f) << 3;
					g = ((color >> 5) & 0x1f) << 3;
					b = ((color >> 10) & 0x1f) << 3;
					r = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*r + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp]) >> 3;
					g = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*g + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp + 1]) >> 3;
					b = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*b + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp + 2]) >> 3;
					vram16[offset] = (b << 10)|(g << 5)|r;
					break;
				case 2:// 16-bit RGBA 4:4:4:4
					color = vram16[offset];
					r = (color & 0xf) << 4;
					g = ((color >> 4) & 0xf) << 4;
					b = ((color >> 8) & 0xf) << 4;
					r = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*r + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp]) >> 4;
					g = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*g + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp + 1]) >> 4;
					b = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*b + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp + 2]) >> 4;
					vram16[offset] = (b << 8)|(g << 4)|r;
					break;
				case 3:// 32-bit RGBA 8:8:8:8
					color = vram32[offset];
					r = color & 0xff;
					g = (color >> 8) & 0xff;
					b = (color >> 16) & 0xff;
					r = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*r + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp]);
					g = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*g + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp + 1]);
					b = (unsigned char)(((255.0f - image[pos*sbpp + 3])/255.0f)*b + (image[pos*sbpp + 3]/255.0f)*image[pos*sbpp + 2]);
					vram32[offset] = (b << 16)|(g << 8)|r;
					break;
			}
			pos++;
		}
	}
	
	sceKernelDcacheWritebackAll();
	return;
}

void readimage(unsigned char * image, unsigned int srcx, unsigned int srcy, unsigned int srcw, unsigned int srch)
{
	unsigned int x, y, pos = 0;
	
	sceDisplayWaitVblankStart();
	
	for(y = srcy; y < srcy+srch; y++){
		for(x = srcx; x < srcx+srcw; x++){
			unsigned int color, offset = x + y*bufferwidth;
			
			switch (pixelformat) {
				case 0:	// 16-bit RGB 5:6:5
				case 1:// 16-bit RGBA 5:5:5:1
				case 2:// 16-bit RGBA 4:4:4:4
					color = vram16[offset];
					image[pos] = (color >> 8) & 0xff;
					image[pos+1] = color & 0xff;
					pos += 2;
					break;
				case 3:// 32-bit RGBA 8:8:8:8
					color = vram32[offset];
					image[pos] = (color >> 24) & 0xff;
					image[pos+1] = (color >> 16) & 0xff;
					image[pos+2] = (color >> 8) & 0xff;
					image[pos+3] = color & 0xff;
					pos += 4;
					break;
			}
		}
	}
	return;
}

void restoreimage(unsigned char * image, unsigned int srcw, unsigned int srch, unsigned int destx, unsigned int desty)
{
	unsigned int x, y, pos = 0;
	
	sceDisplayWaitVblankStart();
	
	for(y = desty; y < desty+srch; y++){
		for(x = destx; x < destx+srcw; x++){
			unsigned int offset = x + y*bufferwidth;
			
			switch (pixelformat) {
				case 0:	// 16-bit RGB 5:6:5
				case 1:// 16-bit RGBA 5:5:5:1
				case 2:// 16-bit RGBA 4:4:4:4
					vram16[offset] = ((unsigned short)image[pos] << 8)|image[pos+1];
					pos += 2;
					break;
				case 3:// 32-bit RGBA 8:8:8:8
					vram32[offset] = ((unsigned int)image[pos] << 24)|((unsigned int)image[pos+1] << 16)|((unsigned int)image[pos+2] << 8)|image[pos+3];
					pos += 4;
					break;
			}
		}
	}
	
	sceKernelDcacheWritebackAll();
	return;
}

unsigned int getInput(char * input)
{
	unsigned int pos = 0, temp, skipped = 0;
	SceCtrlLatch latch;
	
	while(1)
	{
		sceCtrlReadLatch(&latch);
		temp = pos;
		if(!skipped)
		{
			skipped = 1;
			continue;
		}
		if (latch.uiBreak & PSP_CTRL_START)
		{
			break;
		}
		if (pos >= 10)
		{
			goto delay_thread;
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
				
		if(pos > 10)
		{
			pos = 10;
		}
		
		if (pos > temp)
		{
			for(; temp < pos; temp++)
			{
				drawimage(mask, mask_w, mask_h, 25 + mask_w*temp, 65);
			}
		}

delay_thread:
		sceKernelDelayThreadCB(10*1000);
	}
	
	input[pos] = '\0';
	
	return pos;
}

void suspendThreads(void)
{
	int x, y;
	
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, thread_buf_now, MAX_THREAD, &thread_count_now);
	#ifdef LOG
	SceUID fp = sceIoOpen("ms0:/seplugins/diff.txt", PSP_O_WRONLY|PSP_O_TRUNC|PSP_O_CREAT, 0777);
	#endif
	for(x = 0; x < thread_count_now; x++)
	{
		unsigned char match = 0;
		SceUID tmp_thid = thread_buf_now[x];
		
		for(y = 0; y < thread_count_start; y++)
		{
			SceKernelThreadInfo status;
			status.size = sizeof(SceKernelThreadInfo);
			if (sceKernelReferThreadStatus(tmp_thid, &status) == 0)
			{
				if (strcmp(status.name, "SceImpose") == 0)
				{
					match = 1;
					y = thread_count_start;
				}
			}
			if((tmp_thid == thread_buf_start[y]) || (tmp_thid == cbThread) || (tmp_thid == wkThread))
			{
				match = 1;
				y = thread_count_start;
			}
		}
		
		if(thread_count_start == 0) match = 1;
		if(match == 0)
		{
			#ifdef LOG
			SceKernelThreadInfo status;
			status.size = sizeof(SceKernelThreadInfo);
			if (sceKernelReferThreadStatus(tmp_thid, &status) == 0)
			{
				sceIoWrite(fp, status.name, strlen(status.name));
				sceIoWrite(fp, "***", 3);
			}
			#endif
			sceKernelSuspendThread(tmp_thid);
		}
	}
	#ifdef LOG
	sceIoClose(fp);
	#endif
	sceDisplayGetMode(&pmode, &pwidth, &pheight);
	sceDisplayGetFrameBuf((void*)&vram32, &bufferwidth, &pixelformat, PSP_DISPLAY_SETBUF_IMMEDIATE);
	vram16 = (unsigned short*) vram32;
	
	return;
}

void resumeThreads(void)
{
	int x, y;
	
	for(x = 0; x < thread_count_now; x++)
	{
		unsigned char match = 0;
		SceUID tmp_thid = thread_buf_now[x];
		
		for(y = 0; y < thread_count_start; y++)
		{
			SceKernelThreadInfo status;
			status.size = sizeof(SceKernelThreadInfo);
			if (sceKernelReferThreadStatus(tmp_thid, &status) == 0)
			{
				if (strcmp(status.name, "SceImpose") == 0)
				{
					match = 1;
					y = thread_count_start;
				}
			}
			if((tmp_thid == thread_buf_start[y]) || (tmp_thid == cbThread) || (tmp_thid == wkThread))
			{
				match = 1;
				y = thread_count_start;
			}
		}
		
		if(thread_count_start == 0) match = 1;
		if(match == 0)
		{
			sceKernelResumeThread(tmp_thid);
		}
	}
	
	return;
}

int power_callback(int unknown, int pwrflags, void *common)
{
	int i;
	
	if (pwrflags & PSP_POWER_CB_RESUMING)
	{
		flags_resuming = pwrflags;
		cbdisabled = 1;
	}
    if (pwrflags & PSP_POWER_CB_RESUME_COMPLETE)
	{
		flags_resume_complete = pwrflags;
		if (!locked)
		{
			locked = 1;
			suspendThreads();
			sceKernelWakeupThread(wkThread);
		}
	}
	if ((pwrflags & PSP_POWER_CB_SUSPENDING) && !cbdisabled)
	{
		for (i = 0; i <= 15; i++)
		{
			if (exit_cblist[i])
			{
				sceKernelUnregisterExitCallback(exit_cblist[i]);
			}
		}
	}
	
	if (!cbdisabled)
	{
		for (i = 0; i <= 15; i++)
		{
			if (cblist[i])
			{
				sceKernelNotifyCallback(cblist[i], pwrflags);
			}
		}
	}
	
	return 0;
}

int CallbackThread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Power Callback", power_callback, NULL);
    scePowerRegisterCallback(15, cbid);
    sceKernelSleepThreadCB();
	
	return 0;
}

int WorkThread(SceSize args, void *argp)
{
	char input[11];
	unsigned int len;
	int i;
	
	SceUID fp = sceIoOpen("flash0:/buttons.ini", PSP_O_RDONLY, 0777);
	
	if (fp < 0)
	{
		fp = sceIoOpen("ms0:/seplugins/buttons.ini", PSP_O_RDONLY, 0777);
		if (fp < 0)
		{
			goto no_ini;
		}
	}
	
	sceIoRead(fp, buttons, 11);
	sceIoClose(fp);
	
no_ini:
	cbThread = sceKernelCreateThread("ldnCallbackThread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if (cbThread >= 0) sceKernelStartThread(cbThread, 0, NULL);
	
thread_reentry:
	sceKernelSleepThreadCB();
	
	readimage(mask_bg, 25, 65, mask_w*10, mask_h);
	readimage(password_bg, 25, 139, password_w, password_h);
	readimage(incorrect_bg, 25+password_w, 139, incorrect_w, incorrect_h);
	
	drawimage(enter, enter_w, enter_h, 25, 23);
	drawimage(password, password_w, password_h, 25+enter_w, 23);
	
get_input:
	len = getInput(input);
	if (strcmp(input, buttons) != 0)
	{
		drawimage(password, password_w, password_h, 25, 139);
		drawimage(incorrect, incorrect_w, incorrect_h, 25+password_w, 139);
		
		sceKernelDelayThreadCB(2000*1000);
		
		restoreimage(mask_bg, mask_w*10, mask_h, 25, 65);
		restoreimage(password_bg, password_w, password_h, 25, 139);
		restoreimage(incorrect_bg, incorrect_w, incorrect_h, 25+password_w, 139);
		goto get_input;
	}
	
	restoreimage(mask_bg, mask_w*10, mask_h, 25, 65);
	restoreimage(password_bg, password_w, password_h, 25, 139);
	restoreimage(incorrect_bg, incorrect_w, incorrect_h, 25+password_w, 139);
	
	for (i = 0; i <= 15; i++)
	{
		if (exit_cblist[i])
		{
			sceKernelRegisterExitCallback(exit_cblist[i]);
		}
	}
	
	resumeThreads();
	
	cbdisabled = 0;
	locked = 0;
		
	for (i = 0; i <= 15; i++)
	{
		if (cblist[i])
		{
			sceKernelNotifyCallback(cblist[i], flags_resuming);
		}
	}
	
	sceKernelDelayThreadCB(1000*1000);
	
	for (i = 0; i <= 15; i++)
	{
		if (cblist[i])
		{
			sceKernelNotifyCallback(cblist[i], flags_resume_complete);
		}
	}
	
	goto thread_reentry;
	
	return sceKernelExitDeleteThread(0);
}

int module_start (SceSize args, void *argp)
{
	bzero(cblist, sizeof(SceUID)*16);
	bzero(exit_cblist, sizeof(SceUID)*16);
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, thread_buf_start, MAX_THREAD, &thread_count_start);
	
	sctrlHENPatchSyscall(sctrlHENFindFunction("scePower_Service", "scePower", 0x04B7766E), scePowerRegisterCallback_Patched);
	sctrlHENPatchSyscall(sctrlHENFindFunction("scePower_Service", "scePower", 0xDB9D28DD), scePowerUnregitserCallback_Patched);
	sctrlHENPatchSyscall(sctrlHENFindFunction("scePower_Service", "scePower", 0xDFA8BAF8), scePowerUnregisterCallback_Patched);
	sctrlHENPatchSyscall(sctrlHENFindFunction("sceLoadExec", "LoadExecForUser", 0x4AC57943), sceKernelRegisterExitCallback_Patched);
	
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
	
	wkThread = sceKernelCreateThread("ldnWorkThread", WorkThread, 0x11, 0xFA0, 0, 0);
	if (wkThread >= 0) sceKernelStartThread(wkThread, 0, NULL);
	
	return 0;
}

int module_stop (void)
{
	sctrlHENPatchSyscall((u32)scePowerRegisterCallback_Patched, (void *)sctrlHENFindFunction("scePower_Service", "scePower", 0x04B7766E));
	sctrlHENPatchSyscall((u32)scePowerUnregitserCallback_Patched, (void *)sctrlHENFindFunction("scePower_Service", "scePower", 0xDB9D28DD));
	sctrlHENPatchSyscall((u32)scePowerUnregisterCallback_Patched, (void *)sctrlHENFindFunction("scePower_Service", "scePower", 0xDFA8BAF8));
	sctrlHENPatchSyscall((u32)sceKernelRegisterExitCallback_Patched, (void *)sctrlHENFindFunction("sceLoadExec", "LoadExecForUser", 0x4AC57943));
	
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
	
	scePowerUnregisterCallback(15);
	
	int i;
	for (i = 0; i <= 15; i++)
	{
		if (cblist[i])
		{
			scePowerRegisterCallback(i, cblist[i]);
		}
	}
	
	return 0;
}
