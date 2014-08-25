#ifndef HEADEREDITORWIDGET_H
#define HEADEREDITORWIDGET_H

#include <QWidget>
#include "EditorData.h"
#ifdef HAVE_KDTOOLS
#include <KDToolsGui/KDPropertyView>
#include <KDToolsGui/KDPropertyModel>
#include <KDToolsGui/KDMultipleChoiceProperty>
#endif

class ReportTextEdit;
class QTabBar;

class HeaderEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HeaderEditorWidget(EditorData* editorData, EditorData::Scope scope, QWidget *parent = 0);
    void updateDocument();
    void removeAllTabs();

public slots:
    void updateTabs();
    void addTab(HeaderData *header);
    void changeLocation();

protected slots:
    void slotTabChanged(int index);
    void slotAdd();
    void slotRemove();

private:
    QString labelFromLocations(HeaderData::Locations loc) const;
    HeaderData::Locations createLocationSelectionWidget(HeaderData::Locations usedLoc);
    int findIndex(HeaderData::Locations loc) const;

    EditorData* m_editorData;
    EditorData::Scope m_scope;
    QMap<int, HeaderData*> m_tabsMap;
    QTabBar* m_tabBar;
    ReportTextEdit* m_reportTextEdit;
#ifdef HAVE_KDTOOLS
    KDPropertyView* m_property;
    KDPropertyModel* m_propertyModel;
#endif
};

#endif // HEADEREDITORWIDGET_H
