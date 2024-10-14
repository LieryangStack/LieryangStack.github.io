#include "model.h"

Animal::Animal(const QString &type, const QString &size)
    : m_type(type), m_size(size)
{
}

QString Animal::type() const
{
    return m_type;
}

QString Animal::size() const
{
    return m_size;
}


AnimalModel::AnimalModel(QObject *parent)
    : QAbstractListModel(parent)
{
}


/*
* 自定义函数，实现动态添加item，动态更新ListView
*/
void AnimalModel::addAnimal(const Animal &animal)
{
    /* 必须先调用插入行函数 */
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_animals << animal;
    /* 结束插入行函数 */
    endInsertRows();
}

/*
* 这是QAbstractListModel的虚函数实现：返回当前模型对象有多少个行row（项item）
*/
int AnimalModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_animals.count();
}

/*
* 这是QAbstractListModel的虚函数实现：根据索引@index和@role，返回存储的数据
*/
QVariant AnimalModel::data(const QModelIndex & index, int role) const {
    /* 如果索引超出rowCount，则返回一个空的 QVariant 对象 */
    if (index.row() < 0 || index.row() >= m_animals.count())
        return QVariant();

    const Animal &animal = m_animals[index.row()];
    if (role == TypeRole)
        return animal.type(); /* 返回字符串动物的类型 */
    else if (role == SizeRole)
        return animal.size(); /* 返回字符串动物的尺寸 */

    /* 如果查询的角色错误，则返回一个空的 QVariant 对象 */
    return QVariant();
}


/*
* 这是QAbstractListModel的虚函数实现：把自定义的角色名称和对应索引值，创建一个哈希表返回
*/
QHash<int, QByteArray> AnimalModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "type";
    roles[SizeRole] = "size";
    return roles;
}

