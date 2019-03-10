#include "map.h"
#include <stdio.h>
#include <stdlib.h>

struct user_t {
    const char* name;
    int age;
};

const int NUSERS = 4;
struct user_t users[NUSERS] = {{"john", 25},
    {"jane", 33},
    {"albert", 38},
    {"ross", 18}
};

// utility function
static void print_users(const struct map_t* map) {
    struct map_node_t* node;

    for (node = map->head; node; node = node->next) {
        struct user_t* u = (struct user_t*)node->value;
        printf("[%s] => {%s:%d}\n", node->key, u->name, u->age);
    }
}

int main() {
    struct map_t* test_map = new_map();

    // build map with some test users
    int i;
    for (i = 0; i < NUSERS; i++) {
        map_set(test_map, users[i].name, (void*)&users[i]);
    }

    print_users(test_map);

    // search and delete user
    const char *name = "albert";
    struct user_t* p = map_get(test_map, name);
    if (p) {
        printf("\nFound %s => {%s:%d}\n\n", name, p->name, p->age);
        map_del(test_map, name);
    }

    print_users(test_map);

    destroy_map(&test_map);
}