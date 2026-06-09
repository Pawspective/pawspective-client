#pragma once

#include <QStringList>
#include <QVariantList>
#include <optional>

#include "base.hpp"
#include "models/post_dto.hpp"
#include "models/post_update_dto.hpp"
#include "services/post_service.hpp"

namespace pawspective::viewmodels {

class UpdatePostViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool isDirty READ isDirty NOTIFY dirtyChanged)
    Q_PROPERTY(QStringList photos READ photos NOTIFY photosChanged)

public:
    explicit UpdatePostViewModel(services::PostService& postService, QObject* parent = nullptr);

    QString text() const { return m_changes.text.value_or(m_originalData.text); }
    bool isDirty() const { return m_isDirty; }
    QStringList photos() const { return m_changes.photos.value_or(m_originalData.photos); }

    void setText(const QString& value);

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;
    Q_INVOKABLE void saveChanges();
    Q_INVOKABLE void discardChanges();
    Q_INVOKABLE void setPostData(
        qint64 postId,
        const QString& text,
        const QDateTime& createdAt,
        const QStringList& photos
    );
    Q_INVOKABLE void addPhoto(const QString& fileName);
    Q_INVOKABLE void removePhoto(const QString& fileName);

signals:
    void textChanged();
    void dirtyChanged();
    void photosChanged();

    void loadCompleted();
    void loadFailed(const QString& errorMessage);
    void saveCompleted();
    void saveFailed(const QString& errorMessage);

private slots:
    void handleUpdateSuccess(const models::PostDTO& post);
    void handleUpdateFailed(QSharedPointer<services::BaseError> error);

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    void setupConnections();
    void updateDirtyStatus();
    void setDirty(bool dirty);

    services::PostService& m_postService;
    qint64 m_postId = 0;

    models::PostDTO m_originalData;
    models::PostUpdateDTO m_changes;
    bool m_isDirty = false;
};

}  // namespace pawspective::viewmodels