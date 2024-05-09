#ifndef BPT_FILE
#define BPT_FILE

#include <fstream>

namespace sjtu {

template<class T, int info_len>
class File {
private:
  int tail = (info_len + 1) * sizeof(int);
  std::fstream file;
  std::string file_name;
  int siz_T = sizeof(T);

public:
  File() = default;

  File(const std::string& file_name) : file_name(file_name) {}

  void initialise(std::string file_n = "") {
    if(file_n != "") file_name = file_n;
    file.open(file_name, std::ios::out);
    int tmp = 0;
    for(int i = 0; i <= info_len; ++i)
      file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  void get_info(int &tmp, int n = 1) {
    if (n > info_len) return;
    file.open(file_name, std::ios::in);
    file.seekg(n * sizeof(int));
    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  void write_info(int tmp, int n = 1) {
    if (n > info_len) return;
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekp(n * sizeof(int));
    file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  int write(T &t) {
    file.open(file_name, std::ios::in | std::ios::out);
    int pos;
    file.read(reinterpret_cast<char *>(&pos), sizeof(int));
    if(pos == 0) {
      file.seekp(tail);
      file.write(reinterpret_cast<char *>(&t), siz_T);
      tail += siz_T;
      file.close();
      return tail - siz_T;
    }
    else {
      file.seekg(pos);
      int next_pos;
      file.read(reinterpret_cast<char *>(&next_pos), sizeof(int));
      file.seekp(0);
      file.write(reinterpret_cast<char *>(&next_pos), sizeof(int));
      file.seekp(pos);
      file.write(reinterpret_cast<char *>(&t), siz_T);
      file.close();
      return pos;
    }
  }

  void update(T &t, const int index) {
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&t), siz_T);
    file.close();
  }

  void read(T &t, const int index) {
    file.open(file_name, std::ios::in);
    file.seekg(index);
    file.read(reinterpret_cast<char *>(&t), siz_T);
    file.close();
  }

  void Delete(int index) {
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekg(0);
    int next_pos;
    file.read(reinterpret_cast<char *>(&next_pos), sizeof(int));
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&next_pos), sizeof(int));
    file.seekp(0);
    file.write(reinterpret_cast<char *>(&index), sizeof(int));
    file.close();
  }
};

}

#endif //BPT_FILE
