#include "symbol_model.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>
#include "str_tok.h"

namespace model {

address_model::address_model(QWidget *parent) : QAbstractTableModel(parent)
{}

void address_model::offerKey(const QString &key)
{
    cur_key_ = key;
    if(!cur_symbols_.empty()) {
        removeRows(0, (int)cur_symbols_.size(), QModelIndex());
    }
    cur_symbols_.clear();
    if(!key.size()) {
        return;
    }
    auto kstr = key.toStdString();
    std::transform(kstr.begin(), kstr.end(), kstr.begin(), ::tolower);
    for(auto it = symb_map_.lower_bound(kstr); it != symb_map_.end(); it++) {
        if(it->first.find(kstr) == 0) {
            cur_symbols_.push_back(&it->second);
        } else {
            break;
        }
    }
    if(!cur_symbols_.empty()) {
        insertRows(0, cur_symbols_.size(), QModelIndex());
    }
}

int model::address_model::rowCount(const QModelIndex &parent) const
{
    return (int)cur_symbols_.size();
}

int model::address_model::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant address_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }
    if(orientation == Qt::Horizontal) {
        return section == 0 ? tr("Source File") : tr("Symbol");
    } else {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
}

QVariant model::address_model::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(index.row() >= (int)cur_symbols_.size() || index.row() < 0) {
        return QVariant();
    }
    if(index.column() != 0 && index.column() != 1) {
        return QVariant();
    }
    if(index.column() == 1 && role == Qt::DisplayRole) {
        return QString(cur_symbols_.at(index.row())->entry_str_.c_str());
    }
    if(index.column() == 0 && role == Qt::DisplayRole) {
        return QString(cur_symbols_.at(index.row())->finfo_.absoluteFilePath());
    }
    return QVariant();
}

bool model::address_model::insertRows(int position, int rows, const QModelIndex &index)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);
    endInsertRows();
    return true;
}

bool model::address_model::removeRows(int position, int rows, const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    cur_symbols_.clear();
    endRemoveRows();
    return true;
}

const std::string sym_seps(" $@?");

void address_model::add_symbol_file_to_model(const QFileInfo &finfo,
                                             const std::string &dumpbin_str)
{
    utl::str_tok tknz(dumpbin_str);
    std::string line;
    uint32_t no = 13;
    while(--no && tknz.next_token(line, "\r\n"));

    //read symbols no
    {
        utl::str_tok symbnotknz(line);
        std::string symbno;
        symbnotknz.next_token(symbno);
        std::stringstream ss(symbno);
        ss >> no;
    }
    while(no-- && tknz.next_token(line, "\r\n")) {
        utl::trim(line);
        std::string symbol;
        std::set<std::string> current_symbol_set;
        utl::str_tok strtk(line);
        strtk.next_token(symbol, sym_seps.c_str()); //1 token not useful
        while(strtk.next_token(symbol, sym_seps.c_str())) {
            if(symbol.size() <= 2) {
                continue;
            }
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
            if(current_symbol_set.insert(symbol).second) {
                auto i_e_str = entry_str_pool_.insert(line);
                symb_map_.insert(std::pair<std::string, symbol_entry>(symbol, {*i_e_str.first, finfo}));
            }
        }
        //qDebug() << tkn.c_str();
    }
}

void address_model::drop_symbols()
{
    cur_key_.clear();
    cur_symbols_.clear();
    symb_map_.clear();
    entry_str_pool_.clear();
}

QString address_model::cur_key() const
{
    return cur_key_;
}

void highlight_delegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    if(!index.isValid()) {
        return;
    }
    if(index.column() == 0) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    QString dataHighlight((static_cast<const address_model *>(index.model()))->cur_key()); // The text to highlight.
    const QString value(index.model()->data(index, Qt::DisplayRole).toString());

    QTextDocument doc(value);
    QTextCharFormat selection;
    QTextCursor cur(&doc);

    // We have to iterate through the QTextDocument to find ALL matching places
    while(!(cur = doc.find(dataHighlight, cur.position())).isNull()) {
        if(cur.position() == value.size() ||
                (cur.position() < value.size() && sym_seps.find(value[cur.position()].toLatin1()) != std::string::npos)) {
            selection.setBackground(Qt::green);
        } else {
            selection.setBackground(Qt::yellow);
        }
        cur.setCharFormat(selection);
    }

    painter->save();
    painter->translate(option.rect.x(), option.rect.y());
    doc.drawContents(painter);
    painter->restore();
}

}
