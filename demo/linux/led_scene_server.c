/**
 * @brief led scene server
 * @file led_scene_server.c
 * @version 1.0
 * @author Su YouJiang
 * @date 2017-12-27 15:7:8
 * @par Copyright:
 * Copyright (c) LW-SDK 2000-2017. All rights reserved.
 *
 * @history
 * 1.Date        : 2017-12-27 15:7:8
 *   Author      : Su YouJiang
 *   Modification: Created file
 */

#include <stdio.h>
#include <assert.h>
#include "led_scene_server.h"

/*
********************************************************************************
* static
********************************************************************************
*/
static rgb_scene_obj_t *scene_obj[SCENE_OBJECT_MAX];

/* sys reset */
const rgb_scene_t c_sys_reset = {
    .cycle = CYCLE_ALWAYS,
    .num = 2,

    .action[0] = {
        .cycle = 3,
        .type = ACTION_BLINK,
        .sub.blink.action1.value.r  = 0xFF,
        .sub.blink.action1.value.g  = 0,
        .sub.blink.action1.value.b  = 0,
        .sub.blink.action1.lifetime = 100*RGB_MSEC,
        .sub.blink.action2.value.r  = 0,
        .sub.blink.action2.value.g  = 0,
        .sub.blink.action2.value.b  = 0,
        .sub.blink.action2.lifetime = 100*RGB_MSEC,
    },
    .action[1] = {
        .cycle = 1,
        .type = ACTION_ONOFF,
        .sub.onoff.value.r = 0,
        .sub.onoff.value.g = 0,
        .sub.onoff.value.b = 0,
        .sub.onoff.lifetime = 500*RGB_MSEC,
    },
};

/* PAIRING */
const rgb_scene_t c_sys_pairing = {
    .cycle = CYCLE_ALWAYS,
    .num = 1,

    .action[0] = {
        .cycle = 1,
        .type = ACTION_BLINK,
        .sub.blink.action1.value.r  = 0xFF,
        .sub.blink.action1.value.g  = 0,
        .sub.blink.action1.value.b  = 0,
        .sub.blink.action1.lifetime = 300*RGB_MSEC,
        .sub.blink.action2.value.r  = 0,
        .sub.blink.action2.value.g  = 0,
        .sub.blink.action2.value.b  = 0,
        .sub.blink.action2.lifetime = 300*RGB_MSEC,
    },
};

/* PAIRING Success */
const rgb_scene_t c_sys_pairing_success = {
    .cycle = 1,
    .num = 1,

    .action[0] = {
        .cycle = 1,
        .type = ACTION_ONOFF,
        .sub.onoff.value.r = 0xFF,
        .sub.onoff.value.g = 0,
        .sub.onoff.value.b = 0,
        .sub.onoff.lifetime = 3*RGB_SEC,
    },
};

/* PAIRING Fail */
const rgb_scene_t c_sys_pairing_fail = {
    .cycle = 5,
    .num = 4,

    .action[0] = {
        .cycle = 1,
        .type = ACTION_FADE,
        .sub.fade.start.r = 0,
        .sub.fade.start.g = 0,
        .sub.fade.start.b = 0,
        .sub.fade.end.r = 0xFF,
        .sub.fade.end.g = 0,
        .sub.fade.end.b = 0,
        .sub.fade.step = 100,
        .sub.fade.interval = 10*RGB_MSEC,
    },
    .action[1] = {
        .cycle = 1,
        .type = ACTION_ONOFF,
        .sub.onoff.value.r = 0xFF,
        .sub.onoff.value.g = 0,
        .sub.onoff.value.b = 0,
        .sub.onoff.lifetime = 400*RGB_MSEC,
    },
    .action[2] = {
        .cycle = 1,
        .type = ACTION_FADE,
        .sub.fade.start.r = 0xFF,
        .sub.fade.start.g = 0,
        .sub.fade.start.b = 0,
        .sub.fade.end.r = 0,
        .sub.fade.end.g = 0,
        .sub.fade.end.b = 0,
        .sub.fade.step = 100,
        .sub.fade.interval = 10*RGB_MSEC,
    },
    .action[3] = {
        .cycle = 1,
        .type = ACTION_ONOFF,
        .sub.onoff.value.r = 0,
        .sub.onoff.value.g = 0,
        .sub.onoff.value.b = 0,
        .sub.onoff.lifetime = 1*RGB_SEC,
    },
};

/* WIFI AP */
const rgb_scene_t c_sys_wifi_ap = {
    .cycle = CYCLE_ALWAYS,
    .num = 1,

    .action[0] = {
        .cycle = 1,
        .type = ACTION_BLINK,
        .sub.blink.action1.value.r  = 0xFF,
        .sub.blink.action1.value.g  = 0,
        .sub.blink.action1.value.b  = 0,
        .sub.blink.action1.lifetime = 1*RGB_SEC,
        .sub.blink.action2.value.r  = 0,
        .sub.blink.action2.value.g  = 0,
        .sub.blink.action2.value.b  = 0,
        .sub.blink.action2.lifetime = 1*RGB_SEC,
    },
};

const rgb_scene_t c_sys_wifi_sta_ing = {
    .cycle = CYCLE_ALWAYS,
    .num = 1,

    .action[0] = {
        .cycle = 1,
        .type = ACTION_ONOFF,
        .sub.onoff.value.r = 0,
        .sub.onoff.value.g = 0,
        .sub.onoff.value.b = 0,
        .sub.onoff.lifetime = 3*RGB_SEC,
    },
};

const rgb_scene_t c_sys_wifi_sta = {
    .cycle = CYCLE_ALWAYS,
    .num = 1,

    .action[0] = {
        .cycle = 1,
        .type = ACTION_BLINK,
        .sub.blink.action1.value.r  = 0xFF,
        .sub.blink.action1.value.g  = 0,
        .sub.blink.action1.value.b  = 0,
        .sub.blink.action1.lifetime = 50*RGB_MSEC,
        .sub.blink.action2.value.r  = 0,
        .sub.blink.action2.value.g  = 0,
        .sub.blink.action2.value.b  = 0,
        .sub.blink.action2.lifetime = 950*RGB_MSEC,
    },
};

const rgb_scene_tab_t led1_scene_tab[SYS_SCENE_MAX] = {
    {.prio = SYS_PRIORITY_RESET,       .scene = &c_sys_reset},
    {.prio = SYS_PRIORITY_PAIRING,     .scene = &c_sys_pairing},
    {.prio = SYS_PRIORITY_PAIRING,     .scene = &c_sys_pairing_success},
    {.prio = SYS_PRIORITY_PAIRING,     .scene = &c_sys_pairing_fail},
    {.prio = SYS_PRIORITY_WIFI,        .scene = &c_sys_wifi_ap},
    {.prio = SYS_PRIORITY_WIFI,        .scene = &c_sys_wifi_sta_ing},
    {.prio = SYS_PRIORITY_WIFI,        .scene = &c_sys_wifi_sta},
};

static void led1Output(uint16_t led_mask, const rgb_value_t *rgb)
{
    printf("sys out:r:%d, g:%d, b:%d\r\n", rgb->r, rgb->g, rgb->b);
}

void LedSceneServerInit(void)
{
    scene_obj[SCENE_OBJECT_LED1] = RgbSceneObjCreate(SYS_PRIORITY_MAX, SYS_SCENE_MAX,
                                             led1_scene_tab, led1Output);
    assert(scene_obj[SCENE_OBJECT_LED1] != NULL);
}

void LedSceneStart(uint32_t obj_id, uint32_t scene_id)
{
    RgbSceneStart(scene_obj[obj_id], scene_id);
}

void LedSceneStop(uint32_t obj_id, uint32_t scene_id)
{
    RgbSceneStop(scene_obj[obj_id], scene_id);
}

