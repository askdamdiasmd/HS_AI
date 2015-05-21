#include "common.h"
#include "validation.h"
#include "Cards.h"
#include "creatures.h"
#include "players.h"
#include "collection.h"
#include "decks.h"
#include "actions.h"


PInstance ScriptedEngine::random(ListPInstance& instances) {
  NI;
  return PInstance();
}

//void ScriptedEngine::test_add_minion(PEngine engine, Player* pl, PConstCard card, float pos = 500.f) {
//  PConstCardMinion card_minion = CASTP(card->copy(), const Card_Minion);
//  engine->add_thing(pl->state.hero.get(), card_minion->instanciate(pl), Slot(pl, -1, pos));
//}

static string s =
"setup:\n"
" Heroes:\n"
"  P1 Jaina Proudmore name=Jerome\n"
"  P2 Anduin Wrynn name=IA\n"
" Decks:\n"
"  P1 The Coin nb=30\n"
"  P2 The Coin nb=30\n"
" Board:\n"
"  P1 Minion Haunted Creeper\n"
"  P2 Minion Unstable Ghoul hp=1\n"
"action:\n"
"  Attack HC UG\n"
"result:\n"
" Board:\n";

void ScriptedEngine::validate_script(const string& script, bool viz) {
  auto read_player = [this](ListString& line) {
    string p = pop_at(line, 0);
    if (p == "P1")
      return board.players[0];
    elif(p == "p2")
      return board.players[1];
    else
      error("error: bad player name '%s'",p.c_str())
    return (Player*)nullptr;
  };
  typedef unordered_map<string, string> KwArgs;
  auto read_kwargs = [](ListString& words) {
    KwArgs res; // keyword args
    for (int i = 0; i < len(words); i++) {
      if (words[i].find('=') != string::npos) {
        ListString keyval = split(pop_at(words, i--),"=");
        assert(len(keyval) == 2);
        res[keyval[0]] = keyval[1];
      }
    }
    return res;
  };

  auto add_minion = [this](Player* pl, PConstCard card, float pos) {
    PConstCardMinion card_minion = CASTP(card->copy(), const Card_Minion);
    add_thing(pl->state.hero.get(), card_minion->instanciate(pl), Slot(pl, -1, pos));
  };

  vector<string> lines = split(lower(script), "\n");
  enum Step {
    None,
    Setup,
    Action,
    Result,
  };
  enum SubStep {
    SubNone,
    SubHeroes,
    SubDecks,
    SubBoard,
  }; 
  Step step = None;
  SubStep substep = SubNone;
  PPlayer player1, player2;
  PHero hero1, hero2;
  Deck deck1, deck2;

  for (string& line : lines) {
    if (line == "setup:") {
      assert(step == None);
      step = Setup;
      substep = SubNone;
    }
    elif (line == "decks:" && step == Setup) {
      substep = SubDecks;
    }
    elif (line == "board:" && step == Setup) {
      substep = SubBoard;
    }
    elif (line == "action:") {
      assert(step == Setup);
      step = Action;
      substep = SubNone;
    }
    elif (line == "result:") {
      assert(step == Action);
      step = Result;
      substep = SubNone;
    }
    elif(line == "board:" && step == Result) {
      substep = SubBoard;
    }
    else {
      vector<string> words = split(line);
      KwArgs args = read_kwargs(words);
      if (step == Setup && substep == SubHeroes) {
        string pl = pop_at(words, 0);
        string name = join(" ", words);
        if (pl == "p1") {
          assert(!player1);
          hero1 = NEWP(Hero, cardbook.get_hero(name));
          player1 = NEWP(ScriptedPlayer, hero1, getdefault<string>(args, "name", words[0]), &deck1);
        }
        elif(pl == "p2") {
          assert(!player2);
          hero2 = NEWP(Hero, cardbook.get_hero(name));
          player2 = NEWP(ScriptedPlayer, hero2, getdefault<string>(args, "name", words[0]), &deck2);
        }
        else
          error("error: bad player tag %s (should be P1 or P2)", pl.c_str());
        
        if (player1 && player2) // we are ready for some shit
          init_players(player1.get(), player2.get());
      }
      elif (step == Setup && substep == SubDecks) {
        Player* pl = read_player(words);
        string card_name = join(" ", words);
        for (int i = 0; i < atoi(getdefault<string,string>(args, "nb", "1").c_str()); i++)
          pl->deck->cards.push_back(cardbook.get_by_name(card_name)->copy());
      } 
    }
  }
}

/*
void test_add_minion(PEngine engine, Player* pl, PConstCard card, float pos = 500.f) {
  PConstCardMinion card_minion = CASTP(card->copy(), const Card_Minion);
  engine->add_thing(pl->state.hero.get(), card_minion->instanciate(pl), Slot(pl, -1, pos));
}

void test_setup(const Collection& cardbook, PEngine engine) {
  Player* pl1 = engine->player1;
  Player* pl2 = engine->player2;
  Hero* hero1 = pl1->state.hero.get();
  Hero* hero2 = pl2->state.hero.get();
  test_add_minion(engine, pl2, cardbook.get_by_name("Haunted Creeper"));
  test_add_minion(engine, pl1, cardbook.get_by_name("Unstable Ghoul"));
  //engine->play_turn();
}
*/
ListPCard ScriptedPlayer::mulligan(ListPCard & cards) const {
  error("shouldn't be here"); //  should never reach this part, as we never mulligan
  return cards;
}

const Action* ScriptedPlayer::choose_actions(ListAction actions, Instance*& choice, Slot& slot) const {
  NI;
  return nullptr;
}
