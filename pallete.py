import pygame as pg
from simpleui import root


class settings:
    WIDTH = 500
    HEIGHT = 400

settings = settings()



def main():
    is_run =  True
    screen = pg.display.set_mode((settings.WIDTH, settings.HEIGHT))

    gui = root(screen)

    while is_run:
        gui.update()
        for event in pg.event.get():
            if event.type == pg.QUIT:
                self.is_run = False
            pg.display.flip()
            
    
if __name__=="__main__":
    main()
       
    


