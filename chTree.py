
def triangle(hight, base):
    for i in range(hight, 0, -1):
        str = [" "] * base 
        frst_pos = int(base / 2 - (hight - i) - 1)
        sec_pos = int(base / 2 + (hight - i) + 1) 
        str[frst_pos] = "/" 
        str[sec_pos] = "\\"
        res = ""
        for x in str:
            res += x
        print(res)
    str = [" "] * base
    for i in range(hight):
        


if __name__ == "__main__":
    triangle(10, 100)
    triangle(10, 100)
    triangle(10, 100)