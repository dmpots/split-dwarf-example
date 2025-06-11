#include <vector>
#include <set>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <string>
#include <bitset>
#include <tuple>

// Add lots of c++ stdlib types to generate some interesting dwarf.
int foo() {
    std::set<int> s;
    s.insert(1);

    std::list<int> l;
    l.emplace_back(1);

    std::map<int, int> m;
    m[1] = 1;

    std::unordered_map<int, bool> um;
    um[1] = false;

    std::unordered_set<double> us;
    us.insert(1.0);

    std::deque<int> d;
    d.push_front(1);

    std::bitset<8> b8;
    b8.set(1);

    std::bitset<16> b16;
    b16.set(1);

    std::string str = "hello";

    std::tuple<int, int, int> t = std::make_tuple(1, 2, 3);
    std::get<0>(t);

    std::vector<int> v;
    v.push_back((1));
    return v.back();
}
