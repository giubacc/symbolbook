#include "main_dlg.h"
#include "ui_symbolbook.h"

#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#define SYM_PATH_SET_FILE "locs.txt"
//#define DUMPBIN_PRG "C:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\bin\\amd64\\dumpbin.exe"
#define DUMPBIN_PRG "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Tools\\MSVC\\14.16.27023\\bin\\Hostx64\\x64\\dumpbin.exe"
//#define DUMPBIN_PRG "C:\\Users\\giuse\\source\\repos\\ConsoleApplication1\\Debug\\ConsoleApplication1.exe"

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

void MainDlg::load_scan_dir_set()
{
    std::ifstream ifs(SYM_PATH_SET_FILE);
    std::string line;
    while(std::getline(ifs, line)) {
        qDebug() << line.c_str();
        scan_dir_set_.insert(line);
    }
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

void load_indicator_wgt()
{
    QLabel *lbl = new QLabel;
    QMovie *movie = new QMovie(":/icons/ajax-loader.gif");
    lbl->setMovie(movie);
    lbl->show();
    movie->start();
}

void MainDlg::generate_sym_db()
{
    //load_indicator_wgt();
    std::for_each(sym_file_list_.begin(), sym_file_list_.end(), [&](auto &it) {
        QProcess dumpbin(this);
        dumpbin.setProcessChannelMode(QProcess::MergedChannels);
        QStringList dumpbin_args;
        dumpbin_args << "/LINKERMEMBER:1"
                     << it.absoluteFilePath();
        //<< tr("/OUT:%1.%2").arg(it.completeBaseName(), "sym");

        dumpbin.start(DUMPBIN_PRG, dumpbin_args);
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
}

void MainDlg::on_input_box_textEdited(const QString &arg1)
{
    if(arg1.size() >= 3) {
        model_->offerKey(arg1);
    } else {
        model_->offerKey(QString());
    }
}

void MainDlg::on_actionLoad_Symbols_triggered()
{
    //reload scan dir path set
    load_scan_dir_set();

    //load all files path
    std::for_each(scan_dir_set_.begin(), scan_dir_set_.end(), [&](auto &it) {
        obtain_sym_files(tr(it.c_str()),
                         QStringList() << "*.lib");
    });

    //generate local db of symbols
    generate_sym_db();
}
