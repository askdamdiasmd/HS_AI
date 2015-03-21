#ifndef __EVENTS_H__
#define __EVENTS_H__
/* List of codes for each possible event/trigger */

enum Event {
  StartTurn,
  EndTurn,

  StartHeroPower,
  EndHeroPower,
  
  StartSpell,
  EndSpell,
  
  StartAttack,
  EndAttack,
  
  AddMinion,
  AddWeapon,
  AddSecret,
  AddHero,

  MinionPopup,
  WeaponPopup,
  SecretPopup,
  HeroPopup,
};



#endif