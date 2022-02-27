#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */
struct list_head *find_mid(struct list_head *head)
{
    struct list_head *slow = head->next;
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next) {
        slow = slow->next;
    }
    return slow;
}


/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    struct list_head *current, temp;

    list_for_each (current, l) {
        element_t *container;
        container = list_entry(current, element_t, list);
        temp = *current;
        free(container->value);
        free(container);
        current = &temp;
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;


    new->value = malloc(strlen(s) + 1);
    if (!new->value) {
        free(new);
        return false;
    }
    strncpy(new->value, s, strlen(s) + 1);


    list_add(&new->list, head);

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;


    new->value = malloc(strlen(s) + 1);
    if (!new->value) {
        free(new);
        return false;
    }
    strncpy(new->value, s, strlen(s) + 1);


    list_add_tail(&new->list, head);

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;


    element_t *node = list_entry(head->next, element_t, list);

    if (!sp) {
        list_del_init(head->next);
        return node;
    }
    char *str = node->value;
    int len = strlen(str) + 1;
    if (len <= bufsize)
        strncpy(sp, str, len);
    else {
        strncpy(sp, str, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }

    list_del_init(head->next);
    return node;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;


    element_t *node = list_entry(head->prev, element_t, list);

    if (!sp) {
        list_del_init(head->prev);
        return node;
    }
    char *str = node->value;
    int len = strlen(str) + 1;
    if (len <= bufsize)
        strncpy(sp, str, len);
    else {
        strncpy(sp, str, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }

    list_del_init(head->prev);
    return node;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;

    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    if (!head)
        return false;
    if (list_empty(head))
        return false;

    struct list_head *mid = find_mid(head);

    mid->prev->next = mid->next;
    mid->next->prev = mid->prev;
    element_t *node = list_entry(mid, element_t, list);
    free(node->value);
    free(node);


    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;
    if (list_empty(head))
        return true;

    element_t *current, *first = list_entry(head->next, element_t, list),
                        *last = first;
    list_for_each_entry (current, head, list) {
        if (!strcmp(current->value, first->value))
            last = current;
        else {
            if (first != last) {
                first->list.prev->next = last->list.next;
                last->list.next->prev = first->list.prev;
                while (first != current) {
                    element_t *temp =
                        list_entry(first->list.next, element_t, list);
                    q_release_element(first);
                    first = temp;
                }
            }
            first = current;
            last = current;
        }
    }
    if (first != last) {
        first->list.prev->next = last->list.next;
        last->list.next->prev = first->list.prev;
        while (first != current) {
            element_t *temp = list_entry(first->list.next, element_t, list);
            q_release_element(first);
            first = temp;
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    if (!head)
        return;

    for (struct list_head *first = head->next, *second = head->next->next;
         first != head && second != head;
         first = first->next, second = first->next) {
        struct list_head *prev = first->prev;
        struct list_head *next = second->next;
        prev->next = second;
        second->prev = prev;
        second->next = first;
        first->prev = second;
        next->prev = first;
        first->next = next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    struct list_head *current, *safe;
    list_for_each_safe (current, safe, head) {
        struct list_head *prev = current->prev;
        struct list_head *next = current->next;
        current->next = prev;
        current->prev = next;
    }
    struct list_head *prev = head->prev;
    struct list_head *next = head->next;
    head->next = prev;
    head->prev = next;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */


void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head less, greater;
    INIT_LIST_HEAD(&less);
    INIT_LIST_HEAD(&greater);

    element_t *pivot;
    element_t *first = list_first_entry(head, element_t, list);
    element_t *last = list_last_entry(head, element_t, list);
    element_t *mid = list_entry(find_mid(head), element_t, list);
    if (strcmp(first->value, last->value) > 0 &&
        strcmp(first->value, mid->value) < 0)
        pivot = first;
    else if (strcmp(last->value, first->value) > 0 &&
             strcmp(last->value, mid->value) < 0)
        pivot = last;
    else
        pivot = mid;

    list_del(&pivot->list);
    element_t *current = NULL, *save = NULL;
    list_for_each_entry_safe (current, save, head, list) {
        if (strcmp(pivot->value, current->value) < 0)
            list_move(&current->list, &greater);
        else
            list_move(&current->list, &less);
    }

    q_sort(&greater);
    q_sort(&less);

    list_splice(&less, head);
    list_add_tail(&pivot->list, head);
    list_splice_tail(&greater, head);
    return;
}
