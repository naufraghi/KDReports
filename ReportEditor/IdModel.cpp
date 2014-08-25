#include "IdModel.h"
#include "structure/ReportData.h"

IdModel::IdModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_reportData(0)
{
}

int IdModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid() && m_reportData) {
        return m_idList.size();
    }
    return 0;
}

int IdModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid() && m_reportData) {
        return 2;
    }
    return 0;
}

QVariant IdModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
          return QVariant();
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        QString id(m_idList.at(index.row()));
        if (index.column() == 0)
            return id;
        if (index.column() == 1)
            return valueFromId(id);

    }
    return QVariant();
}

QVariant IdModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole ){
        switch(section){
        case 0: // First column, id
            return tr("Id");
        case 1: // Second column, value
            return tr("Value");
        }

    }
    return QVariant();
}

Qt::ItemFlags IdModel::flags(const QModelIndex &index) const
{
    if (index.column() == 1)
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    return QAbstractTableModel::flags(index);
}

bool IdModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        if (index.column() == 1) {
            QString id(m_idList.at(index.row()));
            insertIdValue(id, value.toString());
            return true;
        }
    }
    return false;
}

void IdModel::slotAddId(const QString &id)
{
    insertIdValue(id, QString());
}

void IdModel::slotRemoveId(const QString &id)
{
    removeFromId(id);
}

void IdModel::slotModifyId(const QString &newId, const QString &oldId)
{
    changeId(newId, oldId);
}

QString IdModel::valueFromId(const QString &id) const
{
    int p = m_idList.indexOf(id);
    return m_valueList.at(p);
}

void IdModel::insertIdValue(const QString &id, const QString &value)
{
    int p = m_idList.indexOf(id);
    if (p != -1) {
        QModelIndex index = createIndex(p, 1);
        m_valueList.removeAt(p);
        m_valueList.insert(p, value);
        emit dataChanged(index, index);
    } else {
        beginInsertRows(QModelIndex(), m_idList.size(), m_idList.size());
        m_idList.append(id);
        m_valueList.append(value);
        endInsertRows();
    }
}

void IdModel::removeFromId(const QString &id)
{
    int p = m_idList.indexOf(id);
    if ( p!=-1 ) {
        beginRemoveRows(QModelIndex(), p, p);
        m_idList.removeAt(p);
        m_valueList.removeAt(p);
        endRemoveRows();
    }
}

void IdModel::changeId(const QString &newId, const QString &oldId)
{
    int row = m_idList.indexOf(oldId);
    QModelIndex index = createIndex(row, 0);
    int p = m_idList.indexOf(oldId);
    if (p!= -1) {
        if (m_idList.contains(newId)) {
            m_idList.removeAt(p);
            m_valueList.removeAt(p);
        } else {
            m_idList.removeAt(p);
            m_idList.insert(p, newId);
        }
    }
    emit dataChanged(index, index);
}

void IdModel::setReportData(ReportData *reportData)
{
    beginResetModel();
    m_reportData = reportData;
    m_idList.clear();
    m_valueList.clear();
    foreach (QString id, m_reportData->idMap().uniqueKeys())
        insertIdValue(id, QString());
    connect (m_reportData, SIGNAL(idAdded(QString)), SLOT(slotAddId(QString)));
    connect (m_reportData, SIGNAL(idRemoved(QString)), SLOT(slotRemoveId(QString)));
    connect (m_reportData, SIGNAL(idModified(QString,QString)), SLOT(slotModifyId(QString,QString)));
    endResetModel();
}
