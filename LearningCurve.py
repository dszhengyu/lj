import pandas as pd
from datetime import datetime
from sklearn.cross_validation import StratifiedKFold, StratifiedShuffleSplit
from sklearn.metrics import f1_score, classification_report, precision_score
from sklearn.learning_curve import learning_curve

def showLearningCurve(clf, X, y):
    print ('calculate to print learning curve' + str(datetime.now()))
    train_sizes, train_scores, valid_scores = learning_curve(clf, X, y, 
                                                            scoring = 'f1',
                                                            cv = StratifiedKFold(y, 3),
                                                            n_jobs = 2)
    plt.figure()
    plt.title('learning curve')
    plt.xlabel("Training examples")
    plt.ylabel("Score")
    train_scores_mean = np.mean(train_scores, axis=1)
    train_scores_std = np.std(train_scores, axis=1)
    valid_scores_mean = np.mean(valid_scores, axis=1)
    valid_scores_std = np.std(valid_scores, axis=1)
    plt.grid()
    plt.fill_between(train_sizes, train_scores_mean - train_scores_std,
                     train_scores_mean + train_scores_std, alpha=0.1,
                     color="r")
    plt.fill_between(train_sizes, valid_scores_mean - valid_scores_std,
                     valid_scores_mean + valid_scores_std, alpha=0.1, color="g")
    plt.plot(train_sizes, train_scores_mean, 'o-', color="r",
             label="Training score")
    plt.plot(train_sizes, valid_scores_mean, 'o-', color="g",
             label="Cross-validation score")
    plt.legend(loc="best")
    plt.show()