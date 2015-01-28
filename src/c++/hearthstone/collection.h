#include "common.h"
#include "cards.h"

/* List of all possible cards
*/
struct Collection
{
  unordered_map<string, PCard> cardbook_by_name;
  vector<PCard> cardbook_by_id;

  Collection(); // add all possible cards to cardbook

  void add(PCard & card);

  static Collection only_instance;

public:
  static const Collection& Instance() {
    return only_instance;
  }
};