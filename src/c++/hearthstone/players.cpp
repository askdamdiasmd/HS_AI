#include "engine.h"
#include "players.h"
#include "creatures.h"
#include "Cards.h"
#include "decks.h"
#include "messages.h"


Engine* Player::engine = nullptr;


Player::Player(PHero hero, string name, Deck* deck) :
  name(name), deck(deck), act_end_turn() {
  state.mana = state.max_mana = 0;
  state.hero = hero;
  state.minions_pos = { 0.f, 1000.f };

  state.hero->set_controller(this);
  deck->set_owner(this);
}

/*def save_state(num = 0) :
state.deck.save_state(num)
state.saved[num] = dict(cards = list(state.cards), minions = list(state.minions), minions_pos = list(state.minions_pos),
mana = state.mana, max_mana = state.max_mana, weapon = state.weapon, secrets = list(state.secrets))
def restore_state(num = 0) :
state.deck.restore_state(num)
state.__dict__.update(state.saved[num])
state.cards = list(state.cards)
state.minions = list(state.minions)
state.minions_pos = list(state.minions_pos)
state.secrets = list(state.secrets)
def end_simulation() :
state.saved = dict()*/

string Player::tostr() const {
  return state.hero->tostr();
}

bool Player::add_thing(PInstance thing, Slot pos) {
  PMinion m;
  if (issubclassP(thing, Hero)) {
    NI;
  }
  else if (issubclassP(thing, Weapon)) {
    NI;
    //if (state.weapon) // kill it
    //  state.weapon.ask_for_death();
    //state.weapon = dynamic_pointer_cast<Weapon>(thing);
    //engine->send_message(Msg_WeaponPopup(state.weapon));
  }
  else if (issubclassP(thing, Secret)) {
    NI;
    //state.secrets.push_back(dynamic_pointer_cast<Secret>(thing));
    //engine->send_message(Msg_SecretPopup(state.secrets.back()));
  }
  else if (state.minions.size() < 7 && (m = issubclassP(thing, Minion))) {
    const int n = state.minions_pos.size();
    if (pos.fpos >= 1000)  // helper
      pos.fpos = (state.minions_pos[n - 2] + state.minions_pos[n - 1]) / 2;
    const int i = pos.insert_after_pos(state.minions_pos);
    if (state.minions_pos[i - 1] == pos.fpos) // already exist, so create new number
      pos.fpos = (pos.fpos + state.minions_pos[i]) / 2;
    pos.pos = i - 1;
    state.minions_pos.insert(state.minions_pos.begin()+i, pos.fpos);
    state.minions.insert(state.minions.begin() + (i - 1), m);
    UPDATE_PLAYER("minions", Msg_AddMinion, m.get(), pos);
    engine->signal(m.get(), Event::AddMinion);
  }
  else
    return false;
  return true;
}

void Player::remove_thing(PInstance thing) {
  PSecret secret;
  PMinion minion;
  if (thing == state.weapon)
    state.weapon = nullptr;
  else if (minion=issubclassP(thing, Minion)) {
    int i = index(state.minions, minion);
    state.minions_pos.erase(state.minions_pos.begin() + i + 1);
    state.minions.erase(state.minions.begin() + i);
  }
  else if (secret = issubclassP(thing, Secret))
    remove(state.secrets, secret);
}

void Player::draw_init_cards(int nb, bool coin) {
  ListPCard kept = deck->draw_init_cards(nb, &Player::mulligan);
  if (coin) {
    PCard coin = NEWP(Card_Coin);
    UPDATE_PLAYER("cards", Msg_NewCard, nullptr, coin);
    kept.push_back(coin);
  }
  for (auto& card : kept) {
    state.cards.push_back(card);
    UPDATE_PLAYER("cards", Msg_ReceiveCard, nullptr, card, this);
  }
}

void Player::draw_card() {
  PCard card = deck->draw_one_card();
  if (card) give_card(card, nullptr);
}

void Player::give_card(PCard card, Instance* origin) {
  if (len(state.cards) < 10) {
    state.cards.push_back(card);
    UPDATE_PLAYER("cards",  Msg_ReceiveCard, origin, card, this);
  }
  else 
    UPDATE_PLAYER("cards", Msg_BurnCard, origin, card, this);
}

void Player::throw_card(PCard card) {
  remove(state.cards, card);
  UPDATE_PLAYER("cards", Msg_ThrowCard, state.hero.get(), card);
}

void Player::add_mana_crystal(int nb, bool useit) {
  state.mana = min(10, state.mana + nb);
  state.max_mana = min(10, state.max_mana + nb);
  if (useit) use_mana(nb);
  UPDATE_PLAYER_STATE("mana max_mana");
}

void Player::use_mana(int nb) {
  state.mana -= nb;
  assert(state.mana >= 0);
  UPDATE_PLAYER_STATE("mana");
}

void Player::gain_mana(int nb) {
  state.mana = min(10, state.mana + nb);
  UPDATE_PLAYER_STATE("mana");
}

void Player::start_turn() {
  // then add mana crystal and draw a card
  add_mana_crystal(1);
  state.mana = state.max_mana;
  engine->draw_card(nullptr, this);
  // inform interface = last thing !
  UPDATE_PLAYER("all",Msg_StartTurn, state.hero.get());
}

void Player::end_turn() {
  // inform interface (kinda useless)
  UPDATE_PLAYER("all", Msg_EndTurn, state.hero.get());
}

ListAction Player::list_actions() {
  ListAction res = { &act_end_turn };
  // first, hero power
  state.hero->list_actions(res);
  // then, all card's actions
  for (auto& card : state.cards)
    card->list_actions(res);
  // then, weapon / hero's attack (if any)
  if (state.weapon)
    state.weapon->list_actions(res);
  // then, all minions actions
  for (auto& m : state.minions)
    m->list_actions(res);
  return res;
}

float Player::score_situation() {
  //float res = state.hero->score_situation();
  //for (card : state.cards)
  //  res += card->score;
  //if (state.weapon)
  //  res += state.weapon->score_situation();
  //for (m : state.minions)
  //  res += m->score_situation();
  //for (m : state.secrets)
  //  res += m->score_situation();
  //return res;
  assert(0); return 0;
}

const Action* RandomPlayer::choose_actions(ListAction actions, Instance*& choice, Slot& slot) const {
  // select one action in the list
  int r = randint(0, len(actions) - 1);
  const Action* action = actions[r];

  // select one target for this action
  if (action->need_slot) {
    ListSlot slots = engine->board.get_free_slots(const_cast<RandomPlayer*>(this));
    slot = slots[randint(0, len(slots))];
  }

  if (action->need_target()) {
    ListPInstance choices = action->target.resolve(const_cast<RandomPlayer*>(this));
    choice = choices[randint(0, len(choices)-1)].get();
  }
  return action;
}
