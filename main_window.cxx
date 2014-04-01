#include "main_window.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

#include <fstream>

#include <QDebug>
namespace sap
{
MainWindow::MainWindow( QWidget *parent )
    : QWidget{  parent }
    , m_table{  new QTableView{ this } }
    , m_model{  new LexemsModel{ this } }
    , m_text{   new QTextEdit{ this } }
{
    m_table->setModel( m_model.get() );

    std::unique_ptr< QVBoxLayout > main_layout{ new QVBoxLayout{ this } };
    main_layout->addWidget( m_table.get() );
    main_layout->addWidget( m_text.get() );

    main_layout.release();

    openFile();
}

MainWindow::~MainWindow()
{
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr("Open file"),
            "", tr("Basic sources (*.ba)"));

    if (!fileName.isNull()) {
        std::ifstream in{ fileName.toStdString() };
        in.unsetf( std::ios::skipws );
        const std::string str{ std::istreambuf_iterator< char >{ in }
                             , std::istreambuf_iterator< char >{} };

        m_text->setText( QString::fromStdString( str ));
        try {
            Lexems lexems = sap::parse( str );
            m_model->setLexems( lexems );
        } catch ( std::exception& ex ) {
            qDebug() << ex.what();
        }

    }
}
} // namespace sap
