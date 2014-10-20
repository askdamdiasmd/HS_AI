"""
full set of hearthstone cards
"""
from effects import *
from creatures import Minion, Weapon
from heroes import Card_Hero, Card_HeroAbility


def get_cardbook():
  cardbook = {}
  def add( card ):
    name = card.name
    if name in cardbook:
      name = "%s-%d-%d-%d" %(name,card.cost,card.atq,card.hp)
    assert name not in cardbook, "error: %s already in cardbook"%card.name
    cardbook[name] = cardbook[name.lower()] = card
    if card.name_fr:
      name_fr = card.name_fr
      if name_fr in cardbook:
        name_fr = "%s-%d-%d-%d" %(name_fr,card.cost,card.atq,card.hp)
      assert name_fr not in cardbook, "error: %s already in cardbook"%card.name
      cardbook[name_fr] = cardbook[name_fr.lower()] = card


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
        targets="characters", name_fr="Boule de feu", 
        desc="Deal 1 damage.", cls="mage") )

  add( Card_HeroAbility(2, "Reinforce", 
        lambda self: [Msg_AddMinion(self.caster, 
                Minion(cardbook["Silver Hand Recruit"],owner=self.caster), 
                Slot(self.caster, fpos="right"))],
        desc="Summon a 1/1 Silver Hand Recruit.", cls="paladin") )

  add( Card_HeroAbility(2, "Lesser Heal",
        lambda self: [Msg_HeroHeal(self.caster,self.choices[0],2)],
        targets="characters", name_fr="Soins inferieurs", 
        desc="Restore 2 Health.", cls="priest") )
        
  add( Card_HeroAbility(2, "Mind Spike", 
        lambda self: [Msg_HeroDamage(self.caster,self.choices[0],1)], 
        targets="characters", name_fr="Pointe mentale", 
        desc="Deal 2 damage.", desc_fr="Inflige 2 points de degats", cls="priest") )
  
  add( Card_HeroAbility(2, "Dagger Mastery", 
        lambda self: [Msg_AddWeapon(self.caster,Weapon(cardbook["Wicked Knife"],owner=self.caster))],
        desc="Equip a 1/2 Dagger.", cls="rogue") )
  
  #add( Card_HeroAbility(2, "Totemic Call", name_fr="", 
        #desc="Summon a random Totem.", cls="shaman") )
  
  add( Card_HeroAbility(2, "INFERNO!", 
        lambda self: [Msg_AddMinion(self.caster, 
                Minion(cardbook["Infernal"],owner=self.caster), 
                Slot(self.caster, fpos="right"))],
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
  add( Card_Minion(1, 2, 2, "Treant", name_fr="Treant", cls="druid", collectible=False, 
        desc="Charge.  At the end of the turn, destroy this minion.", effects="charge") )
  #add( Card_Minion(1, 2, 2, "Treant", name_fr="", 
        #desc="Taunt", cls="druid", effects="taunt") )
  add( Card_Minion(2, 3, 2, "Panther", name_fr="Panthere", cls="druid", cat="beast", collectible=False) )
  #add( Card_Minion(4, 2, 4, "Keeper of the Grove", name_fr="Gardien du bosquet", 
        #desc="Choose One - Deal 2 damage; or Silence a minion.", cls="druid") )
  add( Card_Minion(5, 4, 4, "Druid of the Claw", name_fr="Druide de la griffe", 
         desc="Charge", cls="druid", effects="charge", collectible=False) )
  add( Card_Minion(5, 4, 6, "Druid of the Claw", name_fr="Druide de la griffe", 
         desc="Taunt", cls="druid", effects="taunt", collectible=False) )
  #add( Card_Minion(5, 4, 4, "Druid of the Claw", name_fr="", 
        #desc="Choose One - Charge; or +2 Health and Taunt.", cls="druid") )
  #add( Card_Minion(7, 5, 5, "Ancient of Lore", name_fr="", 
        #desc="Choose One - Draw 2 cards; or Restore 5 Health.", cls="druid") )
  #add( Card_Minion(7, 5, 5, "Ancient of War", name_fr="", 
        #desc="Choose One - Taunt and +5 Health; or +5 Attack.", cls="druid") )
  add( Card_Minion(8, 8, 8, "Ironbark Protector", name_fr="Protecteur Ecorcefer", 
        desc="Taunt", cls="druid", effects="taunt") )
  #add( Card_Minion(9, 5, 8, "Cenarius",  cls="druid",
        #desc="Choose One - Give your other minions +2/+2; or Summon two 2/2 Treants with Taunt."))
  
  add( Card_Minion(0, 1, 1, "Snake", name_fr="Serpent", cls="hunter", cat="beast", collectible=False) )
  add( Card_Minion(1, 1, 1, "Timber Wolf", name_fr="Loup des bois", cls="hunter", cat="beast", 
        effects=[Eff_BuffFriends(1,0,cat="beast")],
        desc="Your other Beasts have +1 Attack.", desc_fr="Vos autres betes ont +1 ATQ") )
  
  add( Card_Minion(2, 2, 2, "Hyena", name_fr="Hyene", 
        cls="hunter", cat="beast", collectible=False) )
  
  add( Card_Minion(2, 2, 2, "Scavenging Hyena", name_fr="Hyene charognarde", 
        effects=['effect',Eff_Trigger(Msg_DeadMinion,
        lambda self,msg: msg.caster.owner is self.owner.owner and msg.caster.card.cat=="beast",
        lambda self,msg: Msg_BindEffect(msg.caster,self.owner,Eff_BuffMinion(2,1)), immediate=0)],
        desc="Whenever a friendly Beast dies, gain +2/+1.", cls="hunter", cat="beast") )
  
  add( Card_Minion(5, 3, 2, "Starving Buzzard", name_fr="Busard affame", 
        effects=['effect',Eff_DrawCard(Msg_DeadMinion,
        lambda self,msg: msg.caster.owner is self.owner.owner and msg.caster.card.cat=="beast",
        immediate=False)],
        desc="Whenever you summon a Beast, draw a card.", cls="hunter", cat="beast") )
  
  add( Card_Minion(3, 4, 2, "Huffer", name_fr="Souffleur", collectible=False, 
        desc="Charge", cls="hunter", cat="beast", effects="charge") )
  
  add( Card_Minion(3, 2, 4, "Leokk", name_fr="Leokk", 
        effects=[Eff_BuffFriends(1,0)], collectible=False,
        desc="Other friendly minions have +1 Attack.", cls="hunter", cat="beast") )

  add( Card_Minion(3, 4, 4, "Misha", name_fr="Misha", collectible=False, 
        desc="Taunt", cls="hunter", cat="beast", effects="taunt") )

  add( Card_Minion_BC(4, 4, 3, "Houndmaster", Eff_BuffMinion(2,2,others="taunt"), 
        "friendly beasts", name_fr="Maitre des chiens", 
        desc="Battlecry: Give a friendly Beast +2/+2 and Taunt.", cls="hunter") )
  
  add( Card_Minion(5, 2, 5, "Tundra Rhino", name_fr="Rhino de la toundra", effects=['charge', 
        Eff_GiveCharge(lambda self,minion: minion.card.cat=='beast',retroactive=True)],
        desc="Your Beasts have Charge.", cls="hunter", cat="beast") )

  add( Card_Minion(6, 6, 5, "Savannah Highmane", name_fr="Grande Criniere des Savanes", 
        effects=[Eff_DR_Invoke_Minion(cardbook["Hyena"]),
                Eff_DR_Invoke_Minion(cardbook["Hyena"])],
        desc="Deathrattle Summon two 2/2 Hyenas.", cls="hunter", cat='beast',
        desc_fr="Rale d'agonie: Invoque 2 hyenes 2/2") )

  add( Card_Minion(9, 8, 8, "King Krush", name_fr="Roi Krush", desc="Charge", cls="hunter",
        cat="beast", effects="charge") )
  
  add( Card_Minion(0, 0, 2, "Mirror Image", name_fr="Image mirroir", 
        desc="Taunt", cls="mage", effects="taunt") )
  
  add( Card_Minion(0, 1, 3, "Spellbender", name_fr="Courbe-sort", cls="mage") )
  
  add( Card_Minion(1, 1, 3, "Mana Wyrm", name_fr="Wyrm de mana",
        effects=['effect',Eff_Trigger(Msg_StartSpell,
        lambda self,msg: self.owner.owner is msg.caster, 
        lambda self,msg: Msg_BindEffect(msg.caster,self.owner,Eff_BuffMinion(1,0)), immediate=0)],
        desc="Whenever you cast a spell, gain +1 Attack.", cls="mage") )
  
  add( Card_Minion(2, 3, 2, "Sorcerer's Apprentice", name_fr="Apprentie du sorcier",
        effects=['effect',Acf_SpellCost(-1)],
        desc="Your spells cost (1) less.", cls="mage") )
  
  #add( Card_Minion_BC(3, 4, 3, "Kirin Tor Mage", name_fr="", 
        #desc="Battlecry: The next Secret you play this turn costs (0).", cls="mage") )
  
  add( Card_Minion(4, 3, 3, "Ethereal Arcanist", name_fr="Arcaniste etherien",
        effects=['effect',Eff_Trigger(Msg_EndTurn, lambda self,msg: msg.caster is self.owner.owner,
        lambda self,msg: Msg_BindEffect(msg.caster,self.owner,Eff_BuffMinion(2,2)))],
        desc="If you control a Secret at the end of your turn, gain +2/+2.", cls="mage") )
  
  add( Card_Minion(4, 3, 6, "Water Elemental", name_fr="Elementaire d'eau", 
        effects=[Eff_Trigger(Msg_StartAttack, lambda self,msg: self.owner is msg.caster,
        lambda self,msg: Msg_BindEffect(self.owner,msg.target,Eff_BuffMinion(others='frozen'))),
        Eff_Trigger(Msg_StartAttack, lambda self,msg: self.owner is msg.target,
        lambda self,msg: Msg_BindEffect(self.owner,msg.caster,Eff_BuffMinion(others='frozen')))],
        desc="Freeze any character damaged by this minion.", cls="mage") )
  
  add( Card_Minion(7, 5, 7, "Archmage Antonidas", name_fr="Archimage Antonidas", 
        effects=['effect',Eff_Trigger(Msg_StartSpell,
        lambda self,msg: msg.caster is self.owner.owner,
        lambda self,msg: Msg_GiveCard(msg.caster,cardbook["Fireball"],origin=self.owner))],
        desc="Whenever you cast a spell, put a 'Fireball' spell into your hand.", cls="mage") )
  
  add( Card_Minion(0, 0, 2, "Barrel", name_fr="Tonneau", 
         desc="Is something in this barrel?", collectible=False) )
  
  add( Card_Minion(0, 1, 1, "Chicken", name_fr="Poulet", 
        desc="Hey Chicken!", cat="beast", collectible=False) )
  
  add( Card_Minion(0, 0, 1, "Frog", name_fr="Grenouille", 
        cat="beast", effects="taunt", collectible=False) )
  
  add( Card_Minion(0, 1, 1, "Murloc Scout", name_fr="Eclaireur Murloc", 
        cat="murloc", collectible=False) )
  
  add( Card_Minion(0, 1, 1, "Sheep", cat="beast", name_fr="Mouton", collectible=False) )
  
  add( Card_Minion(0, 1, 1, "Violet Apprentice", name_fr="Apprenti pourpre", collectible=False) )
  
  add( Card_Minion(0, 1, 1, "Wisp", name_fr="Feu follet") )

  add( Card_Minion_BC(1, 2, 1, "Abusive Sergeant", Eff_BuffMinion(2,0,True), "minions",
        name_fr="Sergent Grossier", 
        desc="Battlecry: Give a friendly minion +2 Attack this turn.",
        desc_fr="Cri de guerre: confere +2 ATQ a un serviteur pendant ce tour") )
  
  #add( Card_Minion(1, 1, 1, "Angry Chicken", name_fr="Poulet furieux", 
        #desc="Enrage: +5 Attack.", cat="beast", effects="enrage") )
  
  add( Card_Minion(1, 1, 1, "Argent Squire", name_fr="Ecuyere d'argent", 
        desc="Divine Shield", effects="divine_shield") )
  
  add( Card_Minion_BC(1, 1, 2, "Bloodsail Corsair", 
        Eff_Weapon(lambda self: Msg_Damage(self.owner,self.weapon,1)), "prespecified",
        name_fr="Forban de la voile sanglante", 
        desc="Battlecry: Remove 1 Durability from your opponent's weapon.", cat="pirate") )
  
  add( Card_Minion(1, 1, 1, "Boar", name_fr="Sanglier", cat="beast", collectible=False) )
  
  add( Card_Minion(1, 1, 1, "Crazed Hunter", name_fr="Chasseur fou", collectible=False) )
  
  #add( Card_Minion_BC(1, 1, 2, "Crazy Monkey", name_fr="Singe cingle", 
  #      desc="Battlecry: Throw Bananas.", collectible=False) )
  
  add( Card_Minion(1, 2, 1, "Damaged Golem", name_fr="Golem endommage", collectible=False) )
  
  add( Card_Minion_BC(1, 1, 1, "Elven Archer", 
        Eff_Message(lambda self: Msg_Damage(self.caster,self.owner,1)), "characters", 
        name_fr="Archere elfe", desc="Battlecry: Deal 1 damage.") )
        
  #add( Card_Minion(1, 0, 4, "Emboldener 3000", name_fr="", collectible=False, 
        #desc="At the end of your turn, give a random minion +1/+1.") )
  
  add( Card_Minion(1, 2, 1, "Flame of Azzinoth", collectible=False) )

  add( Card_Minion(1, 1, 1, "Gnoll", collectible=False) )

  add( Card_Minion(1, 1, 2, 'Goldshire Footman',
        name_fr='Soldat de Comte de l\'Or', effects='taunt') )

  #add( Card_Minion(1, 1, 1, "Grimscale Oracle", name_fr="", 
        #desc="ALL other Murlocs have +1 Attack.", cat="murloc") )
  
  #add( Card_Minion(1, 0, 1, "Homing Chicken", name_fr="", 
        #desc="At the start of your turn, destroy this minion and draw 3 cards.") )
  
  #add( Card_Minion_BC(1, 1, 2, "Hungry Crab", name_fr="Crabe affame", 
        #desc="Battlecry: Destroy a Murloc and gain +2/+2.", cat="beast") )

  add( Card_Minion(1, 1, 1, "Imp", cat="demon", collectible=False) )

  add( Card_Minion(1, 2, 1, "Leper Gnome", name_fr="Gnome lepreux", 
        effects=[Eff_DeathRattle(lambda self,pos: Msg_Damage(self.owner,
        self.engine.board.get_enemy_hero(self.owner.owner),2))],
        desc="Deathrattle Deal 2 damage to the enemy hero.") )

  add( Card_Minion(1, 1, 2, "Lightwarden", name_fr="Gardelumiere", 
        effects=['effect',Eff_Trigger(Msg_Heal,lambda self, msg:True, lambda self,msg: 
        Msg_BindEffect(self.owner,self.owner,Eff_BuffMinion(atq=2)))],
        desc="Whenever a character is healed, gain +2 Attack.") )
  
  add( Card_Minion(1, 2, 1, "Mechanical Dragonling", name_fr="Petit Dragon mecanique", collectible=False) )
  
  add( Card_Minion(1, 2, 1, 'Murloc Raider',name_fr='Ecumeur murloc', cat="murloc") )

  #add( Card_Minion(1, 1, 2, "Murloc Tidecaller", name_fr="", 
        #desc="Whenever a Murloc is summoned, gain +1 Attack.", cat="murloc") )
  
  add( Card_Minion(1, 1, 1, "Naga Myrmidon", collectible=False) )

  add( Card_Minion(1, 1, 1, "Pandaren Scout", collectible=False) )

  #add( Card_Minion(1, 0, 3, "Poultryizer", name_fr="", 
        #desc="At the start of your turn, transform a random minion into a 1/1 Chicken.") )

  #add( Card_Minion(1, 0, 3, "Repair Bot", name_fr="", 
        #desc="At the end of your turn, restore 6 Health to a damaged character.") )

#  add( Card_Minion(1, 1, 2, "Secretkeeper", name_fr="", 
#        effects=['effect',Eff_Trigger(Act_Play,lambda self, msg:True, lambda self,msg: 
#        Msg_BindEffect(self.owner,self.owner,Eff_BuffMinion(atq=2)))],
#        desc="Whenever a Secret is played, gain +1/+1.") )

  add( Card_Minion(1, 0, 4, "Shieldbearer", name_fr="Porte bouclier", 
        desc="Taunt", effects="taunt") )
  
  add( Card_Minion(1, 1, 1, "Skeleton", collectible=False) )
  
  #add( Card_Minion(1, 2, 1, "Southsea Deckhand", name_fr="", 
        #desc="Has Charge while you have a weapon equipped.", cat="pirate") )
  
  add( Card_Minion(1, 2, 2, "Squire", name_fr="Ecuyer", collectible=False) )
  
  add( Card_Minion(1, 1, 1, "Squirrel", cat="beast", name_fr="Ecureuil", collectible=False) )
  
  add( Card_Minion(1, 1, 1, "Stonetusk Boar", name_fr="Sanglier brocheroc", 
        desc="Charge", cat="beast", effects="charge") )
  
  add( Card_Minion_BC(1, 2, 1, "Voodoo Doctor", 
        Eff_Message(lambda self: Msg_Heal(self.caster,self.owner,2)), "characters", 
        name_fr="Docteur Vaudou", desc="Battlecry: Restore 2 Health.") )
  
  add( Card_Minion(1, 1, 1, "Whelp", name_fr="Dragonnet", cat="dragon", collectible=False) )
  
  add( Card_Minion(1, 2, 1, "Worgen Infiltrator", name_fr="Infiltrateur Worgen", 
        desc="Stealth", effects="stealth") )
        
  add( Card_Minion(1, 1, 1, "Young Dragonhawk", name_fr="Jeune faucon-dragon", 
        desc="Windfury", cat="beast", effects="windfury") )
  
  #add( Card_Minion(1, 2, 1, "Young Priestess", name_fr="", 
        #desc="At the end of your turn, give another random friendly minion +1 Health.") )
  
  add( Card_Minion_BC(2, 3, 2, "Acidic Swamp Ooze", 
        Eff_Weapon(lambda self: Msg_DeadWeapon(self.weapon),enemy=True), "prespecified",
        name_fr="Limon des marais acides", 
        desc="Battlecry: Destroy your opponent's weapon.") )
  
  #add( Card_Minion(2, 2, 3, "Amani Berserker", name_fr="", 
        #desc="Enrage: +3 Attack", effects="enrage") )
  
  #add( Card_Minion(2, 4, 5, "Ancient Watcher", name_fr="", 
        #desc="Can't Attack.") )
  
  #add( Card_Minion(2, 3, 2, "Bloodfen Raptor", cat="beast") )
  #add( Card_Minion(2, 1, 1, "Bloodmage Thalnos", name_fr="", 
        #desc="Spell Damage +1. Deathrattle Draw a card.", effects="spell damage deathrattle") )
  #add( Card_Minion_BC(2, 2, 3, "Bloodsail Raider", name_fr="", 
        #desc="Battlecry: Gain Attack equal to the Attack of your weapon.", cat="pirate") )
  #add( Card_Minion(2, 2, 1, "Bluegill Warrior", name_fr="", 
        #desc="Charge", cat="murloc", effects="charge") )
  #add( Card_Minion_BC(2, 1, 1, "Captain's Parrot", name_fr="", 
        #desc="Battlecry: Put a random Pirate from your deck into your hand.", cat="beast") )
  #add( Card_Minion_BC(2, 2, 2, "Crazed Alchemist", name_fr="", 
        #desc="Battlecry: Swap the Attack and Health of a minion.") )

  add( Card_Minion(2, 2, 2, "Dire Wolf Alpha", name_fr="Loup alpha redoutable", cat="beast",
        effects=[Eff_BuffLeftRight(1,0)], 
        desc="Adjacent minions have +1 Attack.", desc_fr="Les serviteurs adjacents ont +1 ATQ") )
  
  #add( Card_Minion(2, 0, 7, "Doomsayer", name_fr="", 
        #desc="At the start of your turn, destroy ALL minions.") )

  #add( Card_Minion(2, 3, 2, "Faerie Dragon", name_fr="", 
        #desc="Can't be targeted by Spells or Hero Powers.", cat="dragon") )
  
  #add( Card_Minion(2, 3, 3, "Finkle Einhorn") )
  
  add( Card_Minion(2, 2, 2, "Frostwolf Grunt", name_fr="Grunt loup-de-givre", 
        desc="Taunt", effects="taunt") )
  
  #add( Card_Minion(2, 2, 2, "Gnoll", name_fr="", 
        #desc="Taunt", effects="taunt") )
  
  #add( Card_Minion(2, 1, 3, "Hidden Gnome", name_fr="", 
        #desc="Was hiding in a barrel!") )
  
  add( Card_Minion_BC(2, 2, 1, "Ironbeak Owl", Eff_Silence(), "minions", 
       name_fr="Chouette bec-de-fer", desc="Battlecry: Silence a minion.", cat="beast",
       desc_fr="Cri de guerre: reduit au silence un autre serviteur") )
  
  #add( Card_Minion(2, 3, 2, "Knife Juggler", name_fr="", 
        #desc="After you summon a minion, deal 1 damage to a random enemy.") )

  #add( Card_Minion(2, 2, 2, "Kobold Geomancer", name_fr="", 
        #desc="Spell Damage +1", effects="spell damage") )

  add( Card_Minion(2, 2, 1, "Loot Hoarder", name_fr="Amasseur de butin",
        effects=[Eff_DeathRattle(lambda self,pos: Msg_DrawCard(self.owner.owner))],
        desc="Deathrattle Draw a card.") )

  #add( Card_Minion(2, 0, 4, "Lorewalker Cho", name_fr="", 
        #desc="Whenever a player casts a spell, put a copy into the other player's hand.") )
  #add( Card_Minion_BC(2, 3, 2, "Mad Bomber", name_fr="", 
        #desc="Battlecry: Deal 3 damage randomly split between all other characters.") )
  #add( Card_Minion(2, 1, 3, "Mana Addict", name_fr="", 
        #desc="Whenever you cast a spell, gain +2 Attack this turn.") )
  #add( Card_Minion(2, 2, 2, "Mana Wraith", name_fr="", 
        #desc="ALL minions cost (1) more.") )
  #add( Card_Minion(2, 1, 3, "Master Swordsmith", name_fr="", 
        #desc="At the end of your turn, give another random friendly minion +1 Attack.") )
  #add( Card_Minion_BC(2, 4, 4, "Millhouse Manastorm", name_fr="", 
        #desc="Battlecry: Enemy spells cost (0) next turn.") )
  #add( Card_Minion_BC(2, 2, 1, "Murloc Tidehunter", name_fr="", 
        #desc="Battlecry: Summon a 1/1 Murloc Scout.", cat="murloc") )
  #add( Card_Minion(2, 0, 4, "Nat Pagle", name_fr="", 
        #desc="At the start of your turn, you have a 50% chance to draw an extra card.") )
  #add( Card_Minion_BC(2, 1, 1, "Novice Engineer", name_fr="", 
        #desc="Battlecry: Draw a card.") )
  #add( Card_Minion(2, 2, 2, "Pint-Sized Summoner", name_fr="", 
        #desc="The first minion you play each turn costs (1) less.") )
  
  add( Card_Minion(2, 2, 3, 'River Crocolisk', name_fr='Crocilisque des rivieres',cat='beast') )
  
  #add( Card_Minion(2, 2, 2, "Shado-Pan Monk") )
  
  add( Card_Minion_BC(2, 2, 3, "Sunfury Protector", Eff_BuffMinion(0,0,others='taunt'), 'neighbors',
       name_fr="Protectrice Solfury", desc="Battlecry: Give adjacent minions Taunt.",
       desc_fr="Cri de guerre: confere Provocation aux serviteurs adjacents") )
  
  #add( Card_Minion(2, 3, 2, "Wild Pyromancer", name_fr="", 
        #desc="After you cast a spell, deal 1 damage to ALL minions.") )
  #add( Card_Minion_BC(2, 3, 2, "Youthful Brewmaster", name_fr="", 
        #desc="Battlecry: Return a friendly minion from the battlefield to your hand.") )
  #add( Card_Minion(20, 8, 8, "Molten Giant", name_fr="", 
        #desc="Costs (1) less for each damage your hero has taken.") )
  
  add( Card_Minion(3, 1, 3, "Acolyte of Pain", name_fr="Acolyte de la souffrance", 
       effects=[Eff_DrawCard(Msg_Damage,lambda self,msg: msg.target is self.owner, immediate=True)],
       desc="Whenever this minion takes damage, draw a card.",
       desc_fr="Cri de guerre: Detruit les serviteurs adjacents et gagne leurs pts d'Attaque et de Vie."))
  
  #add( Card_Minion(3, 0, 3, "Alarm-o-Bot", name_fr="", 
        #desc="At the start of your turn, swap this minion with a random one in your hand.") )
  #add( Card_Minion_BC(3, 4, 2, "Arcane Golem", name_fr="", 
        #desc="Charge. Battlecry: Give your opponent a Mana Crystal.", effects="charge") )
  #add( Card_Minion_BC(3, 4, 2, "Big Game Hunter", name_fr="", 
        #desc="Battlecry: Destroy a minion with an Attack of 7 or more.") )
  #add( Card_Minion_BC(3, 3, 3, "Blood Knight", name_fr="", 
        #desc="Battlecry: All minions lose Divine Shield. Gain +3/+3 for each Shield lost.") )
  #add( Card_Minion_BC(3, 2, 2, "Coldlight Oracle", name_fr="", 
        #desc="Battlecry: Each player draws 2 cards.", cat="murloc") )
  #add( Card_Minion_BC(3, 2, 3, "Coldlight Seer", name_fr="", 
        #desc="Battlecry: Give ALL other Murlocs +2 Health.", cat="murloc") )
  #add( Card_Minion(3, 1, 4, "Dalaran Mage", name_fr="", 
        #desc="Spell Damage +1", effects="spell damage") )
  #add( Card_Minion(3, 1, 4, "Demolisher", name_fr="", 
        #desc="At the start of your turn, deal 2 damage to a random enemy.") )
  #add( Card_Minion_BC(3, 3, 3, "Earthen Ring Farseer", name_fr="", 
        #desc="Battlecry: Restore 3 Health.") )
  #add( Card_Minion(3, 2, 3, "Emperor Cobra", name_fr="", 
        #desc="Destroy any minion damaged by this minion.", cat="beast") )
  #add( Card_Minion(3, 2, 3, "Flesheating Ghoul", name_fr="", 
        #desc="Whenever a minion dies, gain +1 Attack.") )
  
  add( Card_Minion(3, 2, 3, "Harvest Golem", 
       effects=[Eff_DR_Invoke_Minion(cardbook["Damaged Golem"])],
       name_fr="Golem des moissons", desc="Deathrattle Summon a 2/1 Damaged Golem.",
       desc_fr="Rale d'agonie: Invoque un golem endommage 2/1", ) )
  
  #add( Card_Minion(3, 1, 5, "Imp Master", name_fr="", 
        #desc="At the end of your turn, deal 1 damage to this minion and summon a 1/1 Imp.") )
  #add( Card_Minion_BC(3, 4, 7, "Injured Blademaster", name_fr="", 
        #desc="Battlecry: Deal 4 damage to HIMSELF.") )
  #add( Card_Minion_BC(3, 2, 2, "Ironforge Rifleman", name_fr="", 
        #desc="Battlecry: Deal 1 damage.") )
  #add( Card_Minion(3, 3, 3, "Ironfur Grizzly", name_fr="", 
        #desc="Taunt", cat="beast", effects="taunt") )
  #add( Card_Minion(3, 4, 2, "Jungle Panther", name_fr="", 
        #desc="Stealth", cat="beast", effects="stealth") )
  #add( Card_Minion_BC(3, 5, 5, "King Mukla", name_fr="", 
        #desc="Battlecry: Give your opponent 2 Bananas.", cat="beast") )
  #add( Card_Minion(3, 3, 5, "Laughing Sister", name_fr="", 
        #desc="Can't be targeted by Spells or Hero Powers.") )
  #add( Card_Minion(3, 5, 1, "Magma Rager") )
  #add( Card_Minion_BC(3, 3, 3, "Mind Control Tech", name_fr="", 
        #desc="Battlecry: If your opponent has 4 or more minions, take control of one at random.") )
  #add( Card_Minion(3, 3, 3, "Murloc Warleader", name_fr="", 
        #desc="ALL other Murlocs have +2/+1.", cat="murloc") )
  #add( Card_Minion(3, 2, 2, "Questing Adventurer", name_fr="", 
        #desc="Whenever you play a card, gain +1/+1.") )
  #add( Card_Minion(3, 3, 3, "Raging Worgen", name_fr="", 
        #desc="Enrage: Windfury and +1 Attack", effects="enrage") )
  #add( Card_Minion(3, 2, 2, "Raid Leader", name_fr="", 
        #desc="Your other minions have +1 Attack.") )
  #add( Card_Minion_BC(3, 2, 3, "Razorfen Hunter", name_fr="", 
        #desc="Battlecry: Summon a 1/1 Boar.") )
  #add( Card_Minion(3, 3, 1, "Scarlet Crusader", name_fr="", 
        #desc="Divine Shield", effects="divine_shield") )

  add( Card_Minion_BC(3, 3, 2, "Shattered Sun Cleric", Eff_BuffMinion(1,1,False), 'friendly minions',
       desc="Battlecry: Give a friendly minion +1/+1.", name_fr="Clerc du Soleil brise",
       desc_fr="Cri de guerre: confere +1/+1 a un serviteur allie") )

  #add( Card_Minion(3, 1, 4, "Silverback Patriarch", name_fr="", 
        #desc="Taunt", cat="beast", effects="taunt") )
  #add( Card_Minion(3, 3, 3, "Southsea Captain", name_fr="", 
        #desc="Your other Pirates have +1/+1.", cat="pirate") )
  #add( Card_Minion(3, 2, 3, "Tauren Warrior", name_fr="", 
        #desc="Taunt. Enrage: +3 Attack", effects="taunt enrage") )
  #add( Card_Minion(3, 2, 3, "Thrallmar Farseer", name_fr="", 
        #desc="Windfury", effects="windfury") )
  #add( Card_Minion_BC(3, 3, 3, "Tinkmaster Overspark", name_fr="", 
        #desc="Battlecry: Transform another random minion into a 5/5 Devilsaur or a 1/1 Squirrel at random.") )
  #add( Card_Minion(3, 3, 1, "Wolfrider", name_fr="", 
        #desc="Charge", effects="charge") )
  #add( Card_Minion_BC(4, 5, 4, "Ancient Brewmaster", name_fr="", 
        #desc="Battlecry: Return a friendly minion from the battlefield to your hand.") )
  #add( Card_Minion_BC(4, 2, 5, "Ancient Mage", name_fr="", 
        #desc="Battlecry: Give adjacent minions Spell Damage +1.") )

  add( Card_Minion(4, 4, 5, "Baine Bloodhoof") )
  
  #add( Card_Minion(4, 4, 4, "Brewmaster") )

  add( Card_Minion(4, 4, 5, 'Chillwind Yeti', name_fr='Yeti Noroit') )
  
  #add( Card_Minion(4, 4, 2, "Cult Master", name_fr="", 
        #desc="Whenever one of your other minions dies, draw a card.") )

  add( Card_Minion_BC(4, 4, 4, "Dark Iron Dwarf", Eff_BuffMinion(2,0,True), "minions",
       name_fr="Nain sombrefer", desc="Battlecry: Give a minion +2 Attack this turn.",
       desc_fr="Cri de guerre: confere +2 ATQ a un serviteur pendant ce tour") )
  
  add( Card_Minion_BC(4, 2, 3, "Defender of Argus", Eff_BuffMinion(1,1,others='taunt'), 'neighbors',
       name_fr="Defenseur d'Argus", desc="Battlecry: Give adjacent minions +1/+1 and Taunt.",
       desc_fr="Cri de guerre: donne aux serviteurs adjacents +1/+1 et Provocation") )
  
  #add( Card_Minion_BC(4, 2, 4, "Dragonling Mechanic", name_fr="", 
        #desc="Battlecry: Summon a 2/1 Mechanical Dragonling.") )
  #add( Card_Minion(4, 3, 3, "Dread Corsair", name_fr="", 
        #desc="Taunt. Costs (1) less per Attack of your weapon.", cat="pirate", effects="taunt") )
  #add( Card_Minion(4, 7, 6, "Emerald Drake", cat="dragon") )
  #add( Card_Minion_BC(4, 2, 4, "Gnomish Inventor", name_fr="", 
        #desc="Battlecry: Draw a card.") )
  #add( Card_Minion_BC(4, 6, 2, "Leeroy Jenkins", name_fr="", 
        #desc="Charge. Battlecry: Summon two 1/1 Whelps for your opponent.", effects="charge") )
  #add( Card_Minion(4, 5, 2, "Massive Gnoll") )
  #add( Card_Minion(4, 1, 7, "Mogu'shan Warden", name_fr="", 
        #desc="Taunt", effects="taunt") )
  #add( Card_Minion(4, 2, 7, "Oasis Snapjaw", cat="beast") )
  #add( Card_Minion(4, 4, 4, "Ogre Magi", name_fr="", 
        #desc="Spell Damage +1", effects="spell damage") )
  #add( Card_Minion(4, 2, 4, "Old Murk-Eye", name_fr="", 
        #desc="Charge. Has +1 Attack for each other Murloc on the battlefield.", cat="murloc", effects="charge") )
  #add( Card_Minion(4, 3, 5, "Sen'jin Shieldmasta", name_fr="", 
        #desc="Taunt", effects="taunt") )
  #add( Card_Minion(4, 3, 3, "Silvermoon Guardian", name_fr="", 
        #desc="Divine Shield", effects="divine_shield") )
  #add( Card_Minion_BC(4, 4, 3, "Spellbreaker", name_fr="", 
        #desc="Battlecry: Silence a minion.") )
  #add( Card_Minion(4, 2, 5, "Stormwind Knight", name_fr="", 
        #desc="Charge", effects="charge") )
  #add( Card_Minion_BC(4, 4, 1, "Twilight Drake", name_fr="", 
        #desc="Battlecry: Gain +1 Health for each card in your hand.", cat="dragon") )
  #add( Card_Minion(4, 3, 5, "Violet Teacher", name_fr="", 
        #desc="Whenever you cast a spell, summon a 1/1 Violet Apprentice.") )
  #add( Card_Minion(5, 4, 4, "Abomination", name_fr="", 
        #desc="Taunt. Deathrattle Deal 2 damage to ALL characters.", effects="taunt deathrattle") )
  #add( Card_Minion_BC(5, 4, 4, "Azure Drake", name_fr="", 
        #desc="Spell Damage +1. Battlecry: Draw a card.", cat="dragon", effects="spell damage") )
  #add( Card_Minion(5, 5, 4, "Booty Bay Bodyguard", name_fr="", 
        #desc="Taunt", effects="taunt") )
  #add( Card_Minion_BC(5, 5, 4, "Captain Greenskin", name_fr="", 
        #desc="Battlecry: Give your weapon +1/+1.", cat="pirate") )
  #add( Card_Minion_BC(5, 4, 5, "Darkscale Healer", name_fr="", 
        #desc="Battlecry: Restore 2 Health to all friendly characters.") )
  #add( Card_Minion(5, 5, 5, "Devilsaur", cat="beast") )
  #add( Card_Minion_BC(5, 3, 3, "Faceless Manipulator", name_fr="", 
        #desc="Battlecry: Choose a minion and become a copy of it.") )
  #add( Card_Minion(5, 3, 6, "Fen Creeper", name_fr="", 
        #desc="Taunt", effects="taunt") )
  #add( Card_Minion_BC(5, 4, 4, "Frostwolf Warlord", name_fr="", 
        #desc="Battlecry: Gain +1/+1 for each other friendly minion on the battlefield.") )
  #add( Card_Minion(5, 4, 4, "Gadgetzan Auctioneer", name_fr="", 
        #desc="Whenever you cast a spell, draw a card.") )
  #add( Card_Minion(5, 2, 7, "Gurubashi Berserker", name_fr="", 
        #desc="Whenever this minion takes damage, gain +3 Attack.") )
  #add( Card_Minion_BC(5, 5, 4, "Harrison Jones", name_fr="", 
        #desc="Battlecry: Destroy your opponent's weapon and draw cards equal to its Durability.") )
  #add( Card_Minion_BC(5, 4, 4, "Nightblade", name_fr="", 
        #desc="Battlecry: Deal 3 damage to the enemy hero.") )
  #add( Card_Minion_BC(5, 4, 4, "Silver Hand Knight", name_fr="", 
        #desc="Battlecry: Summon a 2/2 Squire.") )
  #add( Card_Minion(5, 4, 6, "Spiteful Smith", name_fr="", 
        #desc="Enrage: Your weapon has +2 Attack.", effects="enrage") )
  #add( Card_Minion_BC(5, 3, 5, "Stampeding Kodo", name_fr="", 
        #desc="Battlecry: Destroy a random enemy minion with 2 or less Attack.", cat="beast") )
  #add( Card_Minion_BC(5, 4, 2, "Stormpike Commando", name_fr="", 
        #desc="Battlecry: Deal 2 damage.") )
  #add( Card_Minion(5, 5, 5, "Stranglethorn Tiger", name_fr="", 
        #desc="Stealth", cat="beast", effects="stealth") )
  #add( Card_Minion(5, 7, 6, "Venture Co. Mercenary", name_fr="", 
        #desc="Your minions cost (3) more.") )
  #add( Card_Minion(6, 4, 7, "Archmage", name_fr="", 
        #desc="Spell Damage +1", effects="spell damage") )
  #add( Card_Minion(6, 4, 2, "Argent Commander", name_fr="", 
        #desc="Charge, Divine Shield", effects="charge divine_shield") )
  #add( Card_Minion(6, 6, 7, "Boulderfist Ogre") )
  
  add( Card_Minion(6, 4, 5, "Cairne", effects=[Eff_DR_Invoke_Minion(cardbook["Baine Bloodhoof"])],
       desc="Deathrattle Summon a 4/5 Baine Bloodhoof.",
       desc_fr="Rale d'agonie: Invoque un Baine 4/5" ) )
  
  #add( Card_Minion_BC(6, 5, 5, "Frost Elemental", name_fr="", 
        #desc="Battlecry: Freeze a character.") )
  #add( Card_Minion_BC(6, 6, 6, "Gelbin Mekkatorque", name_fr="", 
        #desc="Battlecry: Summon an AWESOME invention.") )
  #add( Card_Minion(6, 4, 4, "Hogger", name_fr="", 
        #desc="At the end of your turn, summon a 2/2 Gnoll with Taunt.") )
  #add( Card_Minion(6, 7, 5, "Illidan Stormrage", name_fr="", 
        #desc="Whenever you play a card, summon a 2/1 Flame of Azzinoth.", cat="demon") )
  #add( Card_Minion(6, 6, 5, "Lord of the Arena", name_fr="", 
        #desc="Taunt", effects="taunt") )
  #add( Card_Minion(6, 10, 10, "Mukla's Big Brother", name_fr="", 
        #desc="So strong! And only 6 Mana?!") )
  #add( Card_Minion_BC(6, 5, 4, "Priestess of Elune", name_fr="", 
        #desc="Battlecry: Restore 4 Health to your hero.") )
  #add( Card_Minion(6, 5, 2, "Reckless Rocketeer", name_fr="", 
        #desc="Charge", effects="charge") )
  #add( Card_Minion(6, 4, 5, "Sunwalker", name_fr="", 
        #desc="Taunt. Divine Shield", effects="taunt divine_shield") )
  #add( Card_Minion(6, 5, 5, "Sylvanas Windrunner", name_fr="", 
        #desc="Deathrattle Take control of a random enemy minion.", effects="deathrattle") )
  #add( Card_Minion(6, 9, 7, "The Beast", name_fr="", 
        #desc="Deathrattle Summon a 3/3 Finkle Einhorn for your opponent.", cat="beast", effects="deathrattle") )
  #add( Card_Minion_BC(6, 4, 5, "The Black Knight", name_fr="", 
        #desc="Battlecry: Destroy an enemy minion with Taunt.") )
  #add( Card_Minion(6, 4, 5, "Windfury Harpy", name_fr="", 
        #desc="Windfury", effects="windfury") )
  #add( Card_Minion(7, 7, 5, "Baron Geddon", name_fr="", 
        #desc="At the end of your turn, deal 2 damage to ALL other characters.") )
  #add( Card_Minion(7, 9, 5, "Core Hound", cat="beast") )
  #add( Card_Minion(7, 7, 5, "Ravenholdt Assassin", name_fr="", 
        #desc="Stealth", effects="stealth") )
  #add( Card_Minion(7, 6, 6, "Stormwind Champion", name_fr="", 
        #desc="Your other minions have +1/+1.") )
  #add( Card_Minion(7, 7, 7, "War Golem") )
  #add( Card_Minion(8, 7, 7, "Gruul", name_fr="", 
        #desc="At the end of each turn, gain +1/+1 .") )
  #add( Card_Minion(8, 8, 8, "Ragnaros the Firelord", name_fr="", 
        #desc="Can't Attack.  At the end of your turn, deal 8 damage to a random enemy.") )
  #add( Card_Minion_BC(9, 8, 8, "Alexstrasza", name_fr="", 
        #desc="Battlecry: Set a hero's remaining Health to 15.", cat="dragon") )
  #add( Card_Minion(9, 4, 12, "Malygos", name_fr="", 
        #desc="Spell Damage +5", cat="dragon", effects="spell damage") )
  #add( Card_Minion(9, 8, 8, "Nozdormu", name_fr="", 
        #desc="Players only have 15 seconds to take their turns.", cat="dragon") )
  #add( Card_Minion_BC(9, 8, 8, "Onyxia", name_fr="", 
        #desc="Battlecry: Summon 1/1 Whelps until your side of the battlefield is full.", cat="dragon") )
  #add( Card_Minion(9, 4, 12, "Ysera", name_fr="", 
        #desc="At the end of your turn, draw a Dream Card.", cat="dragon") )
  #add( Card_Minion(1, 2, 1, "Defender", cls="paladin") )
  #add( Card_Minion(1, 1, 1, "Silver Hand Recruit", cls="paladin") )
  #add( Card_Minion_BC(2, 2, 2, "Argent Protector", name_fr="", 
        #desc="Battlecry: Give a friendly minion Divine Shield.", cls="paladin") )
  #add( Card_Minion_BC(3, 3, 3, "Aldor Peacekeeper", name_fr="", 
        #desc="Battlecry: Change an enemy minion's Attack to 1.", cls="paladin") )
  #add( Card_Minion_BC(7, 5, 6, "Guardian of Kings", name_fr="", 
        #desc="Battlecry: Restore 6 Health to your hero.", cls="paladin") )
  #add( Card_Minion(8, 6, 6, "Tirion Fordring", name_fr="", 
        #desc="Divine Shield. Taunt. Deathrattle Equip a 5/3 Ashbringer.", cls="paladin", effects="divine_shield taunt deathrattle") )
  #add( Card_Minion(0, 0, 1, "Shadow of Nothing", name_fr="", 
        #desc="Mindgames whiffed! Your opponent had no minions!", cls="priest") )
  #add( Card_Minion(1, 1, 3, "Northshire Cleric", name_fr="", 
        #desc="Whenever a minion is healed, draw a card.", cls="priest") )
  #add( Card_Minion(2, 0, 5, "Lightwell", name_fr="", 
        #desc="At the start of your turn, restore 3 Health to a damaged friendly character.", cls="priest") )
  #add( Card_Minion(4, 3, 5, "Auchenai Soulpriest", name_fr="", 
        #desc="Your cards and powers that restore Health now deal damage instead.", cls="priest") )
  #add( Card_Minion(4, 0, 5, "Lightspawn", name_fr="", 
        #desc="This minion's Attack is always equal to its Health.", cls="priest") )
  #add( Card_Minion_BC(6, 4, 5, "Cabal Shadow Priest", name_fr="", 
        #desc="Battlecry: Take control of an enemy minion that has 2 or less Attack.", cls="priest") )
  #add( Card_Minion_BC(6, 6, 6, "Temple Enforcer", name_fr="", 
        #desc="Battlecry: Give a friendly minion +3 Health.", cls="priest") )
  #add( Card_Minion(7, 7, 7, "Prophet Velen", name_fr="", 
        #desc="Double the damage and healing of your spells and Hero Power.", cls="priest") )
  #add( Card_Minion(1, 2, 1, "Defias Bandit", cls="rogue") )
  #add( Card_Minion(2, 2, 2, "Defias Ringleader", name_fr="", 
        #desc="Combo: Summon a 2/1 Defias Bandit.", cls="rogue", effects="combo") )
  #add( Card_Minion(2, 1, 1, "Patient Assassin", name_fr="", 
        #desc="Stealth. Destroy any minion damaged by this minion.", cls="rogue", effects="stealth") )
  #add( Card_Minion(3, 2, 2, "Edwin VanCleef", name_fr="", 
        #desc="Combo: Gain +2/+2 for each card played earlier this turn.", cls="rogue", effects="combo") )
  #add( Card_Minion(3, 3, 3, "SI:7 Agent", name_fr="", 
        #desc="Combo: Deal 2 damage.", cls="rogue", effects="combo") )
  #add( Card_Minion_BC(4, 4, 4, "Master of Disguise", name_fr="", 
        #desc="Battlecry: Give a friendly minion Stealth.", cls="rogue") )
  #add( Card_Minion(6, 5, 3, "Kidnapper", name_fr="", 
        #desc="Combo: Return a minion to its owner's hand.", cls="rogue", effects="combo") )
  #add( Card_Minion(1, 3, 1, "Dust Devil", name_fr="", 
        #desc="Windfury. Overload: (2)", cls="shaman", effects="windfury overload") )
  #add( Card_Minion(1, 0, 2, "Healing Totem", name_fr="", 
        #desc="At the end of your turn, restore 1 Health to all friendly minions.", cls="shaman", cat="totem") )
  #add( Card_Minion(1, 1, 1, "Searing Totem", cls="shaman", cat="totem") )
  #add( Card_Minion(1, 0, 2, "Stoneclaw Totem", name_fr="", 
        #desc="Taunt", cls="shaman", cat="totem", effects="taunt") )
  #add( Card_Minion(1, 0, 2, "Wrath of Air Totem", name_fr="", 
        #desc="Spell Damage +1", cls="shaman", cat="totem", effects="spell damage") )
  #add( Card_Minion(2, 0, 3, "Flametongue Totem", name_fr="", 
        #desc="Adjacent minions have +2 Attack.", cls="shaman", cat="totem") )
  #add( Card_Minion(2, 2, 3, "Spirit Wolf", name_fr="", 
        #desc="Taunt", cls="shaman", effects="taunt") )
  #add( Card_Minion(3, 0, 3, "Mana Tide Totem", name_fr="", 
        #desc="At the end of your turn, draw a card.", cls="shaman", cat="totem") )
  #add( Card_Minion(3, 2, 4, "Unbound Elemental", name_fr="", 
        #desc="Whenever you play a card with Overload, gain +1/+1.", cls="shaman") )
  #add( Card_Minion_BC(4, 3, 3, "Windspeaker", name_fr="", 
        #desc="Battlecry: Give a friendly minion Windfury.", cls="shaman") )
  #add( Card_Minion(5, 7, 8, "Earth Elemental", name_fr="", 
        #desc="Taunt. Overload: (3)", cls="shaman", effects="taunt overload") )
  #add( Card_Minion_BC(6, 6, 5, "Fire Elemental", name_fr="", 
        #desc="Battlecry: Deal 3 damage.", cls="shaman") )
  #add( Card_Minion(8, 3, 5, "Al'Akir the Windlord", name_fr="", 
        #desc="Windfury, Charge, Divine Shield, Taunt", cls="shaman", effects="windfury charge divine_shield taunt") )
  #add( Card_Minion(1, 0, 1, "Blood Imp", name_fr="", 
        #desc="Stealth. At the end of your turn, give another random friendly minion +1 Health.", cls="warlock", cat="demon", effects="stealth") )
  #add( Card_Minion_BC(1, 3, 2, "Flame Imp", name_fr="", 
        #desc="Battlecry: Deal 3 damage to your hero.", cls="warlock", cat="demon") )
  #add( Card_Minion(1, 1, 3, "Voidwalker", name_fr="", 
        #desc="Taunt", cls="warlock", cat="demon", effects="taunt") )
  #add( Card_Minion(1, 1, 1, "Worthless Imp", name_fr="", 
        #desc="You are out of demons! At least there are always imps...", cls="warlock", cat="demon") )
  #add( Card_Minion_BC(2, 4, 3, "Succubus", name_fr="", 
        #desc="Battlecry: Discard a random card.", cls="warlock", cat="demon") )
  #add( Card_Minion_BC(3, 3, 5, "Felguard", name_fr="", 
        #desc="Taunt. Battlecry: Destroy one of your Mana Crystals.", cls="warlock", cat="demon", effects="taunt") )

  add( Card_Minion_BC(3, 3, 3, "Void Terror", Eff_Absorb(), 'neighbors', 
       desc="Battlecry: Destroy adjacent minions and gain their Attack and Health.", 
       name_fr="Terreur du vide", cls="warlock", cat="demon",
       desc_fr="Cri de guerre: Detruit les serviteurs adjacents et gagne leur ATQ et Vie."))
  
  #add( Card_Minion_BC(4, 5, 6, "Pit Lord", name_fr="", 
        #desc="Battlecry: Deal 5 damage to your hero.", cls="warlock", cat="demon") )
  #add( Card_Minion(4, 0, 4, "Summoning Portal", name_fr="", 
        #desc="Your minions cost (2) less, but not less than (1).", cls="warlock") )
  #add( Card_Minion_BC(5, 5, 7, "Doomguard", name_fr="", 
        #desc="Charge. Battlecry: Discard two random cards.", cls="warlock", cat="demon", effects="charge") )
  #add( Card_Minion_BC(6, 6, 6, "Dread Infernal", name_fr="", 
        #desc="Battlecry: Deal 1 damage to ALL other characters.", cls="warlock", cat="demon") )
  #add( Card_Minion(6, 6, 6, "Infernal", cls="warlock", cat="demon") )
  #add( Card_Minion_BC(9, 3, 15, "Lord Jaraxxus", name_fr="", 
        #desc="Battlecry: Destroy your hero and replace him with Lord Jaraxxus.", cls="warlock", cat="demon") )
  #add( Card_Minion(2, 1, 4, "Armorsmith", name_fr="", 
        #desc="Whenever a friendly minion takes damage, gain 1 Armor.", cls="warrior") )
  #add( Card_Minion_BC(2, 2, 2, "Cruel Taskmaster", name_fr="", 
        #desc="Battlecry: Deal 1 damage to a minion and give it +2 Attack.", cls="warrior") )
  #add( Card_Minion(3, 2, 4, "Frothing Berserker", name_fr="", 
        #desc="Whenever a minion takes damage, gain +1 Attack.", cls="warrior") )
  #add( Card_Minion(3, 2, 3, "Warsong Commander", name_fr="", 
        #desc="Whenever you play a minion with 3 or less Attack, give it Charge.", cls="warrior") )
  #add( Card_Minion_BC(4, 3, 3, "Arathi Weaponsmith", name_fr="", 
        #desc="Battlecry: Equip a 2/2 weapon.", cls="warrior") )
  #add( Card_Minion(4, 4, 3, "Kor'kron Elite", name_fr="", 
        #desc="Charge", cls="warrior", effects="charge") )
  #add( Card_Minion(8, 4, 9, "Grommash Hellscream", name_fr="", 
        #desc="Charge.  Enrage: +6 Attack", cls="warrior", effects="charge enrage") )
  #add( Card_Minion_BC(10, 12, 12, "Deathwing", name_fr="", 
        #desc="Battlecry: Destroy all other minions and discard your hand.", cat="dragon") )
  
  #add( Card_Minion(10, 8, 8, "Sea Giant", name_fr="", 
        #desc="Costs (1) less for each other minion on the battlefield.") )
  #add( Card_Minion(12, 8, 8, "Mountain Giant", name_fr="", 
        #desc="Costs (1) less for each other card in your hand.") )

  #add( Card_Spell(0, "Ancient Secrets", name_fr="", 
        #desc="Restore 5 Health.", cls="druid") )
  #add( Card_Spell(0, "Ancient Teachings", name_fr="", 
        #desc="Draw 2 cards.", cls="druid") )
  #add( Card_Spell(0, "Bear Form", name_fr="", 
        #desc="+2 Health and Taunt.", cls="druid") )
  #add( Card_Spell(0, "Cat Form", name_fr="", 
        #desc="Charge", cls="druid") )
  #add( Card_Spell(0, "Demigod's Favor", name_fr="", 
        #desc="Give your other minions +2/+2.", cls="druid") )
  #add( Card_Spell(0, "Dispel", name_fr="", 
        #desc="Silence a minion.", cls="druid") )
  #add( Card_Spell(0, "Excess Mana", name_fr="", 
        #desc="Draw a card. (You can only have 10 Mana in your tray.)", cls="druid") )
  #add( Card_Spell(0, "Innervate", name_fr="", 
        #desc="Gain 2 Mana Crystals this turn only.", cls="druid") )
  #add( Card_Spell(0, "Leader of the Pack", name_fr="", 
        #desc="Give all of your minions +1/+1.", cls="druid") )
  #add( Card_Spell(0, "Mark of Nature", name_fr="", 
        #desc="+4 Attack.", cls="druid") )
  #add( Card_Spell(0, "Mark of Nature", name_fr="", 
        #desc="+4 Health and Taunt.", cls="druid") )
  #add( Card_Spell(0, "Moonfire", name_fr="", 
        #desc="Deal 1 damage.", cls="druid") )
  #add( Card_Spell(0, "Moonfire", name_fr="", 
        #desc="Deal 2 damage.", cls="druid") )
  #add( Card_Spell(0, "Nourish", name_fr="", 
        #desc="Draw 3 cards.", cls="druid") )
  #add( Card_Spell(0, "Nourish", name_fr="", 
        #desc="Gain 2 Mana Crystals.", cls="druid") )
  #add( Card_Spell(0, "Rooted", name_fr="", 
        #desc="+5 Health and Taunt.", cls="druid") )
  #add( Card_Spell(0, "Shan'do's Lesson", name_fr="", 
        #desc="Summon two 2/2 Treants with Taunt.", cls="druid") )
  #add( Card_Spell(0, "Starfall", name_fr="", 
        #desc="Deal 2 damage to all enemy minions.", cls="druid") )
  #add( Card_Spell(0, "Starfall", name_fr="", 
        #desc="Deal 5 damage to a minion.", cls="druid") )
  #add( Card_Spell(0, "Summon a Panther", name_fr="", 
        #desc="Summon a 3/2 Panther.", cls="druid") )
  #add( Card_Spell(0, "Uproot", name_fr="", 
        #desc="+5 Attack.", cls="druid") )
  #add( Card_Spell(0, "Wrath", name_fr="", 
        #desc="Deal 1 damage to a minion. Draw a card.", cls="druid") )
  #add( Card_Spell(0, "Wrath", name_fr="", 
        #desc="Deal 3 damage to a minion.", cls="druid") )
  #add( Card_Spell(1, "Claw", name_fr="", 
        #desc="Give your hero +2 Attack this turn and 2 Armor.", cls="druid") )
  #add( Card_Spell(1, "Naturalize", name_fr="", 
        #desc="Destroy a minion. Your opponent draws 2 cards.", cls="druid") )
  #add( Card_Spell(1, "Savagery", name_fr="", 
        #desc="Deal damage equal to your hero's Attack to a minion.", cls="druid") )
  #add( Card_Spell(2, "Mark of the Wild", name_fr="", 
        #desc="Give a minion Taunt and +2/+2. (+2 Attack/+2 Health)", cls="druid") )
  #add( Card_Spell(2, "Power of the Wild", name_fr="", 
        #desc="Choose One - Give your minions +1/+1; or Summon a 3/2 Panther.", cls="druid") )
  #add( Card_Spell(2, "Wild Growth", name_fr="", 
        #desc="Gain an empty Mana Crystal.", cls="druid") )
  #add( Card_Spell(2, "Wrath", name_fr="", 
        #desc="Choose One - Deal 3 damage to a minion; or 1 damage and draw a card.", cls="druid") )
  #add( Card_Spell(3, "Healing Touch", name_fr="", 
        #desc="Restore #8 Health.", cls="druid") )
  #add( Card_Spell(3, "Mark of Nature", name_fr="", 
        #desc="Choose One - Give a minion +4 Attack; or +4 Health and Taunt.", cls="druid") )
  #add( Card_Spell(3, "Savage Roar", name_fr="", 
        #desc="Give your characters +2 Attack this turn.", cls="druid") )
  #add( Card_Spell(4, "Bite", name_fr="", 
        #desc="Give your hero +4 Attack this turn and 4 Armor.", cls="druid") )
  #add( Card_Spell(4, "Soul of the Forest", name_fr="", 
        #desc="Give your minions "Deathrattle Summon a 2/2 Treant."", cls="druid") )
  #add( Card_Spell(4, "Swipe", name_fr="", 
        #desc="Deal 4 damage to an enemy and 1 damage to all other enemies.", cls="druid") )
  #add( Card_Spell(5, "Nourish", name_fr="", 
        #desc="Choose One - Gain 2 Mana Crystals; or Draw 3 cards.", cls="druid") )
  #add( Card_Spell(5, "Starfall", name_fr="", 
        #desc="Choose One - Deal 5 damage to a minion; or 2 damage to all enemy minions.", cls="druid") )
  #add( Card_Spell(6, "Force of Nature", name_fr="", 
        #desc="Summon three 2/2 Treants with Charge that die at the end of the turn.", cls="druid") )
  #add( Card_Spell(6, "Starfire", name_fr="", 
        #desc="Deal 5 damage.  Draw a card.", cls="druid") )
  #add( Card_Spell(0, "Hunter's Mark", name_fr="", 
        #desc="Change a minion's Health to 1.", cls="hunter") )
  #add( Card_Spell(1, "Arcane Shot", name_fr="", 
        #desc="Deal 2 damage.", cls="hunter") )
  #add( Card_Spell(1, "Bestial Wrath", name_fr="", 
        #desc="Give a Beast +2 Attack and Immune this turn.", cls="hunter") )
  #add( Card_Spell(1, "Flare", name_fr="", 
        #desc="All minions lose Stealth. Destroy all enemy Secrets. Draw a card.", cls="hunter") )
  #add( Card_Spell(1, "Tracking", name_fr="", 
        #desc="Look at the top three cards of your deck. Draw one and discard the others.", cls="hunter") )
  #add( Card_Spell(2, "Explosive Trap", name_fr="", 
        #desc="Secret: When your hero is attacked, deal 2 damage to all enemies.", cls="hunter") )
  #add( Card_Spell(2, "Freezing Trap", name_fr="", 
        #desc="Secret: When an enemy minion attacks, return it to its owner's hand and it costs (2) more.", cls="hunter") )
  #add( Card_Spell(2, "Misdirection", name_fr="", 
        #desc="Secret: When a character attacks your hero, instead he attacks another random character.", cls="hunter") )
  #add( Card_Spell(2, "Snake Trap", name_fr="", 
        #desc="Secret: When one of your minions is attacked, summon three 1/1 Snakes.", cls="hunter") )
  #add( Card_Spell(2, "Snipe", name_fr="", 
        #desc="Secret: When your opponent plays a minion, deal 4 damage to it.", cls="hunter") )
  #add( Card_Spell(2, "Unleash the Hounds", name_fr="", 
        #desc="For each enemy minion, summon a 1/1 Hound with Charge.", cls="hunter") )
  #add( Card_Spell(3, "Animal Companion", name_fr="", 
        #desc="Summon a random Beast Companion.", cls="hunter") )
  #add( Card_Spell(3, "Deadly Shot", name_fr="", 
        #desc="Destroy a random enemy minion.", cls="hunter") )
  #add( Card_Spell(3, "Kill Command", name_fr="", 
        #desc="Deal 3 damage.  If you have a Beast, deal 5 damage instead.", cls="hunter") )
  #add( Card_Spell(4, "Multi-Shot", name_fr="", 
        #desc="Deal 3 damage to two random enemy minions.", cls="hunter") )
  #add( Card_Spell(5, "Explosive Shot", name_fr="", 
        #desc="Deal 5 damage to a minion and 2 damage to adjacent ones.", cls="hunter") )
  #add( Card_Spell(1, "Arcane Missiles", name_fr="", 
        #desc="Deal 3 damage randomly split among enemy characters.", cls="mage") )
  #add( Card_Spell(1, "Ice Lance", name_fr="", 
        #desc="Freeze a character. If it was already Frozen, deal 4 damage instead.", cls="mage") )
  #add( Card_Spell(1, "Mirror Image", name_fr="", 
        #desc="Summon two 0/2 minions with Taunt.", cls="mage") )
  #add( Card_Spell(10, "Pyroblast", name_fr="", 
        #desc="Deal 10 damage.", cls="mage") )
  #add( Card_Spell(2, "Arcane Explosion", name_fr="", 
        #desc="Deal 1 damage to all enemy minions.", cls="mage") )
  #add( Card_Spell(2, "Frostbolt", name_fr="", 
        #desc="Deal 3 damage to a character and Freeze it.", cls="mage") )
  #add( Card_Spell(3, "Arcane Intellect", name_fr="", 
        #desc="Draw 2 cards.", cls="mage") )
  #add( Card_Spell(3, "Counterspell", name_fr="", 
        #desc="Secret: When your opponent casts a spell, Counter it.", cls="mage") )
  #add( Card_Spell(3, "Frost Nova", name_fr="", 
        #desc="Freeze all enemy minions.", cls="mage") )
  #add( Card_Spell(3, "Ice Barrier", name_fr="", 
        #desc="Secret: As soon as your hero is attacked, gain 8 Armor.", cls="mage") )
  #add( Card_Spell(3, "Ice Block", name_fr="", 
        #desc="Secret: When your hero takes fatal damage, prevent it and become Immune this turn.", cls="mage") )
  #add( Card_Spell(3, "Mirror Entity", name_fr="", 
        #desc="Secret: When your opponent plays a minion, summon a copy of it.", cls="mage") )
  #add( Card_Spell(3, "Spellbender", name_fr="", 
        #desc="Secret: When an enemy casts a spell on a minion, summon a 1/3 as the new target.", cls="mage") )
  #add( Card_Spell(3, "Vaporize", name_fr="", 
        #desc="Secret: When a minion attacks your hero, destroy it.", cls="mage") )
  #add( Card_Spell(4, "Cone of Cold", name_fr="", 
        #desc="Freeze a minion and the minions next to it, and deal 1 damage to them.", cls="mage") )
  #add( Card_Spell(4, "Fireball", name_fr="", 
        #desc="Deal 6 damage.", cls="mage") )
  #add( Card_Spell(4, "Polymorph", name_fr="", 
        #desc="Transform a minion into a 1/1 Sheep.", cls="mage") )
  #add( Card_Spell(6, "Blizzard", name_fr="", 
        #desc="Deal 2 damage to all enemy minions and Freeze them.", cls="mage") )
  #add( Card_Spell(7, "Flamestrike", name_fr="", 
        #desc="Deal 4 damage to all enemy minions.", cls="mage") )
  #add( Card_Spell(0, "Dream", name_fr="", 
        #desc="Return a minion to its owner's hand.") )
  #add( Card_Spell(0, "Nightmare", name_fr="", 
        #desc="Give a minion +5/+5.  At the start of your next turn, destroy it.") )
  #add( Card_Spell(0, "The Coin", name_fr="", 
        #desc="Gain 1 Mana Crystal this turn only.") )
  #add( Card_Spell(1, "Bananas", name_fr="", 
        #desc="Give a friendly minion +1/+1. (+1 Attack/+1 Health)") )
  #add( Card_Spell(1, "Bananas", name_fr="", 
        #desc="Give a minion +1/+1.") )
  #add( Card_Spell(1, "Barrel Toss", name_fr="", 
        #desc="Deal 2 damage.") )
  #add( Card_Spell(1, "Transcendence", name_fr="", 
        #desc="Until you kill Cho's minions, he can't be attacked.") )
  #add( Card_Spell(2, "NOOOOOOOOOOOO", name_fr="", 
        #desc="Somehow, the card you USED to have has been deleted.  Here, have this one instead!") )
  #add( Card_Spell(2, "Stomp", name_fr="", 
        #desc="Deal 2 damage to all enemies.") )
  #add( Card_Spell(2, "Ysera Awakens", name_fr="", 
        #desc="Deal 5 damage to all characters except Ysera.") )
  #add( Card_Spell(3, "Flame Burst", name_fr="", 
        #desc="Shoot 5 missiles at random enemies for 1 damage each.") )
  #add( Card_Spell(3, "Hogger SMASH!", name_fr="", 
        #desc="Deal 4 damage.") )
  #add( Card_Spell(3, "Legacy of the Emperor", name_fr="", 
        #desc="Give your minions +2/+2. (+2 Attack/+2 Health)") )
  #add( Card_Spell(3, "Will of Mukla", name_fr="", 
        #desc="Restore 8 Health.") )
  #add( Card_Spell(6, "Metamorphosis", name_fr="", 
        #desc="Do something crazy.") )
  #add( Card_Spell(1, "Blessing of Might", name_fr="", 
        #desc="Give a minion +3 Attack.", cls="paladin") )
  #add( Card_Spell(1, "Blessing of Wisdom", name_fr="", 
        #desc="Choose a minion.  Whenever it attacks, draw a card.", cls="paladin") )
  #add( Card_Spell(1, "Eye for an Eye", name_fr="", 
        #desc="Secret: When your hero takes damage, deal that much damage to the enemy hero.", cls="paladin") )
  #add( Card_Spell(1, "Hand of Protection", name_fr="", 
        #desc="Give a minion Divine Shield.", cls="paladin") )
  #add( Card_Spell(1, "Humility", name_fr="", 
        #desc="Change a minion's Attack to 1.", cls="paladin") )
  #add( Card_Spell(1, "Noble Sacrifice", name_fr="", 
        #desc="Secret: When an enemy attacks, summon a 2/1 Defender as the new target.", cls="paladin") )
  #add( Card_Spell(1, "Redemption", name_fr="", 
        #desc="Secret: When one of your minions dies, return it to life with 1 Health.", cls="paladin") )
  #add( Card_Spell(1, "Repentance", name_fr="", 
        #desc="Secret: When your opponent plays a minion, reduce its Health to 1.", cls="paladin") )
  #add( Card_Spell(2, "Equality", name_fr="", 
        #desc="Change the Health of ALL minions to 1.", cls="paladin") )
  #add( Card_Spell(2, "Holy Light", name_fr="", 
        #desc="Restore #6 Health.", cls="paladin") )
  #add( Card_Spell(3, "Divine Favor", name_fr="", 
        #desc="Draw cards until you have as many in hand as your opponent.", cls="paladin") )
  #add( Card_Spell(4, "Blessing of Kings", name_fr="", 
        #desc="Give a minion +4/+4. (+4 Attack/+4 Health)", cls="paladin") )
  #add( Card_Spell(4, "Consecration", name_fr="", 
        #desc="Deal 2 damage to all enemies.", cls="paladin") )
  #add( Card_Spell(4, "Hammer of Wrath", name_fr="", 
        #desc="Deal 3 damage.  Draw a card.", cls="paladin") )
  #add( Card_Spell(5, "Blessed Champion", name_fr="", 
        #desc="Double a minion's Attack.", cls="paladin") )
  #add( Card_Spell(5, "Holy Wrath", name_fr="", 
        #desc="Draw a card and deal damage equal to its cost.", cls="paladin") )
  #add( Card_Spell(6, "Avenging Wrath", name_fr="", 
        #desc="Deal 8 damage randomly split among enemy characters.", cls="paladin") )
  #add( Card_Spell(8, "Lay on Hands", name_fr="", 
        #desc="Restore #8 Health. Draw 3 cards.", cls="paladin") )
  #add( Card_Spell(0, "Circle of Healing", name_fr="", 
        #desc="Restore #4 Health to ALL minions.", cls="priest") )
  #add( Card_Spell(0, "Silence", name_fr="", 
        #desc="Silence a minion.", cls="priest") )
  #add( Card_Spell(1, "Holy Smite", name_fr="", 
        #desc="Deal 2 damage.", cls="priest") )
  #add( Card_Spell(1, "Inner Fire", name_fr="", 
        #desc="Change a minion's Attack to be equal to its Health.", cls="priest") )
  #add( Card_Spell(1, "Mind Vision", name_fr="", 
        #desc="Put a copy of a random card in your opponent's hand into your hand.", cls="priest") )
  #add( Card_Spell(1, "Power Word: Shield", name_fr="", 
        #desc="Give a minion +2 Health. Draw a card.", cls="priest") )
  #add( Card_Spell(10, "Mind Control", name_fr="", 
        #desc="Take control of an enemy minion.", cls="priest") )
  #add( Card_Spell(2, "Divine Spirit", name_fr="", 
        #desc="Double a minion's Health.", cls="priest") )
  #add( Card_Spell(2, "Mind Blast", name_fr="", 
        #desc="Deal 5 damage to the enemy hero.", cls="priest") )
  #add( Card_Spell(2, "Shadow Word: Pain", name_fr="", 
        #desc="Destroy a minion with 3 or less Attack.", cls="priest") )
  #add( Card_Spell(3, "Shadow Word: Death", name_fr="", 
        #desc="Destroy a minion with an Attack of 5 or more.", cls="priest") )
  #add( Card_Spell(3, "Shadowform", name_fr="", 
        #desc="Your Hero Power becomes 'Deal 2 damage'. If already in Shadowform: 3 damage.", cls="priest") )
  #add( Card_Spell(3, "Thoughtsteal", name_fr="", 
        #desc="Copy 2 cards from your opponent's deck and put them into your hand.", cls="priest") )
  #add( Card_Spell(4, "Mass Dispel", name_fr="", 
        #desc="Silence all enemy minions. Draw a card.", cls="priest") )
  #add( Card_Spell(4, "Mindgames", name_fr="", 
        #desc="Put a copy of a random minion from your opponent's deck into the battlefield.", cls="priest") )
  #add( Card_Spell(4, "Shadow Madness", name_fr="", 
        #desc="Gain control of an enemy minion with 3 or less Attack until end of turn.", cls="priest") )
  #add( Card_Spell(5, "Holy Nova", name_fr="", 
        #desc="Deal 2 damage to all enemies.  Restore #2 Health to all  friendly characters.", cls="priest") )
  #add( Card_Spell(6, "Holy Fire", name_fr="", 
        #desc="Deal 5 damage.  Restore 5 Health to your hero.", cls="priest") )
  #add( Card_Spell(0, "Backstab", name_fr="", 
        #desc="Deal 2 damage to an undamaged minion.", cls="rogue") )
  #add( Card_Spell(0, "Preparation", name_fr="", 
        #desc="The next spell you cast this turn costs (3) less.", cls="rogue") )
  #add( Card_Spell(0, "Shadowstep", name_fr="", 
        #desc="Return a friendly minion to your hand. It costs (2) less.", cls="rogue") )
  #add( Card_Spell(1, "Cold Blood", name_fr="", 
        #desc="Give a minion +2 Attack. Combo: +4 Attack instead.", cls="rogue") )
  #add( Card_Spell(1, "Conceal", name_fr="", 
        #desc="Give your minions Stealth until your next turn.", cls="rogue") )
  #add( Card_Spell(1, "Deadly Poison", name_fr="", 
        #desc="Give your weapon +2 Attack.", cls="rogue") )
  #add( Card_Spell(1, "Sinister Strike", name_fr="", 
        #desc="Deal 3 damage to the enemy hero.", cls="rogue") )
  #add( Card_Spell(2, "Betrayal", name_fr="", 
        #desc="An enemy minion deals its damage to the minions next to it.", cls="rogue") )
  #add( Card_Spell(2, "Blade Flurry", name_fr="", 
        #desc="Destroy your weapon and deal its damage to all enemies.", cls="rogue") )
  #add( Card_Spell(2, "Eviscerate", name_fr="", 
        #desc="Deal 2 damage. Combo: Deal 4 damage instead.", cls="rogue") )
  #add( Card_Spell(2, "Sap", name_fr="", 
        #desc="Return an enemy minion to its owner's hand.", cls="rogue") )
  #add( Card_Spell(2, "Shiv", name_fr="", 
        #desc="Deal 1 damage. Draw a card.", cls="rogue") )
  #add( Card_Spell(3, "Fan of Knives", name_fr="", 
        #desc="Deal 1 damage to all enemy minions. Draw a card.", cls="rogue") )
  #add( Card_Spell(3, "Headcrack", name_fr="", 
        #desc="Deal 2 damage to the enemy hero. Combo: Return this to your hand next turn.", cls="rogue") )
  #add( Card_Spell(5, "Assassinate", name_fr="", 
        #desc="Destroy an enemy minion.", cls="rogue") )
  #add( Card_Spell(6, "Vanish", name_fr="", 
        #desc="Return all minions to their owner's hand.", cls="rogue") )
  #add( Card_Spell(7, "Sprint", name_fr="", 
        #desc="Draw 4 cards.", cls="rogue") )
  #add( Card_Spell(0, "Ancestral Healing", name_fr="", 
        #desc="Restore a minion to full Health and give it Taunt.", cls="shaman") )
  #add( Card_Spell(0, "Totemic Might", name_fr="", 
        #desc="Give your Totems +2 Health.", cls="shaman") )
  #add( Card_Spell(1, "Earth Shock", name_fr="", 
        #desc="Silence a minion, then deal 1 damage to it.", cls="shaman") )
  #add( Card_Spell(1, "Forked Lightning", name_fr="", 
        #desc="Deal 2 damage to 2 random enemy minions. Overload: (2)", cls="shaman") )
  #add( Card_Spell(1, "Frost Shock", name_fr="", 
        #desc="Deal 1 damage to an enemy character and Freeze it.", cls="shaman") )
  #add( Card_Spell(1, "Lightning Bolt", name_fr="", 
        #desc="Deal 3 damage. Overload: (1)", cls="shaman") )
  #add( Card_Spell(1, "Rockbiter Weapon", name_fr="", 
        #desc="Give a friendly character +3 Attack this turn.", cls="shaman") )
  #add( Card_Spell(2, "Ancestral Spirit", name_fr="", 
        #desc="Choose a minion. When that minion is destroyed, return it to the battlefield.", cls="shaman") )
  #add( Card_Spell(2, "Windfury", name_fr="", 
        #desc="Give a minion Windfury.", cls="shaman") )
  #add( Card_Spell(3, "Far Sight", name_fr="", 
        #desc="Draw a card. That card costs (3) less.", cls="shaman") )
  #add( Card_Spell(3, "Feral Spirit", name_fr="", 
        #desc="Summon two 2/3 Spirit Wolves with Taunt. Overload: (2)", cls="shaman") )
  #add( Card_Spell(3, "Hex", name_fr="", 
        #desc="Transform a minion into a 0/1 Frog with Taunt.", cls="shaman") )
  #add( Card_Spell(3, "Lava Burst", name_fr="", 
        #desc="Deal 5 damage. Overload: (2)", cls="shaman") )
  #add( Card_Spell(3, "Lightning Storm", name_fr="", 
        #desc="Deal 2-3 damage to all enemy minions. Overload: (2)", cls="shaman") )
  #add( Card_Spell(5, "Bloodlust", name_fr="", 
        #desc="Give your minions +3 Attack this turn.", cls="shaman") )
  #add( Card_Spell(0, "Sacrificial Pact", name_fr="", 
        #desc="Destroy a Demon. Restore #5 Health to your hero.", cls="warlock") )
  #add( Card_Spell(0, "Soulfire", name_fr="", 
        #desc="Deal 4 damage. Discard a random card.", cls="warlock") )
  #add( Card_Spell(1, "Corruption", name_fr="", 
        #desc="Choose an enemy minion.   At the start of your turn, destroy it.", cls="warlock") )
  #add( Card_Spell(1, "Mortal Coil", name_fr="", 
        #desc="Deal 1 damage to a minion. If that kills it, draw a card.", cls="warlock") )
  
  add( Card_Spell(1,"Power Overwhleming", name_fr="Puissance accablante",
       actions = lambda self: [Msg_BindEffect(self.caster, self.choices[0], Eff_BuffMinion(4,4)), 
                         Msg_BindEffect(self.caster, self.choices[0], Eff_DieSoon(Msg_EndTurn))], 
       targets='friendly minions', cls="warlock",
       desc="Give a friendly minion +4/+4 until end of turn. Then, it dies. Horribly.",
       desc_fr='Confere +4/+4 a un serviteur allie jusqu\'a la fin du tour. Puis il meurt.') )
  
  #add( Card_Spell(2, "Demonfire", name_fr="", 
        #desc="Deal 2 damage to a minion.   If it's a friendly Demon, give it +2/+2 instead.", cls="warlock") )
  #add( Card_Spell(3, "Drain Life", name_fr="", 
        #desc="Deal 2 damage. Restore #2 Health to your hero.", cls="warlock") )
  #add( Card_Spell(3, "Sense Demons", name_fr="", 
        #desc="Put 2 random Demons from your deck into your hand.", cls="warlock") )
  #add( Card_Spell(3, "Shadow Bolt", name_fr="", 
        #desc="Deal 4 damage to a minion.", cls="warlock") )
  #add( Card_Spell(4, "Hellfire", name_fr="", 
        #desc="Deal 3 damage to ALL characters.", cls="warlock") )
  #add( Card_Spell(4, "Shadowflame", name_fr="", 
        #desc="Destroy a friendly minion and deal its Attack damage to all enemy minions.", cls="warlock") )
  #add( Card_Spell(5, "Bane of Doom", name_fr="", 
        #desc="Deal 2 damage to a character.  If that kills it, summon a random Demon.", cls="warlock") )
  #add( Card_Spell(6, "Siphon Soul", name_fr="", 
        #desc="Destroy a minion. Restore #3 Health to your hero.", cls="warlock") )
  #add( Card_Spell(8, "Twisting Nether", name_fr="", 
        #desc="Destroy all minions.", cls="warlock") )
  #add( Card_Spell(0, "Inner Rage", name_fr="", 
        #desc="Deal 1 damage to a minion and give it +2 Attack.", cls="warrior") )
  #add( Card_Spell(1, "Execute", name_fr="", 
        #desc="Destroy a damaged enemy minion.", cls="warrior") )
  #add( Card_Spell(1, "Shield Slam", name_fr="", 
        #desc="Deal 1 damage to a minion for each Armor you have.", cls="warrior") )
  #add( Card_Spell(1, "Upgrade!", name_fr="", 
        #desc="If you have a weapon, give it +1/+1.  Otherwise equip a 1/3 weapon.", cls="warrior") )
  #add( Card_Spell(1, "Whirlwind", name_fr="", 
        #desc="Deal 1 damage to ALL minions.", cls="warrior") )
  #add( Card_Spell(2, "Battle Rage", name_fr="", 
        #desc="Draw a card for each damaged friendly character.", cls="warrior") )
  #add( Card_Spell(2, "Cleave", name_fr="", 
        #desc="Deal 2 damage to two random enemy minions.", cls="warrior") )
  #add( Card_Spell(2, "Commanding Shout", name_fr="", 
        #desc="Your minions can't be reduced below 1 Health this turn.  Draw a card.", cls="warrior") )
  #add( Card_Spell(2, "Heroic Strike", name_fr="", 
        #desc="Give your hero +4 Attack this turn.", cls="warrior") )
  #add( Card_Spell(2, "Rampage", name_fr="", 
        #desc="Give a damaged minion +3/+3.", cls="warrior") )
  #add( Card_Spell(2, "Slam", name_fr="", 
        #desc="Deal 2 damage to a minion.  If it survives, draw a card.", cls="warrior") )
  #add( Card_Spell(3, "Charge", name_fr="", 
        #desc="Give a friendly minion +2 Attack and Charge.", cls="warrior") )
  #add( Card_Spell(3, "Shield Block", name_fr="", 
        #desc="Gain 5 Armor.  Draw a card.", cls="warrior") )
  #add( Card_Spell(4, "Mortal Strike", name_fr="", 
        #desc="Deal 4 damage.  If your hero has 12 or less Health, deal 6 damage instead.", cls="warrior") )
  #add( Card_Spell(5, "Brawl", name_fr="", 
        #desc="Destroy all minions except one.  (chosen randomly)", cls="warrior") )

  ### Weapons ############################  
  
  #add( Card_Weapon(3, 3, 2, "Eaglehorn Bow", name_fr="", 
        #desc="Whenever a Secret is revealed, gain +1 Durability.", cls="hunter") )
  #add( Card_Weapon(7, 5, 2, "Gladiator's Longbow", name_fr="", 
        #desc="Your hero is Immune while attacking.", cls="hunter") )
  #add( Card_Weapon(2, 2, 2, "Warglaive of Azzinoth") )
  #add( Card_Weapon(6, 4, 2, "Dual Warglaives") )
  #add( Card_Weapon(1, 1, 4, "Light's Justice", cls="paladin") )
  #add( Card_Weapon(3, 1, 5, "Sword of Justice", name_fr="", 
        #desc="Whenever you summon a minion, give it +1/+1 and this loses 1 Durability.", cls="paladin") )
  #add( Card_Weapon(4, 4, 2, "Truesilver Champion", name_fr="", 
        #desc="Whenever your hero attacks, restore 2 Health to it.", cls="paladin") )
  #add( Card_Weapon(5, 5, 3, "Ashbringer", cls="paladin") )
  
  add( Card_Weapon(1, 1, 2, "Wicked Knife", cls="rogue", collectible=False) )
  
  #add( Card_Weapon(3, 2, 2, "Perdition's Blade", name_fr="", 
        #desc="Battlecry: Deal 1 damage. Combo: Deal 2 instead.", cls="rogue") )
  #add( Card_Weapon(5, 3, 4, "Assassin's Blade", cls="rogue") )
  #add( Card_Weapon(2, 2, 3, "Stormforged Axe", name_fr="", 
        #desc="Overload: (1)", cls="shaman") )
  #add( Card_Weapon(5, 2, 8, "Doomhammer", name_fr="", 
        #desc="Windfury, Overload: (2)", cls="shaman") )
  #add( Card_Weapon(3, 3, 8, "Blood Fury", cls="warlock") )
  #add( Card_Weapon(1, 2, 2, "Battle Axe", cls="warrior") )
  #add( Card_Weapon(1, 1, 3, "Heavy Axe", cls="warrior") )
  #add( Card_Weapon(2, 3, 2, "Fiery War Axe", cls="warrior") )
  #add( Card_Weapon(5, 5, 2, "Arcanite Reaper", cls="warrior") )
  #add( Card_Weapon(7, 7, 1, "Gorehowl", name_fr="", 
        #desc="Attacking a minion costs 1 Attack instead of 1 Durability.", cls="warrior") )
  
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
      print '  #add( Card_Minion%s(%s, %s, %s, "%s", name_fr="", '  % (bc, mana, atq, hp, name)
      print '        #desc="%s"%s%s%s) )' % (desc,
             '' if cls=="neutral" else ', cls="%s"'%cls, '' if cat=="none" else ', cat="%s"'%cat,
             effects and ', effects="%s"'%effects or '')
    elif type=="weapon":
      print '  #add( Card_Weapon(%s, %s, %s, "%s", name_fr="", ' % (mana, atq, hp, name)
      print '        #desc="%s"%s) )' % ( desc,
             '' if cls=="neutral" else ', cls="%s"'%cls )
    elif type=="spell":
      print '  #add( Card_Spell(%s, "%s", name_fr="", ' %(mana, name)
      print '        #desc="%s"%s) )' % ( desc, '' if cls=="neutral" else ', cls="%s"'%cls )
    elif type=="hero":
      print '  #add( Card_Hero(%s, 0, %s, "%s", name_fr="", ' % (mana, hp, name)
      print '        #desc="%s"%s) )' % ( desc, '' if cls=="neutral" else ', cls="%s"'%cls )
    elif type=="ability":
      print '  #add( Card_HeroAbility(%s, "%s", name_fr="", ' %(mana, name)
      print '        #desc="%s"%s) )' % ( desc, '' if cls=="neutral" else ', cls="%s"'%cls )
    else:
        assert False, "error: unknown card type "+type


