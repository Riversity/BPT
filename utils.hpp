#ifndef BPT_UTILS_HPP
#define BPT_UTILS_HPP

#include <iostream>
#include <cstring>

namespace sjtu {
  template <int len>
  class Str {
  private:
    char st[len];
  public:
    Str() {
      st[0] = '\0';
    }
    Str(const char* s) {
      strcpy(s, st);
    }
    Str(const std::string &s) {
        strcpy(st, s.c_str());
    }
    char &operator[](int pos) {
        return st[pos];
    }
    Str &operator=(const Str &rhs) {
        if(this == &rhs) return *this;
        strcpy(st, rhs.st);
        return *this;
    }
    Str &operator=(const char *s) {
        strcpy(st, s);
        return *this;
    }
    Str &operator=(const std::string &s) {
        strcpy(st, s.c_str());
        return *this;
    }
    bool operator==(const Str &rhs) const {
        return !strcmp(st, rhs.st);
    }
    bool operator!=(const Str &rhs) const {
        return strcmp(st, rhs.st);
    }
    bool operator<(const Str &rhs) const {
        return strcmp(st, rhs.st) < 0;
    }
    friend std::istream &operator>>(std::istream &is, Str &rhs) {
        return is >> rhs.st;
    }
    friend std::ostream &operator<<(std::ostream &os, const Str &rhs) {
        return os << rhs.st;
    }
  };
}

#endif
