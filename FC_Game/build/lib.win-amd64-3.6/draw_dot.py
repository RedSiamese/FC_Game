# import fcgame as fc
import numpy as np
import cv2
#import tensorflow as tf

def H(x):
    shape = np.prod(np.shape(x))
    x = np.clip(np.reshape(x, shape), 10e-20, 1)
    x = np.mean(np.square(x-np.argsort(x)/shape))
    return x



a1=np.random.normal(0.5, 100000,1000000)
a2=np.random.uniform(0, 1,1000000)
a3=np.random.normal(0.5, 0.000001,1000000)

print(H(a1),H(a2),H(a3))



# import DDPG
# import time

# map=np.array([[0.]*1300]*700)
# for i in range(1300):
#     at = 1/(1+np.exp(-(i-650)/50))-1/(1+np.exp((i-650)/50))
#     map[350-int(at*250),i]=0.9
#     print(i,500-int(at*250))
# #map=np.reshape(map,(1300,700))
# cv2.imshow('0',map)
# cv2.waitKey()
# cv2.waitKey()