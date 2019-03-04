#include "main_dlg.h"
#include "ui_symbolbook.h"

#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

MainDlg::MainDlg(QWidget *parent) :
    QMainWindow(parent),
    model_(new model::address_model(parent)),
    ui(new Ui::SymbolBook)
{
    ui->setupUi(this);
    ui->result_table->setModel(model_.get());
    ui->result_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->result_table->setItemDelegate(new model::highlight_delegate());
}

MainDlg::~MainDlg()
{
    delete ui;
}

QStringList &MainDlg::obtain_files(QStringList &files,
                                   const QString &path,
                                   const QStringList &nameFilters)
{
    QDirIterator it(path, nameFilters, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()){
        files << it.next();
        qDebug() << it.next();
    }
    return files;
}

void MainDlg::on_input_box_textEdited(const QString &arg1)
{
    //qDebug() << arg1;
    if(arg1.size() >= 3){
        model_->offerKey(arg1);
    }else{
        model_->offerKey(QString());
    }
}

void MainDlg::on_actionLoad_Symbols_triggered()
{
    std::ifstream ifs("locs.txt");
    std::string line;
    while(std::getline(ifs, line)){

    }
}
