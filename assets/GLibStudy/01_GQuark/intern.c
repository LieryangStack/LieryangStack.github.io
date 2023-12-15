#include <glib.h>
#include <stdio.h>

int main() {
    // 创建两个内容相同但独立的字符串
    char *original_str = "hello";
    char *duplicate_str = g_strdup("hello");

    // 使用 g_intern_string 规范化这些字符串
    const char *interned_str2 = g_intern_string(duplicate_str);
    const char *interned_str1 = g_intern_static_string(original_str);

    g_print ("original_str = %p\n", original_str);
    g_print ("duplicate_str = %p\n", duplicate_str);

    g_print ("interned_str1 = %p\n", interned_str1);
    g_print ("interned_str2 = %p\n", interned_str2);

    // 比较原始字符串
    if (original_str == duplicate_str)
        printf("原始字符串是相同的实例。\n");
    else
        printf("原始字符串是不同的实例。\n");

    // 比较规范化后的字符串
    if (interned_str1 == interned_str2)
        printf("规范化后的字符串是相同的实例。\n");
    else
        printf("规范化后的字符串是不同的实例。\n");

    // 清理
    g_free(duplicate_str);

    return 0;
}
