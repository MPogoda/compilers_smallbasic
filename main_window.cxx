#include "main_window.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

#include <fstream>

#include <QDebug>
namespace sap
{
MainWindow::MainWindow( QWidget *parent )
    : QWidget{  parent }
    , m_table{  new QTableView{ this } }
    , m_model{  new LexemsModel{ this } }
    , m_text{   new QTextEdit{ this } }
    , m_open{   new QPushButton{ tr("Open file"), this } }
    , m_exec{   new QPushButton{ tr("Parse" ), this } }
    , m_quit{   new QPushButton{ tr("Quit" ), this } }
{
    m_table->setModel( m_model.get() );

    std::unique_ptr< QHBoxLayout > top_layout{  new QHBoxLayout };
        top_layout->addWidget( m_open.get() );
        top_layout->addWidget( m_exec.get() );
        top_layout->addWidget( m_quit.get() );
    std::unique_ptr< QHBoxLayout > bottom_layout{  new QHBoxLayout };
        bottom_layout->addWidget( m_text.get() );
        bottom_layout->addWidget( m_table.get() );

    std::unique_ptr< QVBoxLayout > main_layout{ new QVBoxLayout{ this } };
        main_layout->addLayout( top_layout.release() );
        main_layout->addLayout( bottom_layout.release() );

    main_layout.release();


    connect( m_quit.get(), SIGNAL(clicked()), this, SLOT(close()));
    connect( m_open.get(), SIGNAL(clicked()), this, SLOT(openFile()));
    connect( m_exec.get(), SIGNAL(clicked()), this, SLOT(parseFile()));
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
    }
}

void MainWindow::parseFile()
{
    const std::string str{ m_text->toPlainText().toStdString() };

    try {
        Lexems lexems = sap::parse( str );
        m_model->setLexems( lexems );
    } catch ( std::exception& ex ) {
        QMessageBox::critical( this, tr("Parse error"), ex.what() );
    }
}
} // namespace sap
