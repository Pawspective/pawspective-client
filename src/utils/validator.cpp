#include "validator.hpp"
#include <QRegularExpression>
#include <QString>
#include <string>

namespace pawspective::utils::validation {
bool validate_email(const std::string& email) {
    QString q_email = QString::fromStdString(email);
    static const QRegularExpression
        email_regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = email_regex.match(q_email);
    return match.hasMatch();
}

bool validate_password_strength(const std::string& password) {
    if (password.length() < 8) {
        return false;
    }

    QString qPass = QString::fromStdString(password);

    static const QRegularExpression strength_regex(R"((?=.*[A-Z])(?=.*[a-z])(?=.*\d)(?=.*[!@#$%^&*;,.?"'()\-_=+]))");

    return strength_regex.match(qPass).hasMatch();
}

bool validate_not_empty(const std::string& value) { return !value.empty(); }
}  // namespace pawspective::utils::validation