#pragma once

#include <QWidget>
#include <QTableView>

#include <memory>

#include "lexems_model.h"

namespace sap
{
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow( QWidget *parent = nullptr );
    virtual ~MainWindow();
private:
    std::unique_ptr< QTableView >   m_table;
    std::unique_ptr< LexemsModel >  m_model;

}; // class MainWindow
} // namespace sap
