#! /usr/bin/python3.9
from types import new_class
from importlib import __import__

import pygame
pychess = __import__("pychess")


class status:
    def __init__(self, status):
        self.status = status
        #print(self._status)
        self.set_party_attr()
 
    def set_party_attr(self):
        if not self._status[0]:
            self.IS_PARTY_END = False
            self.MOVE_APPLYED = self._status[1]
            self.BLACK_ON_CHECK = self._status[2] 
            self.WHITE_ON_CHECK = self._status[3]
            self.BLACK_LEFT_CASTLING_DENIED = self._status[4]
            self.BLACK_RIGHT_CASTLING_DENIED = self._status[5]
            self.WHITE_LEFT_CASTLING_DENIED = self._status[6]
            self.WHITE_RIGHT_CASTLING_DENIED = self._status[7]
        elif self._status[0]:
            self.IS_PARTY_END = True
            self.IS_SIDE_WHITE = self._status[1]
            self.IS_MATE = self._status[2]
    
    def get_int_status(self): # Tested, all fine 
        ret_int = 0
        if not self.IS_PARTY_END:
            for i, attr in enumerate(self._status[1:]):
                ret_int += int(attr) * 2**(6 - i)
            return ret_int
        else:
            ret_int = 0 
            if self.IS_SIDE_WHITE:
                ret_int |= 0x20
            if self.IS_MATE:
                ret_int |= 0x40
            return -1 * ret_int 


            
    @property
    def status(self):
        return self._status
    
    @status.setter
    def status(self, stat):
        if type(stat) == int:
            if stat >= 0:
                self._status = [False] * 8
                for i in range(7):
                    self._status[7 - i] = bool(stat % 2)
                    stat //= 2

                assert self._status[0] == False               

            else:
                self._status = [False] * 3 
                self._status[0] = True
                if stat & 0x40:     
                    self._status[2] = True 
                if stat & 0x20:
                    self._status[1] = True
        elif type(stat) == list:
            if len(stat) == 8 and not stat[0]: # Party is not end  
                self._status = stat
            elif len(stat) == 3 and stat[0]:
                self._status = stat




class desk:
    def __init__(self, desk = None, load_addres = None): # Load a desk position 
        if load_addres:
            self.desk_positions = self.load(load_addres)
            
        else:
            self.desk_positions = [0]*64
            for i in range(8, 16):
                self.desk_positions[i] = 1 # Pawn
            self.desk_positions[0], self.desk_positions[7] = 4, 4   # Towers 
            self.desk_positions[1], self.desk_positions[6] = 2, 2   # Knights
            self.desk_positions[2], self.desk_positions[5] = 3, 3   # Bishops
            self.desk_positions[3], self.desk_positions[4] = 5, 6   # Queen, king 

            for i in range(48, 56):
                self.desk_positions[i] = -1
            self.desk_positions[56], self.desk_positions[63] = -4, -4 # Towers 
            self.desk_positions[57], self.desk_positions[62] = -2, -2 # Knights
            self.desk_positions[58], self.desk_positions[61] = -3, -3 # Bishops
            self.desk_positions[59], self.desk_positions[60] = -5, -6 # Queen, king
            
            self.status = status([False for _ in range(8)])
            self.next_move_is_white = True
        #self.id = dict(zip(self.positions, [i for i in range(32)])) # Convert a position to id. Updatable when move
        self.position_busy = [True if i == 0 else False for i in self.desk_positions]
        self._move_stack = []
        self._bite_stack = []

        if desk:
            self.desk_positions = desk


    @property
    def move_stack(self):
        return tuple(self._move_stack)

    # Pushed a move into a move stack. Supposed, that move already done, self.desk_position modified, so require a _old num, which directed to old figure at
    def _push(self, position, new_position, _old): 
        self._bite_stack.append(_old)
        self._move_stack.append((position, new_position, self.status.get_int_status()))

    # Delete a move from move stack, change to a previos position self.desk_positions
    def pop(self):
        position, new_position, int_status = self._move_stack.pop()
        self.desk_positions[position] = self.desk_positions[new_position]
        self.desk_positions[new_position] = self._bite_stack.pop()
        self.status = status(int_status)
        return position, new_position, int_status
        
    # Checks the move for right, if true, change the desk_positions, status, push a move stack
    def move(self, position, new_position):
        _old = self.desk_positions[new_position]
        ret = pychess.move(self.desk_positions, position, new_position, self.status.get_int_status())
        self.desk_positions = ret[0]
        self.status = status(ret[1])
        print(self.status.status)
        if not self.status.IS_PARTY_END:
            if self.status.MOVE_APPLYED:
                self._push(position,  new_position, _old)
                return True
        else:
            self._push(position, new_position, _old)
            print("IS LOST SIDE WHITE", self.status.IS_SIDE_WHITE)
            print("IS MATE", self.status.IS_MATE)
            print(self.status.get_int_status())
        return False

    def is_position_bite(self, position, is_white_bite = True):
        return pychess.is_position_bite(self.desk_positions, position, is_white_bite)

    def save(self, addres):
        with open(addres, "wb") as file:
            b = bytearray(65)
            print(self.status.get_int_status())
            status = self.status.get_int_status()  
            if status > 0:
                b[64] = status 
            else:
                b[64] = abs(status) + 127  
            print(b[64])
            i = 0
            while i < len(self.desk_positions):
                if self.desk_positions[i] < 0:
                    b[i] = 128 + self.desk_positions[i]
                else:
                    b[i] = self.desk_positions[i]
                i += 1
            file.write(b) 
        #print(" addres {0} doesn't exsist or permission denided".format(addres))

    def load(self, addres):
        try:
            with open(addres, "rb") as file:
                b_data = bytearray(file.read())
                data = [int(i) for i in b_data]
                for i in range(64):
                    if data[i] < 121:
                        assert data[i] < 7
                        self.desk_positions[i] = data[i]
                    else:
                        assert data[i] > 120
                        self.desk_positions[i] = data[i] - 128
                print("data", int(data[64])) #FIXME

                st = data[64]
                if st >= 128:
                    st -= 127
                    st = -st
                self.status = status(st) 
        except FileNotFoundError:
            print("File not found!")

class pyDesk(desk):
    def __init__(self, addres, pygame, display, board_pos = [0, 0]):

        super().__init__()
        self.pygame = pygame
        self.display = display
        self.h = self.display.get_rect()[1]
        self.board_pos = board_pos
        self.addres = addres # Addres of saved files
        self.board = self.pygame.image.load(addres + "Board.png") # Board image load
        self.board_size = self.board.get_rect().w; assert self.board_size == self.board.get_rect().h
        self.sprites_load() 
        self.screen_positions = dict(zip(range(64), self.get_screen_positions())) # Dict. Take a absolute position, give the screen position of figure. Updatable when move
        self.selected_figure = None # Selected by gamer figure, array with the figure position and figure type (id)
        self.is_figure_selected = False


    def _get_screen_position(self, pos): # get the screen position of figure in decarte coordinats
        board_step = self.board_size // 8
        y = pos // 8
        x = pos - 8 * y
        return x * board_step + self.board_pos[0], y * board_step + self.board_pos[1] - self.board_size 


    def move(self, position, new_position):
        if super().move(position, new_position):
            self.is_figure_selected = False
            return True
        return False


    def sprites_load(self): 
        figures = [None] # None, white_pawn, white_knigth, white_bishop, white_castle, white_queen, white_king, black_king, black_queen, black_tower, black_bishop, black_knigth, black_pawn
        fig_types = ["pawn", "knight", "bishop", "castle", "queen", "king"]
        for figure in fig_types:
                print("Loading {addr}white_{lit}.png".format(addr = self.addres,  lit = figure))
                figures.append(self.pygame.image.load("{addr}white_{lit}.png".format(addr = self.addres, lit = figure)))
        for figure in fig_types[::-1]:
                print("Loading {addr}black_{lit}.png".format(addr = self.addres,  lit = figure))
                figures.append(self.pygame.image.load("{addr}black_{lit}.png".format(addr = self.addres, lit = figure)))
        self.figures_sprites = figures

    def select_figure(self, x:int, y:int):
        cur_pos = self.get_coursor_position(x, y)
        if self.desk_positions[cur_pos] == 0: 
           self.selected_figure = None
           self.is_figure_selected = False
           return
        self.selected_figure = (cur_pos, self.desk_positions[cur_pos])    
        self.is_figure_selected = True


    def get_screen_positions(self):
        screen_positions = []
        for position in range(64):
            screen_positions.append(self._get_screen_position(position))
        return screen_positions

    # Returns absolute [0 - 64] coursor position at board position
    def get_coursor_position(self, x:int, y:int):
        if x >= self.board_pos[0] and x < self.board_size + self.board_pos[0] and y <= self.board_pos[1] and y >= self.board_pos[1] - self.board_size: # The coursor in board field
            x = x - self.board_pos[0]
            y = y - self.board_pos[1] + self.board_size
            
            
            y = y // (self.board_size // 8)
            x = x // (self.board_size // 8)
            return y * 8 + x 
        return 0 
        
    # Visualizing a board and figures in current coordinats 
    def visualize(self):
        h = self.display.get_size()[1]
        r = self.board.get_rect()
        r.x = self.board_pos[0]
        r.y =  h - self.board_pos[1]
        self.display.blit(self.board, r)
        for pos, figure_id in enumerate(self.desk_positions):
            if figure_id != 0:
                x, y = self.screen_positions[pos]
                y = h - y 
                fig_rect = self.figures_sprites[figure_id].get_rect()
                fig_rect.x = x 
                fig_rect.y = y - 75
                self.display.blit(self.figures_sprites[figure_id], fig_rect)
        

        if self.is_figure_selected:
            fig_rect = self.figures_sprites[self.selected_figure[1]].get_rect()
            x, y = self.screen_positions[self.selected_figure[0]]
            fig_rect.x = x 
            fig_rect.y = h - y - 75  
            self.pygame.draw.rect(self.display, (255, 0, 0), fig_rect, 4)


class test:
    
    def prob_space_test(self):
        dsk = desk()
        print("Without error:")
        ret = pychess.space_of_probs(dsk.desk_positions, 1)
        print(ret)

if __name__ == "__main__":
    print("It is a module.")
    t = test()
    t.prob_space_test()
    