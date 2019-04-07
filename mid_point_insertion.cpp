#include <iostream>
#include <list>
#include <optional>

#define HOT_TRESHOLD 10
#define INFLATION 200

struct Element {
    int value; 
    int hits = 0; 
    int timer = 0; 
    Element(int n) : value(n) {};
};

class Sublist {
public:
    Sublist(const int size) : max_size(size) {};

    void print_data() {
        for (const auto &el : data) {
            std::cout << '(' << el.value << ", " << el.hits << ") ";
        }
        std::cout << std::endl;
    }

    void increment_each_timer() {
        for (auto &el : data) {
            ++el.timer;
        }
    }

    int get_total_hits() {
        return total_hits;
    }

    bool num_exists(int n) {
        for (const auto &el : data) {
            if (el.value == n) return true;
        }
        return false;
    }

    using riter = std::list<Element>::reverse_iterator;
    void swap_current_el(riter &&i) {
        for (auto j = i++; i != data.rend(); ++j, ++i) {
            if ((*j).hits >= (*i).hits) std::swap(*i, *j);
            else return;
        }
    }

    std::optional<Element> add(const Element &el) {
        for (riter i = data.rbegin(); i != data.rend(); ++i) {
            if ((*i).value == el.value) {
                ++total_hits;
                ++(*i).hits;
                swap_current_el(std::move(i));
                return {};
            }
        }
        if (data.size() + 1 > max_size) {
            Element last = data.back();
            data.pop_back();
            data.push_back(el);
            swap_current_el(data.rbegin());
            return last;
        }
        data.push_back(el);
        swap_current_el(data.rbegin());
        return {};
    }

protected:
    std::list<Element> data;
    int hot_treshold = HOT_TRESHOLD;

private:
    int max_size;
    int total_hits = 0; 
};

class WarmSublist : public Sublist {
public:
    WarmSublist(const int size) : Sublist(size) {};

    std::optional<Element> reached_treshold() {
        if (data.front().hits >= hot_treshold) {
            Element tmp = data.front();
            data.pop_front();
            return tmp;
        }
        return {};
    } 
};

class HotSublist : public Sublist {
public:
    HotSublist(const int size) : Sublist(size) {};

    std::list<Element> inflate() {
        std::list<Element> inflated;
        for (auto it = data.begin(); it != data.end(); ) {
            (*it).hits = (*it).hits * 3 / 4;
            if ((*it).hits < hot_treshold) {
                inflated.push_back(*it);
                data.erase(it++);
            } else ++it;
        }
        return inflated;
    }
};

class Cache {
public:
    Cache() : hot(8), warm(120) {};

    int get_total_hits() {
        return hot.get_total_hits() + warm.get_total_hits();
    }

    void add(int n) {
        (hot.num_exists(n)) ? hot.add(n) : warm.add(n);
        auto to_hot = warm.reached_treshold();
        if (to_hot) {
            auto to_warm = hot.add(to_hot.value());
            if (to_warm) 
                warm.add(to_warm.value());
        }
        if (++time % inflation == 0) {
            std::list<Element> temp = hot.inflate();
            for (auto &el : temp) {
                warm.add(el);
            }
        }
        hot.increment_each_timer();
        warm.increment_each_timer();
    }

private:
    HotSublist hot;
    WarmSublist warm;
    int inflation = INFLATION;
    int time = 0;
};

int main() {
    Cache cache;
    int n = 0;
    while (std::cin >> n) {
        cache.add(n);
    } 
    std::cout << cache.get_total_hits() << '\n';

    return 0;
}