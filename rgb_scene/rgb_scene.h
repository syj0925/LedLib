/**
 * @brief rgb scene
 * @file rgb_scene.h
 * @version 1.0
 * @author Su YouJiang
 * @date 2020-11-27 15:15:5
 * @par Copyright:
 * Copyright (c) LedLib 2018-2020. All rights reserved.
 *
 * @history
 * 1.Date        : 2020-11-27 15:15:5
 *   Author      : Su YouJiang
 *   Modification: Created file
 */

#ifndef __RGB_SCENE_H__
#define __RGB_SCENE_H__

#ifdef __cplusplus
extern "C"{
#endif  // __cplusplus

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
/*
********************************************************************************
* config
********************************************************************************
*/
#define CYCLE_ALWAYS          0xFFFF

#define RGB_TICK              (10)       /**< timer tick, unit:ms */
#define RGB_MSEC              (0.1)      /* 1/RGB_TICK */
#define RGB_SEC               (1000/RGB_TICK)

/*
********************************************************************************
* typedef
********************************************************************************
*/

typedef enum action_type
{
    ACTION_ONOFF = 0,             /* 开关模式 */
    ACTION_BLINK,                 /* 闪烁模式 */
    ACTION_FADE,                  /* 渐变模式 */
} action_type_t;

/*
********************************************************************************
* Scene struct
********************************************************************************
*/
typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_value_t;

typedef struct
{
    rgb_value_t value;            /* RGB值 */
    uint16_t    lifetime;         /* 持续时间    */
} action_onoff_t;

typedef struct
{
    action_onoff_t action1;       /* blink上半个动作 */
    action_onoff_t action2;       /* blink下半个动作 */
} action_blink_t;

typedef struct
{
    rgb_value_t start;            /* 渐变前RGB值 */
    rgb_value_t end;              /* 渐变后RGB值 */
    uint8_t     step;             /* 渐变拍数 */
    uint16_t    interval;         /* 时间间隔 */
} action_fade_t;

typedef struct
{
    uint16_t      rgb_mask;       /* rgb掩码 */
    uint16_t      cycle;          /* action循环次数，0xFFFF表示forever */
    action_type_t type;           /* 动作类型 */
    union
    {
        action_onoff_t onoff;     /* ON/OFF */
        action_blink_t blink;     /* 闪烁 */
        action_fade_t  fade;      /* 渐变 */
    } sub;
} action_t;

typedef struct
{
    uint16_t cycle;               /* 场景循环次数，0xFFFF表示forever */
    uint16_t num;                 /* 场景的action个数 */
    action_t action[];            /* 场景的action列表 */
} rgb_scene_t;

typedef struct
{
    uint8_t prio;                 /* 场景优先级 */
    const rgb_scene_t *scene;     /* 场景 */
} rgb_scene_tab_t;                /* 描述:场景id作为数组下标，快速查表 */


/********************************分割线***************************************/
typedef void (*rgb_output_t)(uint16_t rgb_mask, const rgb_value_t *rgb);

struct rgb_scene_obj;
typedef struct rgb_scene_obj rgb_scene_obj_t;

rgb_scene_obj_t *RgbSceneObjCreate(uint8_t prio_num, uint8_t scene_nun,
                                   const rgb_scene_tab_t *scene_tab, rgb_output_t output);

int RgbSceneStart(rgb_scene_obj_t *obj, uint8_t sceneid);

int RgbSceneStop(rgb_scene_obj_t *obj, uint8_t sceneid);

int RgbSceneTick(void);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // __RGB_SCENE_H__

