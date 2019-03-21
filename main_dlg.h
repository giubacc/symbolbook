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

#pragma once
#pragma warning(disable:4100)

#include <QMainWindow>
#include "symbol_model.h"
#include "waitingspinnerwidget.h"

#include <string>
#include <set>

namespace Ui {
class SymbolBook;
}

class MainDlg : public QMainWindow {
        Q_OBJECT

    public:
        explicit MainDlg(QWidget *parent = nullptr);
        ~MainDlg() override;

    protected:
        void closeEvent(QCloseEvent *event) override;

    private:
        void load_dumpbin();
        void load_scan_dir_set();
        void setup_spinner();

        void obtain_sym_files(const QString &path,
                              const QStringList &nameFilters,
                              std::list<QFileInfo> &sym_file_list);

        void load_syms(const std::list<QFileInfo> &sym_file_list);
        void drop_syms();

        void afterScan();

    private slots:
        void onDumpbinError();
        void onModelChanged();
        void onResultTableEnterPressed(const QModelIndex &index);
        void onProcessingSourceFile(const QString &source_file);

        void on_input_box_textEdited(const QString &arg1);
        void on_actionLoad_Symbols_triggered();
        void on_actionDrop_Symbols_triggered();
        void on_actionSelect_Source_triggered();

    signals:
        void dumpbinError();
        void modelChanged();
        void processingSourceFile(const QString source_file);

    private:
        std::unique_ptr<QFileSystemModel> file_browser_model_;
        std::string dumpbin_;
        std::unique_ptr<model::address_model> model_;
        std::set<std::string> source_scan_set_;
        std::unique_ptr<std::thread> load_symbs_worker_;
        std::unique_ptr<WaitingSpinnerWidget> spinner_;

    private:
        Ui::SymbolBook *ui;
};
