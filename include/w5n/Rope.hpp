#pragma once

#include <algorithm>
#include <cwchar>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#ifdef W5N_ROPE_UTF8_SUPPORT
#include <iterator>
#include <uni_algo/break_grapheme.h>
#include <uni_algo/ranges.h>
#endif

struct RopeNode : std::enable_shared_from_this<RopeNode>
{
    std::shared_ptr<const RopeNode> left;
    std::shared_ptr<const RopeNode> right;
    size_t size;
#ifdef W5N_ROPE_UTF8_SUPPORT
    size_t charCount;
#endif

    std::shared_ptr<std::string> buffer;

    RopeNode();

    RopeNode(std::shared_ptr<const RopeNode> left_node, std::shared_ptr<const RopeNode> right_node);

    RopeNode(std::string_view value);

#ifdef W5N_ROPE_UTF8_SUPPORT
    std::string at(size_t index) const;
#else
    char at(size_t index) const;
#endif

    std::pair<std::shared_ptr<const RopeNode>, std::shared_ptr<const RopeNode>> split(size_t index) const;

    size_t weight() const;

    size_t depth() const;

    std::vector<std::shared_ptr<const RopeNode>> collect_leaves() const;

    bool is_leaf() const;

    std::string to_string() const;
};

struct Rope
{
  public:
    Rope();

    Rope(const Rope&) = default;
    Rope& operator=(const Rope&) = default;

    ~Rope();

    void rebalance();

    bool is_balanced() const;

    void append(std::string_view content);

    void prepend(std::string_view content);

    void clear();

    bool insert(size_t position, std::string_view content);

    bool erase(size_t position, size_t size);

    std::string to_string() const;

    std::string substring(size_t from) const;

    std::string substring(size_t from, size_t size) const;

    size_t size() const;

#ifdef W5N_ROPE_UTF8_SUPPORT
    size_t charCount() const;
#endif

#ifdef W5N_ROPE_UTF8_SUPPORT
    const std::string at(size_t index) const;
#else
    char at(size_t index) const;
#endif

  private:
    Rope(std::shared_ptr<const RopeNode> r);

    std::shared_ptr<const RopeNode> root;

    std::pair<std::shared_ptr<const RopeNode>, std::shared_ptr<const RopeNode>> split(size_t index) const;

    std::shared_ptr<const RopeNode> concat(std::shared_ptr<const RopeNode> left,
                                           std::shared_ptr<const RopeNode> right) const;

    bool is_balanced(std::shared_ptr<const RopeNode> node) const;

    std::shared_ptr<const RopeNode> do_merge(const std::vector<std::shared_ptr<const RopeNode>>& leaves,
                                             size_t start,
                                             size_t end) const;
};
