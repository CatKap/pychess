import pygame, pygame.gfxdraw
from collections import namedtuple

class scene:
    def __init__(self, root):
        self.root = root
        self.__gui_buffer = []     

    def __call__(self):
        self.root.gui_objects = self.gui_buffer
        self.root.scene = self

    def append(self, obj):
        self.__gui_buffer.append(obj)
            
    @property
    def gui_buffer(self):
        return tuple(self.__gui_buffer)


class root:
    # Global variables for all gui's
    display  = None
    __gui_objects = [] # Objects thats visualizing immideatly
    names = {}
    mouse_pos = (0, 0)
    _scene = None
    #scroll_enable = False
    
    def __init__(self, display, mouse_btn = 0, scroll_box_parametrs = None):
        root.display = display
        self.mouse_btn = mouse_btn
        root._scene = scene(self)
        #if not scroll_box_parametrs:
        #    self.scr_pr = ()
        
    def update(self):
        root.mouse_pos = pygame.mouse.get_pos() 
        for objct in self.__gui_objects:
            if objct.visible:
                objct.visualize()
            if objct.animate:
                for num, animation in enumerate(objct.animations):
                    if not next(animation):
                        objct.animations.pop(num)
                objct.animate = bool(len(objct.animations))
            elif objct.active:
                objct.logic_update()

    def clear(self):
        root.__gui_objects = []

    def append_global_object(self, objct, name = None):
        #if objct.y > self.display.get_height() and not scroll_enable:
        #    root.scroll_enable = True
        #    self.scrol_box = scrol_box(self.scr_pr)
        root.__gui_objects.append(objct)
        if name:
            self.names.update({name : objct})
        root._scene.append(objct)

    def new_scene(self): # This function return callable scene class all objects thats will be added after call of this function will be add to the scene     
        old = self.scene
        self.clear()
        root._scene = scene(self)
        return old, self.scene
    

    @property
    def gui_objects(self):
        return tuple(self.__gui_objects)

    @gui_objects.setter
    def gui_objects(self, other):
        print("Change gui scene to", other)
        root.__gui_objects = list(other)

    @property
    def scene(self):
        return root._scene

    @scene.setter
    def scene(self, scn):
        root._scene = scn 
        print("ROOT: Change scene to", scn)

    @staticmethod
    def mouse_on_rect(pos, rect):
        if pos[0] >= rect.x and pos[1] >= rect.y and pos[0] <= rect.x + rect.w and pos[1] <= rect.y + rect.h:
            return True



class element(root):
    def __init__(self, position):
        super().append_global_object(self)
        self.x, self.y = position
        self.animate = False
        self.animations = []
        self.visible = True
        self.active = True

    @property
    def position(self):
        return self.x, self.y

    @position.setter
    def position(self, pos):
        self.x, self.y = pos

    @property
    def animation(self):
        return self.animate

    @animation.setter
    def animation(self, generator):
        self.animations.append(generator)
        self.animate = True

    @property
    def is_mouse_on_box(self):
        pass

buttonConf = namedtuple("buttonConf", ["font", "position", "bg_colour", "text_colour", "size", "scale"])

class button(element):
    def __init__(self, position, text,  button_func, font, bg_colour = (0, 0, 0), text_colour = (255, 255, 255), size = (100, 50), scale = 1):
        super().__init__(position)
        assert type(text) == str
        assert type(font) == pygame.font.Font
        assert len(bg_colour) == 3

        self.text = text
        self.func = button_func
        self.bg_colour = bg_colour
        self.text_colour = text_colour
        self.w = size[0] * scale
        self.h = size[1] * scale
        self.x, self.y = position
        self.__scale = scale
        self.font = font
        self.mouse_flag = False
        self.start_w = self.w
        self.start_h = self.h
        self.start_bg = self.bg_colour
        self.start_rect = pygame.rect.Rect(self.x, self.y, self.w, self.h)
        self.pressed = False
        self.execute = False
        self.border_radius = 8 

        # Scale animation
        self.animation_func = self.scale_animation
        self.when_select_animation_parametrs = namedtuple("when_select_animation_parametrs", ["stop_scale", "step"])(1.1, 0.08)
        self.when_choise_animation_parametrs = namedtuple("when_choise_animation_parametrs", ["stop_scale", "step", "choise_bg_colour"])(0.9, 1, (0x7C, 0x72, 0x87))

    def visualize(self):
        rect = pygame.rect.Rect(self.x - (self.w - self.start_w) // 2, self.y - (self.h - self.start_h) // 2, self.w, self.h) 
        text_im = self.font.render(self.text, True, self.text_colour)
        im_rect = text_im.get_rect()
        pygame.draw.rect(self.display, self.bg_colour, rect, border_radius = self.border_radius) 
        im_rect.center = rect.center
        self.display.blit(text_im, im_rect)

    def logic_update(self):
        if self.is_mouse_on_button():
            if not self.mouse_flag:
                self.mouse_flag = True
                print("call start")
                self.animation = self.on_see_animation()   #self.animation = self.animation_func(*self.when_select_animation_parametrs)
        else:
            if self.mouse_flag:
                self.mouse_flag = False
                if self.bg_colour != self.start_bg or self.w != self.start_w:
                    self.restore()

        if self.pressed and not self.execute:
            print("\nCALL FUNC")
            self.execute = True
            self.restore()
            self.func()
            self.pressed = False
            return True 
 
        if self.is_mouse_on_button() and pygame.mouse.get_pressed()[0] and not self.pressed and not self.execute:
            self.pressed = True
            self.execute = False
            self.animation = self.press_animation()            
            return 
 
    def scale_animation(self, stop_scale, iterations):
        step = (stop_scale - self.scale)/ iterations
        while iterations > 0:
            self.scale += step
            yield True 
            iterations -= 1
        yield False

    def colour_animation(self, stop_colour, iterations):
        distanses = [stop_colour[i] - self.bg_colour[i] for i in range(3)]
        steps = [d / iterations for d in distanses]
        while iterations > 0:
            self.bg_colour = [self.bg_colour[i] + steps[i] for i in range(3)]
            yield True
            iterations -= 1
        yield False


    def on_see_animation(self):
        return self.colour_animation((0x7C, 0x72, 0x87), 5)

    def press_animation(self):
        return self.scale_animation(self.when_choise_animation_parametrs.stop_scale, self.when_choise_animation_parametrs.step)
        

    @property
    def scale(self):
        return self.__scale

    @scale.setter
    def scale(self, value:float):
        self.w = self.start_w * value
        self.h = self.start_h * value
        self.__scale = value

    @property
    def size(self):
        return self.w, self.h

    @size.setter
    def size(self, value):
        self.w, self.h = value


    def is_mouse_on_button(self):
        m_pos = self.mouse_pos
        if m_pos[0] >= self.x and m_pos[0] <= self.x + self.w:
            if m_pos[1] >= self.y and m_pos[1] <= self.y + self.h:
                return True
        return False

    def restore(self):
        self.animation = self.colour_animation(self.start_bg, 5)
        self.mouse_flag = False 
        self.execute = False
            
class game_menu:
    def __init__(self, funcs, btnconf, gap = 100):
        self.funcs = funcs
        self.__keys = funcs.keys()
        i = 0
        self.buttons = []
        for k in self.__keys:
            self.buttons.append(button((btnconf.position[0], btnconf.position[1] + (i * (gap + btnconf.size[1]))), k, self.funcs[k], btnconf.font, btnconf.bg_colour, btnconf.text_colour, btnconf.size, btnconf.scale))
            i += 1

    def update(self):
        ret = False
        for btn in self.buttons:
            ret = ret or btn.update()
        return ret 

class checkbox(button):

    def __init__(self, position, btn_text = "✓", font = None, bg_active_colour = (14, 59, 67), bg_inactive_colour = (120, 120, 120), text_active_colour = (255, 255, 255), text_inactive_colour = (120, 120, 120), size = (20, 20), scale = 1):
        self.is_checked = False
        super().__init__(position, btn_text, self.check_change, font, bg_inactive_colour, text_inactive_colour, size, scale)
        self.bg_active_colour = bg_active_colour
        self.bg_inactive_colour = bg_inactive_colour
        self.text = btn_text
        self.text_active_colour = text_active_colour
        self.text_inactive_colour = text_inactive_colour
        self.anims_steps = 3 
        self.see_scale = 1.2
        self.text_colour = text_inactive_colour
        if not font:
            font = pygame.font.SysFont("FiraCode", 20)
        self.border_radius = 2


    def on_see_animation(self):
        return self.scale_animation(self.see_scale, self.anims_steps)

    def press_animation(self):
        self.animation = self.text_colour_animation(self.text_active_colour, self.anims_steps)
        return self.colour_animation(self.bg_active_colour, self.anims_steps) 

    def restore(self):
        self.execute = False
        self.pressed = False
        self.animation = self.scale_animation(1.0, self.anims_steps)

    def check_change(self):
        if self.is_checked:
            self.animation = self.colour_animation(self.bg_inactive_colour, self.anims_steps)
            self.animation = self.text_colour_animation(self.text_inactive_colour, self.anims_steps)
            self.is_checked = False
        else:
            self.is_checked = True

    def text_colour_animation(self, stop_colour, steps):
        colour_step = [(stop_colour[i] - self.text_colour[i]) / steps for i in range(3)]
        while steps > 0:
            self.text_colour = [self.text_colour[i] + colour_step[i] for i in range(3)]
            steps -= 1
            yield True
        yield False


class text_field(element):

    def __init__(self, position, lenth, inactive_msg, font_name = "FiraCode", font_size = 20, inactive_colour = (120, 120, 120), active_colour = (0, 0, 0), err_colour = (255, 0, 0), err_msg = "error"):
        super().__init__(position)
        self.lenth = lenth
        self.inactive_msg = inactive_msg
        self.inactive_colour = inactive_colour

        self.font = pygame.font.SysFont(font_name, font_size)
        self.inactive_font = pygame.font.SysFont(font_name, int(font_size * 0.7))
        self.err_font = pygame.font.SysFont(font_name, int(font_size * 0.4))
        
        self.active_colour = active_colour
        self.err_colour = err_colour
        self.bg_colour = self.inactive_colour
        self.text_colour = self.inactive_colour
        self.anim_steps = 5 
        self.borders = 5 
        self.active_state = False
        self.err_state = False
        self.__input = [] 
        self.cursor_position = 0
        self.text_w, self.text_h = self.font.size("T")
        self.click_rect = pygame.Rect(self.x, self.y, self.text_w * lenth + self.borders * 2, self.text_h)
        self.linepos = (self.x, self.y + self.text_h), (self.x + self.lenth * self.text_w + self.borders * 2, self.y + self.text_h)
        self.err_msg = err_msg 
        self._b = 2
        self.line_width = 2 
        self.couror_ticks = 8 
        self.courosor_visible = True
        self._cr_counter = self.couror_ticks

  
    def visualize(self):
        pygame.draw.line(self.display, self.bg_colour, self.linepos[0], self.linepos[1], self.line_width)
        # Draw cursor and input
        if self.active_state:
            text_im = self.font.render("".join(self.__input), self.bg_colour, True)
            # Cursor render here
            if self.courosor_visible:
                pygame.draw.line(self.display, self.bg_colour, (self.x + len(self.__input) * self.text_w + self.borders, self.y - self._b), (self.x + len(self.__input) * self.text_w + self.borders, self.y + self.text_h - self._b), self.line_width)
            if self._cr_counter > 0:
                self._cr_counter -= 1
            else:
                self.courosor_visible = not self.courosor_visible
                self._cr_counter = self.couror_ticks

        else:
            if self.err_state:
                print(self.bg_colour)
                err_text = self.err_font.render(self.err_msg, self.bg_colour, True)
                err_rect = err_text.get_rect()
                err_rect.x = self.linepos[0][0] + self._b
                err_rect.y = self.linepos[0][1] + self._b
                self.display.blit(err_text, err_rect)
                return 

            if len(self.__input) == 0:
                text_im = self.inactive_font.render(self.inactive_msg, self.text_colour, True)
            else:
                text_im = self.font.render("".join(self.__input), self.bg_colour, True)

        text_rect = text_im.get_rect()
        text_rect.x = self.x + self.borders / 2
        if self.active_state:
            text_rect.y = self.y + self._b 
        else:
            text_rect.y = self.y + self._b + (self.text_h - text_rect.h) 
        self.display.blit(text_im, text_rect)
        

    def logic_update(self):
        if pygame.mouse.get_pressed()[0]: 
            if self.mouse_on_field():
                if self.active_state:
                    self.active_state = False
                    self.animation = button.colour_animation(self, self.inactive_colour, self.anim_steps)
                else:
                    self.active_state = True
                    self.animation = button.colour_animation(self, self.active_colour, self.anim_steps)
            else:
                if self.active_state:
                    self.active_state = False
                    self.animation = button.colour_animation(self, self.inactive_colour, self.anim_steps)

        # Key cath there 
        if self.active_state:
            for event in pygame.event.get():
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_BACKSPACE and len(self.__input) >= 0:
                        self.__input = self.__input[:-1]
                    elif event.key == pygame.K_ESCAPE or event.key == pygame.K_RETURN:
                        self.active_state = False
                        self.animation = button.colour_animation(self, self.inactive_colour, self.anim_steps)

                    else:
                        if len(self.__input) < self.lenth:
                            self.__input += event.unicode
        if self.err_state:
            self.animation = button.colour_animation(self, self.err_colour, self.anim_steps)

    def mouse_on_field(self):
        if self.mouse_pos[0] <= self.click_rect.x + self.click_rect.width and self.mouse_pos[0] >= self.click_rect.x and self.mouse_pos[1] >= self.click_rect.y and self.mouse_pos[1] <= self.click_rect.y + self.click_rect.h:
            return True
        return False

    @property
    def input(self):
        return "".join(self.__input)
    
    def input_clear(self):
        self.__input = []
        self.active_state = False

# Scroll box applyed to all elemets of scene
class scroll_box(root): # Work on whole screen. 
    def __init__(self, width = 10, alwase_visible = False, opacity = 80, scroll_lenth = 1000):
        super().append_global_object(self)
        self._b = 0.3
        self.screen_size = self.display.get_width(), self.display.get_height()
        self.position = (self.screen_size[0] - width, 0) 
        self.scroll_lenth = scroll_lenth
        self.visible = alwase_visible
        self.line = pygame.Rect(self.position[0], self.position[1], width, self.screen_size[1]) 
        self.careete = pygame.Rect(self.position[0], self.position[1], width, scroll_lenth * 0.3)
        self.line_colour = (255, 0, 0)
        self.careete_colour = (155, 155, 155)
        self.animate = False
        self.active = True

    def visualize(self): # TODO - visualize lines and scroll while mouse on rect 
        if self.visible:
            pygame.draw.rect(self.display, self.line_colour, self.line)
            pygame.draw.rect(self.display, self.careete_colour, self.careete)
              
            



    def logic_update(self): # TODO - updates ajust and apply it to the objects 
        pass
    

if __name__ == "__main__":
    pass

