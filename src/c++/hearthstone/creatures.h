#include "common.h"


class Thing {};

typedef shared_ptr<Thing> PThing;


class Creature : public Thing {};


typedef shared_ptr<Creature> PCreature;
typedef list<Creature> ListCreature;