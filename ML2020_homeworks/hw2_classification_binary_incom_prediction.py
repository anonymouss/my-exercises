# https://www.kaggle.com/c/ml2020spring-hw2

import torch
from torch import nn

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

import os

DATA_ROOT = './data/hw2'
X_TRAIN_FILE_NAME = 'X_train'
Y_TRAIN_FILE_NAME = 'Y_train'
X_TEST_FILE_NAME = 'X_test'
SUBMISSIONS_FILE_NAME = 'submission.csv'

# prepare data
X_train_data = pd.read_csv(os.path.join(DATA_ROOT, X_TRAIN_FILE_NAME))
y_train_data = pd.read_csv(os.path.join(DATA_ROOT, Y_TRAIN_FILE_NAME))
X_test_data = pd.read_csv(os.path.join(DATA_ROOT, X_TEST_FILE_NAME))

X_train_data = X_train_data.iloc[:, 1:].to_numpy(dtype=np.float)
y_train_data = y_train_data.iloc[:, 1].to_numpy(dtype=np.float)
X_test_data = X_test_data.iloc[:, 1:].to_numpy(dtype=np.float)

eps = 1e-8

def normalize(X):
    mean = np.mean(X, axis=0)
    std = np.std(X, axis=0)
    return (X - mean) / (std + eps)

X_train = torch.tensor(normalize(X_train_data)).float()
X_test = torch.tensor(normalize(X_test_data)).float()
y_train = torch.tensor(y_train_data).float()

def split_data(dataset, ratio=0.2):
    n = len(dataset)
    split = int(n * ratio)
    indices = torch.randperm(n).tolist()
    train_set = torch.utils.data.Subset(dataset, indices[:-split])
    test_set = torch.utils.data.Subset(dataset, indices[-split:])
    return train_set, test_set

# train
num_inputs, num_outputs = X_train_data.shape[1], 1
num_epochs, batch_size, lr, weight_decay = 150, 256, 0.0001, 0.05

train_dataset = torch.utils.data.TensorDataset(X_train, y_train)
train_set, valid_set = split_data(train_dataset, 0.1)

train_iter = torch.utils.data.DataLoader(train_set, batch_size=batch_size, shuffle=True)
valid_iter = torch.utils.data.DataLoader(valid_set, batch_size=batch_size, shuffle=True)

class LRNet(nn.Module):
    def __init__(self, num_inputs, num_outputs):
        super(LRNet, self).__init__()
        self.num_inputs = num_inputs
        self.num_outputs = num_outputs
        self.fc1 = nn.Linear(num_inputs, num_outputs)
        self.activate = nn.Sigmoid()

    def forward(self, X):
        y = self.fc1(X)
        return self.activate(y)

net = LRNet(num_inputs, num_outputs)

loss = nn.BCELoss()
optimizer = torch.optim.Adam(net.parameters(), lr=lr, weight_decay=weight_decay)

train_loss, valid_loss = [], []
train_acc, valid_acc = [], []

step = int(num_epochs / 10)
step = max(1, step)

for e in range(num_epochs):
    train_l, valid_l = 0.0, 0.0
    train_a, valid_a = 0.0, 0.0
    train_n, valid_n = 0, 0
    for X, y in train_iter:
        y_hat = net(X).squeeze()
        l = loss(y_hat, y).sum()
        optimizer.zero_grad()
        l.backward()
        optimizer.step()
        train_l += l.item()
        train_a += (y_hat.round() == y).sum().item()
        train_n += y.shape[0]
    train_loss.append(train_l / train_n)
    train_acc.append(train_a / train_n)
    with torch.no_grad():
        for vX, vy in valid_iter:
            vy_hat = net(vX).squeeze()
            vl = loss(vy_hat, vy).sum()
            valid_l += vl.item()
            valid_a += (vy_hat.round() == vy).sum().item()
            valid_n += vy.shape[0]
        valid_loss.append(valid_l / valid_n)
        valid_acc.append(valid_a / valid_n)
    if (e + 1) % step == 0:
        print('Epoch {} - [Train] loss: {:5f}, acc: {:5f}; [Valid] loss: {:5f}, acc: {:5f}'.format(
            e + 1, train_loss[-1], train_acc[-1], valid_loss[-1], valid_acc[-1]
        ))

def plot(x_vals, y_vals, x_label, y_label, x2_vals=None, y2_vals=None, legend=None):
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.plot(x_vals, y_vals)
    if x2_vals and y2_vals:
        plt.plot(x2_vals, y2_vals, linestyle=':')
        plt.legend(legend)
    plt.show()

plot(range(1, num_epochs + 1), train_loss, 'epochs', 'BCE',
    range(1, num_epochs + 1), valid_loss, ['train', 'valid'])

plot(range(1, num_epochs + 1), train_acc, 'epochs', 'Acc',
    range(1, num_epochs + 1), valid_acc, ['train', 'valid'])

# predict
preds_float = net(X_test).squeeze().round().detach().numpy().reshape(1, -1)[0]
preds = [0 if i == 0.0 else 1 for i in preds_float]
ids = np.array([i for i in range(len(X_test))])
subdata = {}
subdata['id'] = pd.Series(ids.reshape(1, -1)[0])
subdata['label'] = pd.Series(preds)
submission = pd.concat([subdata['id'], subdata['label']], axis=1)
submission.columns =['id', 'label']
submission.to_csv(os.path.join(DATA_ROOT, SUBMISSIONS_FILE_NAME), index=False)
