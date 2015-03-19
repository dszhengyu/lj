#include "svmpredictor.h"

void SvmPredictor::train(bool debug)
{
    process(debug);
    castToArray(debug);

/*构建svm_problem以及svm_parameter,训练svm并保存svm_model************************************************************/
    struct svm_problem prob;
    prob.l = m;
    prob.y = y;
    prob.x = x;

    struct svm_parameter param;
    param.svm_type = C_SVC;
    param.kernel_type = POLY;
    param.degree = 6;
    param.gamma = 0;
    param.coef0 = 0;
    param.nu = 0.5;
    param.cache_size = 100;
    param.C = 10;
    param.eps = 1e-5;
    param.p = 0.1;
    param.shrinking = 1;
    param.probability = 0;
    param.nr_weight = 0;
    param.weight = NULL;
    param.weight_label = NULL;

    //检查参数是否正确
    if (svm_check_parameter(&prob, &param) != NULL) {
        qDebug("%s", svm_check_parameter(&prob, &param));
    }

    //构建model并保存
    model = svm_train(&prob, &param);
    svm_save_model("svm_model.txt", model);

    //释放内存
    svm_destroy_param(&param);
    QMessageBox resultMessage(QMessageBox::NoIcon, "SVM训练完成", "训练完成");
    resultMessage.setIconPixmap(QPixmap("images/vector.png"));
    resultMessage.exec();

    if (debug)
        cvDestroyAllWindows();
}

double SvmPredictor::predict(bool debug)
{
    process(debug);
    castToArray();

    if (!model) {
        model = svm_load_model("svm_model.txt");
        if (!model) {
            MessageBox ("Train SVM first!");
            return -1;
        }
    }

    int hit = 0;
    for (int i = 0; i < m; i++) {
        double result;
        result = svm_predict(model, x[i]);
        cout << "label is: " << y[i] << ", predict: " << result << endl;
        hit = (result == y[i]) ? hit + 1 : hit;
    }

    QStringList report;
    report << "total pictures:" << QString::number(m) << "\naccurate predict: " << QString::number(hit)
           << "\naccuracy: " << QString::number(hit * 100 / m) << "%";
    MessageBox (report.join(""), "SVM predict result", "images/vector.png");

    return ((m - hit) / m);
}

void SvmPredictor::castToArray(bool debug)
{
    m = inputs->rows;
    dimension = inputs->cols;
//labels array
    y = (double *)malloc(sizeof(double) * m);
//直接malloc一个二维结构体数组，始终不能成功，从网上参考方法，将二维数组一维化，把地址传递给二维数组指针，妙哉妙哉~~~~~~
    x = new struct svm_node *[m];
    x_space = new struct svm_node [m * (dimension + 1)];

    for (int i = 0; i < m; ++i) {
        //对标签处理
        y[i] = label->at<float>(i, 0);
        for (int j = 0; j < dimension; ++j) {
            x_space[i * (dimension + 1) + j].index = j + 1;
            x_space[i * (dimension + 1) + j].value = inputs->at<float>(i, j);
        }
        x_space[i * (dimension + 1) + dimension].index = -1;
        x_space[i * (dimension + 1) + dimension].value = 0;
        x[i] = &x_space[i * (dimension + 1)];
    }
//    if (debug){
//        for (int i = 0; i < m; i++) {
//            for (int j = 0; j <= dimension; j++) {
//                cout << x[i][j].value << " ";
//            }
//            cout << endl;
//        }
//    }

}
