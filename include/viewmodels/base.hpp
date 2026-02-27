#pragma once

#include <QObject>
#include <QString>
#include <functional>

namespace pawspective::viewmodels {
/**
 * @brief Base class for all ViewModels in the application
 *
 * BaseViewModel provides common functionality for working with the UI layer,
 * implementing the MVVM pattern. It manages loading state, error handling, and UI notifications.
 *
 * @see ErrorType for error types
 */
class BaseViewModel : public QObject {
    Q_OBJECT

    /**
     * @brief Property indicating whether data is being loaded
     *
     * Used to display loading indicator in UI.
     */
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:
    /**
     * @brief Type of error that can occur in ViewModel
     *
     * Used for categorizing and handling different types of errors.
     */
    enum ErrorType : uint8_t {
        /** @brief No error occurred, operation completed successfully */
        None,
        
        /** @brief Network connection error (timeout, no connection, server unreachable) */
        NetworkError,
        
        /** @brief Authentication error (invalid credentials, expired token, unauthorized access) */
        AuthenticationError,
        
        /** @brief Input data validation error (invalid format, missing required fields, constraints violation) */
        ValidationError,
        
        /** @brief Unknown or unexpected error that doesn't fit other categories */
        UnknownError
    };

    Q_ENUM(ErrorType);

    /**
     * @brief BaseViewModel constructor
     * @param parent Parent QObject (optional)
     */
    explicit BaseViewModel(QObject* parent = nullptr);

    ~BaseViewModel() override = default;

    /**
     * @brief Get loading state
     * @return true if data is being loaded, false otherwise
     */
    bool isBusy() const;

    /**
     * @brief Initialize the ViewModel
     *
     * Called when creating the view/screen.
     * Must be implemented in derived classes.
     * Used for loading data, establishing connections, and preparation.
     *
     * @note This is an invokable method and can be called from QML
     */
    Q_INVOKABLE virtual void initialize() = 0;

    /**
     * @brief Clean up ViewModel resources
     *
     * Called when destroying the view/screen.
     * Must be implemented in derived classes.
     * Used for disconnecting signals, clearing cache, and releasing resources.
     *
     * @note This is an invokable method and can be called from QML
     */
    Q_INVOKABLE virtual void cleanup() = 0;

protected:
    bool m_isBusy = false;  // NOLINT misc-non-private-member-variables-in-classes

    /**
     * @brief Method for safely updating properties
     *
     * Updates property only if the new value differs from current.
     * Automatically emits notification signal on change.
     *
     * @tparam T Property type
     * @param property Reference to property variable
     * @param value New property value
     * @param signal Lambda function to emit signal: `[this]() { emit signalName(); }`
     * @return true if property was changed, false if value remained the same
     *
     */
    template <typename T>
    bool updateProperty(T& property, const T& value, const std::function<void()> signal) {
        if (property == value) {
            return false;
        }
        property = value;
        signal();
        return true;
    }

    /**
     * @brief Helper method for emitting error signal
     *
     * Convenient way to send error to UI without using emit directly.
     *
     * @param type Error type (see ErrorType)
     * @param message Textual error description
     *
     * @see errorOccurred
     */
    void emitError(ErrorType type, const QString& message);

public slots:
    /**
     * @brief Set loading state
     *
     * Sets isBusy flag and generates change signal if necessary.
     * Can be called from QML.
     *
     * @param value true to start loading, false to finish
     */
    void setIsBusy(bool value);

signals:
    /**
     * @brief Signal for loading state change
     *
     * Emitted once when isBusy changes.
     * Used for UI updates (show/hide loading spinner).
     */
    void isBusyChanged();

    /**
     * @brief Signal on error occurrence
     *
     * Emitted when an error occurs in ViewModel.
     * UI layer should connect to this signal to handle error messages.
     *
     * @param type Error category
     * @param message Textual error description
     */
    void errorOccurred(ErrorType type, const QString& message);
};
}  // namespace pawspective::viewmodels
