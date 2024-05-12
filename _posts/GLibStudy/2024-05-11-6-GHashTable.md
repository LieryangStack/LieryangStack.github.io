---
layout: post
title: 四、GLib——GHashTable
categories: GLib学习笔记
tags: [GLib]
---


https://zh.wikipedia.org/wiki/%E5%93%88%E5%B8%8C%E8%A1%A8

https://blog.csdn.net/weixin_52109967/article/details/125955738

https://blog.csdn.net/songzihuan/article/details/105712600


## GHashTable

- GHashTable提供了键和值之间的关联，其优化使得在给定键的情况下，可以非常快速地找到关联的值。

- 请注意，插入GHashTable时，键和值都不会被复制，因此它们必须在GHashTable的生命周期内存在。这意味着使用静态字符串是可以的，但应在插入之前使用g_strdup()复制临时字符串（即在缓冲区中创建的字符串和由GTK+小部件返回的字符串）。

- 如果键或值是动态分配的，您必须小心确保在从GHashTable中删除它们时释放它们，并且在它们被新插入到GHashTable中覆盖时也要释放它们。在GHashTable中混合使用静态字符串和动态分配的字符串也不可取，因为这样就很难确定字符串是否应该被释放。

- 要创建一个GHashTable，使用g_hash_table_new()。

- 要将键和值插入到GHashTable中，使用g_hash_table_insert()。

- 要查找与给定键对应的值，使用g_hash_table_lookup()和g_hash_table_lookup_extended()。

- 要删除键和值，使用g_hash_table_remove()。

- 要为每个键和值对调用一个函数，使用g_hash_table_foreach()或使用迭代器迭代哈希表中的键/值对，请参阅GHashTableIter。

- 要销毁GHashTable，使用g_hash_table_destroy()。






