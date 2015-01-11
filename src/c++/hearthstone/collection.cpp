#include "collection.h"

Collection Collection::only_instance = Collection();

Collection::Collection() {
  add(NEWP(Card,"Windchill Yeti", 4, 4, 5));
}

static string lower(const string& str) {
  string res;
  for (char c : str)
    res += char(tolower(c));
  return res;
}

void Collection::add(PCard & card) {
  const string & name = card->name;

  //name = "%s-%d-%d-%d" % (name, card.cost, card.atq, card.hp)

  const int card_count = cardbook_by_id.size();
  card.id = card_count;
  cardbook_by_id.push_back(card);

  assert(!in(name, cardbook_by_name));
  cardbook_by_name[name] = cardbook_by_name[lower(name)] = card;

  const string & name_fr = card->name_fr;
  if (name_fr && name_fr != name) {
    assert(!in(name_fr, cardbook_by_name));
    cardbook_by_name[name_fr] = cardbook_by_name[lower(name_fr)] = card;
  }
}