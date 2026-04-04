#include "validator.hpp"
#include <QRegularExpression>
#include <QString>
#include <algorithm>
#include <string>

namespace pawspective::utils {

Validator& Validator::field(std::string name, std::string value) {
    m_current_field = std::move(name);
    m_current_value = std::move(value);
    return *this;
}

Validator& Validator::notBlank() {
    if (m_current_value.empty() ||
        std::ranges::all_of(m_current_value, [](unsigned char c) { return std::isspace(c); }))
    {
        addError("must not be empty or whitespace-only");
    }
    return *this;
}

Validator& Validator::minLength(std::size_t min) {
    if (m_current_value.length() < min) {
        addError("must be at least " + std::to_string(min) + " characters long");
    }
    return *this;
}

Validator& Validator::maxLength(std::size_t max) {
    if (m_current_value.length() > max) {
        addError("must be at most " + std::to_string(max) + " characters long");
    }
    return *this;
}

Validator& Validator::matches(const QRegularExpression& re, const std::string& msg) {
    if (re.match(QString::fromStdString(m_current_value)).hasMatch()) {
        addError(std::move(msg));
    }
    return *this;
}

Validator& Validator::validateEmail() {
    QString qEmail = QString::fromStdString(m_current_value);
    static const QRegularExpression
        EmailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)", QRegularExpression::CaseInsensitiveOption);
    if (!EmailRegex.match(qEmail).hasMatch()) {
        addError("must be a valid email address");
    }
    return *this;
}

Validator& Validator::validatePasswordStrength() {
    if (m_current_value.length() < 8) {
        addError("Password must be at least 8 characters long");
        return *this;
    }

    QString qPass = QString::fromStdString(m_current_value);

    static const QRegularExpression StrengthRegex(R"((?=.*[A-Z])(?=.*[a-z])(?=.*\d)(?=.*[!@#$%^&*;,.?"'()\-_=+]))");

    if (!StrengthRegex.match(qPass).hasMatch()) {
        addError(
            "Password must contain at least one uppercase letter, one lowercase letter, one digit, and one special "
            "character"
        );
    }
    return *this;
}

Validator& Validator::isOneOf(const std::vector<std::string>& allowed) {
    if (std::ranges::find(allowed, m_current_value) == allowed.end()) {
        std::ostringstream oss;
        oss << "must be one of: [";
        for (size_t i = 0; i < allowed.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            oss << allowed[i];
        }
        oss << "]";

        addError("must be one of: " + oss.str() + " allowed values");
    }
    return *this;
}

std::optional<services::ValidationError> Validator::getValidationError() const {
    if (m_errors.empty()) {
        return std::nullopt;
    }
    return services::ValidationError{m_errors};
}

Validator& Validator::inRange(int min, int max) {
    std::string fieldName = m_current_field;
    if (!fieldName.empty()) {
        fieldName[0] = std::toupper(fieldName[0]);
    }

    try {
        int val = std::stoi(m_current_value);
        if (val < min || val > max) {
            addError(fieldName + " must be between " + std::to_string(min) + " and " + std::to_string(max));
        }
    } catch (...) {
        addError(fieldName + " is required and must be between " + std::to_string(min) + " and " + std::to_string(max));
    }
    return *this;
}

void Validator::addError(std::string msg) { m_errors.push_back({m_current_field, std::move(msg)}); }

}  // namespace pawspective::utils
