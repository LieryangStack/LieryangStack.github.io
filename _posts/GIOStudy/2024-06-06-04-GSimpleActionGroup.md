---
layout: post
title: 四、GIO——GSimpleAction
categories: GIO学习笔记
tags: [GIO]
---

![alt text](/assets/GIOStudy/04_GSimpleActionGroup/image/image.png)

## 1 GAction

接口的名称跟普通对象定义的名称没有区别，`GAction` 和 `G_TYPE_ACTION`，只不过实际使用的结构体是 `GActionInterface` 而不是 `GAction`。

GAction 表示一个单个命名的动作，该接口的包含了Action:

```c
/* gaction.h */
typedef struct _GActionInterface GActionInterface;
struct _GActionInterface
{
  GTypeInterface g_iface;

  /* virtual functions */
  const gchar *        (* get_name)             (GAction  *action);
  const GVariantType * (* get_parameter_type)   (GAction  *action);
  const GVariantType * (* get_state_type)       (GAction  *action);
  GVariant *           (* get_state_hint)       (GAction  *action);

  gboolean             (* get_enabled)          (GAction  *action);
  GVariant *           (* get_state)            (GAction  *action);

  void                 (* change_state)         (GAction  *action,
                                                 GVariant *value);
  void                 (* activate)             (GAction  *action,
                                                 GVariant *parameter);
};

/* gaction.c */
G_DEFINE_INTERFACE (GAction, g_action, G_TYPE_OBJECT)
```

## 2 GSimpleAction

`GSimpleAction` 是 `GAction` 接口的实现。

```c
/* filename: gsimpleaction.h */
struct _GSimpleAction
{
  GObject       parent_instance;

  gchar        *name; /* 动作Action的名称 */
  GVariantType *parameter_type; /* 动作激活（触发）时候回调函数中 @parameter 的类型 */
  gboolean      enabled; /* 该动作能否被激活（初始化的时候赋值为TRUE，能被激活，也就意味着可以调用激活回调函数）） */
  GVariant     *state;  /* 动作可以带有状态，比如toggle按钮的（TRUE和FALSE） */
  GVariant     *state_hint;
  gboolean      state_set_already; /* 带有状态的Action，是否已经被初始化，也就是创建时候的状态已经给赋值到结构体中的state */
};

/* filename: gsimpleaction.c */
G_DEFINE_TYPE_WITH_CODE (GSimpleAction, g_simple_action, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (G_TYPE_ACTION, g_simple_action_iface_init))
```

## 3 GSimpleActionGroup

`GSimpleActionGroup` 实现了 `GActionGroup` 和 `GActionMap` 接口。

`GSimpleActionGroup` 是对一组 `GSimpleAction` 的统一管理。

![alt text](/assets/GIOStudy/04_GSimpleActionGroup/image/image-1.png)