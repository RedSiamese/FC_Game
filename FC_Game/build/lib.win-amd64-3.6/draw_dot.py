import fcgame as fc
import numpy as np
import DDPG
import time


fc.game_init('FCMAP0.PNG')
fc.add_car('0',(90,255))
fc.set_car('0',(90,255), np.pi/2, 0)
wight = len(fc.get_car_sight('0')[0])
fc.set_time_speed(1)
fc.start()