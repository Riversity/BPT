#ifndef BPT_HPP
#define BPT_HPP

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include "vector.hpp"
#include "file.hpp"
#include "utils.hpp"

namespace sjtu {
template<class Key, class T, const int M = 100, const int L = 100>
class BPTree {
private:
  int total;
  int root; // pos in f_i
  using Pair = std::pair<Key, T>;

  struct index_node {
    Pair val[M];
    int pos_index_node[M + 1]; // one more
    int cnt;
    bool isLeaf;
    index_node() {
      cnt = 0;
      isLeaf = true;
    }
    int unequal_higher_bound(const Pair& origin) const { // find the one > origin
      if(cnt == 1) return pos_index_node[0];
      if(val[cnt - 2] <= origin) return pos_index_node[cnt - 1];
      int l = 0, r = cnt - 2;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m] <= origin) l = m + 1;
        else r = m;
      }
      return pos_index_node[l];
    }
    int higher_bound(const Pair& origin) const { // find the one >= origin
      if(cnt == 1) return pos_index_node[0];
      if(val[cnt - 2] < origin) return pos_index_node[cnt - 1];
      int l = 0, r = cnt - 2;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m] < origin) l = m + 1;
        else r = m;
      }
      return pos_index_node[l];
    }
    std::pair<int, int> higher_bound_pair(const Pair& origin) const { // find the one > origin
      if(cnt == 1) return {pos_index_node[0], 0};
      if(val[cnt - 2] <= origin) return {pos_index_node[cnt - 1], cnt - 1};
      int l = 0, r = cnt - 2;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m] <= origin) l = m + 1;
        else r = m;
      }
      return {pos_index_node[l], l};
    }
    int higher_bound_by_key(const Key& origin) const { // find the one >= origin
      if(cnt == 1) return pos_index_node[0];
      if(val[cnt - 2].first < origin) return pos_index_node[cnt - 1];
      int l = 0, r = cnt - 2;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m].first < origin) l = m + 1;
        else r = m;
      }
      return pos_index_node[l];
    }
    void insert_index(const Pair& dat, int pos, int k) {
      // pos_index_node[k] is splitted
      for(int i = cnt - 1; i > k; --i) {
        val[i] = val[i - 1];
        pos_index_node[i + 1] = pos_index_node[i];
      }
      val[k] = dat;
      pos_index_node[k + 1] = pos;
      ++cnt;
    }
    void delete_by_pos(int k) {
      for(int i = k; i < cnt - 1; ++i) {
        pos_index_node[i] = pos_index_node[i + 1];
        val[i - 1] = val[i];
      }
      --cnt;
    }
  };
  struct val_node { // leaf node
    Pair val[L + 1];
    int siz;
    int nxt_pos;
    val_node() {
      siz = 0;
      nxt_pos = -1;
    }
    int higher_bound(const Pair& origin) const { // find the one >= origin
      if(siz == 0 || val[siz - 1] < origin) return siz;
      int l = 0, r = siz - 1;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m] < origin) l = m + 1;
        else r = m;
      }
      return l;
    }
    int higher_bound_by_key(const Key& origin) const { // find the one >= origin
      if(siz == 0 || val[siz - 1].first < origin) return -1; // Here specialized
      int l = 0, r = siz - 1;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m].first < origin) l = m + 1;
        else r = m;
      }
      return l;
    }
    bool insert(const Pair& origin) {
      //siz < L
      int hi_bound = higher_bound(origin);
      if(hi_bound != siz && origin == val[hi_bound]) return false;
      for(int i = siz; i > hi_bound; --i) {
        val[i] = val[i - 1];
      }
      val[hi_bound] = origin;
      ++siz;
      return true;
    }
    bool erase(const Pair& origin) {
      if(siz == 0) return false;
      int hi_bound = higher_bound(origin);
      if(hi_bound == siz || origin != val[hi_bound]) return false;
      for(int i = hi_bound + 1; i < siz; ++i) {
        val[i - 1] = val[i];
      }
      --siz;
      return true;
    }
  };

  File<index_node, 2> f_index; // 1 for total, 2 for root
  File<val_node, 2> f_val;

public:
  explicit BPTree(const std::string &name_f_index, const std::string &name_f_val) 
                                        : f_index(name_f_index), f_val(name_f_val) {
    //std::fstream file;
    //file.open(name_f_index, std::ios::in);
    if(!std::filesystem::exists(name_f_index)) {
      f_index.initialise();
      f_val.initialise();
      total = 0;
      root = -1;
    }
    else {
      f_index.get_info(total, 1);
      f_index.get_info(root, 2);
    }
  }

  ~BPTree() {
    f_index.write_info(total, 1);
    f_index.write_info(root, 2);
  }

  int find_first(int index_node_pos, const Key &key) { // Return pos of the block in f_val
    index_node r;
    f_index.read(r, index_node_pos);
    if(r.isLeaf) {
      return r.higher_bound_by_key(key);
    }
    else {
      int nxt_pos = r.higher_bound_by_key(key);
      return find_first(nxt_pos, key);
    }
  } // Notice, not necessarily find the one, maybe the preceding one.
  vector<T> find(const Key &key) {
    vector<T> ret;
    if(root == -1) return ret;
    int pos = find_first(root, key);
    val_node block;
    f_val.read(block, pos);
    int k = block.higher_bound_by_key(key);

    if(k == -1) {
      pos = block.nxt_pos;
      if(pos == -1) return ret;
      f_val.read(block, pos);
      k = 0;
    }
    while(block.siz == 0) {
      pos = block.nxt_pos;
      if(pos == -1) return ret;
      f_val.read(block, pos);
    }
    while(block.val[k].first == key) {
      ret.push_back(block.val[k].second);
      if(k >= block.siz - 1) { // end of the block
        do {
          pos = block.nxt_pos;
          if(pos == -1) return ret;
          f_val.read(block, pos);
        } while(block.siz == 0);
        k = 0;
      }
      else {
        ++k;
      }
    }
    return ret;
  }

  std::pair<bool, int> val_insert(const Pair& dat, int pos, Pair& ret) {
    val_node cur;
    f_val.read(cur, pos);
    if(cur.siz == L) {
      // L-L/2 to original, L/2 to new
      val_node new_cur;
      new_cur.nxt_pos = cur.nxt_pos;
      for(int i = 0; i < L/2; ++i) {
        new_cur.val[i] = cur.val[i + L-L/2];
      }
      cur.siz = L-L/2;
      new_cur.siz = L/2;
      if(new_cur.val[0] <= dat) new_cur.insert(dat);
      else cur.insert(dat);
      ret = new_cur.val[0];
      int new_cur_pos = f_val.write(new_cur);
      cur.nxt_pos = new_cur_pos;
      f_val.update(cur, pos);
      return {true, new_cur_pos};
    }
    else {
      if(cur.insert(dat)) {
        ++total;
        f_val.update(cur, pos);
      }
      return {false, 0};
    }
  }
  
  std::pair<bool, int> insert_at(const Pair& dat, int index_pos, Pair& ret) {
    // return value is status: whether splitted, where new block
    index_node cur;
    f_index.read(cur, index_pos);

    std::pair<int, int> pos = cur.higher_bound_pair(dat);

    std::pair<bool, int> res;
    Pair new_ret;
    if(cur.isLeaf) {
      res = val_insert(dat, pos.first, new_ret);
    }
    else {
      res = insert_at(dat, pos.first, new_ret);
    }
    if(res.first) {
      // splitted, node + 1
      if(cur.cnt == M) {
        // M-M/2 to original, M/2+1 to new
        cur.insert_index(new_ret, res.second, pos.second);
        index_node new_cur;
        int new_cur_pos;
        for(int i = 0; i < M/2; ++i) {
          new_cur.pos_index_node[i] = cur.pos_index_node[M-M/2 + i];
          new_cur.val[i] = cur.val[M-M/2 + i];
        }
        new_cur.pos_index_node[M/2] = cur.pos_index_node[M];
        ret = cur.val[M-M/2 - 1];
        new_cur.cnt = M/2 + 1;
        cur.cnt = M-M/2;
        new_cur.isLeaf = cur.isLeaf;
        f_index.update(cur, index_pos);
        new_cur_pos = f_index.write(new_cur);
        if(index_pos == root) {
          index_node new_root;
          new_root.isLeaf = false;
          new_root.cnt = 2;
          new_root.val[0] = ret;
          new_root.pos_index_node[0] = index_pos;
          new_root.pos_index_node[1] = new_cur_pos;
          root = f_index.write(new_root);
        }
        return {true, new_cur_pos};
      }
      else {
        cur.insert_index(new_ret, res.second, pos.second);
        f_index.update(cur, index_pos);
        return {false, 0};
      }
    }
    else return {false, 0};
  }
  void insert(const Pair& dat) {
    if(root == -1) {
      val_node v;
      v.val[0] = dat;
      v.siz = 1;
      index_node i;
      i.isLeaf = true;
      i.cnt = 1;
      i.pos_index_node[0] = f_val.write(v);
      root = f_index.write(i);
    }
    else {
      Pair ret;
      insert_at(dat, root, ret);
    }
  }
  std::pair<bool, int> erase_at(const Pair& dat, int index_pos) {
  // std::pair<bool, int> erase_at(const Pair& dat, int index_pos, Pair& new_pivot) {
    // first return value whether erased, second the pos of the deleted node
    // void erase_at(const Pair& dat, int index_pos) { // no merge
    index_node r;
    f_index.read(r, index_pos);
    std::pair<int, int> pos_right = r.higher_bound_pair(dat);
    int pos_left = -1;
    if(pos_right.second > 0) pos_left = r.pos_index_node[pos_right.second - 1];
    if(r.isLeaf) {
      val_node v_right;
      f_val.read(v_right, pos_right.first);
      if(v_right.erase(dat)) {
        --total;
        val_node v_left;
        if(pos_left != -1 && v_right.siz < M/2) {
          f_val.read(v_left, pos_left);
          if(v_left.siz + v_right.siz <= M) { // merge
            for(int i = 0; i < v_right.siz; ++i) {
              v_left.val[v_left.siz + i] = v_right.val[i];
            }
            v_left.siz += v_right.siz;
            v_left.nxt_pos = v_right.nxt_pos;
            f_val.Delete(pos_right.first);
            f_val.update(v_left, pos_left);
            return {true, pos_right.second};
          }
        }
      f_val.update(v_right, pos_right.first);
      }
      return {false, -1};
    }
    else {
      std::pair<bool, int> ret = erase_at(dat, pos_right.first);
      if(ret.first) {
        index_node i_right;
        f_index.read(i_right, pos_right.first);
        i_right.delete_by_pos(ret.second);
        //int pos_left = -1;
        //index_node i_left;

        f_index.update(i_right, pos_right.first);
      }
      return {false, -1};
    }
  }
  void erase(const Pair& dat) {
    if(root == -1) return;
    erase_at(dat, root);
  }
  /*
  void put(int pos) {
    val_node s;
    f_val.read(s, pos);
    std::cerr<<"Now block at "<<pos<<std::endl;
    std::cerr<<"Block size is "<<s.siz<<std::endl;
    for(int i = 0; i < s.siz; ++i) {
      std::cerr<<s.val[i].first<<s.val[i].second<<std::endl;
    }
    std::cerr<<"Block End!"<<std::endl;
  }
  void traverse(int pos) {
    index_node r;
    f_index.read(r, pos);
    std::cerr<<"Now index at "<<pos<<std::endl;
    std::cerr<<"Index size is "<<r.cnt<<std::endl;
    if(r.isLeaf) {
      for(int i = 0; i < r.cnt - 1; ++i) {
        put(r.pos_index_node[i]);
        std::cerr<<"Pivot is "<<r.val[i].first<<r.val[i].second<<std::endl;
      }
      put(r.pos_index_node[r.cnt - 1]);
    }
    else {
      for(int i = 0; i < r.cnt - 1; ++i) {
        traverse(r.pos_index_node[i]);
        std::cerr<<"Pivot is "<<r.val[i].first<<r.val[i].second<<std::endl;
      }
      traverse(r.pos_index_node[r.cnt - 1]);
    }
    std::cerr<<"Index "<<pos<<" End!"<<std::endl;
  }
  void traverse() {
    traverse(root);
  }
  void test(int m) {
    const char* kk = "aaaa";
    Key k(kk);
    for(int i = 0; i < m; ++i) {
      insert({k, i});
      erase({k, i-1});
    }
    auto v = find(k);
    for(int i : v) {
      std::cout<<i<<std::endl;
    }
  }
  */
};

}

#endif
