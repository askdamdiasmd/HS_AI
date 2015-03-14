#include "engine.h"
#include "players.h"
#include "creatures.h"
#include "Cards.h"
#include "decks.h"
#include "messages.h"


Engine* Player::engine = nullptr;

Player::Player(PHero hero, string name, Deck* deck) :
  name(name), deck(deck) {
  memset(&state, 0, sizeof(state));
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
  assert(0);
//  if (issubclass(thing, Hero)) {
//    assert(!"todo");
//  }
//  else if (issubclass(thing, Weapon)) {
//    if (state.weapon) // kill it
//      state.weapon.ask_for_death();
//    state.weapon = dynamic_pointer_cast<Weapon>(thing);
//    engine->send_message(Msg_WeaponPopup(state.weapon));
//  }
//  else if (issubclass(thing, Secret)) {
//    state.secrets.push_back(dynamic_pointer_cast<Secret>(thing));
//    engine->send_message(Msg_SecretPopup(state.secrets.back()));
//  }
//  else if (issubclass(thing, Minion) && state.minions.size() < 7) {
//    auto n = state.minions_pos.size();
//    if (pos.fpos >= 1000)  // helper
//      pos.fpos = (state.minions_pos[n - 2] + state.minions_pos[n - 1]) / 2;
//    int i = pos.searchsorted(state.minions_pos);
//    if (state.minions_pos[i - 1] == pos.fpos) // already exist, so create new number
//      pos.fpos = (pos.fpos + mp[i]) / 2;
//    state.minions_pos.insert(i, pos.fpos);
//    state.minions.insert(i - 1, dynamic_pointer_cast<Minion>(thing));
//    engine->send_message(Msg_MinionPopup(state.minions[i - 1], i - 1));
//  }
//  else
//    return false;
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

void Player::add_mana_crystal(int nb, bool useit) {
  state.mana = min(10, state.mana + nb);
  state.max_mana = min(10, state.max_mana + nb);
  if (useit) use_mana(nb);
}

void Player::use_mana(int nb) {
  state.mana -= nb;
  assert(state.mana >= 0);
}

void Player::gain_mana(int nb) {
  state.mana = min(10, state.mana + nb);
}

void Player::start_turn() {
  // activated by Msg_StartTurn(player)
  state.hero->start_turn();
  for (auto m : state.minions)
    m->start_turn();
  if (state.weapon)
    state.weapon->start_turn();
  state.mana = state.max_mana;
  add_mana_crystal(1);
  assert(0);
  //engine->send_message(Msg_DrawCard(this));
}

void Player::end_turn() {
  state.hero->end_turn();
  for (auto m : state.minions)
    m->end_turn();
}

ListAction Player::list_actions() {
  assert(0); return{};
//  ListPAction res = { make_shared<Act_EndTurn>(this) };
//  // first, hero power
//  res.push_back(state.hero->list_action());
//  // then, all card's actions
//  for (card : state.cards)
//    res.push_back(card->list_action());
//  // then, weapon / hero's attack (if any)
//  if (state.weapon)
//    res.push_back(state.weapon->list_action());
//  // then, all minions actions
//  for (m : state.minions)
//    res.push_back(m.list_action());
//  return res;
}

void Player::draw_card() {
  assert(0);
//  PCard card = deck->draw_one_card();
//  if (card) {
//    if (state.cards.size() < 10) {
//      state.cards.append(card);
//      SEND_MSG(Msg_CardDrawn, this, card);
//    }
//    else
//      SEND_MSG(Msg_DrawBurnCard, this, card);
//  }
}

void Player::give_card(PCard card, Instance* origin) {
  assert(0);
//  state.cards.append(card);
//  SEND_MSG(Msg_CardDrawn, this, card, origin);
}

void Player::draw_init_cards(int nb, bool coin) {
  state.cards = deck->draw_init_cards(nb, &Player::mulligan);
  if (coin) {
    PCard coin = NEWP(Card_Coin);
    SEND_MSG(Msg_CardDrawn, PInstance(), coin, this);
    state.cards.push_back(coin);
  }
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

const Action* RandomPlayer::choose_actions(ListAction actions) const {
  // select one action in the list
  int r = randint(0, len(actions) - 1);
  const Action* action = actions[r];
  // select one target for this action
  NI;
  //for (ch : action.choices) {
  //  if ch :
  //  action.select(i, random.randint(0, len(ch) - 1));
  //}
  return action;
}
