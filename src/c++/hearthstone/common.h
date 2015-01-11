#include <assert.h>

#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For shared_ptr, unique_ptr

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <unordered_map>

using namespace std;

typedef long htype;
typedef vector<string> ArrayString;

#define issubclass( obj, cls)   dynamic_cast<cls>(obj)
#define in(el, set) (set.find(el) != set.end())

#define NEWP(type, ...) shared_ptr<type>(new type(##__VA_ARGS__))


inline string string_format(const string fmt_str, ...) {
  int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
  string str;
  unique_ptr<char[]> formatted;
  va_list ap;
  while (1) {
    formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
    strcpy(&formatted[0], fmt_str.c_str());
    va_start(ap, fmt_str);
    final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
    va_end(ap);
    if (final_n < 0 || final_n >= n)
      n += abs(final_n - n + 1);
    else
      break;
  }
  return string(formatted.get());
}