#include "HeaderEditorWidget.h"
#include "structure/ReportData.h"
#include "ReportTextEdit.h"
#include <QTabBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>

HeaderEditorWidget::HeaderEditorWidget(EditorData *editorData, EditorData::Scope scope, QWidget *parent)
    : QWidget(parent)
    , m_editorData(editorData)
    , m_scope(scope)
    , m_tabBar(new QTabBar(this))
    , m_reportTextEdit(new ReportTextEdit(editorData, this))
#ifdef HAVE_KDTOOLS
    , m_property(new KDPropertyView)
    , m_propertyModel(0)
#endif
{
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);
    QPushButton* addButton;
    QPushButton* removeButton;
    if (m_scope == EditorData::Header) {
        addButton = new QPushButton(QIcon(":/add"), tr("Add header"));
        removeButton = new QPushButton(QIcon(":/remove"), tr("Delete header"));
    } else {
        addButton = new QPushButton(QIcon(":/add"), tr("Add footer"));
        removeButton = new QPushButton(QIcon(":/remove"), tr("Delete footer"));
    }
    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(addButton);
    layout2->addWidget(removeButton);
    layout2->addStretch();
    layout->addLayout(layout2);
    layout->addWidget(m_tabBar);
    layout->addWidget(m_reportTextEdit);
    connect(m_editorData, SIGNAL(headerChanged()), SLOT(updateTabs()));
    connect(m_editorData, SIGNAL(headerAdded(HeaderData*)), SLOT(addTab(HeaderData*)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));
    connect(addButton, SIGNAL(clicked()), SLOT(slotAdd()));
    connect(removeButton, SIGNAL(clicked()), SLOT(slotRemove()));
}

void HeaderEditorWidget::updateTabs()
{
    disconnect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));
    removeAllTabs();
    if (m_editorData->reportData()) {
        if (m_scope == EditorData::Header) {
            foreach (HeaderData::Locations loc, m_editorData->reportData()->headerMap().keys()) {
                int id = m_tabBar->addTab(labelFromLocations(loc));
                HeaderData* header = m_editorData->reportData()->headerMap().value(loc);
                m_tabsMap.insert(id, header);
                connect(header, SIGNAL(locationChanged()), SLOT(changeLocation()));
            }
        } else if (m_scope == EditorData::Footer) {
            foreach (HeaderData::Locations loc, m_editorData->reportData()->footerMap().keys()) {
                int id = m_tabBar->addTab(labelFromLocations(loc));
                FooterData* footer = m_editorData->reportData()->footerMap().value(loc);
                m_tabsMap.insert(id, footer);
                connect(footer, SIGNAL(locationChanged()), SLOT(changeLocation()));
            }
        }
    }
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));
}

void HeaderEditorWidget::changeLocation()
{
    if (HeaderData* send = qobject_cast<HeaderData* >(sender())) {
        m_editorData->reportData()->updateHeaderLocation(send);
        const int idx = findIndex(send->location());
        m_tabBar->setTabText(idx, labelFromLocations(send->location()));
        slotTabChanged(idx);
    }
}

void HeaderEditorWidget::addTab(HeaderData* header)
{
    if (m_editorData->reportData()) {
        if (m_scope == EditorData::Footer && qobject_cast<FooterData*>(header)) {
            int id = m_tabBar->addTab(labelFromLocations(header->location()));
            m_tabsMap.insert(id, header);
            connect(header, SIGNAL(locationChanged()), SLOT(changeLocation()));
        } else if (m_scope == EditorData::Header) {
            int id = m_tabBar->addTab(labelFromLocations(header->location()));
            m_tabsMap.insert(id, header);
            connect(header, SIGNAL(locationChanged()), SLOT(changeLocation()));
        }
    }
}

void HeaderEditorWidget::removeAllTabs()
{
    while (m_tabBar->count() > 0) {
        m_tabBar->removeTab(0);
    }
    qDeleteAll(m_tabsMap);
    m_tabsMap.clear();
}

void HeaderEditorWidget::slotTabChanged(int index)
{
    if (index == -1 || m_tabsMap.isEmpty())
        return;
    m_editorData->setLocations(m_tabsMap.value(index)->location());
    m_editorData->updateModel();
    m_reportTextEdit->setDocument(&m_editorData->document());
}

void HeaderEditorWidget::slotAdd()
{
    if (!m_editorData->reportData())
        return;

    QList<HeaderData::Locations> list;
    if (m_scope == EditorData::Header) {
        list = m_editorData->reportData()->headerMap().keys();
    } else {
        list = m_editorData->reportData()->footerMap().keys();
    }

    HeaderData::Locations usedLocation = 0;
    foreach(HeaderData::Locations loc, list) {
        if (loc.testFlag(HeaderData::FirstPage)) {
            usedLocation |= HeaderData::FirstPage;
        }
        if (loc.testFlag(HeaderData::EvenPages)) {
            usedLocation |= HeaderData::EvenPages;
        }
        if (loc.testFlag(HeaderData::OddPages)) {
            usedLocation |= HeaderData::OddPages;
        }
        if (loc.testFlag(HeaderData::LastPage)) {
            usedLocation |= HeaderData::LastPage;
        }
    }

    if (HeaderData::Locations loc = createLocationSelectionWidget(usedLocation)) {
        if (m_scope == EditorData::Header) {
            m_editorData->insertHeader(loc);
        } else {
            m_editorData->insertFooter(loc);
        }
        const int idx = findIndex(loc);
        const int currentIdx = m_tabBar->currentIndex();
        m_tabBar->setCurrentIndex(idx);
        if (idx == currentIdx)
            slotTabChanged(idx);
    }
}

int HeaderEditorWidget::findIndex(HeaderData::Locations loc) const
{
    HeaderData* header;
    if (m_scope == EditorData::Header)
        header = m_editorData->reportData()->headerMap().value(loc);
    else
        header = m_editorData->reportData()->footerMap().value(loc);
    return m_tabsMap.key(header);
}

HeaderData::Locations HeaderEditorWidget::createLocationSelectionWidget(HeaderData::Locations usedLoc)
{
#ifdef HAVE_KDTOOLS
    QDialog *dialog = new QDialog;
    if (m_propertyModel)
        m_property->deleteLater();
    m_propertyModel = new KDPropertyModel(this);
    m_property->setModel(m_propertyModel);

    QStringList list;
    int first, even, odd, last;
    int i = 0;
    if (!usedLoc.testFlag(HeaderData::FirstPage)) {
        list.append(tr("First page"));
        first = i++;
    }
    if (!usedLoc.testFlag(HeaderData::EvenPages)) {
        list.append(tr("Even pages"));
        even = i++;
    }
    if (!usedLoc.testFlag(HeaderData::OddPages)) {
        list.append(tr("Odd pages"));
        odd = i++;
    }
    if (!usedLoc.testFlag(HeaderData::LastPage)) {
        list.append(tr("Last page"));
        last = i;
    }

    QList<QVariant> indexList;
    m_propertyModel->addProperty(new KDMultipleChoiceProperty(list, indexList, tr("pages")));
    dialog->setModal(true);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_property);
    dialog->setLayout(layout);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    if (dialog->exec()) {
        QVariant value = m_propertyModel->propertyAt(0)->value();
        QList<QVariant> values = qVariantValue<QList<QVariant> >(value);
        int flag = 0;
        foreach(const QVariant &v, values){
            if (v.toInt() == first)
                flag |= HeaderData::FirstPage;
            if (v.toInt() == even)
                flag |= HeaderData::EvenPages;
            if (v.toInt() == odd)
                flag |= HeaderData::OddPages;
            if (v.toInt() == last)
                flag |= HeaderData::LastPage;
        }
        return HeaderData::Locations(flag);
    }
#else
    Q_UNUSED(usedLoc);
#endif
    return 0;
}

void HeaderEditorWidget::slotRemove()
{
    HeaderData* header = m_tabsMap.value(m_tabBar->currentIndex(), 0);
    if (header)
        m_editorData->removeObject(header);
}

void HeaderEditorWidget::updateDocument()
{
    slotTabChanged(m_tabBar->currentIndex());
}

QString HeaderEditorWidget::labelFromLocations(HeaderData::Locations loc) const
{
    QString label;
    if (loc.testFlag(HeaderData::FirstPage)) {
        label.append(tr("First"));
    }
    if (loc.testFlag(HeaderData::EvenPages)) {
        if (!label.isEmpty())
            label.append(" - ");
        label.append(tr("Even"));
    }
    if (loc.testFlag(HeaderData::OddPages)) {
        if (!label.isEmpty())
            label.append(" - ");
        label.append(tr("Odd"));
    }
    if (loc.testFlag(HeaderData::LastPage)) {
        if (!label.isEmpty())
            label.append(" - ");
        label.append(tr("Last"));
    }

    return label;
}
