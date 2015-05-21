#ifndef __EVENTS_H__
#define __EVENTS_H__
/* List of codes for each possible event/trigger */

#define ENUM(i) (1<<(i))

enum Event {
  StartTurn       = ENUM(0),
  EndTurn         = ENUM(1),

  StartHeroPower  = ENUM(2),
  EndHeroPower    = ENUM(3),
  
  StartSpell      = ENUM(4),
  EndSpell        = ENUM(5),
  
  StartAttack     = ENUM(6),
  Attack          = ENUM(7),
  EndAttack       = ENUM(8),
  
  AddMinion       = ENUM(9),
  AddWeapon       = ENUM(10),
  AddSecret       = ENUM(11),
  AddHero         = ENUM(12),

  MinionPopup     = ENUM(13),
  WeaponPopup     = ENUM(14),
  SecretPopup     = ENUM(15),
  HeroPopup       = ENUM(16),
  ThingPopup      = MinionPopup | WeaponPopup,

  RemoveMinion    = ENUM(17),
  RemoveWeapon    = ENUM(18),
  RemoveSecret    = ENUM(19),
  RemoveThing     = RemoveMinion | RemoveWeapon,

  MinionDead      = ENUM(20),
  WeaponDead      = ENUM(21),
  ThingDead       = MinionDead | WeaponDead,
};

#undef ENUM

#endif