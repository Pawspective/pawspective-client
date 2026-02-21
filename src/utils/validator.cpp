#include "validator.hpp"
#include <QRegularExpression>
#include <QString>
#include <string>

namespace pawspective::utils::validation {
bool validateEmail(const std::string& email) {
    QString qEmail = QString::fromStdString(email);
    static const QRegularExpression
        EmailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = EmailRegex.match(qEmail);
    return match.hasMatch();
}

bool validatePasswordStrength(const std::string& password) {
    if (password.length() < 8) {
        return false;
    }

    QString qPass = QString::fromStdString(password);

    static const QRegularExpression StrengthRegex(R"((?=.*[A-Z])(?=.*[a-z])(?=.*\d)(?=.*[!@#$%^&*;,.?"'()\-_=+]))");

    return StrengthRegex.match(qPass).hasMatch();
}

bool minLength(const std::string& value, size_t length) { return value.length() >= length; }

bool isOneOf(const std::string& value, const std::vector<std::string>& options) {
    return std::find(options.begin(), options.end(), value) != options.end();
}

bool validateNotEmpty(const std::string& value) { return !value.empty(); }
}  // namespace pawspective::utils::validation