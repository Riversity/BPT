#include <iostream>
#include <cstdio>
#include <string>

#include "bptree.hpp"

int main() {
  //std::ios::sync_with_stdio(false);
  //std::cin.tie(nullptr);
  //std::cout.tie(nullptr);
  //freopen("2.in", "r", stdin);
  //freopen("2.out", "w", stdout);
  int n;
  std::cin >> n;
  sjtu::BPTree<sjtu::Str<65>, int, 1000, 1000> bpt("index", "val");
  for(int i = 0; i < n; ++i) {
    std::string cmd;
    std::cin >> cmd;
    sjtu::Str<65> name;
    int val;
    if(cmd == "insert") {
      std::cin >> name >> val;
      bpt.insert({name, val});
    }
    else if(cmd == "delete") {
      std::cin >> name >> val;
    }
    else if(cmd == "find") {
      std::cin >> name;
      sjtu::vector<int> v = bpt.find(name);
      if(v.empty()) {
        std::cout << "null\n";
      }
      else {
        for(int i : v) {
          std::cout << i << ' ';
        }
        std::cout << '\n';
      }
    }
  }
  //fclose(stdin);
  //fclose(stdout);
  return 0;
}
