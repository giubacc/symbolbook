#ifndef SYMBOLBOOK_H
#define SYMBOLBOOK_H

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
        ~MainDlg();

    private:
        void load_dumpbin();
        void load_scan_dir_set();

        void obtain_sym_files(const QString &path,
                              const QStringList &nameFilters);

        void generate_sym_db();

    private slots:
        void on_input_box_textEdited(const QString &arg1);
        void on_actionLoad_Symbols_triggered();
        void onSymbolsLoaded();

    signals:
        void symbolsLoaded();

    private:
        std::string dumpbin_;
        std::unique_ptr<model::address_model> model_;
        std::set<std::string> scan_dir_set_;
        std::list<QFileInfo> sym_file_list_;
        std::unique_ptr<std::thread> load_symbs_worker_;
        std::unique_ptr<WaitingSpinnerWidget> spinner_;

    private:
        Ui::SymbolBook *ui;
};

#endif // SYMBOLBOOK_H
