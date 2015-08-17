Lockdown v4.0 & Lockdown XMB Style v4.1
Author: Torch

Lockdown allows you to password protect your PSP from prying eyes.
You can also protect the Recovery Menu so that the protection can't be bypassed at all.


------------------------------------------
Credits
------------------------------------------
Thanks to Dark_Alex for his VLF library, without which Lockdown XMB Style would have been impossible, and also for adding the Plugins Disable feature in 5.00M33-2


------------------------------------------
Features
------------------------------------------
* Completely customizable GUI. On PSP Slim, Lockdown XMB Style uses the XMB Wave as the background.
* Uses PSP keys like Square, Cross, Circle etc as password, instead of fumbling with an on screen keyboard
* Fully configurable from the PSP
* Protects Recovery Menu as well
* Supports 2 modes of operation: 1st mode will only ask for password when the PSP is rebooted or powered on. 2nd mode will ask for password every time the XMB is launched, such as after quitting a game etc.
* Application errors (Game could not be started, etc) will be displayed normally in the XMB unlike other password programs which won't display these messages. So you know why you were sent back to the XMB.


------------------------------------------
Installation
------------------------------------------
**WARNING: Messing with flash0: can brick your PSP if you screw up**

First of all make sure you have enough space in flash0: to install all the files.

1. Open flash0 via USB. Paste the following files in flash0:

   lockdown.thm
   loader.prx


   For Lockdown XMB Style on the Slim Only also paste the following files in flash0:

   vlf.prx
   iop.prx
   intrafont.prx

2. Go to the /vsh/module folder. Rename the vshmain.prx to vshmain_real.prx, DO NOT OVERWRITE THE ORIGINAL FILE!
   Paste the vshmain.prx from Lockdown into this folder.

3. Reboot the PSP. You will see the password screen asking you to set a new password. You can use the PSP face buttons, the D-Pad and L-Trigger & R-Trigger. You can enter a maximum of 10 buttons. Press Start after you are done. It will ask you to re-type the password for confirmation. Then choose the mode you want (ask password only on reboot, or ask password everytime XMB is started). After that the XMB will launch.

4. If the above worked out correctly, then you can now proceed to protect the recovery menu. If it didn't work then use recovery mode to restore your original vshmain.prx.

5. To protect recovery mode, open flash0 via USB and go to the /vsh/module folder. Rename the recovery.prx to recovery_real.prx, DO NOT OVERWRITE THE ORIGINAL FILE!
   Paste the recovery.prx from Lockdown into this folder.

Ensure that you have renamed the files correctly or you might brick your PSP.


------------------------------------------
Enhanced Security (Optional)
------------------------------------------
Since 5.00M33-2, you can disable the loading of plugins in the Recovery menu, under Advanced Configuration. If you set XMB Plugins to Disabled then it is impossible to bypass Lockdown using plugins.

If you create a blank file in flash0: called "loadplugins.txt" then Lockdown will load the XMB plugins after the correct password is entered, even if XMB Plugins is Disabled.

There is a small problem with having Lockdown load the plugins though. Some plugins which hook functions by waiting for the target firmware module to start may not work, because the module would have already been started before Lockdown loads the plugin. Such plugins have to be manually installed in the pspbt*nf.bin files before the modules they try to hook are loaded.

If you are not using the Disable XMB Plugins options, then there is no problem.


------------------------------------------
Uninstallation
------------------------------------------
Delete the vshmain.prx and recovery.prx and rename the vshmain_real.prx and recovery_real.prx back to the original names.
Also delete these additional files from flash0: (lockdown.thm, buttons.ini, vlf.prx, iop.prx, infrafont.prx)


------------------------------------------
Installing Themes
------------------------------------------
Themes should be named "lockdown.thm" and placed in the root of flash0:
As a safety precaution, Lockdown will first look for lockdown.thm in the Memory Stick root and load that if present. This will allow you to recover your PSP in case you install a non-working theme in flash0:


------------------------------------------
Creating Themes
------------------------------------------
The mktheme folder in the archive contains the necessary files to make custom themes.

You can use PNG images with alpha channel transparency.

The coords.txt file contains the x & y coordinates to draw each image at, and the width and height of the image.
The x & y coordinates represent the top left corner of the image.

e.g.
background.png	0 0 480 272

This will draw background.png at 0,0 (the top left corner of the screen) and it will be 480 pixels wide and 272 pixels in height. (The width and height specified for the image in coords.txt can be less than the actual dimensions of the PNG file. The image will only be drawn upto the size specified.)

Do not change the names of the PNG files. Do not change the image names in coords.txt. You only need to change the coordinates and width/height of in coords.txt.

To generate the .thm file, open a command prompt in the mktheme folder (the PNG files and coords.txt must be in the same folder as mktheme.exe) and type:
mktheme coords.txt
This will generate a file called "lockdown.thm".

You can also specify the name of the output file:
mktheme coords.txt mytheme.thm

However, you must rename the file to lockdown.thm in order to install it.

Due to insufficient memory on the Phat PSP, the usable memory by Lockdown has been limited to 2MB. So if your images are too large it might cause Lockdown to crash.
