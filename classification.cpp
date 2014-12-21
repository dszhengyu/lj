#include "classification.h"

classification::classification()
{
}

void classification::trainSvm(QStringList fileNames)
{

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
            //qDebug("%s\n", featureList.at(j).toLocal8Bit().data());
        }
    }
    //cvDestroyAllWindows();

/*处理标签和特征值，生成数组**********************************************************************/
/*对 labeList 和 totalFeature 进行处理*********************************************************/
    int l = totalFeature.length();//总特征数
    int dimension = totalFeature.at(0).split("'").length();//每个特征的维数
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
            x_space[i * (dimension + 1) + j].value = tmpList.at(j).toDouble();

        }
        x_space[i * (dimension + 1) + dimension].index = -1;
        x_space[i * (dimension + 1) + dimension].value = 0;
        x[i] = &x_space[i * (dimension + 1)];
    }


    for (int i = 0; i < l; i ++) {
        qDebug("\t%f", y[i]);
        for (int j = 0; j <= dimension; j++)
            qDebug("index = %d, value = %f", x[i][j].index, x[i][j].value);
        qDebug("\n");
    }


/*构建svm_problem以及svm_parameter,训练svm并保存svm_model************************************************************/
    struct svm_problem prob;
    prob.l = l;
    prob.y = y;
    prob.x = x;

    struct svm_parameter param;
    param.svm_type = C_SVC;
    param.kernel_type = RBF;
    param.degree = 3;
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
    svm_model *model = svm_train(&prob, &param);
    svm_save_model("svm_model.txt", model);

    //释放内存
    svm_destroy_param(&param);
    svm_free_and_destroy_model(&model);
    delete[] prob.x;
    delete[] prob.y;
    delete[] x_space;
}

