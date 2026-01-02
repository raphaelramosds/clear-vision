#include "../include/levenshtein-search.hpp"

#include <fstream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <set>

LevenshteinSearch::LevenshteinSearch(const std::string& json_path) {
    load_json(json_path);
}

void LevenshteinSearch::load_json(const std:: string& json_path) {
    std::ifstream file(json_path);
    json j;
    file >> j;
    
    std::set<std::string> unique_classes;
    
    for (const auto& frame_json : j) {
        Frame frame;
        frame.frame_number = frame_json["frame_number"];
        
        for (const auto& det_json : frame_json["detections"]) {
            Detection det;
            det.ts = det_json["ts"];
            det.className = det_json["class_name"];
            det.tsStr = det_json["tsStr"];
            det.confidence = det_json["confidence"];
            det.x = det_json["x"];
            det.y = det_json["y"];
            det.w = det_json["width"];
            det.h = det_json["height"];
            
            frame.detections.push_back(det);
            unique_classes.insert(det.className);
        }
        
        data_.push_back(frame);
    }
    
    class_names_ = std::vector<std::string>(unique_classes.begin(), unique_classes.end());
}

std::vector<std::string> LevenshteinSearch::tokenize(const std::string& text) {
    std::vector<std:: string> tokens;
    std:: stringstream ss(text);
    std::string token;
    
    while (ss >> token) {
        // Convert to lowercase
        std::transform(token.begin(), token.end(), token.begin(), ::tolower);
        // Remove punctuation
        token.erase(std::remove_if(token.begin(), token.end(), ::ispunct), token.end());
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

/**
 * Solve levenshtein distance matrix and convert to similarity score
 * Reference: https://blog.paperspace.com/measuring-text-similarity-using-levenshtein-distance/
 */
float LevenshteinSearch::levenshtein_similarity(const std::string& a, const std:: string& b) {
    std::vector<std::vector<int>> dp(a.size() + 1, std::vector<int>(b.size() + 1));
    
    for (size_t i = 0; i <= a.size(); ++i) dp[i][0] = i;
    for (size_t j = 0; j <= b. size(); ++j) dp[0][j] = j;
    
    for (size_t i = 1; i <= a. size(); ++i) {
        for (size_t j = 1; j <= b.size(); ++j) {
            if (a[i-1] == b[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
            }
        }
    }
    
    int distance = dp[a.size()][b.size()];
    int max_len = std::max(a.size(), b.size());
    return 1.0f - (static_cast<float>(distance) / max_len);
}

float LevenshteinSearch::calculate_similarity(const std::string& query, 
                                                  const std::string& class_name) {
    auto query_tokens = tokenize(query);
    auto class_tokens = tokenize(class_name);
    
    float max_similarity = 0.0f;
    
    // Compare each token in query with each token in class name and get the one with max similarity
    for (const auto& q_token : query_tokens) {
        for (const auto& c_token : class_tokens) {
            // EXACT match
            if (q_token == c_token) {
                max_similarity = std::max(max_similarity, 1.0f);
                continue;
            }
                       
            // APPROXIMATE match: Levenshtein similarity
            float lev_sim = levenshtein_similarity(q_token, c_token);
            max_similarity = std::max(max_similarity, lev_sim * 0.7f);
        }
    }
    
    return max_similarity;
}

std::vector<SearchResult> LevenshteinSearch::search(const std::string& query, 
                                                        float min_similarity) {
    std::vector<SearchResult> results;
    
    for (const auto& frame : data_) {
        for (const auto& detection : frame.detections) {
            float similarity = calculate_similarity(query, detection.className);
            
            if (similarity >= min_similarity) {
                results.push_back({detection.tsStr, detection.className, frame.frame_number, similarity});
            }
        }
    }
    
    std::sort(results.begin(), results.end(),
              [](const SearchResult& a, const SearchResult& b) {
                  return a.similarity_score > b.similarity_score;
              });
    
    return results;
}

void LevenshteinSearch::toJson(const std::string &output_path, const std::vector<SearchResult> &results)
{
    std::ofstream resultsFile(output_path);
    resultsFile << "[\n";

    for (size_t i = 0; i < results.size(); ++i)
    {
        const auto &result = results[i];
        resultsFile << "  {\n";
        resultsFile << "    \"tsStr\": \"" << result.tsStr << "\",\n";
        resultsFile << "    \"className\": \"" << result.className << "\",\n";
        resultsFile << "    \"similarity_score\": " << result.similarity_score << "\n";
        resultsFile << "  }";
        if (i < results.size() - 1)
            resultsFile << ",";
        resultsFile << "\n";
    }

    resultsFile << "]\n";
    resultsFile.close();
}