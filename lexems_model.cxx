#include "lexems_model.h"

#include <QModelIndex>
#include <QVariant>

#include <cassert>
#include <sstream>

namespace sap
{
LexemsModel::LexemsModel( QObject* parent )
    : QAbstractTableModel( parent )
{
}

LexemsModel::~LexemsModel()
{
}

int LexemsModel::rowCount( const QModelIndex& ) const
{
    return m_lexems.size();
}

int LexemsModel::columnCount( const QModelIndex& ) const
{
    return 2;
}

QVariant LexemsModel::data( const QModelIndex& index, const int role ) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    const auto item = m_lexems.at( index.row() );

    std::stringstream ss;
    switch (index.column()) {
        case 0:
            ss << item.type_;
            break;
        case 1:
            ss << item.value_;
            break;
    } // switch
    return QString{ ss.str().c_str() };

    assert( !"Wrong column number!" );
    return QVariant();
}

QVariant
LexemsModel::headerData( const int section, Qt::Orientation orientation, const int role ) const
{
    if (Qt::DisplayRole == role ) {
        if (Qt::Horizontal == orientation) {
            switch (section) {
                case 0:
                    return "Type";
                case 1:
                    return "Value";
            }
        } else {
            return section;
        }
    }

    return QVariant();
}

void
LexemsModel::setLexems( Lexems lexems )
{
    m_lexems = std::move( lexems );
    reset();
}
} // namespace sap

