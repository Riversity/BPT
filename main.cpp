#include <iostream>
#include <cstdio>
#include <string>

#include "bptree.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  //freopen("1.in", "r", stdin);
  //freopen("1.out", "w", stdout);
  int n;
  std::cin >> n;
  sjtu::BPTree<sjtu::Str<64>, int, 54, 56> bpt("index", "val");
  //bpt.test(500);
  for(int i = 0; i < n; ++i) {
    std::string cmd;
    std::cin >> cmd;
    sjtu::Str<64> name;
    int val;
    if(cmd == "insert") {
      std::cin >> name >> val;
      bpt.insert({name, val});
    }
    else if(cmd == "delete") {
      std::cin >> name >> val;
      bpt.erase({name, val});
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
  //bpt.traverse();
  //fclose(stdin);
  //fclose(stdout);
  return 0;
}
