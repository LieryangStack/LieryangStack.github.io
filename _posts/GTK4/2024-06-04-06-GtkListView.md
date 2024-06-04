---
layout: post
title: 四、GTK4——GtkListView
categories: GTK4
tags: [GTK4]
---

- GtkListView 显示一个大的动态项目列表。

- GtkListView 使用其工厂为每个可见项目生成一个行小部件，并在垂直或水平的线性显示中显示它们。

- GtkListView 属性提供了一种简单的方法来显示行之间的分隔符。

- GtkListView 允许用户根据模型的选择特性选择项目。对于允许多个选定项目的模型，可以使用 GtkListView 启用框选功能。

## 1 GListModel

1. `GListModel` 是一种接口（必须是GObject对象，才能去实现该接口）。

2. `GListModel` 表示可变 `GObject` 列表的接口。具有 `items-changed` 变化信号，列表中的项发生变化，会触发该信号。

3. `GListModel` 中的所有项都必须要来自同一种类型。

### 1.1 GtkStringList

1. `GtkStringList` 列表实现了 `GListModel` 模型接口。

2. `GtkStringList` 中的每一项都是 `GtkStringObject`，这是一个封装字符串的 `GObject` 对象。

    ```c
    struct _GtkStringObject
    {
      GObject parent_instance;
      char *string;
    };
    ```

    ![alt text](/assets/GTK4/06_GtkListView/image/image-20.png)

#### 1.1.1 GtkStringList函数总结
```c
/**
 * 共同点：都是在List的最后添加一项（item）字符串。
 * 不同点： gtk_string_list_append 添加的是常量字符串（内部会进行 g_strdup ）
 *         gtk_string_list_take 已经是堆区创建的字符串 （里面会自动进行 g_free，不需要我们释放）
*/
void 
gtk_string_list_append (GtkStringList *self, const char*string);

void 
gtk_string_list_take (GtkStringList *self, char *string);                                                 
```

## 2 GtkSelectionModel

1. 继承 `GListModel` 的一种接口，为了判断那个位置，那个 `item` 被选中。

    ```c
    G_DECLARE_INTERFACE (GtkSelectionModel, gtk_selection_model, GTK, SELECTION_MODEL, GListModel)
    ```

2. GtkSelectionModel是一个支持选择的接口。有了这个模型，用户可以通过点击来选择项目。它由GtkMultiSelection、GtkNoSelection和GtkSingleSelection对象实现。这三个对象通常足以构建应用程序。它们是用GListModel创建的。你也可以单独创建它们，然后添加一个GListModel。

    - `GtkNoSelection`不支持选择。当需要GtkSelectionModel时，它是GListModel的包装器
  
    - `GtkSingleSelection`支持单选。
  
    - `GtkMultiSelection`支持多选。
  

### 2.1 GtkNoSelection

### 2.2 GtkSingleSelection

### 2.3 GtkMultiSelection

## 3 GtkListItemFactory

### 3.1 GtkSignalListItemFactory

### 3.2 GtkBuilderListItemFactory