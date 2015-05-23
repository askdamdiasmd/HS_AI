#include <fstream>
#include <streambuf>
#include "common.h"
#include "validation.h"
#include "Cards.h"
#include "creatures.h"
#include "players.h"
#include "collection.h"
#include "decks.h"
#include "actions.h"

#define assert_error(cond,msg,...)  if(!(cond)) error((string("error at line %d: ")+msg).c_str(),num,##__VA_ARGS__)


typedef unordered_map<string, string> KwArgs;

inline KwArgs read_kwargs(ListString& words) {
  KwArgs res; // keyword args
  for (int i = 0; i < len(words); i++) {
    if (words[i].find('=') != string::npos) {
      ListString keyval = split(pop_at(words, i--), "=");
      assert(len(keyval) == 2);
      res[keyval[0]] = keyval[1];
    }
  }
  return res;
};

inline int read_int(const int num, const KwArgs& args, const string& key, int defaut) {
  auto it = args.find(key);
  if (it == args.end()) return defaut;
  try{ return convert_str_to<int>(it->second); }
  catch (const char* msg) {
    assert_error(false, "could not convert arg %s='%s' to int (reason: %s)",
      key.c_str(), it->second.c_str(), msg);
  }
  return -1;
};
inline float read_float(const int num, const KwArgs& args, const string& key, float defaut) {
  auto it = args.find(key);
  if (it == args.end()) return defaut;
  try{ return convert_str_to<float>(it->second); }
  catch (const char* msg) {
    assert_error(false, "could not convert arg %s='%s' to float (reason: %s)",
      key.c_str(), it->second.c_str(), msg);
  }
  return -1.f;
};

bool ScriptedEngine::validate_script(const string& script, bool viz) {
  vector<string> lines = split(lower(script), "\n");
  lines.push_back("finished:"); // last state
  enum Step {
    None,
    Setup,
    Action,
    Result,
  };
  enum SubStep {
    SubNone = 0,
    SubHeroes = 1,
    SubDecks = 2,
    SubBoard = 4,
  }; 
  Step step = None;
  SubStep substep = SubNone;
  PPlayer player1, player2;
  PHero hero1, hero2;
  Deck deck1, deck2;

  int num;  // current line number
  auto read_player = [this, &num](ListString& line) {
    assert_error(board.player1 && board.player2, "players are not yet defined!");
    string p = pop_at(line, 0);
    if (p == "p1")
      return CAST(board.players[0], ScriptedPlayer);
    elif(p == "p2")
      return CAST(board.players[1], ScriptedPlayer);
    else
      assert_error(false, "bad player name '%s'", p.c_str());
    return (ScriptedPlayer*)nullptr;
  };
  auto get_card_by_name = [this, &num](const string& name) {
    assert_error(cardbook.exist(name), "card '%s' does not exist", name.c_str());
    return cardbook.get_by_name(name);
  };
  auto read_card = [this, &num, get_card_by_name](ListString& words) {
    assert_error(len(words), "missing card type");
    string type = pop_at(words, 0);
    assert_error(len(words), "missing card type or name");
    string name = join(" ", words);
    PConstCard card;
    if (type == "minion") {
      card = get_card_by_name(name);
      assert_error(issubclassP(card, const Card_Minion), "card '%s' is not a minion", name.c_str());
    }
    elif (type == "weapon") {
      card = get_card_by_name(name);
      assert_error(issubclassP(card, const Card_Weapon), "card '%s' is not a weapon", name.c_str());
    }
    elif(type == "spell") {
      card = get_card_by_name(name);
      assert_error(issubclassP(card, const Card_Spell), "card '%s' is not a spell", name.c_str());
    }
    elif(type == "hero") {
      card = get_card_by_name(name);
      assert_error(issubclassP(card, const Card_Hero), "card '%s' is not a hero", name.c_str());
    }
    else
      assert_error(false, "bad card type '%s': should be Hero, Minion, Spell or Weapon", type.c_str());
    return card;
  };
  auto add_minion = [this](Player* pl, PConstCard card, float pos) {
    PConstCardMinion card_minion = CASTP(card, const Card_Minion);
    PMinion minion = card_minion->instanciate(pl);
    board.add_thing(pl->state.hero.get(), minion, Slot(pl, -1, pos));
    return minion;
  };
  int checked_results = 0;
  for (num = 0; num < len(lines); num++) {
    string& line = trim(lines[num]);
    if (line.empty()) continue;

    if(line.find(':') != string::npos) {
      if (line == "setup:") {
        assert_error(step == None, "unexpected label '%s' (should come first)", line.c_str());
        step = Setup;
        substep = SubNone;
      }
      elif(line == "action:") {
        assert_error(step == Setup, "unexpected label '%s' (should come after setup:)", line.c_str());
        step = Action;
        substep = SubNone;
      }
      elif(line == "result:") {
        assert_error(step == Action, "unexpected label '%s' (should come after action:)", line.c_str());
        // play game now
        while (board.play_turn());
        step = Result;
        checked_results = substep = SubNone;
      }
      elif(line == "finished:") {
        if (checked_results | SubBoard) { // we have verified the board
          assert_error(step == Result, "unexpected end of script (before encountering 'result:')");
          assert_error(player1->state.minions.empty(), "remaining non-expected minions for Player1");
          assert_error(player2->state.minions.empty(), "remaining non-expected minions for Player2");
        }
        if (checked_results | SubHeroes) { // we have verified the heroes
          assert_error(!player1->state.weapon, "remaining non-expected weapon for Player1");
          assert_error(!player2->state.weapon, "remaining non-expected weapon for Player2");
          assert_error(player1->state.secrets.empty(), "remaining non-expected secrets for Player1");
          assert_error(player2->state.secrets.empty(), "remaining non-expected secrets for Player2");
        }
        reset();
        return true;  // everything is alright
      }
      elif(line == "decks:" && (step == Setup || step == Result)) {
        substep = SubDecks;
        if (step == Result) checked_results |= substep;
      }
      elif(line == "heroes:" && (step == Setup || step == Result)) {
        substep = SubHeroes;
        if (step == Result) checked_results |= substep;
      }
      elif(line == "board:" && (step == Setup || step == Result)) {
        substep = SubBoard;
        if (step == Result) checked_results |= substep;
      }
      else
        assert_error(false, "unexpected ':' character in '%s'", line.c_str());
    }
    else {
      vector<string> words = split(line);
      KwArgs args = read_kwargs(words);

      if (step == Setup && substep == SubHeroes) {
        string pl = pop_at(words, 0);
        string type = pop_at(words, 0);
        assert_error(type == "hero", "bad type, expected 'hero' but got '%s'", type.c_str());
        string name = join(" ", words);
        if (pl == "p1") {
          assert_error(!player1, "player1 was already defined");
          hero1 = NEWP(Hero, cardbook.get_hero(name));
          player1 = NEWP(ScriptedPlayer, hero1, getdefault<string>(args, "name", words[0]), &deck1);
        }
        elif(pl == "p2") {
          assert_error(!player2, "player2 was already defined");
          hero2 = NEWP(Hero, cardbook.get_hero(name));
          player2 = NEWP(ScriptedPlayer, hero2, getdefault<string>(args, "name", words[0]), &deck2);
        }
        else
          assert_error(false, "bad player tag '%s' (should be P1 or P2)", pl.c_str());

        if (player1 && player2) { // we are ready for some shit
          Engine::init_players(player1.get(), player2.get());
          if (viz)  board.viz = NEWP(VizBoard, &board);
        }
      }
      elif(step == Setup && substep == SubDecks) {
        Player* pl = read_player(words);
        string card_name = join(" ", words);
        int nb = read_int(num, args, "nb", 1);
        for (int i = 0; i < nb; i++)
          pl->deck->cards.push_back(cardbook.get_by_name(card_name)->copy());
      }
      elif(step == Setup && substep == SubBoard) {
        Player* pl = read_player(words);
        string type = pop_at(words, 0); 
        if (type == "minion") {
          string name = join(" ", words);
          PConstCard card = cardbook.get_by_name(name);
          float pos = read_float(num, args, "fpos", 500.0);
          PMinion minion = add_minion(pl, card, pos);
          for (auto& keyval : args) {
            if (keyval.first == "hp")
              minion->change_hp(convert_str_to<int>(keyval.second) - minion->state.hp);
          }
        }
        else
          assert_error("unexpected board type '%s' (should be minion or weapon)", type.c_str());
      }
      elif(step == Action) {
        ScriptedPlayer* pl = read_player(words);
        pl->add_action(num,line);
      }
      elif(step == Result && substep == SubBoard) {
        ScriptedPlayer* pl = read_player(words);
        PInstance thing = pl->read_thing(num, words);
        PMinion minion;
        if (minion=issubclassP(thing, Minion)) {
          for (auto& keyval : args) {
            if (keyval.first == "hp")
              assert_error(minion->state.hp == convert_str_to<int>(keyval.second),
              "unexpected hp for minion '%s' (%d instead of %s)",
              cstr(minion), minion->state.hp, keyval.second);
            if (keyval.first == "atq")
              assert_error(minion->state.atq == convert_str_to<int>(keyval.second),
              "unexpected atq for minion '%s' (%d instead of %s)",
              cstr(minion), minion->state.atq, keyval.second);
          }
          pl->remove_thing(minion);  // this one is validated
        }
      }
      else
        assert_error(false, "chouldn't be here");
    }
  }
  reset();
  return false;
}

bool ScriptedEngine::validate_script_file(const string& file_name, bool viz) {
  ifstream file(file_name);
  string content;
  content.assign(istreambuf_iterator<char>(file),
                 istreambuf_iterator<char>());
  return validate_script(content, viz);
}

// ScriptedPlayer ---------------

ListPConstCard ScriptedPlayer::mulligan(ListPConstCard & cards) {
  error("shouldn't be here"); //  should never reach this part, as we never mulligan
  return cards;
}

inline bool match_name(string name, const string& dim) {
  name = lower(name);
  if (name == dim)  return true;
  if (startswith(name, dim.c_str())) return true;
  string accronym;
  for (string sub : split(name))  accronym += sub[0];
  if (accronym == dim)  return true;
  return false;
};

PInstance ScriptedPlayer::read_thing(int num, ListString& words) {
  assert_error(len(words), "missing thing type");
  const string& type = pop_at(words, 0);
  string name = join(" ", words);

  if (type == "hero") {
    return state.hero;
  }
  elif(type == "weapon") {
    return state.weapon;
  }
  else {  // minion type can be avoided
    if (type != "minion")
      name = type + (name.empty() ? "" : " " + name);

    for (auto& m : state.minions)
      if (match_name(m->card->name, name))
        return m;
  }
  assert_error(false, "%s '%s' was not found.", type.c_str(), name.c_str());
  return PInstance();
}

PCard ScriptedPlayer::read_card(int num, ListString& words) {
  const string& type = pop_at(words, 0);
  string name = join(" ", words);

  PCard card;
  for (auto& c : state.cards)
    if (match_name(c->name, name)) {
      card = c;
      break;
    }
  assert_error(card, "%s card '%s' was not found in hand.", type.c_str(), name.c_str());
  if (type == "minion") {
    assert_error(issubclassP(card, Card_Minion),
      "card '%s' retrieved by '%s' in not a %s card", cstr(card), name.c_str(), type.c_str());
  }
  elif(type == "weapon") {
    assert_error(issubclassP(card, Card_Weapon),
      "card '%s' retrieved by '%s' in not a %s card", cstr(card), name.c_str(), type.c_str());
  }
  else
    assert_error(false, "bad thing type '%s': should be Hero, Minion, Spell or Weapon", type.c_str());
  return card;
}

PInstance ScriptedEngine::random(ListPInstance& instances) {
  NI;
  return PInstance();
}

const Action* ScriptedPlayer::choose_actions(ListAction actions, Instance*& choice, Slot& slot) {
  if (script.empty())  return nullptr; // no cmd anymore
  
  line_t script_line = pop_at(script, 0);
  const int num = script_line.first;

  ListString words = split(script_line.second);
  string player = pop_at(words, 0);
  assert_error(player == "p1" || player == "p2", "bad player '%s'", player.c_str());
  string type = pop_at(words, 0);
  assert_error(len(words), "missing action description");
  KwArgs args = read_kwargs(words);
  assert_error(len(words), "missing action type");

  PCard card;
  PInstance atq, dfd;
  if (type == "attack") {
    ListString names = split(join(" ", words), "->");
    assert_error(len(names) == 2, "bad 'attacker -> defender' syntax");
    atq = read_thing(num, split(names[0]));
    ScriptedPlayer* adv = CAST(engine->board.get_other_player(this), ScriptedPlayer);
    dfd = adv->read_thing(num, split(names[1]));
  } 
  elif(type == "play_card") {
    NI;
  }

  for (const Action* a : actions) {
    const Act_Attack* act_atq = nullptr;
    const Act_PlayCard* act_play = nullptr;
    
    if (type == "attack" && (act_atq=issubclass(a, const Act_Attack))) {
      if (act_atq->creature == atq.get()) { // this one is the one
        ListPInstance targets = a->target.resolve(this);
        assert_error(in(dfd, targets), "'%s' can't attack this target '%s'", cstr(atq), cstr(dfd));
        choice = dfd.get();
        return a;
      }
    } 
    elif(type == "play_card" && (act_play = issubclass(a, const Act_PlayCard))) {
      if (a->need_slot) {
        slot.pos = read_int(num, args, "pos", -1);
        slot.fpos = read_float(num, args, "fpos", -1);
      }
      NI;
    }
  }

  assert_error(false, "action type '%s' is not available currently", type.c_str());
  return nullptr;
}
