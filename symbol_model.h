#ifndef RESULT_TABLE_H
#define RESULT_TABLE_H

#pragma warning(disable:4100)

#include <QtWidgets>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <memory>

namespace model{

struct address_entry{
    std::string entry_str_;
};

class highlight_delegate : public QStyledItemDelegate
{
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

signals:


public slots:

    void offerKey(const QString &key);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex()) override;

    QString cur_key() const;

private:
    void load_model();

private:
    std::multimap<std::string, address_entry> ae_map_;
    std::vector<address_entry*> cur_ae_;
    QString cur_key_;
};

}

#endif // RESULT_TABLE_H
