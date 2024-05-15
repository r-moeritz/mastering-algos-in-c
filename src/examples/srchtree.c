#include "../../include/bistree.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    const char* surname;
    const char* name;
    const char* phone;
} Contact;

Contact* create_contact(const char* surname,
                        const char* name,
                        const char* phone) {
    Contact* c = malloc(sizeof(Contact));
    if (!c) {
        return NULL;
    }

    c->surname = surname;
    c->name = name;
    c->phone = phone;

    return c;
}

void build_contacts(BisTree* contacts) {
    Contact* c = create_contact("Fonda", "Jane", "0491 570 006");
    bistree_insert(contacts, c);

    c = create_contact("Nixon", "Richard", "0491 571 804");
    bistree_insert(contacts, c);

    c = create_contact("Manson", "Charles", "1800 160 401");
    bistree_insert(contacts, c);

    c = create_contact("Bellafonte", "Harry", "1300 975 707");
    bistree_insert(contacts, c);

    c = create_contact("Kelly", "Grace", "0491 578 888");
    bistree_insert(contacts, c);
}

int compare_contacts_by_surname(const void* contact1, const void* contact2) {
    const Contact* c1 = (const Contact*) contact1;
    const Contact* c2 = (const Contact*) contact2;

    return strcmp(c1->surname, c2->surname);
}

int main(void) {
    BisTree contacts;
    Contact* c;
    int notfound;
    char surname[32];

    bistree_init(&contacts, compare_contacts_by_surname, free);

    build_contacts(&contacts);

    printf("Surname to search for? ");
    fgets(surname, sizeof(surname) - 1, stdin);
    surname[strlen(surname) - 1] = '\0'; // chop off trailing \n

    c = create_contact(surname, NULL, NULL);
    notfound = bistree_lookup(&contacts, (void**) &c);
    if (notfound) {
        printf("No number found for \"%s\"", surname);
    }
    else {
        puts("Found:");
        printf("%s, %s: %s", c->surname, c->name, c->phone);
    }

    puts(""); /* final newline */

    return 0;
}
