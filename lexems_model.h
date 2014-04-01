#pragma once

#include <QAbstractTableModel>

#include "grammar.h"
#include "lexeme.h"

class QModelIndex;
class QVariant;

namespace sap
{
class LexemsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LexemsModel( QObject *parent = nullptr);
    virtual ~LexemsModel();
    void setLexems( Lexems lexems );

    virtual int rowCount( const QModelIndex& parent = QModelIndex()) const override;
    virtual int columnCount( const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
private:
    Lexems m_lexems;
}; // class LexemsModel
} // namespace sap

