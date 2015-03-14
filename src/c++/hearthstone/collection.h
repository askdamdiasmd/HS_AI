#ifndef __COLLECTION_H__
#define __COLLECTION_H__
#include "common.h"

/* List of all possible cards
*/
struct Collection
{
private:
  unordered_map<string, PConstCard> by_name;
  ListConstCard by_id;
  ListConstCard  collectibles;

  Collection(); // add all possible cards to cardbook

  PCard add(PCard card);

  static Collection only_instance;

public:
  static const Collection& Instance() {
    return only_instance;
  }

  PConstCard get_by_name(const string& name) const {
    assert(in(name, by_name));
    return by_name.at(name);
  }
  PConstCard get_by_id(int id) const {
    assert(0 <= id && id < len(by_id));
    return by_id[id];
  }
  const ListConstCard& get_collectibles() const {
    return collectibles;
  }
};

#endif