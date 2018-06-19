import numpy as np
import tensorflow as tf 

def sigmoid_(x):
    return tf.nn.sigmoid(x)*2-1

class DDPG(object):
    def __init__(self, s_dim, a_dim, r_dim, a_bound):
        self.REPLAY_CAPACITY=10000
        self.replay = np.zeros((self.REPLAY_CAPACITY, s_dim * 2 + a_dim + r_dim), dtype=np.float32)
        self.frame = 0
        self.sess = tf.Session()

        self.a_dim, self.s_dim, self.r_dim, self.a_bound = a_dim, s_dim, r_dim, a_bound,
        self.S = tf.placeholder(tf.float32, [None, s_dim], 's')
        self.S_ = tf.placeholder(tf.float32, [None, s_dim], 's_')
        self.R = tf.placeholder(tf.float32, [None, r_dim], 'r')

        self.a = self.actor_net(self.S)
        self.q = self.critic_net(self.S, self.a)

        ema = tf.train.ExponentialMovingAverage(decay=0.99)
        a_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, scope='vActor')
        c_params = tf.get_collection(tf.GraphKeys.TRAINABLE_VARIABLES, scope='vCritic')
        target_update = [ema.apply(a_params), ema.apply(c_params)]

        def ema_getter(getter, name, *args, **kwargs):
            return ema.average(getter(name, *args, **kwargs))

        a_ = self.actor_net(self.S_, reuse=True, custom_getter=ema_getter)
        q_ = self.critic_net(self.S_, a_, reuse=True, custom_getter=ema_getter)

        a_loss = - tf.reduce_mean(self.q) + tf.reduce_mean(tf.square(self.a))/32
        self.a_regression = tf.train.AdamOptimizer(0.001).minimize(loss=a_loss, var_list=a_params)

        with tf.control_dependencies(target_update):
            q_target = self.R + 0.9*q_
            q_loss = tf.losses.mean_squared_error(labels=q_target, predictions=self.q)
            self.c_regression = tf.train.AdamOptimizer(0.002).minimize(loss=q_loss, var_list=c_params)

        self.sess.run(tf.global_variables_initializer())

    def actor_net(self, s, reuse=None, custom_getter=None):
        trainable = True if reuse is None else False
        with tf.name_scope('Actor'):
            with tf.variable_scope('vActor', reuse=reuse, custom_getter=custom_getter):
                a = tf.layers.dense(s, 32, activation=tf.nn.relu, trainable=trainable, kernel_regularizer=tf.nn.l2_loss )
                return tf.layers.dense(a, self.a_dim, activation=sigmoid_, trainable=trainable, kernel_regularizer=tf.nn.l2_loss)
                

    def critic_net(self, s, a, reuse=None, custom_getter=None):
        trainable = True if reuse is None else False
        with tf.name_scope('Critic'):
            with tf.variable_scope('vCritic', reuse=reuse, custom_getter=custom_getter):
                net=tf.concat([s,a], 1)
                net = tf.layers.dense(net, 32, activation=tf.nn.relu, trainable=trainable, kernel_regularizer=tf.nn.l2_loss)
                return tf.layers.dense(net, self.r_dim, trainable=trainable, kernel_regularizer=tf.nn.l2_loss)
                
    def predict(self, s):
        return self.sess.run([self.a, self.q], feed_dict={self.S:s[np.newaxis, :]})[0]*np.array([self.a_bound])

    def fit(self):
        indices = np.random.choice(min(self.frame, self.REPLAY_CAPACITY), size=32)
        bt = self.replay[indices, :]
        bs = bt[:, :self.s_dim]
        ba = bt[:, self.s_dim: self.s_dim + self.a_dim]
        br = bt[:, -self.s_dim - self.r_dim: -self.s_dim]
        bs_ = bt[:, -self.s_dim:]

        self.sess.run(self.c_regression, {self.S: bs, self.a: ba, self.R: br, self.S_: bs_})
        self.sess.run(self.a_regression, {self.S: bs})

    def record(self, s, a, r, s_):
        transition = np.hstack((s, a, r, s_))
        index = self.frame % self.REPLAY_CAPACITY  # replace the old memory with new memory
        self.replay[index, :] = transition
        self.frame += 1

