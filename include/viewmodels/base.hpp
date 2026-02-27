#pragma once

#include <QObject>
#include <QString>
#include <functional>

namespace pawspective::viewmodels {
class BaseViewModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:
    enum ErrorType : uint8_t { None, NetworkError, AuthenticationError, ValidationError, UnknownError };

    Q_ENUM(ErrorType);

    explicit BaseViewModel(QObject* parent = nullptr);
    ~BaseViewModel() override = default;

    bool isBusy() const;

    Q_INVOKABLE virtual void initialize() = 0;
    Q_INVOKABLE virtual void cleanup() = 0;

protected:
    bool m_isBusy = false;  // NOLINT misc-non-private-member-variables-in-classes

    template <typename T>
    bool updateProperty(T& property, const T& value, const std::function<void()> signal) {
        if (property == value) {
            return false;
        }
        property = value;
        signal();
        return true;
    }

    void emitError(ErrorType type, const QString& message) { emit errorOccurred(type, message); }

public slots:
    void setIsBusy(bool value);
signals:
    void isBusyChanged();
    void errorOccurred(ErrorType type, const QString& message);
};
}  // namespace pawspective::viewmodels
