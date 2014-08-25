#ifndef TABLEEDITORWIDGET_H
#define TABLEEDITORWIDGET_H

#include <QWidget>
#include "ui_TableEditor.h"

class TableElementData;
class CellData;
class ObjectData;
class EditorData;
class QPushButton;

class TableEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TableEditorWidget(EditorData* editorData, QWidget *parent = 0);
    ~TableEditorWidget();
public slots:
    void changeCurrentElement();
private slots:
    void removeColumn();
    void removeRow();
    void addRowAfter();
    void addRowBefore();
    void addColumnAfter();
    void addColumnBefore();
private:
    Ui::TableEditor *ui;
    EditorData* m_editorData;
    TableElementData *m_table;
    CellData *m_cell;
};

#endif // TABLEEDITORWIDGET_H
