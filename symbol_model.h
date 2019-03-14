#ifndef RESULT_TABLE_H
#define RESULT_TABLE_H

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

#endif // RESULT_TABLE_H
