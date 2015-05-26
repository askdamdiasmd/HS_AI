#ifndef __COMMON_H__
#define __COMMON_H__

#include <assert.h>

#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For shared_ptr, unique_ptr
#include <algorithm>

#include <sstream>
#include <string>
#include <cstring>  // strlen(.)
#include <iterator>
#include <vector>
#include <list>
#include <unordered_map>

using namespace std;

//typedef long htype;
typedef vector<string> ArrayString;

#define NEWP(type, ...) make_shared<type>(__VA_ARGS__)

#define issubclass( obj, cls)   dynamic_cast<cls*>(obj)
#define issubclassP( obj, cls)   dynamic_pointer_cast<cls>(obj)
#ifdef _DEBUG
template <typename T>
inline T check_ptr(T ptr) {
  assert(ptr != nullptr);
  return ptr;
}
#define CAST(obj, cls)  check_ptr(dynamic_cast<cls*>(obj))
#define CASTP(obj, cls)  check_ptr(dynamic_pointer_cast<cls>(obj))
#define CONSTCAST(obj, cls)  const_cast<cls*>(obj)
#define CONSTCASTP(obj,cls)  const_pointer_cast<cls>(obj)
#else
#define CAST(obj, cls)  static_cast<cls*>(obj)
#define CASTP(obj, cls)  static_pointer_cast<cls>(obj)
#endif

#ifndef _WIN32
inline int _vscprintf (const char * format, va_list pargs) { 
  int retval; 
  va_list argcopy; 
  va_copy(argcopy, pargs); 
  retval = vsnprintf(NULL, 0, format, argcopy); 
  va_end(argcopy); 
  return retval; 
}
#endif

inline string string_format(const char* format, ...) {
  va_list args;
  va_start(args, format);
  int len = _vscprintf(format, args) + 1; // terminating '\0'
  unique_ptr<char[]>  buffer(new char[len]);
  #ifdef _WIN32
  vsprintf_s(buffer.get(), len, format, args);
  #else
  vsprintf(buffer.get(), format, args);
  #endif
  return string(buffer.get());
}
#define cstr(s) ((s) ? (s)->tostr().c_str() : "nullptr")

#define elif else if
#define NI  assert(!"not implemented!")

#ifdef _WIN32
#ifdef _DEBUG
#include <windows.h>
#define TRACE(fmt,...) OutputDebugString(string_format(fmt,__VA_ARGS__).c_str());
#define error(msg, ...)  {string errmsg=string_format(msg, __VA_ARGS__)+"\n"; \
                          TRACE(errmsg.c_str()); fprintf(stderr, errmsg.c_str()); assert(0);}
#else
#define TRACE false && _trace
#define error(msg, ...)  {fprintf(stderr, (string_format(msg, __VA_ARGS__)+"\n").c_str()); assert(0);}
#endif
#endif

#define SET_ENGINE()  \
  static Engine* engine; \
  static void set_engine(Engine* e) {engine = e;}

#define NAMED_PARAM(cls, type, param)  cls* set_##param(type v) { param = v; return this; }

//#define ENGINE Engine* engine = this;
#define GETP(ptr) engine->board.getP(ptr)
#define GETPTHING(ptr)  engine->board.getPThing(ptr)
#define SEND_DISPLAY_MSG(type, ...)  if(!engine->is_simulation) engine->send_display_message(NEWP(type,__VA_ARGS__))
#define UPDATE_THING_STATE(what)  SEND_DISPLAY_MSG(Msg_ThingUpdate, GETPTHING(this), this->state, what)
#define UPDATE_PLAYER_STATE(what) SEND_DISPLAY_MSG(Msg_PlayerUpdate, this->state.hero, this->state, what)
#define UPDATE_THING(what, type, ...) \
  SEND_DISPLAY_MSG(type, __VA_ARGS__); \
  UPDATE_THING_STATE(what)
#define UPDATE_PLAYER(what, type, ...) \
  SEND_DISPLAY_MSG(type, __VA_ARGS__); \
  UPDATE_PLAYER_STATE(what)

inline bool startswith(string s, const char* comp) {
  return !s.compare(0, strlen(comp), comp);
}

const float INF = 9e9f; // 1.f / 0.f;

inline double pow2(double x) {
  return x*x;
}

template<typename T>
inline int len(const T& container) {
  return (int)container.size();
}

typedef vector<string> ListString;

inline string lower(const string& str) {
  string res;
  for (char c : str)
    res += char(tolower(c));
  return res;
}
inline ListString split(string text, string sep = " ") {
  ListString tokens;
  size_t pos;
  while ((pos = text.find(sep)) != string::npos) {
    if (pos > 0) tokens.push_back(text.substr(0, pos));
    text = text.substr(pos + sep.length());
  }
  tokens.push_back(text); // remainder
  return tokens;
}
inline string join(string chr, const ListString& words) {
  string res;
  for (int i = 0; i < len(words); ++i) {
    res += words[i];
    if (i + 1 < len(words)) res += chr;
  }
  return res;
}
template <typename T>
inline T convert_str_to(const string& str) {
  std::istringstream iss(str);
  T obj;
  iss >> std::ws >> obj >> std::ws;
  if (!iss.eof())
    throw "dammit!";
  return obj;
}
inline string trim(const string& str) {
  int beg = 0, end = len(str);
  while (isspace(str[beg])) beg++;
  while (end>beg && isspace(str[end-1])) end--;
  return str.substr(beg, end - beg);
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
inline int index(const vector<Type> & vec, const Type& ref) {
  for (int i = 0; i < (signed)vec.size(); ++i)
    if (vec[i] == ref) 
      return i;
  return -1;
}
template<typename Type>
inline int indexP(const vector<shared_ptr<Type> > & vec, const Type* ptr) {
  for (int i = 0; i < len(vec); ++i)
    if (vec[i].get() == ptr)
      return i;
  return -1;
}
template<typename Type>
inline shared_ptr<Type> findP(const vector<shared_ptr<Type> > & vec, const Type* ptr) {
  int i = indexP(vec, ptr);
  return i < 0 ? nullptr : vec[i];
}
template<typename Type>
inline void remove(vector<Type> & vec, const Type& ref) {
  int i = index(vec, ref);
  assert(i >= 0);
  vec.erase(vec.begin() + i);
}
template<typename Type>
inline void removeP(vector<shared_ptr<Type> > & vec, const Type* ref) {
  int i = indexP(vec, ref);
  assert(i >= 0);
  vec.erase(vec.begin() + i);
}
template<typename Type>
inline void fast_remove(vector<Type> & vec, int index) {
  swap(vec[index], vec[len(vec) - 1]);
  vec.pop_back();
}
template <typename T>
inline bool in(const T& el, const vector<T>& vec) { 
  return index(vec, el) >= 0; 
}
template <typename T, typename T2>
inline bool in(const T& el, const unordered_map<T,T2>& vec) { 
  return vec.find(el) != vec.end();
}
template <typename T, typename T2>
inline const T2& getdefault(const unordered_map<T, T2>& vec, const T& el, const T2& def) {
  auto it = vec.find(el);
  return (it == vec.end()) ? def : it->second;
}

// perform dest += src
#define append(dest,src) copy(src.begin(), src.end(), back_inserter(dest))
#define append_if(dest,src,cond) copy_if(src.begin(), src.end(), back_inserter(dest), cond)

inline int randint(int min, int max) {
  // max is included in range
  assert(min <= max);
  return (rand() % (max + 1 - min)) + min;
}


// declarations of classes and subtypes

struct Collection;

struct Engine;
typedef shared_ptr<Engine> PEngine;
struct Player;
typedef shared_ptr<Player> PPlayer;
struct Board;
struct Slot;
typedef vector<Slot> ListSlot;

struct Card;
typedef shared_ptr<Card> PCard;
typedef shared_ptr<const Card> PConstCard;
typedef vector<PConstCard> ListPConstCard;
typedef vector<PCard> ListPCard;
struct Card_Instance;
typedef shared_ptr<Card_Instance> PCardInstance;
typedef shared_ptr<const Card_Instance> PConstCardInstance;
struct Card_Thing;
typedef shared_ptr<Card_Thing> PCardThing;
typedef shared_ptr<const Card_Thing> PConstCardThing;
struct Card_Minion;
typedef shared_ptr<Card_Minion> PCardMinion;
typedef shared_ptr<const Card_Minion> PConstCardMinion;
struct Card_Hero;
typedef shared_ptr<Card_Hero> PCardHero;
typedef shared_ptr<const Card_Hero> PConstCardHero;
struct Card_HeroAbility;
typedef shared_ptr<Card_HeroAbility> PCardHeroAbility;
typedef shared_ptr<const Card_HeroAbility> PConstCardHeroAbility;
struct Card_Weapon;
typedef shared_ptr<Card_Weapon> PCardWeapon;
typedef shared_ptr<const Card_Weapon> PConstCardWeapon;
struct Card_Spell;
typedef shared_ptr<Card_Spell> PCardSpell;
typedef shared_ptr<const Card_Spell> PConstCardSpell;
struct Card_Secret;
typedef shared_ptr<Card_Secret> PCardSecret;
typedef shared_ptr<const Card_Secret> PConstCardSecret;
struct Card_TargetedSpell;
typedef shared_ptr<Card_TargetedSpell> PCardTargetedSpell;
struct Card_AreaSpell;
typedef shared_ptr<Card_AreaSpell> PCardAreaSpell;

struct Deck;
typedef shared_ptr<Deck> PDeck;

struct Action;
//typedef shared_ptr<Action> PAction;
typedef vector<const Action*> ListAction;
struct Act_Battlecry;
typedef shared_ptr<Act_Battlecry> PAct_Battlecry;
typedef vector<PAct_Battlecry> ListPAct_Battlecry;

struct Effect;
typedef shared_ptr<Effect> PEffect;
typedef vector<PEffect> ListPEffect;
typedef vector<Effect*> ListEffect;
struct Eff_Presence;
typedef shared_ptr<Eff_Presence> PEff_Presence;
typedef vector<PEff_Presence> ListPEff_Presence;
struct Eff_DeathRattle;
typedef shared_ptr<Eff_DeathRattle> PEff_DeathRattle;
typedef vector<PEff_DeathRattle> ListPEff_DeathRattle;

struct Instance;
typedef vector<Instance*> ListInstance;
typedef shared_ptr<Instance> PInstance;
typedef shared_ptr<const Instance> PConstInstance;
typedef vector<PInstance> ListPInstance;
struct Thing;
typedef shared_ptr<Thing> PThing;
typedef shared_ptr<const Thing> PConstThing;
typedef vector<PThing> ListPThing;
#define EFF Thing::StaticEffect
struct Secret;
typedef shared_ptr<Secret> PSecret;
typedef vector<PSecret> ListPSecret;
struct Weapon;
typedef shared_ptr<Weapon> PWeapon;
typedef shared_ptr<const Weapon> PConstWeapon;
struct Creature;
typedef shared_ptr<Creature> PCreature;
typedef vector<PCreature> ListPCreature;
struct Minion;
typedef shared_ptr<Minion> PMinion;
typedef shared_ptr<const Minion> PConstMinion;
typedef vector<PMinion> ListPMinion;
struct Hero;
typedef shared_ptr<Hero> PHero;
typedef shared_ptr<const Hero> PConstHero;

struct VizInstance;

struct Message;
typedef shared_ptr<Message> PMessage;
struct Msg_Status;
typedef shared_ptr<Msg_Status> PMsgStatus;

#define FUNCACTION    [] (const Action* a, Instance* from, Instance* target, const Slot& slot)
typedef bool(*FuncAction)(const Action* a, Instance* from, Instance* target, const Slot& slot);
typedef ListPConstCard(Player::*FuncMulligan)(ListPConstCard&);

#include "targets.h"
#define TGT Target

#include "events.h"
#define FUNCEFFECT    [] (const Effect* eff, Event ev, Instance* caster)
typedef bool(*FuncEffect)(const Effect* eff, Event ev, Instance* caster);

#endif
