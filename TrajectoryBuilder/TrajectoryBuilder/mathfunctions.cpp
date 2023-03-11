#include "mathfunctions.h"
#include <QVector>


MathTypes::AbstractType * MathOperators::MathOperatorAssign::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    if ((arg_l == nullptr) || (arg_r == nullptr)) return nullptr;
    return arg_l->assign(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorAssign::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if (arg_type_l == arg_type_r) return arg_type_l;
    else return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
}

MathTypes::AbstractType * MathOperators::MathOperatorSum::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    if ((arg_l == nullptr) || (arg_r == nullptr)) return nullptr;
    return arg_l->sum(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorSum::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_NONE) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_NONE)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE;
    }
}

MathTypes::AbstractType * MathOperators::MathOperatorSubtract::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    if ((arg_l == nullptr) || (arg_r == nullptr)) return nullptr;
    return arg_l->subtract(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorSubtract::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_NONE) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_NONE)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE;
    }
}

MathTypes::AbstractType * MathOperators::MathOperatorDivision::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    if ((arg_l == nullptr) || (arg_r == nullptr)) return nullptr;
    return arg_l->divide(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorDivision::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_NONE) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_NONE)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE;
    }
}

MathTypes::AbstractType * MathOperators::MathOperatorMultiply::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    if ((arg_l == nullptr) || (arg_r == nullptr)) return nullptr;
    return arg_l->multiply(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorMultiply::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_NONE) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_NONE)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE;
    }
}

MathTypes::AbstractType * MathOperators::MathOperatorPower::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    if ((arg_l == nullptr) || (arg_r == nullptr)) return nullptr;
    return arg_l->to_power(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorPower::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_NONE) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_NONE)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionSin::
Execute(QVector<MathTypes::AbstractType *> * args){
    for (auto iter = args->begin(); iter != args->end(); iter++){
        if (*iter == nullptr) return nullptr;
    }
    if (args->size() < this->ArgumentsAmount)
        return nullptr;
    else
        return args->at(0)->sin();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionSin::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionCos::Execute(QVector<MathTypes::AbstractType *> * args){
    for (auto iter = args->begin(); iter != args->end(); iter++){
        if (*iter == nullptr) return nullptr;
    }
    if (args->size() < this->ArgumentsAmount)
        return nullptr;
    else
        return args->at(0)->cos();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionCos::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionTan::
Execute(QVector<MathTypes::AbstractType *> * args){
    for (auto iter = args->begin(); iter != args->end(); iter++){
        if (*iter == nullptr) return nullptr;
    }
    if (args->size() < this->ArgumentsAmount)
        return nullptr;
    else
        return args->at(0)->tg();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionTan::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionCtg::
Execute(QVector<MathTypes::AbstractType *> * args){
    for (auto iter = args->begin(); iter != args->end(); iter++){
        if (*iter == nullptr) return nullptr;
    }
    if (args->size() < this->ArgumentsAmount)
        return nullptr;
    else
        return args->at(0)->ctg();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionCtg::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionAbs::
Execute(QVector<MathTypes::AbstractType *> * args){
    for (auto iter = args->begin(); iter != args->end(); iter++){
        if (*iter == nullptr) return nullptr;
    }
    if (args->size() < this->ArgumentsAmount)
        return nullptr;
    else
        return args->at(0)->abs();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionAbs::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionLog::
Execute(QVector<MathTypes::AbstractType *> * args){
    for (auto iter = args->begin(); iter != args->end(); iter++){
        if (*iter == nullptr) return nullptr;
    }
    if (args->size() < this->ArgumentsAmount)
        return nullptr;
    else
        return args->at(0)->log();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionLog::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionExp::
Execute(QVector<MathTypes::AbstractType *> * args){
    for (auto iter = args->begin(); iter != args->end(); iter++){
        if (*iter == nullptr) return nullptr;
    }
    if (args->size() < this->ArgumentsAmount)
        return nullptr;
    else
        return args->at(0)->exp();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionExp::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionRange::
Execute(QVector<MathTypes::AbstractType *> * args){
    for (auto iter = args->begin(); iter != args->end(); iter++){
        if (*iter == nullptr) return nullptr;
    }
    if (args->size() < this->ArgumentsAmount)
        return nullptr;
    else{
        qreal start = (dynamic_cast<MathTypes::TypeDouble *>(args->at(0))->GetValue());
        qreal end = (dynamic_cast<MathTypes::TypeDouble *>(args->at(1))->GetValue());
        qreal step = std::abs(dynamic_cast<MathTypes::TypeDouble *>(args->at(2))->GetValue());

        QVector<qreal> r_vec;
        if (start > end){
            qreal eps = (start - end) * 0.001;
            for (; start > (end - eps); start -= step)
                r_vec.push_back(start);
        }
        else if (start < end){
            qreal eps = (end - start) * 0.001;
            for (; start < (end + eps); start += step)
                r_vec.push_back(start);
        }

        MathTypes::TypeVector * ret_vec = new MathTypes::TypeVector(r_vec);
        return ret_vec;
    }
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionRange::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (this->ArgumentsAmount <= args.size()){
        if ((args[0] == MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE) &&
            (args[1] == MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE) &&
            (args[2] == MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE)){
            return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
        }
        else{
            return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
        }
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionPlot3D::
Execute(QVector<MathTypes::AbstractType *> * args){
    if (!args) return nullptr;
    if (args->size() != this->ArgumentsAmount) return nullptr;
    QVector<QVector<qreal>> data;
    for (auto iter = args->begin(); iter != args->end(); iter++){
        if ((*iter)->GetType() == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR){
            data.push_back(dynamic_cast<MathTypes::TypeVector *>((*iter))->GetRawCopy());
        }
        else if ((*iter)->GetType() == MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE){
            QVector<qreal> tmp;
            tmp.push_back(dynamic_cast<MathTypes::TypeDouble *>((*iter))->GetValue());
            data.push_back(tmp);
        }
        else{
            return nullptr;
        }
    }
    QVector<QVector<qreal>> plot_data(3);
    QVector<qreal>::iterator iter[3] = {
        data[0].begin(),
        data[1].begin(),
        data[2].begin()
    };
    for (; (iter[0] != data[0].end()) ||
           (iter[1] != data[1].end()) ||
           (iter[2] != data[2].end()); ){
        if (iter[0] == data[0].end()) iter[0]--;
        if (iter[1] == data[1].end()) iter[1]--;
        if (iter[2] == data[2].end()) iter[2]--;
        plot_data[0].push_back(*(iter[0])); plot_data[1].push_back(*(iter[1])); plot_data[2].push_back(*(iter[2]));
        iter[0]++; iter[1]++; iter[2]++;
    }

    QtDataVisualization::Q3DScatter * plot = new QtDataVisualization::Q3DScatter();
    QWidget * plot_widget = QWidget::createWindowContainer(plot);
    plot_widget->setAttribute(Qt::WA_DeleteOnClose);

    plot->axisX()->setAutoAdjustRange(true);
    plot->axisY()->setAutoAdjustRange(true);
    plot->axisZ()->setAutoAdjustRange(true);

    plot->activeTheme()->setType(QtDataVisualization::Q3DTheme::ThemePrimaryColors);
    plot->activeTheme()->setFont(QFont("Sans Serif", 50, QFont::Weight::Normal));
    plot->setShadowQuality(QtDataVisualization::QAbstract3DGraph::ShadowQualityNone);

    QtDataVisualization::QScatterDataProxy * data_proxy = new QtDataVisualization::QScatterDataProxy;
    QtDataVisualization::QScatter3DSeries * data_series = new QtDataVisualization::QScatter3DSeries(data_proxy);

    data_series->setItemSize(0.1);
    data_series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    data_series->setMeshSmooth(true);
    data_series->setBaseColor(QColor(255, 0, 0));
    plot->addSeries(data_series);

    for (int iter = 0; (iter < plot_data[0].size()) && (iter < plot_data[1].size()) && (iter < plot_data[2].size()); iter++){
        plot->seriesList().at(0)->dataProxy()->addItem(
                    QtDataVisualization::QScatterDataItem(QVector3D(plot_data[0][iter], plot_data[1][iter], plot_data[2][iter])));
    }

    plot_widget->show();

    return nullptr;
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionPlot3D::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
}

MathTypes::AbstractType * MathFunctions::MathFunctionPlot2D::
Execute(QVector<MathTypes::AbstractType *> * args){
    if (!args) return nullptr;
    if (args->size() != this->ArgumentsAmount) return nullptr;
    QVector<QVector<qreal>> data;
    for (auto iter = args->begin(); iter != args->end(); iter++){
        if ((*iter)->GetType() == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR){
            data.push_back(dynamic_cast<MathTypes::TypeVector *>((*iter))->GetRawCopy());
        }
        else if ((*iter)->GetType() == MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE){
            QVector<qreal> tmp;
            tmp.push_back(dynamic_cast<MathTypes::TypeDouble *>((*iter))->GetValue());
            data.push_back(tmp);
        }
        else{
            return nullptr;
        }
    }
    QVector<QVector<qreal>> plot_data(2);
    QVector<qreal>::iterator iter[2] = {
        data[0].begin(),
        data[1].begin()
    };
    for (; (iter[0] != data[0].end()) ||
           (iter[1] != data[1].end()); ){
        if (iter[0] == data[0].end()) iter[0]--;
        if (iter[1] == data[1].end()) iter[1]--;
        plot_data[0].push_back(*(iter[0])); plot_data[1].push_back(*(iter[1]));
        iter[0]++; iter[1]++;
    }
    QCustomPlot * plot = new QCustomPlot();
    plot->setAttribute(Qt::WA_DeleteOnClose);
    plot->addGraph();
    plot->graph(0)->setData(plot_data[0], plot_data[1]);
    plot->rescaleAxes();
    plot->setMinimumSize(QSize(400, 400));
    plot->show();

    return nullptr;
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionPlot2D::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
}

uint8_t MathFunctions::AbstractFunction::
GetArgumentsExpected(void){
    return this->ArgumentsAmount;
}

MathTypes::AbstractType * MathFunctions::MathFunctionExport3DTrajectory::
Execute(QVector<MathTypes::AbstractType *> * args){
    if (!args) return nullptr;
    if (args->size() != this->ArgumentsAmount) return nullptr;
    QVector<QVector<qreal>> data(3);
    auto data_iter = data.begin();
    for (auto iter = args->begin(); iter != args->end(); iter++){
        if ((*iter)->GetType() == MathTypes::AbstractType::MATH_VAR_TYPE_NONE) return nullptr;
        else if ((*iter)->GetType() == MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE){
            qreal val = dynamic_cast<MathTypes::TypeDouble *>(*iter)->GetValue();
            data_iter->push_back(val);
            data_iter++;
        }
        else if ((*iter)->GetType() == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR){
            *data_iter = (dynamic_cast<MathTypes::TypeVector *>((*iter))->GetRawCopy());
            data_iter++;
        }
        else{
            return nullptr;
        }
    }
    QVector<qreal>::iterator iter[3] = {
        data[0].begin(),
        data[1].begin(),
        data[2].begin()
    };
    QString filter;
    QString fname = QFileDialog::getSaveFileName(nullptr, "Export 3D Trajectory", QDir::currentPath(), "CSV (*.csv)", &filter);
    QFile file(fname + ".csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream file_stream(&file);
        file_stream << "X;Y;Z\n";
        for (; (iter[0] != data[0].end()) ||
               (iter[1] != data[1].end()) ||
               (iter[2] != data[2].end()); ){
            if (iter[0] == data[0].end()) iter[0]--;
            if (iter[1] == data[1].end()) iter[1]--;
            if (iter[2] == data[2].end()) iter[2]--;
            file_stream << QString::asprintf("%.2f;%.2f;%.2f\n", *(iter[0]), *(iter[1]), *(iter[2]));
            iter[0]++; iter[1]++; iter[2]++;
        }
        file.close();
    }
    return nullptr;
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionExport3DTrajectory::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
}
