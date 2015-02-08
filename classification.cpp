#include "classification.h"

classification::classification()
{
}

void classification::trainSvm(QStringList fileNames)
{

    if (fileNames.length() < 1)
        return;

//    //尝试使用文件保存特征，看是否可以加快速度？？？？
//    QFile tmpFile1 = ("tmpFile1.dat");\
//    QFile tmpFile2 = ("tmpFile2.dat");
//    if (!tmpFile1.open(QIODevice::Append) || !tmpFile2.open(QIODevice::Append)) {
//        qDebug("can't open file to read && write!");
//        return;
//    }
//    QTextStream outLable(&tmpFile1);
//    QTextStream outFeature(&tmpFile2);

/*处理所选的所有图片，输出为两个QStringList， 标签和特征值**************************************/
    QStringList labeList, totalFeature;
    for (int i = 0; i < fileNames.length(); i++) {
        //qDebug("i = %d", i);

        //处理路径名并打开图片
        QString eachFile = fileNames.at(i);//qDebug("%s", eachFile.toLocal8Bit().data());
        IplImage* eachImage = cvLoadImage(eachFile.toLocal8Bit().data());
        cout << eachFile.toLocal8Bit().data() << endl;

        //取标签（文件名第一个字符）
        QStringList tmp = eachFile.split("/");
        QString lable = tmp.at(tmp.length() - 1).left(1);
        //qDebug("%s", lable.toLocal8Bit().data());

        //处理每张图片获取特征值（可能会有许多），并且把相应个数的标签放入lable的list中
        QStringList featureList;
        featureList = nclseg::seg4train(eachImage);
        for (int j = 0; j < featureList.length(); j++) {
            labeList << lable;
            totalFeature << featureList.at(j);
        }
        eachFile.clear();
        featureList.clear();
        cvReleaseImage(&eachImage);
        tmp.clear();
    }
//    fileNames.clear();
//    //刷新流，再把文件内容读进来
//    outLable << labeList.join("|") << "|";
//    outFeature << totalFeature.join("|") << "|";
//    outLable.flush();
//    outFeature.flush();
//    tmpFile1.close();
//    tmpFile2.close();
//    tmpFile1.open(QIODevice::ReadOnly);
//    tmpFile2.open(QIODevice::ReadOnly);

//    labeList = outLable.readAll().split("|");
//    totalFeature = outFeature.readAll().split("|");
//    qDebug("Done");



/************************nomalization!!!!!!!!!!!!!!!!!!!!!!!!*********************************/
    int l = labeList.length();//总特征数

    int dimension = totalFeature.at(0).split("'").length();//每个特征的维数

    double *max = (double*)malloc(sizeof(double) * dimension);
    double *min = (double*)malloc(sizeof(double) * dimension);

    for (int j = 0; j < dimension; j++) {
        max[j] = -11111111111111111;
        min[j] = 111111111111111111;
    }

    for (int i = 0; i < l; i++) {

        QStringList tmpList = totalFeature.at(i).split("'");

        for (int j = 0; j < dimension; j++) {
            max[j] = max[j] > tmpList.at(j).toDouble() ? max[j] : tmpList.at(j).toDouble();
            min[j] = min[j] < tmpList.at(j).toDouble() ? min[j] : tmpList.at(j).toDouble();
        }
        tmpList.clear();
    }

//    for (int j = 0; j < dimension; j++) {
//        qDebug("j = %d, max = %lf, min = %lf", j, max[j], min[j]);
//    }

/*处理标签和特征值，生成数组**********************************************************************/
/*对 labeList 和 totalFeature 进行处理*********************************************************/

    double *y = (double *)malloc(sizeof(double) * l);

    //直接malloc一个二维结构体数组，始终不能成功，从网上参考方法，将二维数组一维化，把地址传递给二维数组指针，妙哉妙哉~~~~~~
    struct svm_node ** x = new struct svm_node *[l];
    struct svm_node *x_space = new struct svm_node [l * (dimension + 1)];

    for (int i = 0; i < l; i++) {

        //对标签处理
        y[i] = labeList.at(i).toDouble();

        //对数组处理
        QStringList tmpList = totalFeature.at(i).split("'");

        for (int j = 0; j < dimension; j++) {
            x_space[i * (dimension + 1) + j].index = j + 1;
//            x_space[i * (dimension + 1) + j].value = tmpList.at(j).toDouble();
            x_space[i * (dimension + 1) + j].value = (tmpList.at(j).toDouble() - min[j]) / (max[j] - min[j]);
        }
        x_space[i * (dimension + 1) + dimension].index = -1;
        x_space[i * (dimension + 1) + dimension].value = 0;
        x[i] = &x_space[i * (dimension + 1)];

        tmpList.clear();
    }

    for (int i = 0; i < l; i++) {
        for (int j = 0; j < dimension; j++) {
            cout << x[i][j].index << ": " << x[i][j].value << "\t";
        }
        cout << endl;
    }


/*构建svm_problem以及svm_parameter,训练svm并保存svm_model************************************************************/
    struct svm_problem prob;
    prob.l = l;
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
    struct svm_model *model = svm_train(&prob, &param);
    svm_save_model("svm_model.txt", model);

    //释放内存
    labeList.clear();
    totalFeature.clear();
    svm_destroy_param(&param);
    svm_free_and_destroy_model(&model);
    delete[] prob.x;
    delete[] prob.y;
    delete[] x_space;
    QMessageBox resultMessage(QMessageBox::NoIcon, "SVM训练完成", "训练完成");
    resultMessage.setIconPixmap(QPixmap("images/vector.png"));
    resultMessage.exec();
    return;
}



void classification::svmPredict(QStringList fileNames)
{
/*错误检测，文件名为空以及没有svm_model***************************************************/
    if (fileNames.length() < 1)
        return;
    struct svm_model *model = svm_load_model("svm_model.txt");
    if (model == NULL) {
         QMessageBox aboutMessage(QMessageBox::NoIcon, "出错", "请先训练或者载入SVM");
         aboutMessage.setIconPixmap(QPixmap("images/error.png"));
         aboutMessage.exec();
         return;
     }
/*处理所选的所有图片，输出为两个QStringList， 标签和特征值**************************************/
    QStringList labeList, totalFeature;
    for (int i = 0; i < fileNames.length(); i++) {
        //qDebug("i = %d", i);

        //处理路径名并打开图片
        QString eachFile = fileNames.at(i);//qDebug("%s", eachFile.toLocal8Bit().data());
        IplImage* eachImage = cvLoadImage(eachFile.toLocal8Bit().data());

        //取标签（文件名第一个字符）
        QStringList tmp = eachFile.split("/");
        QString lable = tmp.at(tmp.length() - 1).left(1);

        //处理每张图片获取特征值（可能会有许多），并且把相应个数的标签放入lable的list中
        QStringList featureList;
        featureList = nclseg::seg4train(eachImage);
        for (int j = 0; j < featureList.length(); j++) {
            labeList << lable;
            totalFeature << featureList.at(j);
//            qDebug("%s\n", featureList.at(j).toLocal8Bit().data());
        }
//        eachFile.~QString();
        featureList.clear();
        cvReleaseImage(&eachImage);
        tmp.clear();
    }

/*处理标签和特征值，生成数组**********************************************************************/
/*对 labeList 和 totalFeature 进行处理*********************************************************/
/*生成 struct svm_node ** x 和 double *y  ****************************************************************************/
    int l = totalFeature.length();//总特征数
    int dimension = totalFeature.at(0).split("'").length();//每个特征的维数

//nomalization
    double *max = (double*)malloc(sizeof(double) * dimension);
    double *min = (double*)malloc(sizeof(double) * dimension);

    for (int j = 0; j < dimension; j++) {
        max[j] = -11111111111111111;
        min[j] = 111111111111111111;
    }

    for (int i = 0; i < l; i++) {

        QStringList tmpList = totalFeature.at(i).split("'");

        for (int j = 0; j < dimension; j++) {
            max[j] = max[j] > tmpList.at(j).toDouble() ? max[j] : tmpList.at(j).toDouble();
            min[j] = min[j] < tmpList.at(j).toDouble() ? min[j] : tmpList.at(j).toDouble();
        }
        tmpList.clear();
    }



    double *y = (double *)malloc(sizeof(double) * l);

    //直接malloc一个二维结构体数组，始终不能成功，从网上参考方法，将二维数组一维化，把地址传递给二维数组指针，妙哉妙哉~~~~~~
    struct svm_node ** x = new struct svm_node *[l];
    struct svm_node *x_space = new struct svm_node [l * (dimension + 1)];

    for (int i = 0; i < l; i++) {

        //对标签处理
        y[i] = labeList.at(i).toDouble();

        //对数组处理
        QStringList tmpList = totalFeature.at(i).split("'");
        for (int j = 0; j < dimension; j++) {
            x_space[i * (dimension + 1) + j].index = j + 1;
//            x_space[i * (dimension + 1) + j].value = tmpList.at(j).toDouble();
            x_space[i * (dimension + 1) + j].value = (tmpList.at(j).toDouble() - min[j]) / (max[j] - min[j]);

        }
        x_space[i * (dimension + 1) + dimension].index = -1;
        x_space[i * (dimension + 1) + dimension].value = 0;
        x[i] = &x_space[i * (dimension + 1)];

        tmpList.clear();
    }

    for (int i = 0; i < l; i++) {
        for (int j = 0; j < dimension + 1; j++) {
            cout << x[i][j].index << ": " << x[i][j].value << "\t";
        }
        cout << endl;
    }

/*载入svm_model, 进行预测*****************************************************************************/

    int hit = 0;
    for (int i = 0; i < l; i++) {
        double result;
        result = svm_predict(model, x[i]);
        qDebug("y[i] = %lf, result = %.0lf", y[i], result);
        hit = (result == y[i]) ? hit + 1 : hit;
    }

/*预测完成，释放内存，生成报告*****************************************************************************/
    labeList.clear();
    totalFeature.clear();
    delete[] x_space;
    svm_free_and_destroy_model(&model);
    QStringList report;
    report << "total pictures:" << QString::number(l) << "\naccurate predict: " << QString::number(hit) << "\naccuracy: " << QString::number(hit * 100 / l) << "%";
    QMessageBox resultMessage(QMessageBox::NoIcon, "结果", report.join(""));
    resultMessage.setIconPixmap(QPixmap("images/vector.png"));
    resultMessage.exec();
    report.clear();
    return;

}
