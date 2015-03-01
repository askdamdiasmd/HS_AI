#ifndef __COMMON_H__
#define __COMMON_H__

#include <assert.h>

#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For shared_ptr, unique_ptr
#include <algorithm>

#include <sstream>
#include <string>
#include <iterator>
#include <vector>
#include <list>
#include <deque>
#include <unordered_map>

using namespace std;

typedef long htype;
typedef vector<string> ArrayString;

#define issubclass( obj, cls)   dynamic_cast<cls>(obj)
#define issubclassP( obj, cls)   dynamic_pointer_cast<cls>(obj)
#define in(el, set) (set.find(el) != set.end())

#define NEWP(type, ...) make_shared<type>(##__VA_ARGS__)


inline string string_format(const char* format, ...) {
  va_list args;
  va_start(args, format);
  int len = _vscprintf(format, args) + 1; // terminating '\0'
  unique_ptr<char[]>  buffer(new char[len]);
  vsprintf_s(buffer.get(), len, format, args);
  return string(buffer.get());
}

#define NI  assert(!"not implemented!")

#define SET_ENGINE()  \
  static Engine* engine; \
  static void set_engine(Engine* e) {engine = e;}

#define NAMED_PARAM(cls, type, param)  cls* set_##param(type v) { param = v; return this; }

#define UPDATE_STATUS(which) engine->send_status(NEWP(Msg_Status,PThing(this),which))
#define SEND_MSG(type, ...)  engine->send_message(NEWP(type,##__VA_ARGS__));

inline bool startswith(string s, const char* comp) {
  return !s.compare(0, strlen(comp), comp);
}

const float INF = 9e9f; // 1.f / 0.f;

template<typename T>
inline int len(const T& container) {
  return (int)container.size();
}

inline vector<string> split(string text) {
  istringstream iss(text);
  // no clue what is happening here but well
  vector<string> tokens { istream_iterator<string>{iss},
                          istream_iterator<string>{} };
  // remove empty tokens
  for (unsigned i = 0; i < tokens.size(); ++i) 
    if (tokens[i].empty()) 
      tokens.erase(tokens.begin()+i--);

  return tokens;
}
inline string join(string chr, vector<string> words) {
  string res;
  for (int i = 0; i < len(words); ++i) {
    res += words[i];
    if (i + 1 < len(words)) res += chr;
  }
  return res;
}

template<typename Type>
inline Type pop_at(vector<Type> & vec, int i) {
  assert(i >= 0 && i < len(vec));
  Type res = vec[i];
  vec.erase(vec.begin()+i);
  return res;
}
template<typename Type>
inline Type pop_front(vector<Type> & vec) {
  return pop_at(vec, 0);
}
template<typename Type>
inline Type pop_back(vector<Type> & vec) {
  return pop_at(vec, vec.size()-1);
}
template<typename Type>
inline int index(vector<Type> & vec, const Type& ref) {
  for (int i = 0; i < (signed)vec.size(); ++i)
    if (vec[i] == ref) 
      return i;
  throw exception("no such element in list");
}
template<typename Type>
inline void remove(vector<Type> & vec, const Type& ref) {
  vec.erase(vec.begin() + index(vec,ref));
}

inline int randint(int min, int max) {
  // max is included in range
  assert(min <= max);
  return (rand() % (max + 1 - min)) + min;
}


// declarations of classes and subtypes

struct Engine;
struct Player;
struct Board;
struct VizBoard;
typedef shared_ptr<VizBoard> PVizBoard;
struct Slot;
typedef vector<Slot> ListSlot;
//struct Target;
//typedef vector<Target> ListTarget;

struct Card;
typedef shared_ptr<Card> PCard;
typedef vector<PCard> ListCard;
struct Card_Instance;
typedef shared_ptr<Card_Instance> PCardInstance;
struct Card_Minion;
typedef shared_ptr<Card_Minion> PCardMinion;
struct Card_Hero;
typedef shared_ptr<Card_Hero> PCardHero;
struct Card_HeroAbility;
typedef shared_ptr<Card_HeroAbility> PCardHeroAbility;

struct Deck;
typedef shared_ptr<Deck> PDeck;

struct Action;
//typedef shared_ptr<Action> PAction;
typedef vector<const Action* const> ListAction;

struct Effect;
typedef shared_ptr<Effect> PEffect;
typedef vector<PEffect> ListEffect;

struct Instance;
typedef shared_ptr<Instance> PInstance;
typedef vector<PInstance> ListInstance;
struct Thing;
typedef shared_ptr<Thing> PThing;
typedef vector<PThing> ListThing;
struct Secret;
typedef shared_ptr<Secret> PSecret;
typedef vector<PSecret> ListSecret;
struct Weapon;
typedef shared_ptr<Weapon> PWeapon;
struct Creature;
typedef shared_ptr<Creature> PCreature;
typedef vector<PCreature> ListCreature;
struct Minion;
typedef shared_ptr<Minion> PMinion;
typedef vector<PMinion> ListMinion;
struct Hero;
typedef shared_ptr<Hero> PHero;

struct VizInstance;

struct Message;
struct Msg_Status;
typedef shared_ptr<Msg_Status> PMsgStatus;

//typedef ListAction(*FuncListAction)(Engine* e, PInstance from, PInstance target);
typedef void(*FuncAction)(Engine* e, PInstance from, PInstance target);

#include "targets.h"
#include "events.h"

#endif