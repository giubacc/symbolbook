#include "main_dlg.h"
#include "ui_symbolbook.h"

#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#define KEY_WINDOW      "SymbolBookKeyWindow"
#define KEY_POSITION    "SymbolBookKeyPosition"
#define KEY_SIZE        "SymbolBookKeySize"

#define SYM_LOCS        "symlocs"
#define DUMPBIN_LOC     "dumpbinloc"

MainDlg::MainDlg(QWidget *parent) :
    QMainWindow(parent),
    model_(new model::address_model(parent)),
    ui(new Ui::SymbolBook)
{
    ui->setupUi(this);
    ui->result_table->setModel(model_.get());
    ui->result_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->result_table->setItemDelegate(new model::highlight_delegate());

    //settings bgn
    QSettings settings;
    settings.beginGroup(KEY_WINDOW);
    if(settings.contains(KEY_SIZE)) {
        resize(settings.value(KEY_SIZE).toSize());
    }
    if(settings.contains(KEY_POSITION)) {
        move(settings.value(KEY_POSITION).toPoint());
    }
    settings.endGroup();
    //settings end

    connect(this, SIGNAL(symbolsLoaded()), this, SLOT(onSymbolsLoaded()));
    connect(ui->result_table, SIGNAL(enterPressed(QModelIndex)), this,
            SLOT(onResultTableEnterPressed(const QModelIndex &)));

}

MainDlg::~MainDlg()
{
    delete ui;
}

void MainDlg::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup(KEY_WINDOW);
    settings.setValue(KEY_SIZE, size());
    settings.setValue(KEY_POSITION, pos());
    settings.endGroup();
}

void MainDlg::load_dumpbin()
{
    std::ifstream ifs(DUMPBIN_LOC);
    std::string line;
    std::getline(ifs, line);
    std::string::reverse_iterator rit = line.rbegin();
    if(rit != line.rend() && *rit != '\\') {
        line += '\\';
    }
    dumpbin_.assign(line).append("dumpbin.exe");
}

void MainDlg::load_scan_dir_set()
{
    std::ifstream ifs(SYM_LOCS);
    std::string line;
    while(std::getline(ifs, line)) {
        //qDebug() << line.c_str();
        scan_dir_set_.insert(line);
    }
}

void MainDlg::setup_spinner()
{
    spinner_.reset(new WaitingSpinnerWidget(Qt::ApplicationModal, parentWidget()));
    spinner_->setRoundness(70.0);
    spinner_->setMinimumTrailOpacity(15.0);
    spinner_->setTrailFadePercentage(70.0);
    spinner_->setNumberOfLines(12);
    spinner_->setLineLength(10);
    spinner_->setLineWidth(5);
    spinner_->setInnerRadius(10);
    spinner_->setRevolutionsPerSecond(1);
    spinner_->setColor(QColor(81, 4, 71));
}

void MainDlg::obtain_sym_files(const QString &path,
                               const QStringList &nameFilters)
{
    QDirIterator it(path, nameFilters, QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        qDebug() << it.next();
        sym_file_list_.push_back(it.fileInfo());
    }
}

void MainDlg::generate_sym_db()
{
    setup_spinner();
    spinner_->start();
    load_symbs_worker_.reset(new std::thread([&]() {
        std::for_each(sym_file_list_.begin(), sym_file_list_.end(), [&](auto &it) {
            QProcess dumpbin;
            dumpbin.setProcessChannelMode(QProcess::MergedChannels);
            QStringList dumpbin_args;
            dumpbin_args << "/LINKERMEMBER:1" << it.absoluteFilePath();
            dumpbin.start(dumpbin_.c_str(), dumpbin_args);
            // Get the output
            QByteArray output;
            if(dumpbin.waitForStarted(-1)) {
                while(dumpbin.waitForReadyRead(-1)) {
                    output += dumpbin.readAllStandardOutput();
                }
            }
            dumpbin.waitForFinished();
            model_->add_symbol_file_to_model(it, output.toStdString());
        });
        emit symbolsLoaded();
    }));
}

void MainDlg::on_input_box_textEdited(const QString &arg1)
{
    if(!model_->symbolsCount()) {
        ui->statusBar->setStyleSheet("color : red;");
        ui->statusBar->showMessage(tr("no symbols loaded!"), 5000);
    }
    if(arg1.size() >= 3) {
        model_->offerKey(arg1);
    } else {
        model_->offerKey(QString());
    }
}

void MainDlg::on_actionLoad_Symbols_triggered()
{
    //load dumpbin path
    load_dumpbin();

    //reload scan dir path set
    load_scan_dir_set();

    //load all files path
    std::for_each(scan_dir_set_.begin(), scan_dir_set_.end(), [&](auto &it) {
        obtain_sym_files(tr(it.c_str()), QStringList() << "*.lib");
    });

    //generate local db of symbols
    generate_sym_db();
}

void MainDlg::onSymbolsLoaded()
{
    load_symbs_worker_->join();
    spinner_->stop();
    ui->statusBar->setStyleSheet("color : blue;");
    ui->statusBar->showMessage(tr("%1 symbols loaded").arg(model_->symbolsCount()));
}

void MainDlg::onResultTableEnterPressed(const QModelIndex &index)
{
    const QFileInfo &fileInfo = model_->getFInfo(index);
    QStringList args;
    args << "/select," << QDir::toNativeSeparators(fileInfo.canonicalFilePath());
    QProcess::startDetached("explorer.exe", args);
}
