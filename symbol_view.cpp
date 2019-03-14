#include "symbol_view.h"

namespace view {

void symbol_view::keyPressEvent(QKeyEvent *evt)
{
    if(evt->key()==Qt::Key_Return) {
        emit enterPressed(this->currentIndex());
    } else {
        QTableView::keyPressEvent(evt);
    }
}

}
