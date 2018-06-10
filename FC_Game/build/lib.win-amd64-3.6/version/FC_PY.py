import fcgame as fc
import numpy as np
import DDPG
import time

def get_s(name):
    sight = fc.get_car_sight(name)
    ti = fc.get_ti(name)
    mid = ti['mid']
    while len(mid)<5:
        mid.append(mid[len(mid)-1])
    mid = np.array([mid[i*(len(mid)-1)//5] for i in range(5)])
    mid=mid-np.array([len(fc.get_car_sight(name)[0])/2,0])
    s = np.reshape(mid,10)

    # right = ti['right']
    # left = ti['left']

    # if len(left) == 0:
    #     left.append([0,0])
    # if len(right) == 0:
    #     right.append([wight-1,hight-1])
    # while len(right)<5:
    #     right.append(right[len(right)-1])
    # while len(left)<5:
    #     left.append(left[len(left)-1])

    # right = np.array([right[i*(len(right)-1)//5] for i in range(5)])
    # left = np.array([left[i*(len(left)-1)//5] for i in range(5)])
    # s = np.concatenate([np.reshape(right,10), np.reshape(left,10)], 0)
    return s

def get_r(name):
    r,ti = 0,fc.get_ti(name)
    mid = ti['mid']
    sight = fc.get_car_sight(name)
    if sight[mid[0][1]][mid[0][0]]<0.5:
        r = -1000
    else :
        r = -np.square(mid[0][0]-len(fc.get_car_sight(name)[0])/2)
    
    #r -= np.square((100-fc.get_velocity(name))/50)
    return r



fc.game_init('FCMAP0.PNG')
fc.add_car('0',(430,300))
fc.set_car('0',(430,300), np.pi/2, 0)
wight = len(fc.get_car_sight('0')[0])
hight = len(fc.get_car_sight('0'))
fc.set_time_speed(1)
fc.start()

ddpg = DDPG.DDPG(10,1,np.array([np.pi/3]))
var=np.pi/3

times = 1

while times:

    break_count=0
    s,a,r,s_,pointer = [],[],[],[],0
    
    fc.sleep(100)
    start_time = fc.get_time()
    s += [get_s('0')]

    while break_count < 5:
    
        a += [ddpg.predict(s[-1])]
        print('a',*a[-1])
        a[-1] += np.random.normal(a[-1], var)
            
        fc.ctrl('0',*a[-1],100)

        run_time = fc.get_time()
        while fc.get_time()-run_time < 100:
            if ddpg.frame>500:
                ddpg.fit()
            else:
                fc.sleep(1)

        if ddpg.frame>500:
            var*=.999
            print('var',var)

        r += [get_r('0')-np.square(*a[-1]*2)]
        s_ += [get_s('0')]
        if r[-1] <= -1000:
            break_count += 1
        else:
            break_count = 0

        ddpg.record(s[pointer], a[pointer], r[-1]/100, s_[-1])
        pointer += 1

        s += [s_[-1]]

        print('a',*a[-1],'\t','r',r[-1],'\t','frame',ddpg.frame)
        print('v',fc.get_velocity('0'),'\n')
    
    fc.set_car('0',(430+np.random.normal(0,5),300), np.pi/2, 0.)
    times += 1

    if times%20 ==0 :
        fc.map_clear()
    # fit_time = time.time()
    # while time.time()-fit_time < 5:
    #     ddpg.fit()

    