/**
 * @brief led scene server
 * @file led_scene_server.h
 * @version 1.0
 * @author Su YouJiang
 * @date 2017-12-27 15:8:59
 * @par Copyright:
 * Copyright (c) LW-SDK 2000-2017. All rights reserved.
 *
 * @history
 * 1.Date        : 2017-12-27 15:8:59
 *   Author      : Su YouJiang
 *   Modification: Created file
 */

#ifndef __LED_SCENE_SERVER_H__
#define __LED_SCENE_SERVER_H__

//#include "gpio_api.h"

#ifdef __cplusplus
extern "C"{
#endif  // __cplusplus

#include "rgb_scene.h"

/*
********************************************************************************
* 1.每个独立的led，创建对应的rgb_scene_obj_t对象
* 2.每个rgb_scene_obj_t对象需要定义对应的枚举->优先级和场景ID
* 3.根据优先级和场景ID，定义场景列表rgb_scene_tab_t
* 4.互斥的场景设置相同优先级，这样场景会相互替换
* 5.高优先级场景抢占低优先级场景，高优先级场景运行结束，会继续运行低优先级场景
* 6.设计原则，对于那种紧迫短暂的事件设为高优先级
********************************************************************************
*/

typedef enum scene_object
{
    SCENE_OBJECT_LED1,
    SCENE_OBJECT_MAX
} scene_object_t;

/*
********************************************************************************
* LED1
********************************************************************************
*/
typedef enum
{
    SYS_PRIORITY_RESET,
    SYS_PRIORITY_PAIRING,
    SYS_PRIORITY_WIFI,
    SYS_PRIORITY_MAX,
} sys_priority_t;

typedef enum
{
    SYS_SCENE_RESET,
    SYS_SCENE_PAIRING,
    SYS_SCENE_PAIRING_SUCCESS,
    SYS_SCENE_PAIRING_FAIL,
    SYS_SCENE_WIFI_AP,
    SYS_SCENE_WIFI_STA_ING,
    SYS_SCENE_WIFI_STA,
    SYS_SCENE_MAX,
} sys_scene_id_t;


void LedSceneServerInit(void);

void LedSceneStart(uint32_t obj_id, uint32_t scene_id);

void LedSceneStop(uint32_t obj_id, uint32_t scene_id);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // __LED_SCENE_SERVER_H__

