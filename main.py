#!/usr/bin/env python3.10
import pygame 
from simpleui import *
from PyChess import pyDesk 
from sys import exit 
from os.path import abspath

class Game:
    def __init__(self, w, h, b_x, b_y, board_addres = None, debug = False):
        pygame.init()
        self.is_run = True
        self.w = w
        self.h = h
        self.path = abspath("./")
        print("Set gamepath to ", self.path)
        self.localization = {
                "start" : "Start",
                "quit" : "Quit",
                "settings" : "Settings"
                }

        self.__DEBUG = debug
        self.display = pygame.display.set_mode((w, h))
        self.gui = root(self.display)
        self.pyDesk = pyDesk(pygame = pygame, addres = f"{self.path}/materials/png/", display = self.display, board_pos = (b_x, b_y), debug = self.__DEBUG)
        if board_addres:
            self.pyDesk.load(board_addres)
            

    def run(self):
        start_scene, settigns_scene = self.gui.new_scene() 
        font = pygame.font.Font(f"{self.path}/materials/fonts/static/Montserrat-Medium.ttf", 40)
        chk_font = pygame.font.Font(f"{self.path}/materials/fonts/checkfont.otf", 19)

        menu_bnts_conf = buttonConf(
                font = font,
                position = ((self.w - 200)/2, 100),
                bg_colour = (14, 59, 67), #(0, 0, 0),
                text_colour = (255, 255, 255),
                size = (200, 50),
                scale = 1.0
                )

        clk = pygame.time.Clock()
        resaddres = "/home/kapitan/Progects/Chess/materials/menu/"
        menu = {
                self.localization["start"] : self.debug,
                self.localization["settings"] : settigns_scene,
                self.localization["quit"] :  exit,
                }
        
        start_scene()
        smn = game_menu(menu, menu_bnts_conf, gap = 10)
        settigns_scene() 
        assert self.gui.scene is settigns_scene
        test_chk = checkbox((100, 100), "✓", chk_font, size = (20, 22)) 
        test_chk2 = checkbox((100, 150), "✓", chk_font, size = (20, 22)) 
        test_chk3 = checkbox((100, 200), "✓", chk_font, size = (20, 22)) 
        test_chk3 = checkbox((100, 1000), "✓", chk_font, size = (20, 22)) 
        self.test_text = text_field((100, 300),  100, "input here")
        scr = scroll_box(alwase_visible = True)

        start_scene()
        while self.is_run:
            events = pygame.event.get()
            clk.tick(25)
            self.display.fill((255, 255, 255))
            self.gui.update()
            self.global_events(events)
            pygame.display.flip()

    def global_events(self, events):
        for event in events:
            if event.type == pygame.QUIT:
                self.is_run = False

    def test(self):
        print(self.test_text.input)
        self.test_text.input_clear()
        self.test_text.err_state = True

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
            self.pyDesk.visualize()
            clk.tick(25)
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
                        self.pyDesk.pop()
                         
            if mouse_right_flag:
                event.pos = pygame.mouse.get_pos()
                x, y = tuple(event.pos)
                y = h - y
                ret = self.pyDesk.is_position_bite(self.pyDesk.get_coursor_position(x, y))
                if ret[0]:
                    print("Position on selection ({0}) is bite by following positions:".format(self.pyDesk.get_coursor_position(x, y)), ret[1:])
                else:
                    print("Position on selection ({0}) is not bite.".format(self.pyDesk.get_coursor_position(x, y)))

            if mouse_left_flag:
                event.pos = pygame.mouse.get_pos()
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
            
            for pos in range(64):
                if self.pyDesk.desk_positions[pos] <= 0:
                    if self.pyDesk.is_figure_selected:
                        bitearr = self.pyDesk.is_position_bite(pos, True)
                        if  bitearr[0] and self.pyDesk.selected_figure[0] in bitearr[1:]:
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
    game = Game(1000, 800, 0, 800, board_addres="/home/kapitan/Progects/Chess/Saves/last_save.chsv", debug = True)  
    print(game.pyDesk.is_position_bite(55))
    game.run()
