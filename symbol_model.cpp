#include "symbol_model.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include "str_tok.h"

namespace model{

address_model::address_model(QWidget *parent) : QAbstractTableModel(parent)
{
    load_debug_model();
}

void address_model::offerKey(const QString &key)
{
    cur_key_ = key;
    if(!cur_ae_.empty()){
        removeRows(0, (int)cur_ae_.size(), QModelIndex());
    }
    cur_ae_.clear();
    if(!key.size()){
        return;
    }
    auto kstr = key.toStdString();
    std::transform(kstr.begin(), kstr.end(), kstr.begin(), ::tolower);
    for(auto it = ae_map_.lower_bound(kstr); it != ae_map_.end(); it++){
        if(it->first.find(kstr) == 0){
            cur_ae_.push_back(&it->second);
        }else{
            break;
        }
    }
    if(!cur_ae_.empty()){
        insertRows(0, cur_ae_.size(), QModelIndex());
    }
}

int model::address_model::rowCount(const QModelIndex &parent) const
{
    return (int)cur_ae_.size();
}

int model::address_model::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant model::address_model::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(index.row() >= (int)cur_ae_.size() || index.row() < 0) {
        return QVariant();
    }
    if(index.column() != 0) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QString(cur_ae_.at(index.row())->entry_str_.c_str());
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
    cur_ae_.clear();
    endRemoveRows();
    return true;
}

void model::address_model::load_debug_model()
{
    std::ifstream ifs("debug_list.txt");
    std::string line;
    while(std::getline(ifs, line)){
        std::istringstream iss(line);
        std::string token;
        std::set<std::string> current_token_set;
        utl::str_tok strtk(line);
        while(strtk.next_token(token)){
            std::transform(token.begin(), token.end(), token.begin(), ::tolower);
            if(current_token_set.insert(token).second){
                ae_map_.insert(std::pair<std::string, address_entry>(token, {line}));
            }
        }
    }
}

void address_model::load_model()
{
    QStringList arguments;
}

QString address_model::cur_key() const
{
    return cur_key_;
}

void highlight_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString dataHighlight(((address_model*)index.model())->cur_key()); // The text to highlight.
    QString value = index.model()->data(index, Qt::DisplayRole).toString();

    QTextDocument doc(value);
    QTextCharFormat selection;
    QTextCursor cur(&doc);

    // We have to iterate through the QTextDocument to find ALL matching places
    while (!(cur = doc.find(dataHighlight,cur.position())).isNull()) {
        if(cur.position() < value.size() && value[cur.position()] == ' '){
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
