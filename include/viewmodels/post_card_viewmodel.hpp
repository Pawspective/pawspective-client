#pragma once

#include <QDateTime>
#include <QObject>
#include <QString>

#include "models/post_dto.hpp"

namespace pawspective::viewmodels {

class PostCardViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(QDateTime createdAt READ createdAt NOTIFY createdAtChanged)
    Q_PROPERTY(bool isExpanded READ isExpanded WRITE setIsExpanded NOTIFY isExpandedChanged)
    Q_PROPERTY(QString shortText READ shortText NOTIFY textChanged)
    Q_PROPERTY(bool hasLongText READ hasLongText NOTIFY textChanged)

public:
    explicit PostCardViewModel(QObject* parent = nullptr);
    explicit PostCardViewModel(const models::PostDTO& dto, QObject* parent = nullptr);

    const QString& text() const { return m_text; }
    QDateTime createdAt() const { return m_createdAt; }
    bool isExpanded() const { return m_isExpanded; }
    QString shortText() const;
    bool hasLongText() const;

    void setIsExpanded(bool expanded);

    Q_INVOKABLE void toggleExpand();
    Q_INVOKABLE void setFromDTO(const models::PostDTO& dto);

signals:
    void textChanged();
    void createdAtChanged();
    void isExpandedChanged();

private:
    QString m_text;
    QDateTime m_createdAt;
    bool m_isExpanded = false;
};

}  // namespace pawspective::viewmodels
