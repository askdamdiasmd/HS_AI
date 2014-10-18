"""
full set of hearthstone cards
"""
from effects import *
from creatures import Minion, Weapon
from heroes import Card_Hero, Card_HeroAbility


def get_cardbook():
  cardbook = {}
  def add( card ):
    assert card.name not in cardbook, "error: %s already in cardbook"%card.name
    cardbook[card.name] = card
    if card.name_fr:
      assert card.name_fr not in cardbook, "error: %s already in cardbook"%card.name
      cardbook[card.name_fr] = card


  ### O Mana ##################################
  
  ### 1 Mana ##################################
  
  ### 2 Mana ##################################

  add( Card_Minion(1, 1, 1, "Spectral Spider", name_fr="Araignee spectrale", collectible=False) )
  add( Card_Minion(2, 1, 2, "Haunted Creeper", name_fr="Rampante Hantee", cat='beast',
       effects=[Eff_DR_Invoke_Minion(cardbook["Spectral Spider"]),
                Eff_DR_Invoke_Minion(cardbook["Spectral Spider"])],
       desc_fr="Rale d'agonie: Invoque 2 Araignees spectrales 1/1") )
  
  add( Card_Minion(3, 4, 4, "Nerubian", name_fr="Nerubien", collectible=False) )
  add( Card_Minion(2, 0, 2, "Nerubian Egg", effects=[Eff_DR_Invoke_Minion(cardbook["Nerubian"])],
       name_fr="Oeuf de Nerubien", desc_fr="Rale d'agonie: Invoque un Nerubien 4/4", ) )

  ### 3 Mana ##################################

  ### 4 Mana ##################################
  
  ### 5 Mana ##################################

  add( Card_Minion(1, 1, 2, "Slime", name_fr="Gelee", effects='taunt', collectible=False) )
  add( Card_Minion(5, 3, 5, "Sludge Belcher", effects=['taunt',Eff_DR_Invoke_Minion(cardbook["Slime"])],
       name_fr="Crache-vase", desc_fr="Rale d'agonie: Invoque 1 gelee 1/2 avec provocation") )
  
  ### 6 Mana ##################################

  ### 7 Mana ##################################
  ### 8 Mana ##################################
  ### 9+ Mana #################################
  
  # add fake creatures
  for i in range(1,11):
    add( Card_Minion(i,i+1,i,'Fake Creature %d'%i,name_fr="Fausse creature %d"%i) )
    
  # add fake weapons
  for i in range(1,5):
    add( Card_Weapon(i,i,2,'Fake Weapon %d'%i) )
  
  # add fake spells
  for i in range(1,10):
    add( Card_FakeDamageSpell(i) )


  ### Hero Abilities ##########################

  add( Card_HeroAbility(2, "Shapeshift", 
       lambda self: [Msg_BindEffect(self.caster, self.caster.hero, 
                                    Eff_BuffMinion(1,0,armor=1,temp=True))], 
       desc="+1 Attack this turn. +1 Armor.", cls="druid") )
  
  add( Card_HeroAbility(2, "Steady Shot", 
       lambda self: [Msg_HeroDamage(self.caster,self.engine.board.get_enemy_hero(self.caster),2)],     
      desc="Deal 2 damage to the enemy hero.", cls="hunter") )
  
  add( Card_HeroAbility(2, "Fireblast", 
       lambda self: [Msg_HeroDamage(self.caster,self.choices[0],1)],
       targets="characters", desc="Deal 1 damage.", cls="mage") )

  add( Card_HeroAbility(2, "Reinforce", 
        lambda self: [Msg_AddMinion(self.caster, Minion(cardbook["Silver Hand Recruit"],owner=self.caster), 
                                    Slot(self.caster, fpos="right"))],
        desc="Summon a 1/1 Silver Hand Recruit.", cls="paladin") )

  add( Card_HeroAbility(2, "Lesser Heal",
        lambda self: [Msg_HeroHeal(self.caster,self.choices[0],2)],
        targets="characters", desc="Restore 2 Health.", cls="priest") )
        
  add( Card_HeroAbility(2, "Mind Spike", 
        lambda self: [Msg_HeroDamage(self.caster,self.choices[0],1)], 
        targets="characters", desc="Deal 2 damage.", cls="priest") )
  
  add( Card_HeroAbility(2, "Dagger Mastery", 
        lambda self: [Msg_AddWeapon(self.caster,Weapon(cardbook["Wicked Knife"],owner=self.caster))],
        desc="Equip a 1/2 Dagger.", cls="rogue") )
  
  #add( Card_HeroAbility(2, "Totemic Call", desc="Summon a random Totem.", cls="shaman") )
  
  add( Card_HeroAbility(2, "INFERNO!", 
        lambda self: [Msg_AddMinion(self.caster, Minion(cardbook["Infernal"],owner=self.caster), Slot(self.caster, fpos="right"))],
        desc="Summon a 6/6 Infernal.", cls="warlock") )
  
  add( Card_HeroAbility(2, "Life Tap", 
        lambda self: [Msg_DrawCard(self.caster),Msg_HeroDamage(self.caster, self.caster.hero, 2)],
        desc="Draw a card and take 2 damage.", cls="warlock") )
  
  add( Card_HeroAbility(2, "Armor Up!", 
        lambda self: [Msg_BindEffect(self.caster, self.caster.hero, Msg_BuffMinion(armor=2))], 
        desc="Gain 2 Armor.", cls="warrior") )
  
  ### Heroes #######################################
  
  add( Card_Hero(0, 0, 30, "Malfurion Stormrage", cls="druid", ability=cardbook["Shapeshift"]) )
  add( Card_Hero(0, 0, 30, "Rexxar", cls="hunter", ability=cardbook["Steady Shot"]) )
  add( Card_Hero(0, 0, 30, "Jaina Proudmoore", cls="mage", ability=cardbook["Fireblast"]) )
  add( Card_Hero(0, 0, 30, "Uther Lightbringer", cls="paladin", ability=cardbook["Reinforce"]) )
  add( Card_Hero(0, 0, 30, "Anduin Wrynn", cls="priest", ability=cardbook["Lesser Heal"]) )
  #add( Card_Hero(0, 0, 30, "Valeera Sanguinar", cls="rogue", ability=cardbook[""]) )
  #add( Card_Hero(0, 0, 30, "Thrall", cls="shaman", ability=cardbook[""]) )
  add( Card_Hero(0, 0, 30, "Gul'dan", cls="warlock", ability=cardbook["Life Tap"]) )
  add( Card_Hero(0, 0, 15, "Lord Jaraxxus", cls="warlock", ability=cardbook["INFERNO!"]) )
  add( Card_Hero(0, 0, 30, "Garrosh Hellscream", cls="warrior", ability=cardbook["Armor Up!"]) )
  
  ### Minions #######################################
  
  #add( Card_Minion(1, 2, 2, "Treant", cls="druid") )
  #add( Card_Minion(1, 2, 2, "Treant", desc="Charge.  At the end of the turn, destroy this minion.", cls="druid", effects="charge") )
  #add( Card_Minion(1, 2, 2, "Treant", desc="Taunt", cls="druid", effects="taunt") )
  #add( Card_Minion(2, 3, 2, "Panther", cls="druid", cat="beast") )
  #add( Card_Minion(4, 2, 4, "Keeper of the Grove", desc="Choose One - Deal 2 damage; or Silence a minion.", cls="druid") )
  #add( Card_Minion(5, 4, 4, "Druid of the Claw", desc="Charge", cls="druid", effects="charge") )
  #add( Card_Minion(5, 4, 6, "Druid of the Claw", desc="Taunt", cls="druid", effects="taunt") )
  #add( Card_Minion(5, 4, 4, "Druid of the Claw", desc="Choose One - Charge; or +2 Health and Taunt.", cls="druid") )
  #add( Card_Minion(7, 5, 5, "Ancient of Lore", desc="Choose One - Draw 2 cards; or Restore 5 Health.", cls="druid") )
  #add( Card_Minion(7, 5, 5, "Ancient of War", desc="Choose One - Taunt and +5 Health; or +5 Attack.", cls="druid") )
  #add( Card_Minion(8, 8, 8, "Ironbark Protector", desc="Taunt", cls="druid", effects="taunt") )
  #add( Card_Minion(9, 5, 8, "Cenarius", desc="Choose One - Give your other minions +2/+2; or Summon two 2/2 Treants with Taunt.", cls="druid") )
  #add( Card_Minion(0, 1, 1, "Snake", cls="hunter", cat="beast") )
  #add( Card_Minion(1, 1, 1, "Timber Wolf", desc="Your other Beasts have +1 Attack.", cls="hunter", cat="beast") )
  
  add( Card_Minion(2, 2, 2, "Hyena", name_fr="Hyene", cls="hunter", cat="beast", collectible=False) )
  
  #add( Card_Minion(2, 2, 2, "Scavenging Hyena", desc="Whenever a friendly Beast dies, gain +2/+1.", cls="hunter", cat="beast") )
  #add( Card_Minion(2, 2, 1, "Starving Buzzard", desc="Whenever you summon a Beast, draw a card.", cls="hunter", cat="beast") )
  #add( Card_Minion(3, 4, 2, "Huffer", desc="Charge", cls="hunter", cat="beast", effects="charge") )
  #add( Card_Minion(3, 2, 4, "Leokk", desc="Other friendly minions have +1 Attack.", cls="hunter", cat="beast") )
  #add( Card_Minion(3, 4, 4, "Misha", desc="Taunt", cls="hunter", cat="beast", effects="taunt") )
  #add( Card_Minion_BC(4, 4, 3, "Houndmaster", desc="Battlecry: Give a friendly Beast +2/+2 and Taunt.", cls="hunter") )
  #add( Card_Minion(5, 2, 5, "Tundra Rhino", desc="Your Beasts have Charge.", cls="hunter", cat="beast") )

  add( Card_Minion(6, 6, 5, "Savannah Highmane", name_fr="Grande Criniere des Savanes", 
       effects=[Eff_DR_Invoke_Minion(cardbook["Hyena"]),
                Eff_DR_Invoke_Minion(cardbook["Hyena"])],
       desc="Deathrattle: Summon two 2/2 Hyenas.", cls="hunter", cat='beast',
       desc_fr="Rale d'agonie: Invoque 2 hyenes 2/2") )

  #add( Card_Minion(9, 8, 8, "King Krush", desc="Charge", cls="hunter", cat="beast", effects="charge") )
  #add( Card_Minion(0, null, 2, "Mirror Image", desc="Taunt", cls="mage", effects="taunt") )
  #add( Card_Minion(0, 1, 3, "Spellbender", cls="mage") )
  #add( Card_Minion(1, 1, 3, "Mana Wyrm", desc="Whenever you cast a spell, gain +1 Attack.", cls="mage") )
  #add( Card_Minion(2, 3, 2, "Sorcerer's Apprentice", desc="Your spells cost (1) less.", cls="mage") )
  #add( Card_Minion_BC(3, 4, 3, "Kirin Tor Mage", desc="Battlecry: The next Secret you play this turn costs (0).", cls="mage") )
  #add( Card_Minion(4, 3, 3, "Ethereal Arcanist", desc="If you control a Secret at the end of your turn, gain +2/+2.", cls="mage") )
  #add( Card_Minion(4, 3, 6, "Water Elemental", desc="Freeze any character damaged by this minion.", cls="mage") )
  #add( Card_Minion(7, 5, 7, "Archmage Antonidas", desc="Whenever you cast a spell, put a 'Fireball' spell into your hand.", cls="mage") )
  #add( Card_Minion(0, 1, 1, "Avatar of the Coin", desc="You lost the coin flip, but gained a friend.") )
  #add( Card_Minion(0, null, 2, "Barrel", desc="Is something in this barrel?") )
  #add( Card_Minion(0, 1, 1, "Chicken", desc="Hey Chicken!", cat="beast") )
  #add( Card_Minion(0, null, 1, "Frog", desc="Taunt", cat="beast", effects="taunt") )
  #add( Card_Minion(0, 1, 1, "Murloc Scout", cat="murloc") )
  #add( Card_Minion(0, 1, 1, "Sheep", cat="beast") )
  #add( Card_Minion(0, 1, 1, "Violet Apprentice") )
  
  add( Card_Minion(0, 1, 1, "Wisp", name_fr="Feu follet") )

  add( Card_Minion_BC(1, 2, 1, "Abusive Sergeant", Eff_BuffMinion(2,0,True), "minions",
       name_fr="Sergent Grossier", 
       desc="Battlecry: Give a friendly minion +2 Attack this turn.",
       desc_fr="Cri de guerre: confere +2 ATQ a un serviteur pendant ce tour") )
  
  #add( Card_Minion(1, 1, 1, "Angry Chicken", desc="Enrage: +5 Attack.", cat="beast", effects="enrage") )
  #add( Card_Minion(1, 1, 1, "Argent Squire", desc="Divine Shield", effects="divine shield") )
  #add( Card_Minion_BC(1, 1, 2, "Bloodsail Corsair", desc="Battlecry: Remove 1 Durability from your opponent's weapon.", cat="pirate") )
  #add( Card_Minion(1, 1, 1, "Boar", cat="beast") )
  #add( Card_Minion(1, 1, 1, "Crazed Hunter") )
  #add( Card_Minion_BC(1, 1, 2, "Crazy Monkey", desc="Battlecry: Throw Bananas.") )
  
  add( Card_Minion(1, 2, 1, "Damaged Golem", name_fr="Golem endommage", collectible=False) )
  
  #add( Card_Minion_BC(1, 1, 1, "Elven Archer", desc="Battlecry: Deal 1 damage.") )
  #add( Card_Minion(1, null, 4, "Emboldener 3000", desc="At the end of your turn, give a random minion +1/+1.") )
  #add( Card_Minion(1, 2, 1, "Flame of Azzinoth") )
  #add( Card_Minion(1, 2, 1, "Flame of Azzinoth") )
  #add( Card_Minion(1, 1, 1, "Gnoll") )

  add( Card_Minion(1, 1, 2, 'Goldshire Footman',name_fr='Soldat de Comte de l\'Or',effects='taunt') )

  #add( Card_Minion(1, 1, 1, "Grimscale Oracle", desc="ALL other Murlocs have +1 Attack.", cat="murloc") )
  #add( Card_Minion(1, null, 1, "Homing Chicken", desc="At the start of your turn, destroy this minion and draw 3 cards.") )
  #add( Card_Minion_BC(1, 1, 2, "Hungry Crab", desc="Battlecry: Destroy a Murloc and gain +2/+2.", cat="beast") )
  #add( Card_Minion(1, 1, 1, "Imp", cat="demon") )
  #add( Card_Minion(1, 2, 1, "Leper Gnome", desc="Deathrattle: Deal 2 damage to the enemy hero.", effects="deathrattle") )
  #add( Card_Minion(1, 1, 2, "Lightwarden", desc="Whenever a character is healed, gain +2 Attack.") )
  #add( Card_Minion(1, 2, 1, "Mechanical Dragonling") )
  
  add( Card_Minion(1, 2, 1, 'Murloc Raider',name_fr='Ecumeur murloc', cat="murloc") )

  #add( Card_Minion(1, 1, 2, "Murloc Tidecaller", desc="Whenever a Murloc is summoned, gain +1 Attack.", cat="murloc") )
  #add( Card_Minion(1, 1, 1, "Naga Myrmidon") )
  #add( Card_Minion(1, 1, 1, "Pandaren Scout") )
  #add( Card_Minion(1, null, 3, "Poultryizer", desc="At the start of your turn, transform a random minion into a 1/1 Chicken.") )
  #add( Card_Minion(1, null, 3, "Repair Bot", desc="At the end of your turn, restore 6 Health to a damaged character.") )
  #add( Card_Minion(1, 2, 1, "Riverpaw Gnoll") )
  #add( Card_Minion(1, 1, 2, "Secretkeeper", desc="Whenever a Secret is played, gain +1/+1.") )
  #add( Card_Minion(1, null, 4, "Shieldbearer", desc="Taunt", effects="taunt") )
  #add( Card_Minion(1, 1, 1, "Skeleton") )
  #add( Card_Minion(1, 2, 1, "Southsea Deckhand", desc="Has Charge while you have a weapon equipped.", cat="pirate") )
  #add( Card_Minion(1, 2, 2, "Squire") )
  #add( Card_Minion(1, 1, 1, "Squirrel", cat="beast") )
  #add( Card_Minion(1, 1, 1, "Stonetusk Boar", desc="Charge", cat="beast", effects="charge") )
  #add( Card_Minion_BC(1, 2, 1, "Voodoo Doctor", desc="Battlecry: Restore 2 Health.") )
  #add( Card_Minion(1, 1, 1, "Whelp", cat="dragon") )
  #add( Card_Minion(1, 2, 1, "Worgen Infiltrator", desc="Stealth", effects="stealth") )
  #add( Card_Minion(1, 1, 1, "Young Dragonhawk", desc="Windfury", cat="beast", effects="windfury") )
  #add( Card_Minion(1, 2, 1, "Young Priestess", desc="At the end of your turn, give another random friendly minion +1 Health.") )
  #add( Card_Minion_BC(10, 12, 12, "Deathwing", desc="Battlecry: Destroy all other minions and discard your hand.", cat="dragon") )
  #add( Card_Minion(10, 8, 8, "Sea Giant", desc="Costs (1) less for each other minion on the battlefield.") )
  #add( Card_Minion(12, 8, 8, "Mountain Giant", desc="Costs (1) less for each other card in your hand.") )
  #add( Card_Minion_BC(2, 3, 2, "Acidic Swamp Ooze", desc="Battlecry: Destroy your opponent's weapon.") )
  #add( Card_Minion(2, 2, 3, "Amani Berserker", desc="Enrage: +3 Attack", effects="enrage") )
  #add( Card_Minion(2, 4, 5, "Ancient Watcher", desc="Can't Attack.") )
  #add( Card_Minion(2, 3, 2, "Bloodfen Raptor", cat="beast") )
  #add( Card_Minion(2, 1, 1, "Bloodmage Thalnos", desc="Spell Damage +1. Deathrattle: Draw a card.", effects="spell damage deathrattle") )
  #add( Card_Minion_BC(2, 2, 3, "Bloodsail Raider", desc="Battlecry: Gain Attack equal to the Attack of your weapon.", cat="pirate") )
  #add( Card_Minion(2, 2, 1, "Bluegill Warrior", desc="Charge", cat="murloc", effects="charge") )
  #add( Card_Minion_BC(2, 1, 1, "Captain's Parrot", desc="Battlecry: Put a random Pirate from your deck into your hand.", cat="beast") )
  #add( Card_Minion_BC(2, 2, 2, "Crazed Alchemist", desc="Battlecry: Swap the Attack and Health of a minion.") )

  add( Card_Minion(2, 2, 2, "Dire Wolf Alpha", name_fr="Loup alpha redoutable", cat="beast",
        effects=[Eff_BuffLeftRight(1,0)], 
        desc="Adjacent minions have +1 Attack.", desc_fr="Les serviteurs adjacents ont +1 ATQ") )
  
  #add( Card_Minion(2, null, 7, "Doomsayer", desc="At the start of your turn, destroy ALL minions.") )
  #add( Card_Minion(2, 3, 2, "Faerie Dragon", desc="Can't be targeted by Spells or Hero Powers.", cat="dragon") )
  #add( Card_Minion(2, 3, 3, "Finkle Einhorn") )
  #add( Card_Minion(2, 2, 2, "Frostwolf Grunt", desc="Taunt", effects="taunt") )
  #add( Card_Minion(2, 2, 2, "Gnoll", desc="Taunt", effects="taunt") )
  #add( Card_Minion(2, 1, 3, "Hidden Gnome", desc="Was hiding in a barrel!") )
  #add( Card_Minion_BC(2, 2, 1, "Ironbeak Owl", , ) )
  
  add( Card_Minion_BC(2, 2, 1, "Ironbeak Owl", Eff_Silence(), "minions", 
       name_fr="Chouette bec-de-fer", desc="Battlecry: Silence a minion.", cat="beast",
       desc_fr="Cri de guerre: reduit au silence un autre serviteur") )
  
  #add( Card_Minion(2, 3, 2, "Knife Juggler", desc="After you summon a minion, deal 1 damage to a random enemy.") )
  #add( Card_Minion(2, 2, 2, "Kobold Geomancer", desc="Spell Damage +1", effects="spell damage") )
  #add( Card_Minion(2, 2, 1, "Loot Hoarder", desc="Deathrattle: Draw a card.", effects="deathrattle") )
  #add( Card_Minion(2, null, 4, "Lorewalker Cho", desc="Whenever a player casts a spell, put a copy into the other player's hand.") )
  #add( Card_Minion_BC(2, 3, 2, "Mad Bomber", desc="Battlecry: Deal 3 damage randomly split between all other characters.") )
  #add( Card_Minion(2, 1, 3, "Mana Addict", desc="Whenever you cast a spell, gain +2 Attack this turn.") )
  #add( Card_Minion(2, 2, 2, "Mana Wraith", desc="ALL minions cost (1) more.") )
  #add( Card_Minion(2, 1, 3, "Master Swordsmith", desc="At the end of your turn, give another random friendly minion +1 Attack.") )
  #add( Card_Minion_BC(2, 4, 4, "Millhouse Manastorm", desc="Battlecry: Enemy spells cost (0) next turn.") )
  #add( Card_Minion_BC(2, 2, 1, "Murloc Tidehunter", desc="Battlecry: Summon a 1/1 Murloc Scout.", cat="murloc") )
  #add( Card_Minion(2, null, 4, "Nat Pagle", desc="At the start of your turn, you have a 50% chance to draw an extra card.") )
  #add( Card_Minion_BC(2, 1, 1, "Novice Engineer", desc="Battlecry: Draw a card.") )
  #add( Card_Minion(2, 2, 2, "Pint-Sized Summoner", desc="The first minion you play each turn costs (1) less.") )
  
  add( Card_Minion(2, 2, 3, 'River Crocolisk', name_fr='Crocilisque des rivieres',cat='beast') )
  
  #add( Card_Minion(2, 2, 2, "Shado-Pan Monk") )
  
  add( Card_Minion_BC(2, 2, 3, "Sunfury Protector", Eff_BuffMinion(0,0,others='taunt'), 'neighbors',
       name_fr="Protectrice Solfury", desc="Battlecry: Give adjacent minions Taunt.",
       desc_fr="Cri de guerre: confere Provocation aux serviteurs adjacents") )
  
  #add( Card_Minion(2, 3, 2, "Wild Pyromancer", desc="After you cast a spell, deal 1 damage to ALL minions.") )
  #add( Card_Minion_BC(2, 3, 2, "Youthful Brewmaster", desc="Battlecry: Return a friendly minion from the battlefield to your hand.") )
  #add( Card_Minion(20, 8, 8, "Molten Giant", desc="Costs (1) less for each damage your hero has taken.") )
  
  add( Card_Minion(3, 1, 3, "Acolyte of Pain", name_fr="Acolyte de la souffrance", 
       effects=[Eff_DrawCard(Msg_Damage,lambda self,msg: msg.target is self.owner, immediate=True)],
       desc="Whenever this minion takes damage, draw a card.",
       desc_fr="Cri de guerre: Detruit les serviteurs adjacents et gagne leurs pts d'Attaque et de Vie."))
  
  #add( Card_Minion(3, null, 3, "Alarm-o-Bot", desc="At the start of your turn, swap this minion with a random one in your hand.") )
  #add( Card_Minion_BC(3, 4, 2, "Arcane Golem", desc="Charge. Battlecry: Give your opponent a Mana Crystal.", effects="charge") )
  #add( Card_Minion_BC(3, 4, 2, "Big Game Hunter", desc="Battlecry: Destroy a minion with an Attack of 7 or more.") )
  #add( Card_Minion_BC(3, 3, 3, "Blood Knight", desc="Battlecry: All minions lose Divine Shield. Gain +3/+3 for each Shield lost.") )
  #add( Card_Minion_BC(3, 2, 2, "Coldlight Oracle", desc="Battlecry: Each player draws 2 cards.", cat="murloc") )
  #add( Card_Minion_BC(3, 2, 3, "Coldlight Seer", desc="Battlecry: Give ALL other Murlocs +2 Health.", cat="murloc") )
  #add( Card_Minion(3, 1, 4, "Dalaran Mage", desc="Spell Damage +1", effects="spell damage") )
  #add( Card_Minion(3, 1, 4, "Demolisher", desc="At the start of your turn, deal 2 damage to a random enemy.") )
  #add( Card_Minion_BC(3, 3, 3, "Earthen Ring Farseer", desc="Battlecry: Restore 3 Health.") )
  #add( Card_Minion(3, 2, 3, "Emperor Cobra", desc="Destroy any minion damaged by this minion.", cat="beast") )
  #add( Card_Minion(3, 2, 3, "Flesheating Ghoul", desc="Whenever a minion dies, gain +1 Attack.") )
  
  add( Card_Minion(3, 2, 3, "Harvest Golem", effects=[Eff_DR_Invoke_Minion(cardbook["Damaged Golem"])],
       name_fr="Golem des moissons", desc="Deathrattle: Summon a 2/1 Damaged Golem.",
       desc_fr="Rale d'agonie: Invoque un golem endommage 2/1", ) )
  
  #add( Card_Minion(3, 1, 5, "Imp Master", desc="At the end of your turn, deal 1 damage to this minion and summon a 1/1 Imp.") )
  #add( Card_Minion_BC(3, 4, 7, "Injured Blademaster", desc="Battlecry: Deal 4 damage to HIMSELF.") )
  #add( Card_Minion_BC(3, 2, 2, "Ironforge Rifleman", desc="Battlecry: Deal 1 damage.") )
  #add( Card_Minion(3, 3, 3, "Ironfur Grizzly", desc="Taunt", cat="beast", effects="taunt") )
  #add( Card_Minion(3, 4, 2, "Jungle Panther", desc="Stealth", cat="beast", effects="stealth") )
  #add( Card_Minion_BC(3, 5, 5, "King Mukla", desc="Battlecry: Give your opponent 2 Bananas.", cat="beast") )
  #add( Card_Minion(3, 3, 5, "Laughing Sister", desc="Can't be targeted by Spells or Hero Powers.") )
  #add( Card_Minion(3, 5, 1, "Magma Rager") )
  #add( Card_Minion_BC(3, 3, 3, "Mind Control Tech", desc="Battlecry: If your opponent has 4 or more minions, take control of one at random.") )
  #add( Card_Minion(3, 3, 3, "Murloc Warleader", desc="ALL other Murlocs have +2/+1.", cat="murloc") )
  #add( Card_Minion(3, 2, 2, "Questing Adventurer", desc="Whenever you play a card, gain +1/+1.") )
  #add( Card_Minion(3, 3, 3, "Raging Worgen", desc="Enrage: Windfury and +1 Attack", effects="enrage") )
  #add( Card_Minion(3, 2, 2, "Raid Leader", desc="Your other minions have +1 Attack.") )
  #add( Card_Minion_BC(3, 2, 3, "Razorfen Hunter", desc="Battlecry: Summon a 1/1 Boar.") )
  #add( Card_Minion(3, 3, 1, "Scarlet Crusader", desc="Divine Shield", effects="divine shield") )

  add( Card_Minion_BC(3, 3, 2, "Shattered Sun Cleric", Eff_BuffMinion(1,1,False), 'friendly minions',
       desc="Battlecry: Give a friendly minion +1/+1.", name_fr="Clerc du Soleil brise",
       desc_fr="Cri de guerre: confere +1/+1 a un serviteur allie") )

  #add( Card_Minion(3, 1, 4, "Silverback Patriarch", desc="Taunt", cat="beast", effects="taunt") )
  #add( Card_Minion(3, 3, 3, "Southsea Captain", desc="Your other Pirates have +1/+1.", cat="pirate") )
  #add( Card_Minion(3, 2, 3, "Tauren Warrior", desc="Taunt. Enrage: +3 Attack", effects="taunt enrage") )
  #add( Card_Minion(3, 2, 3, "Thrallmar Farseer", desc="Windfury", effects="windfury") )
  #add( Card_Minion_BC(3, 3, 3, "Tinkmaster Overspark", desc="Battlecry: Transform another random minion into a 5/5 Devilsaur or a 1/1 Squirrel at random.") )
  #add( Card_Minion(3, 3, 1, "Wolfrider", desc="Charge", effects="charge") )
  #add( Card_Minion_BC(4, 5, 4, "Ancient Brewmaster", desc="Battlecry: Return a friendly minion from the battlefield to your hand.") )
  #add( Card_Minion_BC(4, 2, 5, "Ancient Mage", desc="Battlecry: Give adjacent minions Spell Damage +1.") )
  add( Card_Minion(4, 4, 5, "Baine Bloodhoof") )
  #add( Card_Minion(4, 4, 4, "Brewmaster") )

  add( Card_Minion(4, 4, 5, 'Chillwind Yeti', name_fr='Yeti Noroit') )
  
  #add( Card_Minion(4, 4, 2, "Cult Master", desc="Whenever one of your other minions dies, draw a card.") )

  add( Card_Minion_BC(4, 4, 4, "Dark Iron Dwarf", Eff_BuffMinion(2,0,True), "minions",
       name_fr="Nain sombrefer", desc="Battlecry: Give a minion +2 Attack this turn.",
       desc_fr="Cri de guerre: confere +2 ATQ a un serviteur pendant ce tour") )
  
  add( Card_Minion_BC(4, 2, 3, "Defender of Argus", Eff_BuffMinion(1,1,others='taunt'), 'neighbors',
       name_fr="Defenseur d'Argus", desc="Battlecry: Give adjacent minions +1/+1 and Taunt.",
       desc_fr="Cri de guerre: donne aux serviteurs adjacents +1/+1 et Provocation") )
  
  #add( Card_Minion_BC(4, 2, 4, "Dragonling Mechanic", desc="Battlecry: Summon a 2/1 Mechanical Dragonling.") )
  #add( Card_Minion(4, 3, 3, "Dread Corsair", desc="Taunt. Costs (1) less per Attack of your weapon.", cat="pirate", effects="taunt") )
  #add( Card_Minion(4, 7, 6, "Emerald Drake", cat="dragon") )
  #add( Card_Minion_BC(4, 2, 4, "Gnomish Inventor", desc="Battlecry: Draw a card.") )
  #add( Card_Minion_BC(4, 6, 2, "Leeroy Jenkins", desc="Charge. Battlecry: Summon two 1/1 Whelps for your opponent.", effects="charge") )
  #add( Card_Minion(4, 5, 2, "Massive Gnoll") )
  #add( Card_Minion(4, 1, 7, "Mogu'shan Warden", desc="Taunt", effects="taunt") )
  #add( Card_Minion(4, 2, 7, "Oasis Snapjaw", cat="beast") )
  #add( Card_Minion(4, 4, 4, "Ogre Magi", desc="Spell Damage +1", effects="spell damage") )
  #add( Card_Minion(4, 2, 4, "Old Murk-Eye", desc="Charge. Has +1 Attack for each other Murloc on the battlefield.", cat="murloc", effects="charge") )
  #add( Card_Minion(4, 3, 5, "Sen'jin Shieldmasta", desc="Taunt", effects="taunt") )
  #add( Card_Minion(4, 3, 3, "Silvermoon Guardian", desc="Divine Shield", effects="divine shield") )
  #add( Card_Minion_BC(4, 4, 3, "Spellbreaker", desc="Battlecry: Silence a minion.") )
  #add( Card_Minion(4, 2, 5, "Stormwind Knight", desc="Charge", effects="charge") )
  #add( Card_Minion_BC(4, 4, 1, "Twilight Drake", desc="Battlecry: Gain +1 Health for each card in your hand.", cat="dragon") )
  #add( Card_Minion(4, 3, 5, "Violet Teacher", desc="Whenever you cast a spell, summon a 1/1 Violet Apprentice.") )
  #add( Card_Minion(5, 4, 4, "Abomination", desc="Taunt. Deathrattle: Deal 2 damage to ALL characters.", effects="taunt deathrattle") )
  #add( Card_Minion_BC(5, 4, 4, "Azure Drake", desc="Spell Damage +1. Battlecry: Draw a card.", cat="dragon", effects="spell damage") )
  #add( Card_Minion(5, 5, 4, "Booty Bay Bodyguard", desc="Taunt", effects="taunt") )
  #add( Card_Minion_BC(5, 5, 4, "Captain Greenskin", desc="Battlecry: Give your weapon +1/+1.", cat="pirate") )
  #add( Card_Minion_BC(5, 4, 5, "Darkscale Healer", desc="Battlecry: Restore 2 Health to all friendly characters.") )
  #add( Card_Minion(5, 5, 5, "Devilsaur", cat="beast") )
  #add( Card_Minion_BC(5, 3, 3, "Faceless Manipulator", desc="Battlecry: Choose a minion and become a copy of it.") )
  #add( Card_Minion(5, 3, 6, "Fen Creeper", desc="Taunt", effects="taunt") )
  #add( Card_Minion_BC(5, 4, 4, "Frostwolf Warlord", desc="Battlecry: Gain +1/+1 for each other friendly minion on the battlefield.") )
  #add( Card_Minion(5, 4, 4, "Gadgetzan Auctioneer", desc="Whenever you cast a spell, draw a card.") )
  #add( Card_Minion(5, 2, 7, "Gurubashi Berserker", desc="Whenever this minion takes damage, gain +3 Attack.") )
  #add( Card_Minion_BC(5, 5, 4, "Harrison Jones", desc="Battlecry: Destroy your opponent's weapon and draw cards equal to its Durability.") )
  #add( Card_Minion_BC(5, 4, 4, "Nightblade", desc="Battlecry: Deal 3 damage to the enemy hero.") )
  #add( Card_Minion_BC(5, 4, 4, "Silver Hand Knight", desc="Battlecry: Summon a 2/2 Squire.") )
  #add( Card_Minion(5, 4, 6, "Spiteful Smith", desc="Enrage: Your weapon has +2 Attack.", effects="enrage") )
  #add( Card_Minion_BC(5, 3, 5, "Stampeding Kodo", desc="Battlecry: Destroy a random enemy minion with 2 or less Attack.", cat="beast") )
  #add( Card_Minion_BC(5, 4, 2, "Stormpike Commando", desc="Battlecry: Deal 2 damage.") )
  #add( Card_Minion(5, 5, 5, "Stranglethorn Tiger", desc="Stealth", cat="beast", effects="stealth") )
  #add( Card_Minion(5, 7, 6, "Venture Co. Mercenary", desc="Your minions cost (3) more.") )
  #add( Card_Minion(6, 4, 7, "Archmage", desc="Spell Damage +1", effects="spell damage") )
  #add( Card_Minion(6, 4, 2, "Argent Commander", desc="Charge, Divine Shield", effects="charge divine shield") )
  #add( Card_Minion(6, 6, 7, "Boulderfist Ogre") )
  
  add( Card_Minion(6, 4, 5, "Cairne", effects=[Eff_DR_Invoke_Minion(cardbook["Baine Bloodhoof"])],
       desc="Deathrattle: Summon a 4/5 Baine Bloodhoof.",
       desc_fr="Rale d'agonie: Invoque un Baine 4/5" ) )
  
  #add( Card_Minion_BC(6, 5, 5, "Frost Elemental", desc="Battlecry: Freeze a character.") )
  #add( Card_Minion_BC(6, 6, 6, "Gelbin Mekkatorque", desc="Battlecry: Summon an AWESOME invention.") )
  #add( Card_Minion(6, 4, 4, "Hogger", desc="At the end of your turn, summon a 2/2 Gnoll with Taunt.") )
  #add( Card_Minion(6, 7, 5, "Illidan Stormrage", desc="Whenever you play a card, summon a 2/1 Flame of Azzinoth.", cat="demon") )
  #add( Card_Minion(6, 6, 5, "Lord of the Arena", desc="Taunt", effects="taunt") )
  #add( Card_Minion(6, 10, 10, "Mukla's Big Brother", desc="So strong! And only 6 Mana?!") )
  #add( Card_Minion_BC(6, 5, 4, "Priestess of Elune", desc="Battlecry: Restore 4 Health to your hero.") )
  #add( Card_Minion(6, 5, 2, "Reckless Rocketeer", desc="Charge", effects="charge") )
  #add( Card_Minion(6, 4, 5, "Sunwalker", desc="Taunt. Divine Shield", effects="taunt divine shield") )
  #add( Card_Minion(6, 5, 5, "Sylvanas Windrunner", desc="Deathrattle: Take control of a random enemy minion.", effects="deathrattle") )
  #add( Card_Minion(6, 9, 7, "The Beast", desc="Deathrattle: Summon a 3/3 Finkle Einhorn for your opponent.", cat="beast", effects="deathrattle") )
  #add( Card_Minion_BC(6, 4, 5, "The Black Knight", desc="Battlecry: Destroy an enemy minion with Taunt.") )
  #add( Card_Minion(6, 4, 5, "Windfury Harpy", desc="Windfury", effects="windfury") )
  #add( Card_Minion(7, 7, 5, "Baron Geddon", desc="At the end of your turn, deal 2 damage to ALL other characters.") )
  #add( Card_Minion(7, 9, 5, "Core Hound", cat="beast") )
  #add( Card_Minion(7, 7, 5, "Ravenholdt Assassin", desc="Stealth", effects="stealth") )
  #add( Card_Minion(7, 6, 6, "Stormwind Champion", desc="Your other minions have +1/+1.") )
  #add( Card_Minion(7, 7, 7, "War Golem") )
  #add( Card_Minion(8, 7, 7, "Gruul", desc="At the end of each turn, gain +1/+1 .") )
  #add( Card_Minion(8, 8, 8, "Ragnaros the Firelord", desc="Can't Attack.  At the end of your turn, deal 8 damage to a random enemy.") )
  #add( Card_Minion_BC(9, 8, 8, "Alexstrasza", desc="Battlecry: Set a hero's remaining Health to 15.", cat="dragon") )
  #add( Card_Minion(9, 4, 12, "Malygos", desc="Spell Damage +5", cat="dragon", effects="spell damage") )
  #add( Card_Minion(9, 8, 8, "Nozdormu", desc="Players only have 15 seconds to take their turns.", cat="dragon") )
  #add( Card_Minion_BC(9, 8, 8, "Onyxia", desc="Battlecry: Summon 1/1 Whelps until your side of the battlefield is full.", cat="dragon") )
  #add( Card_Minion(9, 4, 12, "Ysera", desc="At the end of your turn, draw a Dream Card.", cat="dragon") )
  #add( Card_Minion(1, 2, 1, "Defender", cls="paladin") )
  #add( Card_Minion(1, 1, 1, "Silver Hand Recruit", cls="paladin") )
  #add( Card_Minion_BC(2, 2, 2, "Argent Protector", desc="Battlecry: Give a friendly minion Divine Shield.", cls="paladin") )
  #add( Card_Minion_BC(3, 3, 3, "Aldor Peacekeeper", desc="Battlecry: Change an enemy minion's Attack to 1.", cls="paladin") )
  #add( Card_Minion_BC(7, 5, 6, "Guardian of Kings", desc="Battlecry: Restore 6 Health to your hero.", cls="paladin") )
  #add( Card_Minion(8, 6, 6, "Tirion Fordring", desc="Divine Shield. Taunt. Deathrattle: Equip a 5/3 Ashbringer.", cls="paladin", effects="divine shield taunt deathrattle") )
  #add( Card_Minion(0, null, 1, "Shadow of Nothing", desc="Mindgames whiffed! Your opponent had no minions!", cls="priest") )
  #add( Card_Minion(1, 1, 3, "Northshire Cleric", desc="Whenever a minion is healed, draw a card.", cls="priest") )
  #add( Card_Minion(2, null, 5, "Lightwell", desc="At the start of your turn, restore 3 Health to a damaged friendly character.", cls="priest") )
  #add( Card_Minion(4, 3, 5, "Auchenai Soulpriest", desc="Your cards and powers that restore Health now deal damage instead.", cls="priest") )
  #add( Card_Minion(4, null, 5, "Lightspawn", desc="This minion's Attack is always equal to its Health.", cls="priest") )
  #add( Card_Minion_BC(6, 4, 5, "Cabal Shadow Priest", desc="Battlecry: Take control of an enemy minion that has 2 or less Attack.", cls="priest") )
  #add( Card_Minion_BC(6, 6, 6, "Temple Enforcer", desc="Battlecry: Give a friendly minion +3 Health.", cls="priest") )
  #add( Card_Minion(7, 7, 7, "Prophet Velen", desc="Double the damage and healing of your spells and Hero Power.", cls="priest") )
  #add( Card_Minion(1, 2, 1, "Defias Bandit", cls="rogue") )
  #add( Card_Minion(2, 2, 2, "Defias Ringleader", desc="Combo: Summon a 2/1 Defias Bandit.", cls="rogue", effects="combo") )
  #add( Card_Minion(2, 1, 1, "Patient Assassin", desc="Stealth. Destroy any minion damaged by this minion.", cls="rogue", effects="stealth") )
  #add( Card_Minion(3, 2, 2, "Edwin VanCleef", desc="Combo: Gain +2/+2 for each card played earlier this turn.", cls="rogue", effects="combo") )
  #add( Card_Minion(3, 3, 3, "SI:7 Agent", desc="Combo: Deal 2 damage.", cls="rogue", effects="combo") )
  #add( Card_Minion_BC(4, 4, 4, "Master of Disguise", desc="Battlecry: Give a friendly minion Stealth.", cls="rogue") )
  #add( Card_Minion(6, 5, 3, "Kidnapper", desc="Combo: Return a minion to its owner's hand.", cls="rogue", effects="combo") )
  #add( Card_Minion(1, 3, 1, "Dust Devil", desc="Windfury. Overload: (2)", cls="shaman", effects="windfury overload") )
  #add( Card_Minion(1, null, 2, "Healing Totem", desc="At the end of your turn, restore 1 Health to all friendly minions.", cls="shaman", cat="totem") )
  #add( Card_Minion(1, 1, 1, "Searing Totem", cls="shaman", cat="totem") )
  #add( Card_Minion(1, null, 2, "Stoneclaw Totem", desc="Taunt", cls="shaman", cat="totem", effects="taunt") )
  #add( Card_Minion(1, null, 2, "Wrath of Air Totem", desc="Spell Damage +1", cls="shaman", cat="totem", effects="spell damage") )
  #add( Card_Minion(2, null, 3, "Flametongue Totem", desc="Adjacent minions have +2 Attack.", cls="shaman", cat="totem") )
  #add( Card_Minion(2, 2, 3, "Spirit Wolf", desc="Taunt", cls="shaman", effects="taunt") )
  #add( Card_Minion(3, null, 3, "Mana Tide Totem", desc="At the end of your turn, draw a card.", cls="shaman", cat="totem") )
  #add( Card_Minion(3, 2, 4, "Unbound Elemental", desc="Whenever you play a card with Overload, gain +1/+1.", cls="shaman") )
  #add( Card_Minion_BC(4, 3, 3, "Windspeaker", desc="Battlecry: Give a friendly minion Windfury.", cls="shaman") )
  #add( Card_Minion(5, 7, 8, "Earth Elemental", desc="Taunt. Overload: (3)", cls="shaman", effects="taunt overload") )
  #add( Card_Minion_BC(6, 6, 5, "Fire Elemental", desc="Battlecry: Deal 3 damage.", cls="shaman") )
  #add( Card_Minion(8, 3, 5, "Al'Akir the Windlord", desc="Windfury, Charge, Divine Shield, Taunt", cls="shaman", effects="windfury charge divine shield taunt") )
  #add( Card_Minion(1, 0, 1, "Blood Imp", desc="Stealth. At the end of your turn, give another random friendly minion +1 Health.", cls="warlock", cat="demon", effects="stealth") )
  #add( Card_Minion_BC(1, 3, 2, "Flame Imp", desc="Battlecry: Deal 3 damage to your hero.", cls="warlock", cat="demon") )
  #add( Card_Minion(1, 1, 3, "Voidwalker", desc="Taunt", cls="warlock", cat="demon", effects="taunt") )
  #add( Card_Minion(1, 1, 1, "Worthless Imp", desc="You are out of demons! At least there are always imps...", cls="warlock", cat="demon") )
  #add( Card_Minion_BC(2, 4, 3, "Succubus", desc="Battlecry: Discard a random card.", cls="warlock", cat="demon") )
  #add( Card_Minion_BC(3, 3, 5, "Felguard", desc="Taunt. Battlecry: Destroy one of your Mana Crystals.", cls="warlock", cat="demon", effects="taunt") )

  add( Card_Minion_BC(3, 3, 3, "Void Terror", Eff_Absorb(), 'neighbors', 
       desc="Battlecry: Destroy adjacent minions and gain their Attack and Health.", 
       name_fr="Terreur du vide", cls="warlock", cat="demon",
       desc_fr="Cri de guerre: Detruit les serviteurs adjacents et gagne leurs pts d'Attaque et de Vie."))
  
  #add( Card_Minion_BC(4, 5, 6, "Pit Lord", desc="Battlecry: Deal 5 damage to your hero.", cls="warlock", cat="demon") )
  #add( Card_Minion(4, null, 4, "Summoning Portal", desc="Your minions cost (2) less, but not less than (1).", cls="warlock") )
  #add( Card_Minion_BC(5, 5, 7, "Doomguard", desc="Charge. Battlecry: Discard two random cards.", cls="warlock", cat="demon", effects="charge") )
  #add( Card_Minion_BC(6, 6, 6, "Dread Infernal", desc="Battlecry: Deal 1 damage to ALL other characters.", cls="warlock", cat="demon") )
  #add( Card_Minion(6, 6, 6, "Infernal", cls="warlock", cat="demon") )
  #add( Card_Minion_BC(9, 3, 15, "Lord Jaraxxus", desc="Battlecry: Destroy your hero and replace him with Lord Jaraxxus.", cls="warlock", cat="demon") )
  #add( Card_Minion(2, 1, 4, "Armorsmith", desc="Whenever a friendly minion takes damage, gain 1 Armor.", cls="warrior") )
  #add( Card_Minion_BC(2, 2, 2, "Cruel Taskmaster", desc="Battlecry: Deal 1 damage to a minion and give it +2 Attack.", cls="warrior") )
  #add( Card_Minion(3, 2, 4, "Frothing Berserker", desc="Whenever a minion takes damage, gain +1 Attack.", cls="warrior") )
  #add( Card_Minion(3, 2, 3, "Warsong Commander", desc="Whenever you play a minion with 3 or less Attack, give it Charge.", cls="warrior") )
  #add( Card_Minion_BC(4, 3, 3, "Arathi Weaponsmith", desc="Battlecry: Equip a 2/2 weapon.", cls="warrior") )
  #add( Card_Minion(4, 4, 3, "Kor'kron Elite", desc="Charge", cls="warrior", effects="charge") )
  #add( Card_Minion(8, 4, 9, "Grommash Hellscream", desc="Charge.  Enrage: +6 Attack", cls="warrior", effects="charge enrage") )
  #add( Card_Spell(0, "Ancient Secrets", desc="Restore 5 Health.", cls="druid") )
  #add( Card_Spell(0, "Ancient Teachings", desc="Draw 2 cards.", cls="druid") )
  #add( Card_Spell(0, "Bear Form", desc="+2 Health and Taunt.", cls="druid") )
  #add( Card_Spell(0, "Cat Form", desc="Charge", cls="druid") )
  #add( Card_Spell(0, "Demigod's Favor", desc="Give your other minions +2/+2.", cls="druid") )
  #add( Card_Spell(0, "Dispel", desc="Silence a minion.", cls="druid") )
  #add( Card_Spell(0, "Excess Mana", desc="Draw a card. (You can only have 10 Mana in your tray.)", cls="druid") )
  #add( Card_Spell(0, "Innervate", desc="Gain 2 Mana Crystals this turn only.", cls="druid") )
  #add( Card_Spell(0, "Leader of the Pack", desc="Give all of your minions +1/+1.", cls="druid") )
  #add( Card_Spell(0, "Mark of Nature", desc="+4 Attack.", cls="druid") )
  #add( Card_Spell(0, "Mark of Nature", desc="+4 Health and Taunt.", cls="druid") )
  #add( Card_Spell(0, "Moonfire", desc="Deal 1 damage.", cls="druid") )
  #add( Card_Spell(0, "Moonfire", desc="Deal 2 damage.", cls="druid") )
  #add( Card_Spell(0, "Nourish", desc="Draw 3 cards.", cls="druid") )
  #add( Card_Spell(0, "Nourish", desc="Gain 2 Mana Crystals.", cls="druid") )
  #add( Card_Spell(0, "Rooted", desc="+5 Health and Taunt.", cls="druid") )
  #add( Card_Spell(0, "Shan'do's Lesson", desc="Summon two 2/2 Treants with Taunt.", cls="druid") )
  #add( Card_Spell(0, "Starfall", desc="Deal 2 damage to all enemy minions.", cls="druid") )
  #add( Card_Spell(0, "Starfall", desc="Deal 5 damage to a minion.", cls="druid") )
  #add( Card_Spell(0, "Summon a Panther", desc="Summon a 3/2 Panther.", cls="druid") )
  #add( Card_Spell(0, "Uproot", desc="+5 Attack.", cls="druid") )
  #add( Card_Spell(0, "Wrath", desc="Deal 1 damage to a minion. Draw a card.", cls="druid") )
  #add( Card_Spell(0, "Wrath", desc="Deal 3 damage to a minion.", cls="druid") )
  #add( Card_Spell(1, "Claw", desc="Give your hero +2 Attack this turn and 2 Armor.", cls="druid") )
  #add( Card_Spell(1, "Naturalize", desc="Destroy a minion. Your opponent draws 2 cards.", cls="druid") )
  #add( Card_Spell(1, "Savagery", desc="Deal damage equal to your hero's Attack to a minion.", cls="druid") )
  #add( Card_Spell(2, "Mark of the Wild", desc="Give a minion Taunt and +2/+2. (+2 Attack/+2 Health)", cls="druid") )
  #add( Card_Spell(2, "Power of the Wild", desc="Choose One - Give your minions +1/+1; or Summon a 3/2 Panther.", cls="druid") )
  #add( Card_Spell(2, "Wild Growth", desc="Gain an empty Mana Crystal.", cls="druid") )
  #add( Card_Spell(2, "Wrath", desc="Choose One - Deal 3 damage to a minion; or 1 damage and draw a card.", cls="druid") )
  #add( Card_Spell(3, "Healing Touch", desc="Restore #8 Health.", cls="druid") )
  #add( Card_Spell(3, "Mark of Nature", desc="Choose One - Give a minion +4 Attack; or +4 Health and Taunt.", cls="druid") )
  #add( Card_Spell(3, "Savage Roar", desc="Give your characters +2 Attack this turn.", cls="druid") )
  #add( Card_Spell(4, "Bite", desc="Give your hero +4 Attack this turn and 4 Armor.", cls="druid") )
  #add( Card_Spell(4, "Soul of the Forest", desc="Give your minions "Deathrattle: Summon a 2/2 Treant."", cls="druid") )
  #add( Card_Spell(4, "Swipe", desc="Deal 4 damage to an enemy and 1 damage to all other enemies.", cls="druid") )
  #add( Card_Spell(5, "Nourish", desc="Choose One - Gain 2 Mana Crystals; or Draw 3 cards.", cls="druid") )
  #add( Card_Spell(5, "Starfall", desc="Choose One - Deal 5 damage to a minion; or 2 damage to all enemy minions.", cls="druid") )
  #add( Card_Spell(6, "Force of Nature", desc="Summon three 2/2 Treants with Charge that die at the end of the turn.", cls="druid") )
  #add( Card_Spell(6, "Starfire", desc="Deal 5 damage.  Draw a card.", cls="druid") )
  #add( Card_Spell(0, "Hunter's Mark", desc="Change a minion's Health to 1.", cls="hunter") )
  #add( Card_Spell(1, "Arcane Shot", desc="Deal 2 damage.", cls="hunter") )
  #add( Card_Spell(1, "Bestial Wrath", desc="Give a Beast +2 Attack and Immune this turn.", cls="hunter") )
  #add( Card_Spell(1, "Flare", desc="All minions lose Stealth. Destroy all enemy Secrets. Draw a card.", cls="hunter") )
  #add( Card_Spell(1, "Tracking", desc="Look at the top three cards of your deck. Draw one and discard the others.", cls="hunter") )
  #add( Card_Spell(2, "Explosive Trap", desc="Secret: When your hero is attacked, deal 2 damage to all enemies.", cls="hunter") )
  #add( Card_Spell(2, "Freezing Trap", desc="Secret: When an enemy minion attacks, return it to its owner's hand and it costs (2) more.", cls="hunter") )
  #add( Card_Spell(2, "Misdirection", desc="Secret: When a character attacks your hero, instead he attacks another random character.", cls="hunter") )
  #add( Card_Spell(2, "Snake Trap", desc="Secret: When one of your minions is attacked, summon three 1/1 Snakes.", cls="hunter") )
  #add( Card_Spell(2, "Snipe", desc="Secret: When your opponent plays a minion, deal 4 damage to it.", cls="hunter") )
  #add( Card_Spell(2, "Unleash the Hounds", desc="For each enemy minion, summon a 1/1 Hound with Charge.", cls="hunter") )
  #add( Card_Spell(3, "Animal Companion", desc="Summon a random Beast Companion.", cls="hunter") )
  #add( Card_Spell(3, "Deadly Shot", desc="Destroy a random enemy minion.", cls="hunter") )
  #add( Card_Spell(3, "Kill Command", desc="Deal 3 damage.  If you have a Beast, deal 5 damage instead.", cls="hunter") )
  #add( Card_Spell(4, "Multi-Shot", desc="Deal 3 damage to two random enemy minions.", cls="hunter") )
  #add( Card_Spell(5, "Explosive Shot", desc="Deal 5 damage to a minion and 2 damage to adjacent ones.", cls="hunter") )
  #add( Card_Spell(1, "Arcane Missiles", desc="Deal 3 damage randomly split among enemy characters.", cls="mage") )
  #add( Card_Spell(1, "Ice Lance", desc="Freeze a character. If it was already Frozen, deal 4 damage instead.", cls="mage") )
  #add( Card_Spell(1, "Mirror Image", desc="Summon two 0/2 minions with Taunt.", cls="mage") )
  #add( Card_Spell(10, "Pyroblast", desc="Deal 10 damage.", cls="mage") )
  #add( Card_Spell(2, "Arcane Explosion", desc="Deal 1 damage to all enemy minions.", cls="mage") )
  #add( Card_Spell(2, "Frostbolt", desc="Deal 3 damage to a character and Freeze it.", cls="mage") )
  #add( Card_Spell(3, "Arcane Intellect", desc="Draw 2 cards.", cls="mage") )
  #add( Card_Spell(3, "Counterspell", desc="Secret: When your opponent casts a spell, Counter it.", cls="mage") )
  #add( Card_Spell(3, "Frost Nova", desc="Freeze all enemy minions.", cls="mage") )
  #add( Card_Spell(3, "Ice Barrier", desc="Secret: As soon as your hero is attacked, gain 8 Armor.", cls="mage") )
  #add( Card_Spell(3, "Ice Block", desc="Secret: When your hero takes fatal damage, prevent it and become Immune this turn.", cls="mage") )
  #add( Card_Spell(3, "Mirror Entity", desc="Secret: When your opponent plays a minion, summon a copy of it.", cls="mage") )
  #add( Card_Spell(3, "Spellbender", desc="Secret: When an enemy casts a spell on a minion, summon a 1/3 as the new target.", cls="mage") )
  #add( Card_Spell(3, "Vaporize", desc="Secret: When a minion attacks your hero, destroy it.", cls="mage") )
  #add( Card_Spell(4, "Cone of Cold", desc="Freeze a minion and the minions next to it, and deal 1 damage to them.", cls="mage") )
  #add( Card_Spell(4, "Fireball", desc="Deal 6 damage.", cls="mage") )
  #add( Card_Spell(4, "Polymorph", desc="Transform a minion into a 1/1 Sheep.", cls="mage") )
  #add( Card_Spell(6, "Blizzard", desc="Deal 2 damage to all enemy minions and Freeze them.", cls="mage") )
  #add( Card_Spell(7, "Flamestrike", desc="Deal 4 damage to all enemy minions.", cls="mage") )
  #add( Card_Spell(0, "Dream", desc="Return a minion to its owner's hand.") )
  #add( Card_Spell(0, "Nightmare", desc="Give a minion +5/+5.  At the start of your next turn, destroy it.") )
  #add( Card_Spell(0, "The Coin", desc="Gain 1 Mana Crystal this turn only.") )
  #add( Card_Spell(1, "Bananas", desc="Give a friendly minion +1/+1. (+1 Attack/+1 Health)") )
  #add( Card_Spell(1, "Bananas", desc="Give a minion +1/+1.") )
  #add( Card_Spell(1, "Barrel Toss", desc="Deal 2 damage.") )
  #add( Card_Spell(1, "Transcendence", desc="Until you kill Cho's minions, he can't be attacked.") )
  #add( Card_Spell(2, "NOOOOOOOOOOOO", desc="Somehow, the card you USED to have has been deleted.  Here, have this one instead!") )
  #add( Card_Spell(2, "Stomp", desc="Deal 2 damage to all enemies.") )
  #add( Card_Spell(2, "Ysera Awakens", desc="Deal 5 damage to all characters except Ysera.") )
  #add( Card_Spell(3, "Flame Burst", desc="Shoot 5 missiles at random enemies for 1 damage each.") )
  #add( Card_Spell(3, "Hogger SMASH!", desc="Deal 4 damage.") )
  #add( Card_Spell(3, "Legacy of the Emperor", desc="Give your minions +2/+2. (+2 Attack/+2 Health)") )
  #add( Card_Spell(3, "Will of Mukla", desc="Restore 8 Health.") )
  #add( Card_Spell(6, "Metamorphosis", desc="Do something crazy.") )
  #add( Card_Spell(1, "Blessing of Might", desc="Give a minion +3 Attack.", cls="paladin") )
  #add( Card_Spell(1, "Blessing of Wisdom", desc="Choose a minion.  Whenever it attacks, draw a card.", cls="paladin") )
  #add( Card_Spell(1, "Eye for an Eye", desc="Secret: When your hero takes damage, deal that much damage to the enemy hero.", cls="paladin") )
  #add( Card_Spell(1, "Hand of Protection", desc="Give a minion Divine Shield.", cls="paladin") )
  #add( Card_Spell(1, "Humility", desc="Change a minion's Attack to 1.", cls="paladin") )
  #add( Card_Spell(1, "Noble Sacrifice", desc="Secret: When an enemy attacks, summon a 2/1 Defender as the new target.", cls="paladin") )
  #add( Card_Spell(1, "Redemption", desc="Secret: When one of your minions dies, return it to life with 1 Health.", cls="paladin") )
  #add( Card_Spell(1, "Repentance", desc="Secret: When your opponent plays a minion, reduce its Health to 1.", cls="paladin") )
  #add( Card_Spell(2, "Equality", desc="Change the Health of ALL minions to 1.", cls="paladin") )
  #add( Card_Spell(2, "Holy Light", desc="Restore #6 Health.", cls="paladin") )
  #add( Card_Spell(3, "Divine Favor", desc="Draw cards until you have as many in hand as your opponent.", cls="paladin") )
  #add( Card_Spell(4, "Blessing of Kings", desc="Give a minion +4/+4. (+4 Attack/+4 Health)", cls="paladin") )
  #add( Card_Spell(4, "Consecration", desc="Deal 2 damage to all enemies.", cls="paladin") )
  #add( Card_Spell(4, "Hammer of Wrath", desc="Deal 3 damage.  Draw a card.", cls="paladin") )
  #add( Card_Spell(5, "Blessed Champion", desc="Double a minion's Attack.", cls="paladin") )
  #add( Card_Spell(5, "Holy Wrath", desc="Draw a card and deal damage equal to its cost.", cls="paladin") )
  #add( Card_Spell(6, "Avenging Wrath", desc="Deal 8 damage randomly split among enemy characters.", cls="paladin") )
  #add( Card_Spell(8, "Lay on Hands", desc="Restore #8 Health. Draw 3 cards.", cls="paladin") )
  #add( Card_Spell(0, "Circle of Healing", desc="Restore #4 Health to ALL minions.", cls="priest") )
  #add( Card_Spell(0, "Silence", desc="Silence a minion.", cls="priest") )
  #add( Card_Spell(1, "Holy Smite", desc="Deal 2 damage.", cls="priest") )
  #add( Card_Spell(1, "Inner Fire", desc="Change a minion's Attack to be equal to its Health.", cls="priest") )
  #add( Card_Spell(1, "Mind Vision", desc="Put a copy of a random card in your opponent's hand into your hand.", cls="priest") )
  #add( Card_Spell(1, "Power Word: Shield", desc="Give a minion +2 Health. Draw a card.", cls="priest") )
  #add( Card_Spell(10, "Mind Control", desc="Take control of an enemy minion.", cls="priest") )
  #add( Card_Spell(2, "Divine Spirit", desc="Double a minion's Health.", cls="priest") )
  #add( Card_Spell(2, "Mind Blast", desc="Deal 5 damage to the enemy hero.", cls="priest") )
  #add( Card_Spell(2, "Shadow Word: Pain", desc="Destroy a minion with 3 or less Attack.", cls="priest") )
  #add( Card_Spell(3, "Shadow Word: Death", desc="Destroy a minion with an Attack of 5 or more.", cls="priest") )
  #add( Card_Spell(3, "Shadowform", desc="Your Hero Power becomes 'Deal 2 damage'. If already in Shadowform: 3 damage.", cls="priest") )
  #add( Card_Spell(3, "Thoughtsteal", desc="Copy 2 cards from your opponent's deck and put them into your hand.", cls="priest") )
  #add( Card_Spell(4, "Mass Dispel", desc="Silence all enemy minions. Draw a card.", cls="priest") )
  #add( Card_Spell(4, "Mindgames", desc="Put a copy of a random minion from your opponent's deck into the battlefield.", cls="priest") )
  #add( Card_Spell(4, "Shadow Madness", desc="Gain control of an enemy minion with 3 or less Attack until end of turn.", cls="priest") )
  #add( Card_Spell(5, "Holy Nova", desc="Deal 2 damage to all enemies.  Restore #2 Health to all  friendly characters.", cls="priest") )
  #add( Card_Spell(6, "Holy Fire", desc="Deal 5 damage.  Restore 5 Health to your hero.", cls="priest") )
  #add( Card_Spell(0, "Backstab", desc="Deal 2 damage to an undamaged minion.", cls="rogue") )
  #add( Card_Spell(0, "Preparation", desc="The next spell you cast this turn costs (3) less.", cls="rogue") )
  #add( Card_Spell(0, "Shadowstep", desc="Return a friendly minion to your hand. It costs (2) less.", cls="rogue") )
  #add( Card_Spell(1, "Cold Blood", desc="Give a minion +2 Attack. Combo: +4 Attack instead.", cls="rogue") )
  #add( Card_Spell(1, "Conceal", desc="Give your minions Stealth until your next turn.", cls="rogue") )
  #add( Card_Spell(1, "Deadly Poison", desc="Give your weapon +2 Attack.", cls="rogue") )
  #add( Card_Spell(1, "Sinister Strike", desc="Deal 3 damage to the enemy hero.", cls="rogue") )
  #add( Card_Spell(2, "Betrayal", desc="An enemy minion deals its damage to the minions next to it.", cls="rogue") )
  #add( Card_Spell(2, "Blade Flurry", desc="Destroy your weapon and deal its damage to all enemies.", cls="rogue") )
  #add( Card_Spell(2, "Eviscerate", desc="Deal 2 damage. Combo: Deal 4 damage instead.", cls="rogue") )
  #add( Card_Spell(2, "Sap", desc="Return an enemy minion to its owner's hand.", cls="rogue") )
  #add( Card_Spell(2, "Shiv", desc="Deal 1 damage. Draw a card.", cls="rogue") )
  #add( Card_Spell(3, "Fan of Knives", desc="Deal 1 damage to all enemy minions. Draw a card.", cls="rogue") )
  #add( Card_Spell(3, "Headcrack", desc="Deal 2 damage to the enemy hero. Combo: Return this to your hand next turn.", cls="rogue") )
  #add( Card_Spell(5, "Assassinate", desc="Destroy an enemy minion.", cls="rogue") )
  #add( Card_Spell(6, "Vanish", desc="Return all minions to their owner's hand.", cls="rogue") )
  #add( Card_Spell(7, "Sprint", desc="Draw 4 cards.", cls="rogue") )
  #add( Card_Spell(0, "Ancestral Healing", desc="Restore a minion to full Health and give it Taunt.", cls="shaman") )
  #add( Card_Spell(0, "Totemic Might", desc="Give your Totems +2 Health.", cls="shaman") )
  #add( Card_Spell(1, "Earth Shock", desc="Silence a minion, then deal 1 damage to it.", cls="shaman") )
  #add( Card_Spell(1, "Forked Lightning", desc="Deal 2 damage to 2 random enemy minions. Overload: (2)", cls="shaman") )
  #add( Card_Spell(1, "Frost Shock", desc="Deal 1 damage to an enemy character and Freeze it.", cls="shaman") )
  #add( Card_Spell(1, "Lightning Bolt", desc="Deal 3 damage. Overload: (1)", cls="shaman") )
  #add( Card_Spell(1, "Rockbiter Weapon", desc="Give a friendly character +3 Attack this turn.", cls="shaman") )
  #add( Card_Spell(2, "Ancestral Spirit", desc="Choose a minion. When that minion is destroyed, return it to the battlefield.", cls="shaman") )
  #add( Card_Spell(2, "Windfury", desc="Give a minion Windfury.", cls="shaman") )
  #add( Card_Spell(3, "Far Sight", desc="Draw a card. That card costs (3) less.", cls="shaman") )
  #add( Card_Spell(3, "Feral Spirit", desc="Summon two 2/3 Spirit Wolves with Taunt. Overload: (2)", cls="shaman") )
  #add( Card_Spell(3, "Hex", desc="Transform a minion into a 0/1 Frog with Taunt.", cls="shaman") )
  #add( Card_Spell(3, "Lava Burst", desc="Deal 5 damage. Overload: (2)", cls="shaman") )
  #add( Card_Spell(3, "Lightning Storm", desc="Deal 2-3 damage to all enemy minions. Overload: (2)", cls="shaman") )
  #add( Card_Spell(5, "Bloodlust", desc="Give your minions +3 Attack this turn.", cls="shaman") )
  #add( Card_Spell(0, "Sacrificial Pact", desc="Destroy a Demon. Restore #5 Health to your hero.", cls="warlock") )
  #add( Card_Spell(0, "Soulfire", desc="Deal 4 damage. Discard a random card.", cls="warlock") )
  #add( Card_Spell(1, "Corruption", desc="Choose an enemy minion.   At the start of your turn, destroy it.", cls="warlock") )
  #add( Card_Spell(1, "Mortal Coil", desc="Deal 1 damage to a minion. If that kills it, draw a card.", cls="warlock") )
  
  add( Card_Spell(1,"Power Overwhleming", name_fr="Puissance accablante",
       actions = lambda self: [Msg_BindEffect(self.caster, self.choices[0], Eff_BuffMinion(4,4)), 
                               Msg_BindEffect(self.caster, self.choices[0], Eff_DieSoon(Msg_EndTurn))], 
       targets='friendly minions', cls="warlock",
       desc="Give a friendly minion +4/+4 until end of turn. Then, it dies. Horribly.",
       desc_fr='Confere +4/+4 a un serviteur allie jusqu\'a la fin du tour. Puis il meurt.') )
  
  #add( Card_Spell(2, "Demonfire", desc="Deal 2 damage to a minion.   If it's a friendly Demon, give it +2/+2 instead.", cls="warlock") )
  #add( Card_Spell(3, "Drain Life", desc="Deal 2 damage. Restore #2 Health to your hero.", cls="warlock") )
  #add( Card_Spell(3, "Sense Demons", desc="Put 2 random Demons from your deck into your hand.", cls="warlock") )
  #add( Card_Spell(3, "Shadow Bolt", desc="Deal 4 damage to a minion.", cls="warlock") )
  #add( Card_Spell(4, "Hellfire", desc="Deal 3 damage to ALL characters.", cls="warlock") )
  #add( Card_Spell(4, "Shadowflame", desc="Destroy a friendly minion and deal its Attack damage to all enemy minions.", cls="warlock") )
  #add( Card_Spell(5, "Bane of Doom", desc="Deal 2 damage to a character.  If that kills it, summon a random Demon.", cls="warlock") )
  #add( Card_Spell(6, "Siphon Soul", desc="Destroy a minion. Restore #3 Health to your hero.", cls="warlock") )
  #add( Card_Spell(8, "Twisting Nether", desc="Destroy all minions.", cls="warlock") )
  #add( Card_Spell(0, "Inner Rage", desc="Deal 1 damage to a minion and give it +2 Attack.", cls="warrior") )
  #add( Card_Spell(1, "Execute", desc="Destroy a damaged enemy minion.", cls="warrior") )
  #add( Card_Spell(1, "Shield Slam", desc="Deal 1 damage to a minion for each Armor you have.", cls="warrior") )
  #add( Card_Spell(1, "Upgrade!", desc="If you have a weapon, give it +1/+1.  Otherwise equip a 1/3 weapon.", cls="warrior") )
  #add( Card_Spell(1, "Whirlwind", desc="Deal 1 damage to ALL minions.", cls="warrior") )
  #add( Card_Spell(2, "Battle Rage", desc="Draw a card for each damaged friendly character.", cls="warrior") )
  #add( Card_Spell(2, "Cleave", desc="Deal 2 damage to two random enemy minions.", cls="warrior") )
  #add( Card_Spell(2, "Commanding Shout", desc="Your minions can't be reduced below 1 Health this turn.  Draw a card.", cls="warrior") )
  #add( Card_Spell(2, "Heroic Strike", desc="Give your hero +4 Attack this turn.", cls="warrior") )
  #add( Card_Spell(2, "Rampage", desc="Give a damaged minion +3/+3.", cls="warrior") )
  #add( Card_Spell(2, "Slam", desc="Deal 2 damage to a minion.  If it survives, draw a card.", cls="warrior") )
  #add( Card_Spell(3, "Charge", desc="Give a friendly minion +2 Attack and Charge.", cls="warrior") )
  #add( Card_Spell(3, "Shield Block", desc="Gain 5 Armor.  Draw a card.", cls="warrior") )
  #add( Card_Spell(4, "Mortal Strike", desc="Deal 4 damage.  If your hero has 12 or less Health, deal 6 damage instead.", cls="warrior") )
  #add( Card_Spell(5, "Brawl", desc="Destroy all minions except one.  (chosen randomly)", cls="warrior") )

  ### Weapons ############################  
  
  #add( Card_Weapon(3, 3, 2, "Eaglehorn Bow", desc="Whenever a Secret is revealed, gain +1 Durability.", cls="hunter") )
  #add( Card_Weapon(7, 5, 2, "Gladiator's Longbow", desc="Your hero is Immune while attacking.", cls="hunter") )
  #add( Card_Weapon(2, 2, 2, "Warglaive of Azzinoth") )
  #add( Card_Weapon(6, 4, 2, "Dual Warglaives") )
  #add( Card_Weapon(1, 1, 4, "Light's Justice", cls="paladin") )
  #add( Card_Weapon(3, 1, 5, "Sword of Justice", desc="Whenever you summon a minion, give it +1/+1 and this loses 1 Durability.", cls="paladin") )
  #add( Card_Weapon(4, 4, 2, "Truesilver Champion", desc="Whenever your hero attacks, restore 2 Health to it.", cls="paladin") )
  #add( Card_Weapon(5, 5, 3, "Ashbringer", cls="paladin") )
  add( Card_Weapon(1, 1, 2, "Wicked Knife", cls="rogue") )
  #add( Card_Weapon(3, 2, 2, "Perdition's Blade", desc="Battlecry: Deal 1 damage. Combo: Deal 2 instead.", cls="rogue") )
  #add( Card_Weapon(5, 3, 4, "Assassin's Blade", cls="rogue") )
  #add( Card_Weapon(2, 2, 3, "Stormforged Axe", desc="Overload: (1)", cls="shaman") )
  #add( Card_Weapon(5, 2, 8, "Doomhammer", desc="Windfury, Overload: (2)", cls="shaman") )
  #add( Card_Weapon(3, 3, 8, "Blood Fury", cls="warlock") )
  #add( Card_Weapon(1, 2, 2, "Battle Axe", cls="warrior") )
  #add( Card_Weapon(1, 1, 3, "Heavy Axe", cls="warrior") )
  #add( Card_Weapon(2, 3, 2, "Fiery War Axe", cls="warrior") )
  #add( Card_Weapon(5, 5, 2, "Arcanite Reaper", cls="warrior") )
  #add( Card_Weapon(7, 7, 1, "Gorehowl", desc="Attacking a minion costs 1 Attack instead of 1 Durability.", cls="warrior") )
  
  return cardbook


if __name__=='__main__':
  import sys
  args = sys.argv[1:]
  
  db = args and args.pop(0) or 'cards_database.csv'
  
  import csv
  f = open(db)
  list_cards = []
  for line in csv.reader(f, delimiter=',', quotechar='"'):
    if line[0]=='id': continue  # first line
    _,name,desc,url,cls,type,_,cat,_,mana,atq,hp,_,objs = line
    list_cards.append((type,cls,mana,name,cls,desc,cat,atq,hp,objs))
  list_cards.sort()
  for card in list_cards:
    (type,cls,mana,name,cls,desc,cat,atq,hp,objs) = card
    desc = desc.replace("\xe2\x80\x99","'")
    if type=="minion":
      effects = ''
      bc = ''
      if objs:
        neff = objs.count("Object")
        if neff:
          effects = [e[:e.find("</b>")] for e in desc.split("<b>") if e]
          effects = " ".join(effects[:neff]).lower()
          if 'battlecry' in effects: 
            bc = '_BC'
            effects = effects.replace("battlecry","").strip()
      print '  #add( Card_Minion%s(%s, %s, %s, "%s", desc="%s"%s%s%s) )' % (bc, mana, atq, hp, name, desc,
             '' if cls=="neutral" else ', cls="%s"'%cls, '' if cat=="none" else ', cat="%s"'%cat,
             effects and ', effects="%s"'%effects or '')
    elif type=="weapon":
      print '  #add( Card_Weapon(%s, %s, %s, "%s", desc="%s"%s) )' % (mana, atq, hp, name, desc,
             '' if cls=="neutral" else ', cls="%s"'%cls )
    elif type=="spell":
      print '  #add( Card_Spell(%s, "%s", desc="%s"%s) )' % (mana, name, desc,
             '' if cls=="neutral" else ', cls="%s"'%cls )
    elif type=="hero":
      print '  #add( Card_Hero(%s, 0, %s, "%s", desc="%s"%s) )' % (mana, hp, name, desc,
             '' if cls=="neutral" else ', cls="%s"'%cls )
    elif type=="ability":
      print '  #add( Card_HeroAbility(%s, "%s", desc="%s"%s) )' % (mana, name, desc,
             '' if cls=="neutral" else ', cls="%s"'%cls )
    else:
        assert False, "error: unknown card type "+type


