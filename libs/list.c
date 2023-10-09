#include <libs/list.h>
#include <kernel/type.h>

/*
    双方向連結リスト(list)に要素(ele)をpush_backする
    eleのメンバ変数の初めは必ず
*/
void two_way_list_push_back(struct two_way_list **list, struct two_way_list *ele) {
    if (*list) {
        // listが空でなければ末尾に追加
        (*list)->prev->next = ele;
        ele->prev = (*list)->prev;
        (*list)->prev = ele;
        ele->next = (*list);
    } else {
        // listが空の場合
        ele->next = ele;
        ele->prev = ele;
        (*list) = ele;
    }
}

/*
    双方向連結リスト(list)を左にシフトする
*/
void *two_way_list_left_shift(struct two_way_list **list) {
    if (!*list) {
        return NULL;
    } else {
        struct two_way_list *ele = (*list);
        (*list) = (*list)->next;

        if (*list) {
            (*list)->prev = ele->prev;
            ele->prev->next = (*list);
        }

        ele->next = NULL;
        ele->prev = NULL;

        return (void *)ele;
    }
}