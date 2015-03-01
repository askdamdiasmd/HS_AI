#ifndef __COLLECTION_H__
#define __COLLECTION_H__
#include "common.h"

/* List of all possible cards
*/
struct Collection
{
  unordered_map<string, PCard> by_name;
  vector<PCard> by_id;
  vector<PCard> collectibles;

  Collection(); // add all possible cards to cardbook

  PCard add(PCard card);

  static Collection only_instance;

public:
  static const Collection& Instance() {
    return only_instance;
  }
};

#endif