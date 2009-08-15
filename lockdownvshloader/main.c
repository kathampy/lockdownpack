#include <pspkernel.h>
#include <string.h>

PSP_MODULE_INFO("PasswordLoader", 0x1000, 3, 0);

void trim(char *str)
{
   int len = strlen(str);
   int i;

   for (i = len-1; i >= 0; i--)
   {
      if (str[i] == 0x20 || str[i] == '\t')
      {
         str[i] = 0;
      }
      else
      {
         break;
      }
   }
}

int GetPlugin(char *buf, int size, char *str, int *activated)
{
   char ch = 0;
   int n = 0;
   int i = 0;
   char *s = str;

   while (1)
   {
      if (i >= size)
         break;

      ch = buf[i];

      if (ch < 0x20 && ch != '\t')
      {
         if (n != 0)
         {
            i++;
            break;
         }
      }
      else
      {
         *str++ = ch;
         n++;
      }

      i++;
   }

   trim(s);

   *activated = 0;

   if (i > 0)
   {
      char *p = strpbrk(s, " \t");
      if (p)
      {
         char *q = p+1;

         while (*q < 0) q++;

         if (strcmp(q, "1") == 0)
         {
            *activated = 1;
         }

         *p = 0;
      }
   }

   return i;
}

int stop_thread(SceSize args, void *argp)
{
	SceModule *module;
	SceUID mod;

	sceKernelDelayThread(200000);

	while ((module = sceKernelFindModuleByName("Lockdown")) != NULL)
	{
		sceKernelStopModule(module->modid, 0, NULL, NULL, NULL);
		sceKernelUnloadModule(module->modid);
		sceKernelDelayThread(10000);
	}

	if ((args == 9) && (strcmp(argp, "recovery") == 0))
	{
		args = 0;
		argp = NULL;
		mod = sceKernelLoadModule("flash0:/vsh/module/recovery_real.prx", 0, NULL);
	}
	else
	{
		SceUID fp;
		if ((fp = sceIoOpen("flash0:/loadplugins.txt", PSP_O_RDONLY, 0777)) > 0)
		{
			sceIoClose(fp);
			if((fp = sceIoOpen("ms0:/seplugins/vsh.txt", PSP_O_RDONLY, 0777)) > 0)
			{
				char plug_buf[1024],plugin[1024];
				int size = sceIoRead(fp, plug_buf, 1024);
				sceIoClose(fp);
				char *p = plug_buf;
				int res;

				do
				{
					int activated = 0;
					memset(plugin, 0, sizeof(plugin));

					res = GetPlugin(p, size, plugin, &activated);
					if (res > 0)
					{
						if (activated)
						{
							mod = sceKernelLoadModule(plugin, 0, NULL);
							sceKernelStartModule(mod, 0, NULL, NULL, NULL);
						}
						size -= res;
						p += res;
					}

				} while (res > 0);
			}
		}

		/*mod = sceKernelLoadModule("flash0:/vsh/module/paf_real.prx", 0, NULL);
		sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		mod = sceKernelLoadModule("flash0:/vsh/module/common_gui_real.prx", 0, NULL);
		sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		mod = sceKernelLoadModule("flash0:/vsh/module/common_util_real.prx", 0, NULL);
		sceKernelStartModule(mod, 0, NULL, NULL, NULL);*/

		mod = sceKernelLoadModule("flash0:/vsh/module/vshmain_real.prx", 0, NULL);
	}
	sceKernelStartModule(mod, args, argp, NULL, NULL);

	sceKernelStopUnloadSelfModule(0, NULL, NULL, NULL);
	return sceKernelExitDeleteThread(0); // should not arrive here
}

int module_start(SceSize args, void *argp)
{
	SceUID thid = sceKernelCreateThread("stop_thread", stop_thread, 0x14, 0x4000, 0, NULL);
	sceKernelStartThread(thid, args, argp);

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
