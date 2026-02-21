#pragma once

#include <string>
#include <vector>

namespace pawspective::utils::validation {
bool validate_email(const std::string& email);
bool validate_password_strength(const std::string& password);
bool validate_not_empty(const std::string& value);
bool min_length(const std::string& value, size_t length);
bool is_one_of(const std::string& value, const std::vector<std::string>& options);
}  // namespace pawspective::utils::validation