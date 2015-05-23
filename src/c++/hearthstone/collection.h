#ifndef __COLLECTION_H__
#define __COLLECTION_H__
#include "common.h"

/* List of all possible cards
*/
struct Collection
{
private:
  unordered_map<string, PConstCard> by_name;
  ListPConstCard by_id;
  ListPConstCard  collectibles;

  Collection(); // add all possible cards to cardbook

  template <typename TCard>
  TCard add(TCard card);

  static Collection only_instance;

public:
  static const Collection& CardBook() {
    return only_instance;
  }

  PConstCard get_by_name(const string& name) const {
    if (!in(name, by_name))
      error("Error: card '%s' is not in the collection", name.c_str());
    return by_name.at(name);
  }
  PConstCard get_by_id(int id) const {
    assert(0 <= id && id < len(by_id));
    return by_id[id];
  }

  bool exist(const string& name) const {
    return by_name.find(name) != by_name.end();
  }
  PConstCardHero get_hero(const string& name) const;
  PConstCardMinion get_minion(const string& name) const;
  PConstCardWeapon get_weapon(const string& name) const;
  PConstCardSecret get_secret(const string& name) const;

  const ListPConstCard& get_collectibles() const {
    return collectibles;
  }
};

#endif