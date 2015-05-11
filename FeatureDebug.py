import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from datetime import datetime
from sklearn.cross_validation import StratifiedKFold, StratifiedShuffleSplit
from sklearn.grid_search import GridSearchCV
from sklearn.linear_model import LogisticRegression
from sklearn import svm
from sklearn.preprocessing import StandardScaler, MinMaxScaler
from sklearn.metrics import f1_score, classification_report, precision_score
from sklearn.learning_curve import learning_curve
from sklearn.externals import joblib
from LearningCurve import showLearningCurve

prefix = "Z:\\lj\\build-lj-Desktop_Qt_5_3_MSVC2013_OpenGL_64bit-Debug\\"
featureFileName = prefix + "X.csv"
labelFileName = prefix + "y.csv"

# Get X and y
X = pd.read_csv(featureFileName, header = None)
X.dropna(axis = 1, inplace = True)
X = MinMaxScaler().fit_transform(X)
y = pd.read_csv(labelFileName, header = None)
y = np.loadtxt(labelFileName, delimiter = ',')

for train_index, test_index in StratifiedShuffleSplit(y, n_iter=1, test_size=0.2):
    break
X_train, X_test = X[train_index], X[test_index]
y_train, y_test = y[train_index], y[test_index]

# Model and param
clf1 = LogisticRegression(class_weight = 'auto')
param1 = {'C' : [0.0001, 0.001, 0.01, 0.1, 1, 10, 100, 100]}
modelFile1 = prefix + 'LR.plk'
clf2 = svm.LinearSVC()
param2 = {'C' : [0.0001, 0.001, 0.01, 0.1, 1, 10, 100, 100],
        'dual' : [True, False]}
modelFile2 = prefix + 'SVM.plk'
        
clfList = [clf1, clf2]
paramList = [param1, param2]
modelFileList = [modelFile1, modelFile2]

# Train and predict
for clf, param, modelFile in zip(clfList, paramList, modelFileList):
    clf = GridSearchCV(estimator = clf, param_grid = param,
                        cv = StratifiedKFold(y_train, 3),
                        scoring = 'f1')
    clf.fit(X_train, y_train)
    clf = clf.best_estimator_
    print (clf)
    #showLearningCurve(clf, X_train, y_train)
    y_pred = clf.predict(X_test)
    print (classification_report(y_test, y_pred))
    print ()
    joblib.dump(clf, modelFile)

