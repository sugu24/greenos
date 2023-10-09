#pragma once

// 連結リストの操作のマクロ
#define NEXT(p) p = p->next
#define PREV(p) p = p->prev

// 双方向連結リストを実現するための構造体
struct two_way_list {
    struct two_way_list *next;
    struct two_way_list *prev;
};

// 単方向連結リストに追加・削除するマクロ
// 左に追加する
#define LIST_LEFT_PUSH(lst, e) \
    e->next = lst;             \
    lst = e;
// 左にシフトして、要素を取得する
#define LIST_SHIFT(lst, e) \
    e = lst;               \
    lst = e->next;         

void two_way_list_push_back(struct two_way_list **list, struct two_way_list *ele);
void *two_way_list_left_shift(struct two_way_list **list);