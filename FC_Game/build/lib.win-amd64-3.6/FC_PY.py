import fc
import cv2
import numpy as np

fc.game_init("FCMAP1.PNG")
fc.add_car("0",(380,50))

sight=fc.get_car_sight("0")
wight=len(sight[0])

#fc.show_car_sight("0")

fc.start()
while 1:
    print(fc.get_time())
    
    mid=fc.get_ti("0")["mid"]
    degree=0.04*(mid[0][0]-wight/2)+0.04*(mid[len(mid)//3][0]-wight/2)
    fc.ctrl("0",degree,100)
    cv2.imshow("0",np.array(fc.get_car_sight("0")))
    fc.sleep(1)
