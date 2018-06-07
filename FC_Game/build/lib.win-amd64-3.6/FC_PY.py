import fcgame as fc
import cv2
import numpy as np

import tensorflow as tf 
import tflearn as tfl



def generator(x, reuse=False):
	with tf.variable_scope('Generator', reuse=reuse):
		x = tfl.fully_connected(x, 256, activation='relu')
		x = tfl.fully_connected(x, 2, activation='sigmoid')
	return x
# Discriminator
def discriminator(x, reuse=False):
	with tf.variable_scope('Discriminator', reuse=reuse):
		x = tfl.fully_connected(x, 256, activation='relu')
		x = tfl.fully_connected(x, 1, activation='sigmoid')
	return x

# Build Networks
gen_input = tfl.input_data(shape=[None, 22], name='input_noise')
disc_input = tfl.input_data(shape=[None, 2], name='disc_input')
disc_input_a = tf.concat([gen_input,disc_input],1)

gen_sample = generator(gen_input)
disc_real = discriminator(disc_input_a)
disc_fake_in=tf.concat([gen_input, gen_sample],1)
disc_fake = discriminator(disc_fake_in, reuse=True)
# Define Loss
disc_loss = -tf.reduce_mean(tf.log(disc_real) + tf.log(1. - disc_fake))
gen_loss = -tf.reduce_mean(tf.log(disc_fake))
# Build Training Ops for both Generator and Discriminator.
# Each network optimization should only update its own variable, thus we need
# to retrieve each network variables (with get_layer_variables_by_scope) and set
# 'placeholder=None' because we do not need to feed any target.

gen_vars = tfl.get_layer_variables_by_scope('Generator')
gen_model = tfl.regression(gen_sample, placeholder=None, optimizer='adam',
loss=gen_loss, trainable_vars=gen_vars,
batch_size=64, name='target_gen', op_name='GEN')

gen_model1 = tfl.regression(gen_sample, placeholder=None, optimizer='adam',
loss=gen_loss, trainable_vars=gen_vars,
batch_size=64, name='target_gen1', op_name='GEN1')

gen_model2 = tfl.regression(gen_sample, placeholder=None, optimizer='adam',
loss=gen_loss, trainable_vars=gen_vars,
batch_size=64, name='target_gen2', op_name='GEN2')

gen_vars3 = tfl.get_layer_variables_by_scope('Generator')
gen_model = tfl.regression(gen_sample, placeholder=None, optimizer='adam',
loss=gen_loss, trainable_vars=gen_vars,
batch_size=64, name='target_gen3', op_name='GEN3')

gen_model4 = tfl.regression(gen_sample, placeholder=None, optimizer='adam',
loss=gen_loss, trainable_vars=gen_vars,
batch_size=64, name='target_gen4', op_name='GEN4')

gen_model5 = tfl.regression(gen_sample, placeholder=None, optimizer='adam',
loss=gen_loss, trainable_vars=gen_vars,
batch_size=64, name='target_gen5', op_name='GEN5')

disc_vars = tfl.get_layer_variables_by_scope('Discriminator')
disc_model = tfl.regression(disc_real, placeholder=None, optimizer='adam',
loss=disc_loss, trainable_vars=disc_vars,
batch_size=64, name='target_disc', op_name='DISC')

# Define GAN model, that output the generated images.
gan = tfl.DNN(tf.concat([gen_model,disc_fake,disc_real],1))


fc.game_init("FCMAP0.PNG")
fc.add_car("0",(430,240))
fc.set_car("0",(435,240), np.pi/2, 0)
fc.show_car_sight("0")
wight=len(fc.get_car_sight("0")[0])
hight=len(fc.get_car_sight("0"))

train_s=[]
train_a=[]
train_r=[]

degree=0
velocity=0

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

    s = [right[i*(len(right)-1)//5] for i in range(5)]
    s += [left[i*(len(left)-1)//5] for i in range(5)]
    s += [[degree/np.pi/2*3+0.5, fc.get_velocity('0')/200]]
    s = np.array(s)
    s = np.reshape(s,[np.prod(np.shape(s))])

    if times>10:
        out = gan.predict({gen_input:[s],
                    disc_input:[train_a[int((len(train_a)-1)*np.random.random())]]})
    else :
        out = gan.predict({gen_input:[s],
                    disc_input:[[0.,0.]]})
    print(out[0])
    train_s+=[s]

#    if not times%20:
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

    train_a+=[out[0][:2]]
    degree=(out[0][0] - 0.5) * 2. * np.pi / 3.
    velocity=out[0][1]*200
    print(out[0])
    out = gan.predict({gen_input:[s],
                    disc_input:[train_a[-1]]})

    fc.ctrl('0',degree+np.random.normal(0,0.4),velocity)

    print(out[0])

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


    # right=ti['right']
    # left=ti['left']

    # if len(left)==0:
    #     left.append([0,0])
    # if len(right)==0:
    #     right.append([wight-1,hight-1])
    # while len(right)<5:
    #     right.append(right[len(right)-1])
    # while len(left)<5:
    #     left.append(left[len(left)-1])

    # s = [right[i*(len(right)-1)//5] for i in range(5)]
    # s += [left[i*(len(left)-1)//5] for i in range(5)]
    # s += [[degree/np.pi/2*3+0.5, fc.get_velocity('0')/200]]
    # s = np.array(s)
    # s = np.reshape(s,[1,np.prod(np.shape(s))])
    # out = gan.predict({gen_input:s})

    # out = gan.predict(s)
    # r = r
    # print(r)

    if(fc.get_time()-reset_time>100):
        train_r+=[[r]]

    if fc.get_time()-reset_time>1000:
        fc.set_car("0",(425+np.random.normal(0,7),240), np.pi/2, 0)
        fc.ctrl('0',0.0,0.0)
        fc.sleep(50)

        while len(train_s)>len(train_r):
            train_s.pop()
            train_a.pop()

        if times>=50:
            gan.fit(
                X_inputs = {gen_input:train_s, disc_input:train_a},
                Y_targets=None,
                n_epoch=50,batch_size=128)
        times+=1
        reset_time=fc.get_time()







# tf.scatter_add
# data=np.array([[0,0],[1,1],[5,5],[6,6]])
# kmeans(data,2)

# a=tf.constant([1,2,3])
# a=tf.cross(a,a)
# with tf.Session() as sess:
#          print(sess.run(a))


# 请教一个关于tensorflow的问题
# 如果有列向量[a1 a2 a3 a4 … ]和行向量[b1 b2 b3 b4 … ]用tf.matmul函数可以得到
# [[a1b1 a1b2 a1b3 a1b4 …] 
# [a2b1 a2b2 a2b3 a2b4 …] 
# [a3b1 a3b2 a3b3 a3b4 …] 
# …]
# ，那如果类似的我想得到
# [[a1-b1 a1-b2 a1-b3 a1-b4 …] 
# [a2-b1 a2-b2 a2-b3 a2-b4 …] 
# [a3-b1 a3-b2 a3-b3 a3-b4 …] 
# …]
# 或者
# [[tf.pow(a1,b1) tf.pow(a1,b2) tf.pow(a1,b3) tf.pow(a1,b4) …] 
# [tf.pow(a2,b1) tf.pow(a2,b2) tf.pow(a2,b3) tf.pow(a2,b4) …] 
# [tf.pow(a3,b1) tf.pow(a3,b2) tf.pow(a3,b3) tf.pow(a3,b4) …] 
# …]
# 应该如何写？

# I have a question in tensorflow
# if I have a col tensor [a1 a2 a3 a4 … ]and a row tensor [b1 b2 b3 b4 … ], I can use tf.matmul() for get a tensor like this
# [[a1b1 a1b2 a1b3 a1b4 …] 
# [a2b1 a2b2 a2b3 a2b4 …] 
# [a3b1 a3b2 a3b3 a3b4 …] 
# …]
# but now, if I want to get a tensor like this
# [[a1-b1 a1-b2 a1-b3 a1-b4 …] 
# [a2-b1 a2-b2 a2-b3 a2-b4 …] 
# [a3-b1 a3-b2 a3-b3 a3-b4 …] 
# …]
# or this
# [[tf.pow(a1,b1) tf.pow(a1,b2) tf.pow(a1,b3) tf.pow(a1,b4) …] 
# [tf.pow(a2,b1) tf.pow(a2,b2) tf.pow(a2,b3) tf.pow(a2,b4) …] 
# [tf.pow(a3,b1) tf.pow(a3,b2) tf.pow(a3,b3) tf.pow(a3,b4) …] 
# …]
# How can I do that?

# in_a=tf.placeholder(tf.float32, [None, 1])
# in_b=tf.placeholder(tf.float32, [None, 1])

# #a=tf.transpose(in_a)
# #b=tf.transpose(in_b)
# a=in_a
# b=in_b


# tile_a=tf.shape(b)[0]
# tile_b=tf.shape(a)[0]
# bb=tf.transpose(b)
# bb=tf.expand_dims(bb,0)
# aa=tf.expand_dims(a,-1)

# bb=tf.tile(bb,[tile_b]+[1]*len(b.get_shape().as_list()) )
# aa=tf.tile(aa,[1]*len(a.get_shape().as_list())+[tile_a])

# r=tf.reduce_sum(bb*aa,1)
# #r=tf.matmul(a,b,False,True)


# with tf.Session() as sess:
#     x,y,z=sess.run([aa,bb,r],feed_dict={in_a:[[1],[2],[3],[4]],in_b:[[2],[3],[4],[5]]})
#     print(x)
#     print(y)
#     print(z)



