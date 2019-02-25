#include "main_dlg.h"
#include "ui_symbolbook.h"

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

void MainDlg::on_input_box_textEdited(const QString &arg1)
{
    //qDebug() << arg1;
    if(arg1.size() >= 3){
        model_->offerKey(arg1);
    }else{
        model_->offerKey(QString());
    }
}
