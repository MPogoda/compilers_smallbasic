#include "main_window.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <fstream>

#include <QDebug>
namespace sap
{
MainWindow::MainWindow( QWidget *parent )
    : QWidget{ parent }
    , m_table{ new QTableView{ this } }
    , m_model{ new LexemsModel{ this } }
{
    m_table->setModel( m_model.get() );

    std::unique_ptr< QVBoxLayout > main_layout{ new QVBoxLayout{ this } };
    main_layout->addWidget( m_table.get() );

    main_layout.release();

    std::ifstream in{ "../test.ba" };
    in.unsetf( std::ios::skipws );
    const std::string str{ std::istreambuf_iterator< char >{ in }
                         , std::istreambuf_iterator< char >{} };

    try {
        Lexems lexems = sap::parse( str );
        m_model->setLexems( lexems );
    } catch ( std::exception& ex ) {
        qDebug() << ex.what();
    }
}

MainWindow::~MainWindow()
{
}
} // namespace sap
