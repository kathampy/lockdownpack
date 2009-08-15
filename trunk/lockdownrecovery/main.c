#include <pspkernel.h>

//#define DEBUG 1

#ifdef DEBUG
#include <pspdebug.h>
#define printf pspDebugScreenPrintf
#endif

PSP_MODULE_INFO("PasswordRecovery", 0, 1, 5);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_HEAP_SIZE_KB(128);

int __psp_free_heap(void);

int main_thread(SceSize args, void *argp)
{
	#ifdef DEBUG
	pspDebugScreenInit();
	#endif

	int status;
	SceUID mod;
	mod = sceKernelLoadModule("flash0:/vsh/module/vshmain.prx", 0, NULL);

	if (mod & 0x80000000)
	{
		#ifdef DEBUG
		printf("\nLoadModule failed 0x%x", mod);
		#endif
	}
	else
	{
		SceUID startmod;
		startmod = sceKernelStartModule(mod, 9, "recovery", &status, NULL);

		if (mod != startmod)
		{
			#ifdef DEBUG
			printf("\nStartModule failed 0x%x", startmod);
			#endif
		}
	}

	sceKernelSelfStopUnloadModule(0, 0, NULL);

	return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, void *argp)
{
	SceUID th;
	th = sceKernelCreateThread("main_thread", main_thread, 0x11, 0xFA0, 0, NULL);

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
