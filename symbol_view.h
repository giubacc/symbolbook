#pragma once
#pragma warning(disable:4100)

#include <QtWidgets>

namespace view {

class symbol_view : public QTableView {
        Q_OBJECT
    public:
        symbol_view(QWidget *parent = nullptr) : QTableView(parent) {}

    protected:
        void keyPressEvent(QKeyEvent *evt) override;

    signals:
        void enterPressed(QModelIndex);
};

}
