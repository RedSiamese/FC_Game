import fcgame as fc
import numpy as np
import MDDPG
import time

def sigmoid_(x):
    return 1/(1+np.exp(-x))

def random_loc(v):
    loc=[[90,255,np.pi/2],
    [180,255,np.pi/2],
    [330,180,0],
    [430,305,-np.pi/2],
    [430,305,np.pi/2],
    [230,60,0],
    [230,60,np.pi]]
    p = int(np.random.uniform(0,0.99)*7)
    return (loc[p][0]+np.random.normal(0, 5), loc[p][1]+np.random.normal(0, 5)), loc[p][2]+np.random.normal(0, 0.1),v

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
    s = np.concatenate([s,np.array([fc.get_velocity(name)])],0)
    return s

def get_r(name):

    sight = fc.get_car_sight(name)
    wight = len(sight[0])
    hight = len(sight)
    r,ti = 0,fc.get_ti(name)
    mid = ti['mid']

    if sight[mid[0][1]][mid[0][0]]<0.5:
        r = -35
    else :
        r = -np.abs(mid[0][0]-wight/2)
    return r



FRAME_SPACING=20

fc.game_init('FCMAP0.PNG')
fc.add_car('0',(90,255))
fc.set_car('0',(90,255), np.pi/2, 0)
#fc.show_car_sight('0')
wight = len(fc.get_car_sight('0')[0])
fc.set_time_speed(0)

ddpg = MDDPG.DDPG(11, 2, 2,np.array([np.pi/3, 256]))
var=np.array([np.pi/3,100])

times = 1
fc.start()
while times:

    start_time = fc.get_time()
    normal_working = 0
    s,a,r,s_ = [],[],[],[]
    
    if times%10 == 1:
        var+=np.array([np.pi/3,100])/5

    s += [get_s('0')]

    while  abs((fc.get_time()-start_time-normal_working)*fc.get_velocity('0')/100000) < 0.3 and abs(normal_working*fc.get_velocity('0')/100000) < 60:
        p=ddpg.predict(s[-1])
        a += [p[0]]
        if ddpg.frame<ddpg.REPLAY_CAPACITY/8:
            a[-1] = np.array([0.,100.])
        a[-1] = np.random.normal(a[-1]*np.random.uniform(max(1-np.sqrt(var[0]),0),2), var,2)

        fc.ctrl('0',*(a[-1]))
        run_time = fc.get_time()
        while fc.get_time()-run_time < FRAME_SPACING:
            if ddpg.frame>ddpg.REPLAY_CAPACITY/8:
                ddpg.fit()
                var*=0.9999
            else:
                fc.sleep(1)

        r += [get_r('0')]
        s_ += [get_s('0')]

        if r[-1] > -30:
            normal_working = fc.get_time()-start_time

        s += [s_[-1]]

    at = np.mean(np.array(s)[:min(len(s),40),10])
    mv = np.mean(np.array(s)[:,10])

    LAST_FRAME=100
    if ddpg.frame>ddpg.REPLAY_CAPACITY/8:
        s,a,r,s_ = s[max(0,len(s)-LAST_FRAME):],a[max(0,len(a)-LAST_FRAME):],r[max(0,len(r)-LAST_FRAME):],s_[max(0,len(s_)-LAST_FRAME):]
    N, ep_reward = 5, 0
    for pointer in range(len(s_)-1):
        r_ =  [np.mean(np.array(r[min(len(r)-1,pointer+N//4):min(pointer+N+1,len(r))]))/49, 
                -(1-sigmoid_((np.mean(np.array(s_[min(len(s_)-1,pointer+N//4):min(pointer+N+1,len(s_))])[:,10]))/64))]
        ddpg.record(s=s[pointer], a=a[pointer], r = r_, s_ = s_[min(pointer + N,len(s_)-1)])
        ep_reward += np.array(r_)
        print(r_)

    print("mr", ep_reward/(normal_working+1)*FRAME_SPACING, "at", int(at), "mv", int(mv))
    if ddpg.frame>ddpg.REPLAY_CAPACITY/8:
        fc.set_time_speed(1)

    if times%5 ==0 or normal_working/100000*fc.get_velocity('0') > 30:
        fc.map_clear()
    times += 1
    fc.set_car('0',*random_loc(0.))
    

    

    