#pragma once

#include <QStringList>
#include <QVariantList>
#include <functional>

#include "base.hpp"
#include "models/post_create_dto.hpp"
#include "services/post_service.hpp"

namespace pawspective::viewmodels {

class CreatePostViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY textChanged)
    Q_PROPERTY(QStringList photos READ photos NOTIFY photosChanged)

public:
    explicit CreatePostViewModel(services::PostService& postService, QObject* parent = nullptr);

    const QString& text() const { return m_createDto.text; }
    bool isValid() const { return !m_createDto.text.trimmed().isEmpty(); }
    const QStringList& photos() const { return m_createDto.photos; }

    void setText(const QString& value) {
        updateProperty(m_createDto.text, value, [this] { emit textChanged(); });
    }

    Q_INVOKABLE void createPost();
    Q_INVOKABLE void setOrganizationId(qint64 organizationId) { m_organizationId = organizationId; }
    Q_INVOKABLE void addPhoto(const QString& fileName);
    Q_INVOKABLE void removePhoto(const QString& fileName);

    void initialize() override;
    void cleanup() override;

signals:
    void textChanged();
    void photosChanged();

    void creationFinished(bool success);

private:
    void setupConnections();
    void onPostCreated();
    void onError(QSharedPointer<services::BaseError> error);
    bool validateRequiredFields();

    services::PostService& m_postService;
    qint64 m_organizationId = 0;

    models::PostCreateDTO m_createDto;
};

}  // namespace pawspective::viewmodels