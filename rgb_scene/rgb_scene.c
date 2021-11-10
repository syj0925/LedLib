/**
 * @brief rgb scene
 * @file rgb_scene.c
 * @version 1.0
 * @author Su YouJiang
 * @date 2020-11-27 17:33:4
 * @par Copyright:
 * Copyright (c) Leedarson 2000-2020. All rights reserved.
 *
 * @history
 * 1.Date        : 2020-11-27 17:33:4
 *   Author      : Su YouJiang
 *   Modification: Created file
 */

#include "rgb_scene.h"

/*
********************************************************************************
* 参数配置
********************************************************************************
*/
#define INVALID         0
#define VALID           1
#define xassert(p)               //if(!(p)) {XLOG_PRINTF(XLOG_ASSERT, XLOG_TAG, NULL, __LINE__, __FUNCTION__, "assert"); while(0);}

/*
********************************************************************************
* 结构定义
********************************************************************************
*/

typedef struct rgb_prio
{
    uint8_t  isvalid;             /* 表示该优先级是否有效 */
    uint8_t  scene_id;            /* 场景ID */
    uint16_t scene_cycle;         /* 记录场景循环次数 */
    uint8_t  scene_idx;           /* 记录scene中当前执行的action */
    uint8_t  action_step;         /* 记录action的执行步骤 */
    uint16_t action_cycle;        /* 记录action循环次数 */
    uint16_t tick;                /* 时间计数器 */
} rgb_prio_t;

struct rgb_scene_obj
{
	struct rgb_scene_obj *next;   /* 单向链表 */
	rgb_output_t output;          /* rgb输出回调函数 */
    const rgb_scene_tab_t *scene_tab; /* 场景表 */
    uint8_t    scene_num;         /* 场景个数 */
    uint8_t    refresh;           /* 刷新rgb显示 */
    uint8_t    prio_num;          /* 记录优先级数目 */
    rgb_prio_t prio[0];
};

/*
********************************************************************************
* 静态变量
********************************************************************************
*/
static rgb_scene_obj_t *head_handle = NULL;

static uint8_t calcStep(int32_t s, int32_t e, int32_t n, int32_t step)
{
    s += ((e - s)*step/n);

    return (uint8_t)s;
}

static int rgbSceneHandler(rgb_scene_obj_t *obj, rgb_prio_t *prio)
{
    int8_t need_refresh = 0;
    const rgb_scene_t *scene = obj->scene_tab[prio->scene_id].scene;

    /* 检测action list一轮执行是否结束 */
    if (prio->scene_idx >= scene->num)
    {
        prio->scene_idx = 0;

        /* 检测action list循环是否结束，结束表示该场景执行完成 */
        prio->scene_cycle++;

         /* action执行结束 */
        if (scene->cycle != CYCLE_ALWAYS &&
            prio->scene_cycle >= scene->cycle)
        {
            prio->scene_cycle = 0;
            prio->isvalid = INVALID;
            return 0;
        }
    }

    const rgb_value_t *rgb = NULL;
    rgb_value_t temp;
    const action_t *action = &scene->action[prio->scene_idx];

    switch (action->type) {
        case ACTION_ONOFF:
        {
            const action_onoff_t *onoff = &action->sub.onoff;

            if (prio->tick == 0 || obj->refresh)
            {
                need_refresh = 1;
                rgb = &onoff->value;
            }

            if (++prio->tick >= onoff->lifetime)
            {
                prio->tick = 0;
                prio->action_cycle++;
            }

            break;
        }
        case ACTION_BLINK:
        {
            const action_onoff_t *onoff;

            if (prio->action_step == 0)
            {
                onoff = &action->sub.blink.action1;
            }
            else
            {
                onoff = &action->sub.blink.action2;
            }

            if (prio->tick == 0 || obj->refresh)
            {
                need_refresh = 1;
                rgb = &onoff->value;
            }

            if (++prio->tick >= onoff->lifetime)
            {
                prio->tick = 0;
                prio->action_step++;
                if (prio->action_step >= 2)
                {
                    prio->action_step = 0;
                    prio->action_cycle++;
                }
            }
            break;
        }
        case ACTION_FADE:
        {
            const action_fade_t *fade = &action->sub.fade;

            if (prio->tick == 0 || obj->refresh)
            {
                need_refresh = 1;

                temp.r = calcStep(fade->start.r, fade->end.r, fade->step, prio->action_step+1);
                temp.g = calcStep(fade->start.g, fade->end.g, fade->step, prio->action_step+1);
                temp.b = calcStep(fade->start.b, fade->end.b, fade->step, prio->action_step+1);
                rgb = &temp;
            }

            if (++prio->tick >= fade->interval)
            {
                prio->tick = 0;
                prio->action_step++;
                if (prio->action_step >= fade->step)
                {
                    prio->action_step = 0;
                    prio->action_cycle++;
                }
            }
            break;
        }
        default:
            prio->isvalid = INVALID;
            return 0;
    }

    if (action->cycle != CYCLE_ALWAYS &&
        prio->action_cycle >= action->cycle)
    {
        prio->action_cycle = 0;
        prio->scene_idx++;
    }

    if (need_refresh)
    {
        obj->refresh = 0;
        obj->output(action->rgb_mask, rgb);
    }

    return 1;
}

static int rgbSceneObjAdd(rgb_scene_obj_t *obj)
{
	rgb_scene_obj_t *target = head_handle;

	while (target)
	{
		if (target == obj) return -1;	                                       /* already exist. */
		target = target->next;
	}

	obj->next = head_handle;
	head_handle = obj;

	return 0;
}

static void rgbSceneObjDelete(rgb_scene_obj_t *obj)
{
	rgb_scene_obj_t **curr;

	for (curr = &head_handle; *curr; )
	{
		rgb_scene_obj_t *entry = *curr;

		if (entry == obj)
		{
			*curr = entry->next;
		}
		else
		{
			curr = &entry->next;
        }
	}
}

rgb_scene_obj_t *RgbSceneObjCreate(uint8_t prio_num, uint8_t scene_num,
                                   const rgb_scene_tab_t *scene_tab, rgb_output_t output)
{
	rgb_scene_obj_t *obj = (rgb_scene_obj_t *)malloc(sizeof(rgb_scene_obj_t) + sizeof(rgb_prio_t) * prio_num);

    if (obj == NULL)
    {
        return NULL;
    }

    memset(obj, 0, sizeof(rgb_scene_obj_t) + sizeof(rgb_prio_t) * prio_num);
    obj->prio_num  = prio_num;
    obj->scene_num = scene_num;
    obj->scene_tab = scene_tab;
    obj->output    = output;

    return obj;
}

int RgbSceneStart(rgb_scene_obj_t *obj, uint8_t sceneid)
{
    if (obj == NULL || sceneid >= obj->scene_num)
    {
        return -1;
    }

    uint8_t prio = obj->scene_tab[sceneid].prio;

    if (prio >= obj->prio_num)
    {
        return -1;
    }

    memset(&obj->prio[prio], 0, sizeof(rgb_prio_t));
    obj->prio[prio].scene_id = sceneid;
    obj->prio[prio].isvalid = VALID;

    rgbSceneObjAdd(obj);

    return 0;
}

int RgbSceneStop(rgb_scene_obj_t *obj, uint8_t sceneid)
{
    if (obj == NULL)
    {
        return -1;
    }

    /* FF表示取消object的所有scene */
    if (sceneid == 0xFF)
    {
        for (int i = 0; i < obj->prio_num; i++)
        {
            obj->prio[i].isvalid = INVALID;
        }

        return 0;
    }

    if (sceneid >= obj->scene_num)
    {
        return -1;
    }

    uint8_t prio = obj->scene_tab[sceneid].prio;

    if (prio >= obj->prio_num)
    {
        return -1;
    }

    memset(&obj->prio[prio], 0, sizeof(rgb_prio_t));
    obj->refresh = 1;

    return 0;
}

int RgbSceneTick(void)
{
    rgb_scene_obj_t *target;

    if (head_handle == NULL)
    {
        return -1;
    }

	for (target = head_handle; target; target = target->next)
	{

        int8_t flag = 0;

        for (int i = 0; i < target->prio_num; i++)
        {
            if (target->prio[i].isvalid == INVALID)
            {
                continue;
            }

            flag = 1;
    		if (rgbSceneHandler(target, &target->prio[i]) == 0)
    		{
                target->refresh = 1;
                continue;
            }
            else
            {
                break;
            }
        }

        /* object的场景全部执行完，把object从链表删除 */
        if (!flag)
        {
            /* 无场景执行，默认关闭rgb */
            rgb_value_t rgb = {0, 0, 0};

            target->output(0xFFFF, &rgb);
            rgbSceneObjDelete(target);
        }
	}

    return 0;
}


