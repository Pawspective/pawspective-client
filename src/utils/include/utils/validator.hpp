#pragma once

#include <string>

namespace pawspective::utils {
bool validate_email(const std::string& email);
bool validate_password_strength(const std::string& password);
bool validate_not_empty(const std::string& value);
}  // namespace pawspective::utils