#include <iostream>
#include <fstream>
#include <list>
#include <optional>
#include <regex>
#include <string>
#include <set>
#include <unordered_set>
#include <queue>
#include <vector>
#include "../shared/base.hpp"

struct Entry2 {
    long num;
    Entry2* prev;
    Entry2* next;
    Entry2* mix_next;

    Entry2(long num, Entry2* prev) : num(num), prev(prev), next(nullptr), mix_next(nullptr) {}
};

class List;

class ListIterator {
    Entry2* entry;
    List* list;
    bool mix;

public:
    ListIterator(Entry2* entry, List* list, bool mix) : entry(entry), list(list), mix(mix) {}

    Entry2* operator*() {
        return entry;
    }

    void operator++() {
        if (mix) {
            entry = entry->mix_next;
        } else {
            entry = entry->next;
        }
    }

    void operator--();

    bool operator==(const ListIterator& other) const {
        return entry == other.entry && mix == other.mix;
    }
};

class WrappingListIterator {
    ListIterator it;
    List* list;
    bool forward;

public:
    WrappingListIterator(ListIterator it, List* list);

    Entry2* operator*() {
        return *it;
    }

    void reverse();

    void operator++();
};

class List {
    Entry2* head;
    Entry2* tail;
    Entry2* mix_head;
    std::size_t size;

    friend class ListIterator;

public:
    List() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    void push_back(long num) {
        Entry2* entry = new Entry2(num, tail);
        if (tail != nullptr) {
            tail->next = entry;
            tail->mix_next = entry;
        } else {
            head = entry;
            mix_head = entry;
        }
        tail = entry;
        size++;
    }

    WrappingListIterator begin() {
        return WrappingListIterator(_begin(), this);
    }

    ListIterator _begin() {
        return ListIterator(head, this, false);
    }

    ListIterator _end() {
        return ListIterator(nullptr, this, false);
    }

    ListIterator mix_begin() {
        return ListIterator(mix_head, this, true);
    }

    ListIterator mix_end() {
        return ListIterator(nullptr, this, true);
    }

    void move(ListIterator it, long amount) {
        amount = amount % ((long) size - 1L);
        if (amount == 0) {
            return;
        }

        Entry2* elem = *it;
        if (elem->prev == nullptr) {
            this->head = elem->next;
        } else {
            elem->prev->next = elem->next;
        }
        if (elem->next == nullptr) {
            this->tail = elem->prev;
        } else {
            elem->next->prev = elem->prev;
        }
    
        Entry2* it_start = elem->next;
        if (it_start == nullptr) {
            it_start = this->head;
        }
        WrappingListIterator move_it = WrappingListIterator(ListIterator(it_start, this, false), this);
        if (amount > 0) {
            for (long i = 0; i < amount; i++) {
                ++move_it;
            }
        } else {
            move_it.reverse();
            for (long i = 0; i > amount; i--) {
                ++move_it;
            }
        }
        
        Entry2* insert_before = *move_it;
        elem->next = insert_before;
        if (insert_before == nullptr) {
            elem->prev = this->tail;
            elem->prev->next = elem;
            this->tail = elem;
        } else {
            Entry2* insert_after = insert_before->prev;
            elem->prev = insert_after;
            if (insert_after == nullptr) {
                this->head = elem;
            } else {
                insert_after->next = elem;
            }
            insert_before->prev = elem;
        }
    }
};

WrappingListIterator::WrappingListIterator(ListIterator it, List* list) : it(it), list(list), forward(true) {
    if (it == list->_end()) {
        it = list->_begin();
    }
    this->it = it;
}

void ListIterator::operator--() {
    if (entry == nullptr) {
        entry = list->tail;
    } else {
        entry = entry->prev;
    }
}

void WrappingListIterator::operator++() {
    if (forward) {
        ++it;
        if (it == list->_end()) {
            it = list->_begin();
        }
    } else {
        --it;
        if (it == list->_begin()) {
            it = list->_end();
        }
    }
}

void WrappingListIterator::reverse() {
    forward = !forward;
    if (forward) {
        if (it == list->_end()) {
            it = list->_begin();
        }
    } else {
        if (it == list->_begin()) {
            it = list->_end();
        }
    }
}

List parse2(std::ifstream& input);

void part2(std::ifstream& input) {
    List list = parse2(input);
    for (int i = 0; i < 10; i++) {
        ListIterator it = list.mix_begin();
        while (it != list.mix_end()) {
            list.move(it, (*it)->num);
            ++it;
        }
    }
    
    WrappingListIterator read_it = list.begin();
    while ((*read_it)->num != 0) {
        ++read_it;
    }

    long result = 0;
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 1000; i++) {
            ++read_it;
        }
        result += (*read_it)->num;
    }
    std::cout << result << std::endl;
}

List parse2(std::ifstream& input) {
    std::string line;
    List result;
    while (std::getline(input, line)) {
        result.push_back(std::stol(line) * 811589153L);
    }
    return result;
}
