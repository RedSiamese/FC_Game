import fcgame as fc
import cv2
import numpy as np

import tensorflow as tf 
import tflearn as tfl


def ctrl_net(c_in,reuse=False):
    with tf.variable_scope('ctrl',reuse=False):
        c_in=tfl.fully_connected(c_in,32,'tanh')
        c_out=tfl.fully_connected(c_in,2,'tanh')
    return c_out
    #return (c_out-0.5)*2.0


def value_net(v_in,reuse=False):
    with tf.variable_scope('value',reuse=False):
        #value=tfl.fully_connected(value,16,'tanh')
        v_out=tfl.fully_connected(v_in,1,'sigmoid')
    return v_out

def image_net(input_map,reuse=False):
    with tf.variable_scope('image',reuse=False):
        img_out=tfl.conv_2d(input_map,8,[3,3], activation='relu')
        img_out=tfl.conv_2d(img_out,8,[3,3], activation='relu')
        img_out=tfl.avg_pool_2d(img_out,[2,2])
        #img_out=tfl.conv_2d(img_out,8,[3,3], activation='relu')
        #img_out=tfl.conv_2d(img_out,8,[3,3], activation='relu')
        #img_out=tfl.avg_pool_2d(img_out,[2,2])
        img_out=tfl.conv_2d(img_out,8,[3,3], activation='relu')
        img_out=tfl.conv_2d(img_out,8,[3,3], activation='relu')
        img_out=tfl.avg_pool_2d(img_out,[2,2])
        img_out=tf.reshape(img_out,[-1,np.prod(np.array(img_out.get_shape().as_list()[1:]))])
        img_out=tfl.fully_connected(img_out,32,'tanh')
    return img_out


input_map=tfl.input_data([None,16,16,1])
input_degree=tfl.input_data([None,1])
input_velocity=tfl.input_data([None,1])
last_ctrl=tf.concat([input_degree,input_velocity],1)


img_out = image_net(input_map)

ctrl_in = tf.concat([img_out,last_ctrl],1)

ctrl = ctrl_net(ctrl_in)

value_in = tf.concat([ctrl_in,ctrl],1)
value = value_net(value_in)

#评分(越大越好)
y=tf.placeholder(tf.float32,[None,1])

value_loss = tf.reduce_mean(tf.square(y - value))
#ctrl_loss = tf.reduce_mean(-tf.log(value))
ctrl_loss = tf.reduce_mean(1-value)

ctrl_vars = tfl.get_layer_variables_by_scope('ctrl')# + tfl.get_layer_variables_by_scope('image')
value_vars = tfl.get_layer_variables_by_scope('value') + tfl.get_layer_variables_by_scope('image')

ctrl_model=tfl.regression(ctrl, placeholder=None, optimizer='adam',
loss=ctrl_loss, trainable_vars=ctrl_vars,
batch_size=64, name='target_ctrl', op_name='ctrl_model')

value_model=tfl.regression(value, placeholder = y, optimizer='adam',
loss=value_loss, trainable_vars=value_vars,
batch_size=64, name='target_value', op_name='value_model')

model=tfl.DNN(tf.concat([ctrl,value],1))

fc.game_init("FCMAP0.PNG")
fc.add_car("0",(430,240))
fc.set_car("0",(430,240), np.pi/2, 0)
fc.show_car_sight("0")
wight=len(fc.get_car_sight("0")[0])

train_sight=[]
train_degree=[]
train_velocity=[]
train_y=[]

degree=0
velocity=100

times=1
fc.set_time_speed(1)
fc.start()
reset_time=fc.get_time()
while times:

    sight=cv2.resize(np.array(fc.get_car_sight("0")),(16,16))
    sight=np.reshape(sight,[16,16,1])
    out=model.predict({input_map:[sight], input_degree:[np.array([degree])], input_velocity:[np.array([fc.get_velocity('0')])]})

    degree=out[0][0]*np.pi/3
#   velocity=(out[0][1]+1)*200
    velocity=100


    fc.ctrl('0',degree,velocity)

    print(out)
    print(len(train_y))

    train_sight+=[sight]
    train_degree+=[np.array([degree])]
    train_velocity+=[np.array([velocity])]

    # train_time=fc.get_time()
    # if(reset_time>1000):
    #     while fc.get_time()-train_time<20:
    #         model.fit({input_map:train_sight[:len(train_y)-1], 
    #         input_degree:train_degree[:len(train_y)-1], 
    #         input_velocity:train_velocity[:len(train_y)-1]},
    #         {y:train_y[:len(train_y)-1]},
    #         n_epoch=1,batch_size=512,show_metric=True,shuffle=True)
    # else:
    #     while fc.get_time()-train_time<20:
    #         fc.sleep(1)
    train_time=fc.get_time()
    while fc.get_time()-train_time<20:
        fc.sleep(1)

    print(fc.get_time()-train_time)

    mid=fc.get_ti("0")["mid"]

    
    delta=0
    if len(mid)>8:
        delta=mid[0][0]-(wight-1)/2
    else :
        delta=(wight-1)/2

    #r = np.exp(-np.power(delta,2)/36)/(1+np.exp(-(velocity-100)))
    r = np.exp(-np.power(delta,2)/64)*np.exp(-np.power(velocity-100,2)/2000)
    print(r)
    r = r+0.5*out[0][2]
    print(r)

    if(fc.get_time()-reset_time>50):
        train_y+=[np.array([r])]

    if len(train_y)>512:
        train_sight.pop(0)
        train_degree.pop(0)
        train_velocity.pop(0)
        train_y.pop(0)

    if fc.get_time()-reset_time>3000:
        fc.set_car("0",(430,240), np.pi/2, 0)
        fc.ctrl('0',0.0,0.0)
        fc.sleep(50)

        while len(train_sight)>len(train_y):
            train_sight.pop(0)
            train_degree.pop(0)
            train_velocity.pop(0)

        model.fit({input_map:train_sight[:len(train_y)-1], 
            input_degree:train_degree[:len(train_y)-1], 
            input_velocity:train_velocity[:len(train_y)-1]},
            {y:train_y[:len(train_y)-1]},
            n_epoch=100,batch_size=512,show_metric=True,shuffle=True)
        times+=1
        reset_time=fc.get_time()

