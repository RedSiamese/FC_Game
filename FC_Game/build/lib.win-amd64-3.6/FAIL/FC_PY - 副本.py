import fcgame as fc
import cv2
import numpy as np

import tensorflow as tf 
import tflearn as tfl


def ctrl_net(c_in, state, reuse=False):
    with tf.variable_scope('ctrl',reuse=reuse):
        # c_in=tfl.conv_2d(c_in,8,[2,1], activation='relu')
        # c_in=tfl.conv_2d(c_in,8,[2,1], activation='relu')
        #c_in=tfl.conv_2d(c_in,8,[2,1], activation='relu')
        #c_in=tfl.conv_2d(c_in,8,[2,1], activation='relu')
        #c_in=tf.reshape(c_in,[-1,np.prod(np.array(c_in.get_shape().as_list()[1:]))])
        c_in=tfl.fully_connected(c_in,64,'tanh')
        c_in=tf.concat([c_in, state],1)
        c_in=tfl.fully_connected(c_in,256,'tanh')
        c_out=tfl.fully_connected(c_in,2,'sigmoid')
    return c_out


def value_net(v_in, state, ctrl, reuse=False):
    with tf.variable_scope('value',reuse=reuse):
        # v_in=tfl.conv_2d(v_in,8,[2,1], activation='relu')
        # v_in=tfl.conv_2d(v_in,8,[2,1], activation='relu')
        #v_in=tfl.conv_2d(v_in,8,[2,1], activation='relu')
        #v_in=tfl.conv_2d(v_in,8,[2,1], activation='relu')
        #v_in=tf.reshape(v_in,[-1,np.prod(np.array(v_in.get_shape().as_list()[1:]))])
        v_in=tfl.fully_connected(v_in,64,'tanh')
        v_in=tf.concat([v_in, state, ctrl],1)
        v_in=tfl.fully_connected(v_in,256,'tanh')
        v_out=tfl.fully_connected(v_in,1,'sigmoid')
    return v_out


input_left=tfl.input_data([None,5,2])
input_right=tfl.input_data([None,5,2])
input_state=tfl.input_data([None,2])

input_example_ctrl=tfl.input_data([None,2])

input_map=tf.concat([input_left,input_right],2)
input_map=tf.reshape(input_map,[-1,5,4,1])


ctrl = ctrl_net(input_map, input_state)
value_example = value_net(input_map, input_state, input_example_ctrl)
value=value_net(input_map, input_state, ctrl, reuse=True)


#评分(越大越好)
y_example=tf.placeholder(tf.float32,[None,1])

value_loss = tf.reduce_mean(tf.square(y_example - value_example))
ctrl_loss = tf.reduce_mean(tf.square(1-value))


value_vars = tfl.get_layer_variables_by_scope('value')
value_reg=tfl.regression(value_example, placeholder = y_example, optimizer='adam',
loss=value_loss, trainable_vars=value_vars,
batch_size=512, name='target_value', op_name='value_reg')

ctrl_vars = tfl.get_layer_variables_by_scope('ctrl')
ctrl_reg=tfl.regression(value, placeholder=None, optimizer='adam',
loss=ctrl_loss, trainable_vars=ctrl_vars,
batch_size=512, name='target_ctrl', op_name='ctrl_reg')

#model_value_example=tfl.DNN(value_example)
model_ctrl=tfl.DNN(tf.concat([ctrl,value],1))


fc.game_init("FCMAP0.PNG")
fc.add_car("0",(430,240))
fc.set_car("0",(435,240), np.pi/2, 0)
fc.show_car_sight("0")
wight=len(fc.get_car_sight("0")[0])
hight=len(fc.get_car_sight("0"))

train_left=[]
train_right=[]
train_state=[]
train_example_ctrl=[]
train_y=[]

degree=0
velocity=100

times=0
fc.set_time_speed(1)
fc.start()
reset_time=fc.get_time()

while True:

    ti=fc.get_ti("0")
    right=ti['right']
    left=ti['left']

    if len(left)==0:
        left.append([0,0])
    if len(right)==0:
        right.append([wight-1,hight-1])
    while len(right)<5:
        right.append(right[len(right)-1])
    while len(left)<5:
        left.append(left[len(left)-1])

    right=np.array([right[i*(len(right)-1)//5] for i in range(5)])
    left=np.array([left[i*(len(left)-1)//5] for i in range(5)])

    train_left+=[left]
    train_right+=[right]
    train_state+=[np.array([degree/np.pi/2*3+0.5, 
            fc.get_velocity('0')/200])]

    out=model_ctrl.predict({input_left:[train_left[-1]], 
            input_right:[train_right[-1]], 
            input_state:[train_state[-1]]})

    if not times%20:
        mid=ti['mid']
        out[0][0]=(mid[0][0]-(wight-1)/2)*0.04+(mid[len(mid)//3][0]-(wight-1)/2)*0.04
        out[0][0]=out[0][0]/2./np.pi*3.+0.5
        out[0][1]=300-fc.get_velocity('0')*2
        out[0][1]=out[0][1]/200
    # else:
    #     mid=ti['mid']
    #     out[0][0]=(mid[0][0]-(wight-1)/2)*0.04+(mid[len(mid)//3][0]-(wight-1)/2)*0.04
    #     out[0][0]=out[0][0]/2./np.pi*3.+0.5
    #     out[0][1]=300-fc.get_velocity('0')*2
    #     out[0][1]=out[0][1]/200
    #     out[0][0]+=np.random.normal(0,0.7)
    #     out[0][1]+=np.random.normal(0,0.7)
    # elif times%3==2:
    #     out[0][0]+=np.random.normal(0,0.7)
    #     out[0][1]+=np.random.normal(0,0.7)

    train_example_ctrl+=[out[0][:2]]
    degree=(out[0][0] - 0.5) * 2. * np.pi / 3.
    velocity=out[0][1]*200

    fc.ctrl('0',degree,velocity)

    print(out)

    train_time=fc.get_time()
    while fc.get_time()-train_time<20:
        fc.sleep(1)



    ti=fc.get_ti("0")
    mid=ti["mid"]

    delta=0
    if len(mid)>8:
        delta=mid[0][0]-(wight-1)/2
    else :
        delta=(wight-1)/2

    r=0
    if times==0:
        r=1
    else:
        r = np.exp(-np.power(delta,2)/100)/4+np.exp(-np.power(fc.get_velocity('0')-100,2)/900)/4
    print(r)

    right=ti['right']
    left=ti['left']

    if len(left)==0:
        left.append([0,0])
    if len(right)==0:
        right.append([wight-1,hight-1])
    while len(right)<5:
        right.append(right[len(right)-1])
    while len(left)<5:
        left.append(left[len(left)-1])

    right=np.array([right[i*(len(right)-1)//5] for i in range(5)])
    left=np.array([left[i*(len(left)-1)//5] for i in range(5)])

    out=model_ctrl.predict({input_left:[left], 
            input_right:[right], 
            input_state:[np.array([degree/np.pi/2*3+0.5, 
            fc.get_velocity('0')/200])]})
    r = r+0.7*out[0][2]
    print(r)

    if(fc.get_time()-reset_time>100):
        train_y+=[np.array([r])]

    if len(train_y)>512:
        train_left.pop(0)
        train_right.pop(0)
        train_state.pop(0)
        train_y.pop(0)

    if fc.get_time()-reset_time>1000:
        fc.set_car("0",(435,240), np.pi/2, 0)
        fc.ctrl('0',0.0,0.0)
        fc.sleep(50)

        while len(train_left)>len(train_y):
            train_left.pop()
            train_right.pop()
            train_state.pop()

        # model_value_example.fit(
        #     X_inputs={input_left:train_left[:len(train_y)], 
        #     input_right:train_right[:len(train_y)],
        #     input_state:train_state[:len(train_y)], 
        #     input_example_ctrl:train_example_ctrl[:len(train_y)]},
        #     Y_targets={y_example:train_y[:len(train_y)]},
        #     n_epoch=200,batch_size=512,show_metric=True,shuffle=True)
        if times>=1:
            model_ctrl.fit(
                X_inputs = {input_left:train_left[:len(train_y)], 
                input_right:train_right[:len(train_y)],
                input_state:train_state[:len(train_y)], 
                input_example_ctrl:train_example_ctrl[:len(train_y)]},
                Y_targets={y_example:train_y[:len(train_y)]},
                n_epoch=3000,batch_size=128,show_metric=True,shuffle=True)
        times+=1
        reset_time=fc.get_time()

