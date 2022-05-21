import tensorflow as tf
import numpy as np
from PyChess import C_API as cp

class agent:

    def __init__(self, side, depth):
        self.side = side
        self.depth = depth
        self.__Caddres = 0x0 # Contain a C structure addres, for optimization

    
    def cost_func(self, positions, who_move):
        return network(positions)


@tf.Module
class network:
    pass

def __call__(self, position):
    return 0


def save(self, path):
    pass

def load(self, path):
    pass

