#pragma once

#include <QWidget>
#include <QTableView>
#include <QTextEdit>

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
    std::unique_ptr< QTableView  >  m_table;
    std::unique_ptr< LexemsModel >  m_model;
    std::unique_ptr< QTextEdit   >  m_text;
private slots:
    void openFile();


}; // class MainWindow
} // namespace sap
