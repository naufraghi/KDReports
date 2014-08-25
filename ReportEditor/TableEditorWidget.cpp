#include "TableEditorWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include "structure/TableElementData.h"
#include "structure/CellData.h"
#include "EditorData.h"

#include <QDebug>

TableEditorWidget::TableEditorWidget(EditorData *editorData, QWidget *parent)
    :  QWidget(parent)
    , ui(new Ui::TableEditor)
    , m_editorData(editorData)
{
    ui->setupUi(this);
    connect(ui->buttonColumnAddAfter, SIGNAL(clicked()), SLOT(addColumnAfter()));
    connect(ui->buttonColumnAddBefore, SIGNAL(clicked()), SLOT(addColumnBefore()));
    connect(ui->buttonRowAddAfter, SIGNAL(clicked()), SLOT(addRowAfter()));
    connect(ui->buttonRowAddBefore, SIGNAL(clicked()), SLOT(addRowBefore()));
    connect(ui->buttonColumnDelete, SIGNAL(clicked()), SLOT(removeColumn()));
    connect(ui->buttonRowDelete, SIGNAL(clicked()), SLOT(removeRow()));

    // set button state
    changeCurrentElement();
}

TableEditorWidget::~TableEditorWidget()
{
    delete ui;
}

void TableEditorWidget::changeCurrentElement()
{
    // check if we are in a table
    ObjectData *object = m_editorData->selectedObject();
    m_cell = 0;
    m_table = 0;
    bool onTable = false;
    while(object && object->parentElement()) {
        if (qobject_cast<CellData*>(object))
            m_cell = qobject_cast<CellData*>(object);
        if (qobject_cast<TableElementData*>(object))
            m_table = qobject_cast<TableElementData*>(object);
        // Stop while if we have the cell and the table
        if (m_table && m_cell) {
            onTable = true;
            break;
        }
        object = object->parentElement();
    }

    ui->buttonColumnDelete->setEnabled(onTable && m_table->columns() > 1);
    ui->buttonRowDelete->setEnabled(onTable && m_table->rows() > 1);
    ui->buttonColumnAddAfter->setEnabled(onTable);
    ui->buttonColumnAddBefore->setEnabled(onTable);
    ui->buttonRowAddAfter->setEnabled(onTable);
    ui->buttonRowAddBefore->setEnabled(onTable);
}

void TableEditorWidget::addRowAfter()
{
    if (m_table && m_cell) {
        m_editorData->insertRow(m_table, m_cell->row() + 1);
    }
}

void TableEditorWidget::addRowBefore()
{
    if (m_table && m_cell) {
        m_editorData->insertRow(m_table, m_cell->row());
    }
}

void TableEditorWidget::addColumnAfter()
{
    if (m_table && m_cell) {
        m_editorData->insertColumn(m_table, m_cell->column() + 1);
    }
}

void TableEditorWidget::addColumnBefore()
{
    if (m_table && m_cell) {
        m_editorData->insertColumn(m_table, m_cell->column());
    }
}

void TableEditorWidget::removeRow()
{
    // check if we have at least 2 rows
    if (m_table && m_cell && m_table->rows() > 1) {
        m_editorData->removeRow(m_table, m_cell->row());
    }
}

void TableEditorWidget::removeColumn()
{
    // check if we have at least 2 columns
    if (m_table && m_cell && m_table->columns() > 1) {
        m_editorData->removeColumn(m_table, m_cell->column());
    }
}
