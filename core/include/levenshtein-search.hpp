#ifndef SIMPLE_SEMANTIC_SEARCH_H
#define SIMPLE_SEMANTIC_SEARCH_H

#include "yolo-detector.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

// Head-only library for JSON parsing
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Frame
{
    int frame_number;
    std::vector<Detection> detections;
};

struct SearchResult
{
    std::string tsStr;
    std::string className;
    int frame_number;
    float similarity_score;
};

class LevenshteinSearch
{
public:
    LevenshteinSearch(const std::string &json_path);

    std::vector<SearchResult> search(const std::string &query, float min_similarity = 0.3f);

    void toJson(const std::string &output_path, const std::vector<SearchResult> &results);

private:
    std::vector<Frame> data_;
    std::vector<std::string> class_names_;

    // Dicionário de sinônimos/palavras relacionadas
    std::unordered_map<std::string, std::vector<std::string>> synonyms_;

    void load_json(const std::string &json_path);
    void init_synonyms();
    float calculate_similarity(const std::string &query, const std::string &class_name);
    std::vector<std::string> tokenize(const std::string &text);
    float levenshtein_similarity(const std::string &a, const std::string &b);
};

#endif