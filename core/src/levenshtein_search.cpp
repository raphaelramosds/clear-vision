#include "levenshtein_search.hpp"

#include <fstream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <set>

std::vector<cvision::Frame> LevenshteinSearch::search(const std::string &query,
                                                    float min_similarity)
{
    std::vector<cvision::Frame> frames_;

    for (const auto &frame : data_)
    {
        cvision::Frame f;
        f.ts = frame.ts;
        f.tsStr = frame.tsStr;
        f.frameNumber = frame.frameNumber;
        f.detections = {};

        for (const auto &detection : frame.detections)
        {
            float similarity = calculate_similarity(query, detection.className);

            if (similarity >= min_similarity)
            {
                f.detections.push_back(detection);
            }
        }

        if (!f.detections.empty()) {
            frames_.push_back(f);
        }
    }

    std::sort(frames_.begin(), frames_.end(),
              [](const cvision::Frame &a, const cvision::Frame &b)
              {
                  return a.frameNumber > b.frameNumber;
              });

    return frames_;
}

std::vector<std::string> LevenshteinSearch::tokenize(const std::string &text)
{
    std::vector<std::string> tokens;
    std::stringstream ss(text);
    std::string token;

    while (ss >> token)
    {
        // Convert to lowercase
        std::transform(token.begin(), token.end(), token.begin(), ::tolower);
        // Remove punctuation
        token.erase(std::remove_if(token.begin(), token.end(), ::ispunct), token.end());
        if (!token.empty())
        {
            tokens.push_back(token);
        }
    }

    return tokens;
}

/**
 * Solve levenshtein distance matrix and convert to similarity score
 * Reference: https://blog.paperspace.com/measuring-text-similarity-using-levenshtein-distance/
 */
float LevenshteinSearch::levenshtein_similarity(const std::string &a, const std::string &b)
{
    std::vector<std::vector<int>> dp(a.size() + 1, std::vector<int>(b.size() + 1));

    for (size_t i = 0; i <= a.size(); ++i)
        dp[i][0] = i;
    for (size_t j = 0; j <= b.size(); ++j)
        dp[0][j] = j;

    for (size_t i = 1; i <= a.size(); ++i)
    {
        for (size_t j = 1; j <= b.size(); ++j)
        {
            if (a[i - 1] == b[j - 1])
            {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else
            {
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }
    }

    int distance = dp[a.size()][b.size()];
    int max_len = std::max(a.size(), b.size());
    return 1.0f - (static_cast<float>(distance) / max_len);
}

float LevenshteinSearch::calculate_similarity(const std::string &query,
                                              const std::string &class_name)
{
    auto query_tokens = tokenize(query);
    auto class_tokens = tokenize(class_name);

    float max_similarity = 0.0f;

    // Compare each token in query with each token in class name and get the one with max similarity
    for (const auto &q_token : query_tokens)
    {
        for (const auto &c_token : class_tokens)
        {
            // EXACT match
            if (q_token == c_token)
            {
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



