#include "common.h"

class Engine;


class Board {
  static Engine* engine;
  Player* players[2];
  ListCreature everything;

public: 
  struct Slot {
    /* a position on the board (insertion index in players.minions[]) */
    const Player* player;
    int index;  // in [0..6]
    float fpos; // float position for smart insertion

    static Engine* engine;
    static void set_engine(Engine* engine) {
      Slot::engine = engine;
    }

    Slot(Player* owner, int index = -1, float fpos = -1) :
      player(owner), index(index), fpos(fpos) {}
    
    string tostr() const {
      return string_format("Position %d", pos);
    }
  };

public:
  Board( PPlayer player1, PPlayer player2 ) :
    players{ player1, player2 } {}

};


typedef shared_ptr<Board> PBoard;