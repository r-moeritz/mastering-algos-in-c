#include <string.h>
#include "../include/list.h"
#include "../include/set.h"

void set_init(Set* set,
              int (*match)(const void* key1, const void* key2),
              void (*destroy)(void* data)) {
    list_init(set, destroy);
    set->match = match;
}


int set_insert(Set* set, const void* data) {
    if (set_is_member(set, data)) {
        return 1;
    }

    return list_ins_next(set, list_tail(set), data);
}

int set_remove(Set* set, void** data) {
    ListElmt* member;
    ListElmt* prev;

    prev = NULL;

    for (member = list_head(set); member != NULL; member = list_next(member)) {
        if (set->match(*data, list_data(member))) {
            break;
        }
        prev = member;
    }

    if (!member) {
        return -1;
    }

    return list_rem_next(set, prev, data);
}

int set_union(Set* setu, const Set* set1, const Set* set2) {
    ListElmt* member;
    void* data;

    set_init(setu, set1->match, NULL);

    /* Insert the members of the 1st set */
    for (member = list_head(set1); member != NULL; member = list_next(member)) {
        data = list_data(member);

        if (list_ins_next(setu, list_tail(setu), data)) {
            set_destroy(setu);
            return -1;
        }
    }

    /* Insert the members of the 2nd set */
    for (member = list_head(set2); member != NULL; member = list_next(member)) {
        if (set_is_member(set1, list_data(member))) {
            continue; /* don't allow duplicates */
        }

        data = list_data(member);

        if (list_ins_next(setu, list_tail(setu), data)) {
            set_destroy(setu);
            return -1;
        }
    }

    return 0;
}

int set_intersection(Set* seti, const Set* set1, const Set* set2) {
    ListElmt* member;
    void* data;

    set_init(seti, set1->match, NULL);

    /* Insert members present in both sets */
    for (member = list_head(set1); member != NULL; member = list_next(member)) {
        if (!set_is_member(set2, list_data(member))) {
            continue; /* skip member not present in both sets */
        }

        data = list_data(member);
        if (list_ins_next(seti, list_tail(seti), data)) {
            set_destroy(seti);
            return -1;
        }
    }

    return 0;
}

int set_difference(Set* setd, const Set* set1, const Set* set2) {
    ListElmt* member;
    void* data;

    set_init(setd, set1->match, NULL);

    /* Insert the members from set1 not present in set2 */
    for (member = list_head(set1); member != NULL; member = list_next(member)) {
        if (set_is_member(set2, list_data(member))) {
            continue; /* skip member present in set2 */
        }

        data = list_data(member);
        if (list_ins_next(setd, list_tail(setd), data)) {
            set_destroy(setd);
            return -1;
        }
    }

    return 0;
}

int set_is_member(const Set* set, const void* data) {
    ListElmt* member;

    for (member = list_head(set); member != NULL; member = list_next(member)) {
        if (set->match(data, list_data(member))) {
            return 1;
        }
    }

    return 0;
}

int set_is_subset(const Set* set1, const Set* set2) {
    ListElmt* member;

    if (set_size(set1) > set_size((set2))) {
        return 0;
    }

    for (member = list_head(set1); member != NULL; member = list_next(member)) {
        if (!set_is_member(set2, list_data(member))) {
            return 0;
        }
    }

    return 1;
}

int set_is_equal(const Set* set1, const Set* set2) {
    if (set_size(set1) != set_size((set2))) {
        return 0;
    }

    return set_is_subset(set1, set2);
}
