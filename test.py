import pychess
from PyChess import desk, status

mate_desk1 = [0] * 64
mate_desk1[0] = 4
mate_desk1[49] = 4
mate_desk1[63] = -6
mate_desk1[7] = 6


def test1():    
    d = desk()
    d.load("/home/kapitan/Progects/Chess/Saves/last_save.chsv")
    for i in range(100):    
        d.move(59, 58)
        print(d.is_position_bite(58))
        d.move(58, 59)
    

def test2():
    for i in range(1000000):
        d = desk()
        for x in range(56, 63):
            assert not d.is_position_bite(x)

def test3():
    d = desk()
    for i in range(10000):
        for x in range(64):
            print(d.is_position_bite(x))

def test4(): #
    d = desk()
    d.desk_positions = [0]*64
    d.desk_positions[0] = -4
    print(d.is_position_bite(24, False))
    print()




def status_test():
    for i in range(-127, 0):
        s = status(i)     
        print(i, s.get_int_status())

if __name__ == "__main__":
   s = status(124) 
   print(s.get_int_status())
   print(s._status)
   status_test()