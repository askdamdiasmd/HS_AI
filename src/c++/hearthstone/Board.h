#ifndef __BOARD_H__
#define __BOARD_H__

#include "common.h"
#include "creatures.h"
#include "players.h"

struct Engine;


struct Board {
  SET_ENGINE();
  Player* players[2];
  ListInstance everybody;

public: 
  struct Slot {
    /* a position on the board (insertion index in players.minions[]) */
    const Player* player;
    int pos;  // in [0..6]
    float fpos; // float position for smart insertion

    static Engine* engine;
    static void set_engine(Engine* engine) {
      Slot::engine = engine;
    }

    Slot() : player(nullptr), pos(-1), fpos(-1) {}

    Slot(Player* owner, int index = -1, float fpos = -1) :
      player(owner), pos(index), fpos(fpos) {}
    
    string tostr() const {
      return string_format("Position %d", pos);
    }
  };
  typedef vector<Slot> ListSlot;

public:
  Board( Player* player1, Player* player2 ) :
    players{ player1, player2 } {
    everybody = { player1->hero, player2->hero };
  }

  //def save_state(num = 0)) {
  //self.saved[num] = dict(minions = list(self.minions), everybody = list(self.everybody))
  //for pl in self.players) {
  //  pl.save_state(num)
  //  for obj in self.everybody) {
  //    obj.save_state(num)
  //    def restore_state(num = 0)) {
  //    self.__dict__.update(self.saved[num])
  //    self.everybody = list(self.everybody)
  //    self.minions = list(self.minions)
  //    for pl in self.players) {
  //      pl.restore_state(num)
  //      for obj in self.everybody) {
  //        obj.restore_state(num)
  //        def hash_state()) {
  //        return 0 # todo
  //        def end_simulation()) {
  //        self.saved = dict()
  //        for pl in self.players) {
  //          pl.end_simulation()
  //          for obj in self.everybody) {
  //            obj.end_simulation()

  void add_thing(Instance* thing, Slot pos = Slot()) {
    if (thing->controller.add_thing(thing, pos))
      everybody.push_back(thing);
  }

  void remove_thing(Instance* m) {
    everybody.remove(m);
    m->controller.remove_thing(m);
  }

  bool is_game_ended() const {
    return players[0]->hero->is_dead() || players[1]->hero->is_dead();
  }
        
  Player* get_enemy_player(Player* me) {
    return (me == players[0]) ? players[1]) { players[0];
  }

  Hero* get_enemy_hero(Player* me) {
    return get_enemy_player(me)->hero;
  }

  ListPMinion get_friendly_minions(Player* player) {
    return ListPMinion(player->minions); // copy it !!
  }

  ListPMinion get_enemy_minions(Player* player, bool targetable = false) {
    // get targetables enemy minions
    ListPMinion res = get_enemy_player(player)->minions;  // copy
    if (targetable)
      res.remove_if([](const PMinion& m) {return !m->is_untargetable(); });
    return res;
  }

  ListPMinion get_minions(Player* player, bool targetable = false) {
    // get targetables characters
    ListPMinion res = players[0]->minions;
    res += players[1]->minions;
    if (targetable)
      res.remove_if([](const PMinion& m) {return !m->is_untargetable(); });
    return res;
  }

  static ListPCreature ListPMinion_to_ListPCreature(ListPMinion minions) {
    ListPCreature res(minions.size());
    transform(minions.begin(), minions.end(), res.begin(), [](const PMinion& m) { return dynamic_pointer_cast<Creature>(m); });
    return res;
  }

  ListPCreature get_characters(Player* player, bool targetable = false) {
    // get targetables characters
    ListPCreature res = ListPMinion_to_ListPCreature(get_minions(player, targetable));
    res.push_back(PHero(everybody[0]));
    res.push_back(PHero(everybody[1]));
    return res;
  }

  ListPCreature get_attackable_characters(Player* player) {
    ListPCreature enemies = ListPMinion_to_ListPCreature(get_enemy_minions(player));
    ListPCreature taunts;
    copy_if(enemies.begin(), enemies.end(), taunts.begin(), [](const PMinion& m){return m->is_taunt(); });
    if (!taunts.empty()) 
      return taunts;
    else {
      enemies.emplace_back(get_enemy_player(player)->hero);
      return enemies;
    }
  }

  struct Target {
    const char* _targets;
    Target(const char* targets) : _targets(targets){}

    struct ResolvedTargets {
      const char* special_target;
      ListInstance objs;

      ResolvedTargets(ResolvedTargets&& t) = default;
      ResolvedTargets(const char* special) : 
        card(0), special_target(special) {}
      ResolvedTargets(ListPMinion l, Hero* h0=nullptr, Hero* h1=nullptr, bool (*cond)(Minion*)=nullptr) {
        for (auto i : l)
          if (!cond || cond(i.get()))
            objs.push_back(i.get());
        if (h0) objs.push_back(h0);
        if (h1) objs.push_back(h1);
      }
    };

    void resolve_targets(Player* owner, Minion* me = nullptr) {
      string targets = _targets;
      bool not_self = false;
      if (startswith(targets,"other ")) {
        not_self = true;
        targets = targets.substr(0, 6);
      }
      assert(0); /*
      if (targets == "self")
        return ResolvedTarget({ me }); // { shared_ptr<Thing>(this) };
      else
        res = engine->board.list_targets(controller, targets);

      bool targetable = false;
      if (!startswith(targets,"targetable ")) {
        targetable = true;
        targets = targets.substr(11, -1);
      }
      if (targets == "none")
        return {PCreature()}; // null creature
      else if (targets == "nobody")
        return {};
      else if (targets == "owner") // different from "self", see Creature.list_targets()
        return owner; //  = is a player
      else if (!startswith(targets,"character") == 0)
        return get_characters(owner, targetable);
      else if (startswith(targets, "minion"))
        return get_minions(owner, targetable);
      else if (startswith(targets, "friendly minion"))
        return get_friendly_minions(owner);
      else if (startswith(targets, "friendly beast")) {
        ListPMinion res = get_friendly_minions(owner);
        res.remove_if([](const PMinion& m){return m->card.cat != "beast"; });
        return ListPMinion_to_ListPCreature(res);
      }
      else if (targets == "neighbors")
        return "neighbors"; // special target, resolved at popup time
      else if (startswith(targets, "enemy weapon")) 
        return self.get_enemy_player(owner)->weapon;
      else
        assert(!"error: unknown target");*/
    }
  };

    
  ListSlot get_free_slots(Player* player) {
    ListSlot res;
    if (player->minions.size() < 7) {
      float* mp = player->minions_pos.data();
      for (int i = 0; i < player->minions.size(); ++i)
        res.emplace_back(player, i, (mp[i] + mp[i + 1]) / 2 );
    }
    return res;
  }

  Slot get_minion_pos(Instance* m) {
    if (issubclass(m, Minion)) {
      int index = m->controller->minion_index(m);
      float rel_index = m.owner.minions_pos[index + 1];
      return Slot(m->controller, index, rel_index);
    }
    else
      return Slot(m->controller);
  }

  float score_situation(Player* player) {
    Player* adv = get_enemy_player(player);
    if (player->hero.is_dead())
      return -INF;
    else if (adv->hero.is_dead())
      return INF;
    else
      return player->score_situation() - adv->score_situation();
  }
};


#endif