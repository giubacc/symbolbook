#ifndef SYMBOLBOOK_H
#define SYMBOLBOOK_H

#pragma warning(disable:4100)

#include <QMainWindow>
#include "symbol_model.h"

namespace Ui {
class SymbolBook;
}

class MainDlg : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainDlg(QWidget *parent = nullptr);
    ~MainDlg();

private slots:
    void on_input_box_textEdited(const QString &arg1);

private:
    std::unique_ptr<model::address_model> model_;

private:
    Ui::SymbolBook *ui;
};

#endif // SYMBOLBOOK_H
