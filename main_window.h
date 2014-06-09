#pragma once

#include <QWidget>
// #include <QTableView>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>

#include <memory>

// #include "lexems_model.h"

namespace sap
{
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow( QWidget *parent = nullptr );
    virtual ~MainWindow();
private:
    // std::unique_ptr< QTableView  >  m_table;
    // std::unique_ptr< LexemsModel >  m_model;
    std::unique_ptr< QTextEdit   >  m_text;
    std::unique_ptr< QPushButton >  m_open;
    std::unique_ptr< QPushButton >  m_exec;
    std::unique_ptr< QPushButton >  m_quit;
    std::unique_ptr< QLabel      >  m_rules;
    std::unique_ptr< QLabel      >  m_code;
private slots:
    void openFile();
    void parseFile();
}; // class MainWindow
} // namespace sap
