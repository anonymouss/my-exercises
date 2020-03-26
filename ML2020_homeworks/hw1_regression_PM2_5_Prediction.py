#!/usr/bin/env python3

# https://www.kaggle.com/c/ml2020spring-hw1/

import torch
from torch import nn

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

import os

DATA_ROOT = './data/hw1'
TRAIN_FILE_NAME = 'train.csv'
TEST_FILE_NAME = 'test.csv'
SUBMISSIONS_FILE_NAME = 'submission.csv'

'''
    train.csv 每月前20天数据（24小时 X 18观测项）
    test.csv 共240笔数据，每笔数据为连续10小时数据，前9小时作为feature，第10小时需要预测
'''

# prepare data
train_data = pd.read_csv(os.path.join(DATA_ROOT, TRAIN_FILE_NAME), encoding='big5')
test_data = pd.read_csv(os.path.join(DATA_ROOT, TEST_FILE_NAME), header=None, encoding='big5')

train_data = train_data.iloc[:, 3:]
test_data = test_data.iloc[:, 2:]
train_data[train_data == 'NR'] = 0
test_data[test_data == 'NR'] = 0
train_data = train_data.to_numpy()
test_data = test_data.to_numpy()

train_data_by_month = {}
for month in range(12):
    sample = np.empty([18, 24 * 20])
    for day in range(20):
        sample[:, day * 24 : (day + 1) * 24] = train_data[
            18 * (20 * month + day) : 18 * (20 * month + day + 1)]
    train_data_by_month[month] = sample
train_X = np.empty([12 * 471, 18 * 9])
train_y = np.empty([12 * 471, 1])
for month in range(12):
    for day in range(20):
        for hour in range(24):
            if day == 19 and hour > 14:
                break
            train_X[month * 471 + day * 24 + hour, :] = train_data_by_month[month][
                :, day * 24 + hour : day * 24 + hour + 9].reshape(1, -1)
            train_y[month * 471 + day * 24 + hour, :] = train_data_by_month[month][
                9, day * 24 + hour + 9]
            # 这个 9 是因为 PM2.5 是第10 个观测项
test_X = test_data.reshape(240, 18 * 9).astype(np.float)

def normalize(X):
    mean = np.mean(X, axis=0)
    std = np.std(X, axis=0)
    return (X - mean) / std

train_X = torch.tensor(normalize(train_X)).float()
test_X = torch.tensor(normalize(test_X)).float()
train_y = torch.tensor(train_y).float()

# train
def split_data(dataset, ratio=0.2):
    n = len(dataset)
    split = int(n * ratio)
    indices = torch.randperm(n).tolist()
    train_set = torch.utils.data.Subset(dataset, indices[:-split])
    test_set = torch.utils.data.Subset(dataset, indices[-split:])
    return train_set, test_set

def semilogy(x_vals, y_vals, x_label, y_label, x2_vals=None, y2_vals=None, legend=None):
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.semilogy(x_vals, y_vals)
    if x2_vals and y2_vals:
        plt.semilogy(x2_vals, y2_vals, linestyle=':')
        plt.legend(legend)
    plt.show()

train_dataset = torch.utils.data.TensorDataset(train_X, train_y)
train_set, valid_set = split_data(train_dataset, 0.2)

batch_size, lr, weight_decay, num_epochs = len(train_dataset), 0.1, 0.001, 1200 # full batch

train_iter = torch.utils.data.DataLoader(train_set, batch_size=batch_size, shuffle=True)
valid_iter = torch.utils.data.DataLoader(valid_set, batch_size=batch_size, shuffle=True)

net = nn.Linear(18 * 9, 1)
optimizer = torch.optim.Adam(net.parameters(), lr=lr, weight_decay=weight_decay)
# optimizer = torch.optim.Adagrad(net.parameters(), lr=lr, weight_decay=weight_decay)
loss = nn.MSELoss()

train_loss, valid_loss = [], []

for e in range(num_epochs):
    train_l, n = 0.0, 0
    for X, y in train_iter:
        y_hat = net(X)
        l = loss(y_hat, y).sum()
        optimizer.zero_grad()
        l.backward()
        optimizer.step()
        train_l += l.item()
        n += y.shape[0]
    train_loss.append(train_l / n)
    with torch.no_grad():
        valid_l, m = 0.0, 0
        for vX, vy in valid_iter:
            vy_hat = net(vX)
            vl = loss(vy_hat, vy).sum()
            valid_l += vl.item()
            m += vy.shape[0]
        valid_loss.append(valid_l / m)
    if (e + 1) % 100 == 0:
        print('Epoch: {}, train loss: {:5f}, valid loss: {:5f}'.format(
            e + 1, train_loss[-1], valid_loss[-1]))
# print(net.weight.detach().numpy())

semilogy(range(1, num_epochs + 1), train_loss, 'epochs', 'MSE',
    range(1, num_epochs + 1), valid_loss, ['train', 'valid'])

# predict
preds = net(test_X).detach().numpy().reshape(1, -1)[0]
ids = np.array(['id_' + str(i) for i in range(len(test_X))])
subdata = {}
subdata['id'] = pd.Series(ids.reshape(1, -1)[0])
subdata['value'] = pd.Series(preds)
submission = pd.concat([subdata['id'], subdata['value']], axis=1)
submission.columns =['id', 'value']
submission.to_csv(os.path.join(DATA_ROOT, SUBMISSIONS_FILE_NAME), index=False)