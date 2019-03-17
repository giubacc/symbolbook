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

#include <QtWidgets>
#include <set>
#include <vector>
#include <map>

namespace model {

struct symbol_entry {
    const std::string &entry_str_;
    const QFileInfo finfo_;
};

class highlight_delegate : public QStyledItemDelegate {
        Q_OBJECT

    public:
        highlight_delegate(QWidget *parent = nullptr) : QStyledItemDelegate(parent) {}

        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
};


class address_model : public QAbstractTableModel {
        Q_OBJECT
    public:
        explicit address_model(QWidget *parent = nullptr);

    public slots:
        void offerKey(const QString &key);

        // QAbstractItemModel interface
    public:
        int rowCount(const QModelIndex &parent) const override;
        int columnCount(const QModelIndex &parent) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex()) override;
        bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex()) override;

        QString cur_key() const;
        void add_symbol_file_to_model(const QFileInfo &finfo, const std::string &dumpbin_str);
        void drop_symbols();

        size_t symbolsCount() {
            return symb_map_.size();
        }

        const QFileInfo &getFInfo(const QModelIndex &index) {
            return cur_symbols_.at(index.row())->finfo_;
        }

    private:
        std::set<std::string> entry_str_pool_;
        std::multimap<std::string, symbol_entry> symb_map_;
        std::vector<symbol_entry *> cur_symbols_;
        QString cur_key_;
};

}
