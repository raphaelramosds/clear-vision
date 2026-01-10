#pragma once

#include "cvision.hpp"
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp> // Head-only library for JSON parsing

using json = nlohmann::json;

class LevenshteinSearch
{
private:
    std::vector<cvision::Frame> data_;
    std::vector<std::string> tokenize(const std::string &text);
    float levenshtein_similarity(const std::string &a, const std::string &b);
    float calculate_similarity(const std::string &query, const std::string &class_name);

public:
    LevenshteinSearch(std::vector<cvision::Frame> data) : data_(data) {}
    // TODO constructor overload that reads from JSON file directly in case of large datasets that won't fit in memory
    std::vector<cvision::Frame> search(const std::string &query, float min_similarity = 0.3f);
};