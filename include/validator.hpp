#pragma once

#include <qregularexpression.h>
#include <string>
#include <vector>
#include "services/errors.hpp"

namespace pawspective::utils {
class Validator {
public:
    explicit Validator() = default;

    Validator& field(std::string name, std::string value);

    Validator& notBlank();

    Validator& minLength(std::size_t min);

    Validator& maxLength(std::size_t max);

    Validator& validateEmail();

    Validator& validatePasswordStrength();

    Validator& matches(const QRegularExpression& re, const std::string& msg);

    Validator& isOneOf(const std::vector<std::string>& allowed);

    std::optional<services::ValidationError> getValidationError() const;

private:
    void addError(std::string msg);

    std::string m_current_field;
    std::string m_current_value;
    std::vector<services::ValidationError::FieldError> m_errors = {};
};

}  // namespace pawspective::utils
