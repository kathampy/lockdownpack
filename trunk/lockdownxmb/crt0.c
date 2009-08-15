#include <pspkernel.h>
#include <vlf.h>

extern int app_main(int argc, char *argv[]);
SceSize vshargs;
void *vshargp;
SceUID modiop, modif, modvlf;

int start_thread(SceSize args, void *argp)
{
   vshargs = args;
   vshargp = argp;
   
   modiop = sceKernelLoadModule("flash0:/iop.prx", 0, NULL);
   sceKernelStartModule(modiop, args, argp, NULL, NULL);
   modif = sceKernelLoadModule("flash0:/intraFont.prx", 0, NULL);
   sceKernelStartModule(modif, args, argp, NULL, NULL);
   modvlf = sceKernelLoadModule("flash0:/vlf.prx", 0, NULL);
   sceKernelStartModule(modvlf, args, argp, NULL, NULL);
   vlfGuiInit(-2048, app_main);
   
   return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, void *argp)
{
	SceUID thid = sceKernelCreateThread("start_thread", start_thread, 0x10, 0x4000, 0, NULL);
	if (thid < 0)
		return thid;

	sceKernelStartThread(thid, args, argp);
	
	return 0;
}
