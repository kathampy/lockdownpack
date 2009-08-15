Lockdown In-Game v2.0
Author: Torch

Lockdown In-Game will require you to enter a password when you resume your PSP from suspend mode. Useful to prevent other people from messing up your progress if you put the PSP in suspend during a game.

Installation:
Put it in the ms0:/seplugins/ folder and make an entry in GAME.txt (e.g. ms0:/seplugins/lockdown.prx 1).

It is recommended that it should be the very first entry in GAME.txt (How ever if you also have Hold+ installed, then make Hold+ first and lockdown.prx second).

Use the included exe to create a password file (buttons.ini) and put it in ms0:/seplugins/

If you also have the flash0: version of Lockdown installed, then this will detect and use the same flash0: password.

It only works in GAME mode. It will not work in the XMB.

Notes:
Its recommended that you don't suspend the PSP while a game video is playing. This includes cases where the main menu has a video in the background etc.

To be specific: You can suspend the PSP while a video is playing. But when you turn it on again and it asks for the password, do not suspend it again while in the password screen. Instead, enter the password to resume the game, then suspend it. This is ONLY necessary IF there was a video playing when you suspended it. Normally you can suspend it again in the password screen even without entering a password.

Changelog v2.0
----------------------------------
Greatly improved suspend/resume method.

Should be compatible with all games now. (Any problems with suspend/resume are probably due to the No-UMD driver that you have selected)

You can suspend the PSP again in the password screen without entering a password using the Power switch.