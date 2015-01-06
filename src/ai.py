# -*- coding: utf-8 -*-
"""
Artificial Intelligence for hearthstone

@author: jere
"""
import pdb, time
from players import Player
from actions import Act_EndTurn


### ------ attempter simple IA based on heuristics without forecasting ------------

class VerySimpleAI (Player):
  '''attempt simple AI based on heuristics without forecasting: 
     actions are selected one by one, independently'''

  def mulligan(self, cards):
      open("tmp.txt","w")
      # keep everything that cost less than 3 mana
      return [card for card in cards if card.cost>3]

  @staticmethod  
  def iter_choices(choices, nmax=None):
      #choices = [([None] if ch is None else ch) for ch in choices]
      if len(choices)==0:
        res = [()]  # one empty choice
      elif len(choices)==1:
        res = [(ch,) for ch in choices[0]]
      elif len(choices)==2:
        res = [(ch0,ch1) for ch0 in choices[0] for ch1 in choices[1]]
      else:
        assert False, "was not expecting more than 2 choices for an action: "+str(choices)
      return res[:nmax] 

  def choose_actions(self, actions):
      # select action with the best return    
      sim = self.engine.launch_simulation() # open simulation
      best = sim.board.score_situation(self), actions[0], ()  # default = end turn !
      assert type(best[1]) == Act_EndTurn
#      tmp = open("tmp.txt","a")
#      print >>tmp, "\nChoose action: init score = %g" % best[0] 
      
      # try all possible actions
      for action in actions[1:]:
        for choices in self.iter_choices(action.choices,10): 
          action.select(choices)
          sim.send_message(action)
          score = sim.board.score_situation( self )
#          print >>tmp, 'action: ',action,' --> score: %g' % score
          sim.restore_state() # reset simulation
          if score >= best[0]: 
            best = score, action, choices
      
      sim.end_simulation()
      action = best[1].select(best[2])
#      print >>tmp, 'final choice:', action
      return action




class SimpleAI (Player):
  '''attempt simple AI based on heuristics without forecasting: 
     actions are selected all at once for one turn'''
  def __init__(self, *args, **kwargs):
      Player.__init__(self, *args)
      self.maxloss = kwargs.get('maxloss',3)
      self.timeout = kwargs.get('timeout',1)

  def mulligan(self, cards):
      open("tmp.txt","w")
      # keep everything that cost less than 3 mana
      return [card for card in cards if card.cost>3]

  @staticmethod  
  def iter_choices(choices, nmax=None, pos_mode=None):
      assert not any([ch==None for ch in choices]), pdb.set_trace()
      if pos_mode=='left':  # only use leftmost slot position to restrict choices
        from board import Slot
        choices = [(ch[:1] if ch and type(ch[0])==Slot else ch) for ch in choices]
          
      if len(choices)==0:
        res = [()]  # one empty choice
      elif len(choices)==1:
        res = [(ch,) for ch in choices[0]]
      elif len(choices)==2:
        res = [(ch0,ch1) for ch0 in choices[0] for ch1 in choices[1]]
      else:
        assert False, "was not expecting more than 2 choices for an action: "+str(choices)
      return res[:nmax] 

  def choose_actions(self, actions):
      # select succession of actions with the best return
      state = 0
      sim = self.engine.launch_simulation(state) # open simulation
      end_turn = (actions[0],())
      assert type(end_turn[0]) == Act_EndTurn
      
      start_time = time.time()
      best = -float('inf')
      predictions = [end_turn]
      todo = [(state, 0, [])]
      while todo and time.time()-start_time<self.timeout:
        cur_state, score, cur_actions = todo.pop(0) # explore paths from this lattice node 
        if score < best-self.maxloss: continue # we don't care, it's already too bad
        sim.restore_state(cur_state)
        actions = sim.list_player_actions(self)
        
        # try all possible actions
        for action in actions:
          if action.neighbors():
            pos_mode='full'
          else:
            pos_mode='left'
          for choice in self.iter_choices(action.choices,10,pos_mode): 
            action.select(choice)
            sim.send_message(action)
            score = sim.board.score_situation( self )
            
            if score > best-self.maxloss:
              # this new state is interesting 
              if type(action) != Act_EndTurn:
                state += 1
                sim.save_state(state)
                todo.append((state, score, cur_actions + [(action, choice)]))
              
              elif score > best: # last action is end turn
                best = score
                predictions = cur_actions + [(action, choice)]
            
            sim.restore_state(cur_state) # reset simulation
      
      sim.end_simulation(0)
      
#      tmp = open("tmp.txt","a")
#      print >>tmp, "\nprediction: "
#      for action, choice in predictions:
#        print >>tmp, str(action)
#      print >>tmp, "final score: %g" % best
      
      action, choice = predictions.pop(0)
      action.select(choice)
      return action












