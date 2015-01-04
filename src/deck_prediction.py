# -*- coding: utf-8 -*-
"""
Prediction model for adversary deck cards, based on bag-of-cards.

@author: jere
"""
from numpy import array
from scipy import sparse
from collection import collection, card_count as idmax


class PredictionModel:
    def __init__(self, cls):
      # list of decks, representative of current meta
      self.cls = cls # prediction model is valid for one given class
      self.all_decks = []
      self.ready = False

    def add_deck(self, deck):
      '''add one deck (possibly incomplete) '''
      indices = [card.id for card in deck]
      self.all_decks.append(indices)
      self.ready = False

    @staticmethod
    def to_csr(decks):
      lil = sparse.lil_matrix((len(decks),idmax), dtype='int32')
      lil.rows = array(decks)
      lil.data = array([[1]*len(l) for l in decks])
      return lil.tocsr()
    
    def prepare(self):
      if self.ready:  return
      self.sparse_decks = self.to_csr(self.all_decks).T # every row = 1 card id 
      self.ready = True
      
    def predict(self, query_deck):
      '''predict possible cards for opponent based on what he played'''
      # convert decks to sparse format
      self.prepare()
      query_deck = [card.id for card in query_deck]
      query_deck = self.to_csr([query_deck])
      
      # compare query with stored decks
      res = query_deck * self.sparse_decks  # similarity of every stored decks with query deck
      res.data **= 2  # square result, so that deck not so similar become even less similar
      
      # now reconstitute cards
      res = self.sparse_decks * res.T
      prediction = res.data / float(res.data.sum())
      return [(collection[res.indptr[i]], prediction[i]) for i in res.indptr]












