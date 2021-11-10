#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include "led_scene_server.h"

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

int main(int argc, char **argv)
{
    LedSceneServerInit();

    LedSceneStart(SCENE_OBJECT_LED1, SYS_SCENE_PAIRING_FAIL);

	while (1) {
	    /* 10ms tick */
        usleep(10 * 1000);
        RgbSceneTick();
	}
}

