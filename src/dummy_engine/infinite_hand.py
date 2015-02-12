import sys, os


class Creature:
    def __init__(self, cost, atk, hlt):
        self.cost = cost
        self.attack = atk
        self.health = hlt
        self.damage_taken = 0

class State:
    def __init__(self):
        self.creatures = []
