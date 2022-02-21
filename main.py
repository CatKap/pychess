#!/usr/bin/env python3
import pygame 
from PyChess import pyDesk 


class Game:
    def __init__(self, w, h, b_x, b_y, board_addres = None):
        pygame.init()
        self.display = pygame.display.set_mode((w, h))
        self.pyDesk = pyDesk(pygame = pygame, addres = "/home/kapitan/Progects/Chess/materials/png/", display = self.display, board_pos = (b_x, b_y))
        if board_addres:
            self.pyDesk.load(board_addres)
        
    def run():
        pass

    def debug(self):
        bite_im = pygame.image.load(self.pyDesk.addres + "bite_rect.png")
        clk = pygame.time.Clock()
        is_run = True
        mouse_left_flag = False
        mouse_right_flag = False
        save_flag = False
        fnt = pygame.font.SysFont("silom.ttf", 24)
        h = self.display.get_height()
        while is_run:
            clk.tick(30)
            self.pyDesk.visualize()
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    is_run = False
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    if event.button == 1:
                        mouse_left_flag = True
                    elif event.button == 3:
                        mouse_right_flag = True
                elif event.type == pygame.MOUSEBUTTONUP:
                    if event.button == 1:
                        mouse_left_flag = False
                    elif event.button == 3:
                        mouse_right_flag = False
                elif event.type == pygame.KEYDOWN:
                    if event.key == 115: #s
                        save_flag = True
                    elif event.key == 122: #z
                        print("AAAAA")
                        self.pyDesk.pop()
                         
            if mouse_right_flag:
                x, y = tuple(event.pos)
                y = h - y
                ret = self.pyDesk.is_position_bite(self.pyDesk.get_coursor_position(x, y))
                if ret[0]:
                    print("Position on selection ({0}) is bite by following positions:".format(self.pyDesk.get_coursor_position(x, y)), ret[1:])
                else:
                    print("Position on selection ({0}) is not bite.".format(self.pyDesk.get_coursor_position(x, y)))

            if mouse_left_flag:
                x, y = tuple(event.pos)
                y = h - y
                im1 = fnt.render("Absolute: {x}, {y}, {pos}".format(x = event.pos[0], y = h - event.pos[1], pos = self.pyDesk.get_coursor_position(x, y)) , True, (255, 0, 0))
                r1 = im1.get_rect()
                r1.x = event.pos[0]
                r1.y = event.pos[1]
                self.display.blit(im1, r1)
                if self.pyDesk.is_figure_selected:
                    if self.pyDesk.move(self.pyDesk.selected_figure[0], self.pyDesk.get_coursor_position(x, y)):
                        self.pyDesk.is_figure_selected = False 
                    mouse_left_flag = False
                self.pyDesk.select_figure(x, y)            
            
            for pos in range(0):
                if self.pyDesk.is_position_bite(pos, True)[0]:
                    x, y = self.pyDesk.screen_positions[pos]
                    r = bite_im.get_rect()
                    r.x = x
                    r.y = h - y - 75
                    self.display.blit(bite_im, r)
            if save_flag:
                self.pyDesk.save("/home/kapitan/Progects/Chess/Saves/last_save.chsv")
                save_flag = False
            pygame.display.flip()                
        pygame.quit()

if __name__ == "__main__":
    game = Game(600, 600, 0, 600, board_addres="/home/kapitan/Progects/Chess/Saves/last_save.chsv")  
   # game.pyDesk.desk_positions = [0] * 64
   # game.pyDesk.desk_positions[45] = 6
   # game.pyDesk.desk_positions[1] = 5
   # game.pyDesk.desk_positions[61] = -6
    print(game.pyDesk.is_position_bite(55))
    game.debug()