---
layout: post
title: 四、GIO——GSimpleActionGroup
categories: GIO学习笔记
tags: [GIO]
---

## 1 GAction

接口的名称跟普通对象定义的名称没有区别，`GAction` 和 `G_TYPE_ACTION`，只不过实际使用的结构体是 `GActionInterface` 而不是 `GAction`。

- GAction 表示一个单个命名的动作。

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
G_DEFINE_TYPE_WITH_CODE (GSimpleAction, g_simple_action, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (G_TYPE_ACTION, g_simple_action_iface_init))
```

