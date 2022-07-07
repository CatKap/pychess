import tensorflow as tf
import numpy as np
from PyChess import C_API as cp


@tf.Module
class network(Module):
    pass

def __call__(self, position):
    return 0


def save(self, path):
    pass

def load(self, path):
    pass

class agent:

    def __init__(self, side, depth, positions):
        self.side = side
        self.depth = depth
        self.__Caddres = 0x0 # Contain a C structure addres, for optimization
        self.network = network
        self.positions = np.array(positions)

    
    def cost_func(self, positions, who_move):
        return self.network(positions)



