import fcgame as fc
import numpy as np
import DDPG
import time

def get_s(name):

    sight = fc.get_car_sight(name)
    wight = len(sight[0])
    hight = len(sight)
    ti = fc.get_ti(name)
    mid = ti['mid']

    while len(mid)<5:
        mid.append(mid[len(mid)-1])
    mid = np.array([mid[i*(len(mid)-1)//6] for i in range(5)])
    mid = mid - np.array([wight/2,0])
    mid = np.array([0,hight]) - mid
    s = np.reshape(mid,10)
    return s

def get_r(name):

    sight = fc.get_car_sight(name)
    wight = len(sight[0])
    hight = len(sight)
    r,ti = 0,fc.get_ti(name)
    mid = ti['mid']

    if sight[mid[0][1]][mid[0][0]]<0.5:
        r = -30
    else :
        r = -np.abs(mid[0][0]-wight/2)
    return r



fc.game_init('FCMAP0.PNG')
fc.add_car('0',(90,255))
fc.set_car('0',(90,255), np.pi/2, 0)
wight = len(fc.get_car_sight('0')[0])
fc.set_time_speed(1)
fc.start()

ddpg = DDPG.DDPG(10,1,np.array([np.pi/3]))
var=3

times = 1

while times:

    start_time = fc.get_time()
    normal_working = 0
    s,a,r,s_ = [],[],[],[]
    
    fc.sleep(100)
    s += [get_s('0')]

    while fc.get_time()-start_time-normal_working < 300 and normal_working < 60000:
    
        p=ddpg.predict(s[-1])
        a += [p[0][0]]
        print('a',*a[-1],'\tq',*p[1][0])
        a[-1] += np.random.normal(a[-1]*np.random.uniform(max(1-np.sqrt(var),0),1), var)
            
        fc.ctrl('0',a[-1],128)

        run_time = fc.get_time()
        while fc.get_time()-run_time < 20:
            if ddpg.frame>512:
                ddpg.fit()
                var*=0.99999
            else:
                fc.sleep(1)

        r += [get_r('0')]
        s_ += [get_s('0')]

        if r[-1] > -30:
            normal_working = fc.get_time()-start_time

        s += [s_[-1]]

    N = 10  
    for pointer in range(len(s_)-1):
        r_ = np.mean(np.array(r[min(len(r)-1,pointer+N//4):min(pointer+N+1,len(r))]))/101
                # - np.var(np.array(a[pointer:min(pointer+N*4+1,len(r))]))
        ddpg.record(s=s[pointer], a=a[pointer], 
        r = r_, 
        s_ = s_[min(pointer + N,len(s_)-1)])
    
    if times%3 ==0 or normal_working>60000:
        fc.map_clear()
    times += 1
    fc.set_car('0',(90+90*(times%2)+np.random.normal(0,8),255), np.pi/2, 128.)
    

    

    