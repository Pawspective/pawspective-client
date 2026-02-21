#pragma once

#include <string>
#include <vector>

namespace pawspective::utils::validation {
bool validateEmail(const std::string& email);
bool validatePasswordStrength(const std::string& password);
bool validateNotEmpty(const std::string& value);
bool minLength(const std::string& value, size_t length);
bool isOneOf(const std::string& value, const std::vector<std::string>& options);
}  // namespace pawspective::utils::validation