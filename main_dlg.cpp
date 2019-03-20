/* Original Work Copyright (c) 2019 Giuseppe Baccini - giuseppe.baccini@live.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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
    file_browser_model_(new QFileSystemModel()),
    model_(new model::address_model(parent)),
    ui(new Ui::SymbolBook)
{
    ui->setupUi(this);
    spinner_.reset(new WaitingSpinnerWidget(ui->result_table));

    //file browser model/view
    file_browser_model_->setRootPath("/");
    file_browser_model_->setNameFilters(QStringList() << "*.lib");
    file_browser_model_->setNameFilterDisables(false);
    ui->file_browser->setModel(file_browser_model_.get());
    ui->file_browser->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->file_browser->hideColumn(1);
    ui->file_browser->hideColumn(2);

    //result table model/view
    ui->result_table->setModel(model_.get());
    ui->result_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->result_table->setItemDelegate(new model::highlight_delegate());

    //load settings
    QSettings settings;
    settings.beginGroup(KEY_WINDOW);
    if(settings.contains(KEY_SIZE)) {
        resize(settings.value(KEY_SIZE).toSize());
    }
    if(settings.contains(KEY_POSITION)) {
        move(settings.value(KEY_POSITION).toPoint());
    }
    settings.endGroup();

    connect(this, SIGNAL(modelChanged()), this, SLOT(onModelChanged()));

    connect(this, SIGNAL(processingSourceFile(const QString)), this,
            SLOT(onProcessingSourceFile(const QString &)));

    connect(ui->result_table, SIGNAL(enterPressed(QModelIndex)), this,
            SLOT(onResultTableEnterPressed(const QModelIndex &)));

    connect(ui->result_table, SIGNAL(doubleClicked(QModelIndex)), this,
            SLOT(onResultTableEnterPressed(const QModelIndex &)));

    connect(ui->file_browser, SIGNAL(spacePressed(QModelIndex)), this,
            SLOT(on_actionSelect_Source_triggered()));
}

MainDlg::~MainDlg()
{
    delete ui;
}

void MainDlg::closeEvent(QCloseEvent *)
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
    if(!ifs) {
        return;
    }
    std::string line;
    while(std::getline(ifs, line)) {
        source_scan_set_.insert(line);
    }
}

void MainDlg::setup_spinner()
{
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
                               const QStringList &nameFilters,
                               std::list<QFileInfo> &sym_file_list)
{
    QDirIterator it(path, nameFilters, QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        it.next();
        sym_file_list.push_back(it.fileInfo());
    }
}

void MainDlg::load_syms(const std::list<QFileInfo> &sym_file_list)
{
    ui->mainToolBar->setEnabled(false);
    ui->input_box->setEnabled(false);
    setup_spinner();
    spinner_->start();
    load_symbs_worker_.reset(new std::thread([ &, sym_file_list]() {
        std::for_each(sym_file_list.begin(), sym_file_list.end(), [&](auto &it) {
            emit processingSourceFile(it.absoluteFilePath());
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
        emit modelChanged();
    }));
}

void MainDlg::drop_syms()
{
    source_scan_set_.clear();
    model_->drop_symbols();
    ui->input_box->clear();
    emit modelChanged();
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

    std::list<QFileInfo> sym_file_list;

    //load all files path
    std::for_each(source_scan_set_.begin(), source_scan_set_.end(), [&](auto &it) {
        QFile f(it.c_str());
        if(!f.exists()) {
            ui->statusBar->setStyleSheet("color : red;");
            ui->statusBar->showMessage(tr("source: %1 does not exist!").arg(it.c_str()));
        } else {
            QFileInfo finfo(f);
            if(finfo.isDir()) {
                obtain_sym_files(tr(it.c_str()), QStringList() << "*.lib", sym_file_list);
            } else {
                sym_file_list.push_back(finfo);
            }
        }
    });

    //load symbols
    load_syms(sym_file_list);
}

void MainDlg::on_actionDrop_Symbols_triggered()
{
    drop_syms();
}

void MainDlg::onModelChanged()
{
    if(load_symbs_worker_ && load_symbs_worker_->joinable()) {
        load_symbs_worker_->join();
    }
    if(spinner_) {
        spinner_->stop();
        ui->mainToolBar->setEnabled(true);
        ui->input_box->setEnabled(true);
    }
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

void MainDlg::onProcessingSourceFile(const QString &source_file)
{
    ui->statusBar->setStyleSheet("color : blue;");
    ui->statusBar->showMessage(tr("scanning source: %1 ..").arg(source_file));
}

void MainDlg::on_actionSelect_Source_triggered()
{
    QModelIndex index = ui->file_browser->currentIndex();
    if(!index.isValid()) {
        ui->statusBar->setStyleSheet("color : red;");
        ui->statusBar->showMessage(tr("select a valid source!"));
    }
    QFileInfo finfo(file_browser_model_->fileInfo(index));
    source_scan_set_.insert(finfo.absoluteFilePath().toStdString());
    ui->statusBar->setStyleSheet("color : blue;");
    ui->statusBar->showMessage(tr("added source: %1").arg(finfo.absoluteFilePath()));
}
