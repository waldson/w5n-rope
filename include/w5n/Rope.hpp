#pragma once

#include <memory>
#include <string>
#include <vector>

namespace w5n {

struct Rope
{
  public:
    Rope();

    Rope(const Rope&) = default;
    Rope& operator=(const Rope&) = default;

    ~Rope();

    void rebalance();

    bool isBalanced() const;

    void append(std::string_view content);

    void prepend(std::string_view content);

    void clear();

    bool insert(size_t position, std::string_view content);

    bool erase(size_t position, size_t size);

    std::string toString() const;

    std::string substring(size_t from) const;

    std::string substring(size_t from, size_t size) const;

    size_t size() const;

#ifndef W5N_ROPE_UTF8_IGNORE
    size_t charCount() const;
#endif

#ifndef W5N_ROPE_UTF8_IGNORE
    const std::string at(size_t index) const;
#else
    char at(size_t index) const;
#endif

  private:
    struct RopeNode : std::enable_shared_from_this<RopeNode>
    {
        std::shared_ptr<const RopeNode> left;
        std::shared_ptr<const RopeNode> right;
        size_t size;
#ifndef W5N_ROPE_UTF8_IGNORE
        size_t charCount;
#endif
        std::shared_ptr<std::string> buffer;

        RopeNode();

        RopeNode(std::shared_ptr<const RopeNode> left_node, std::shared_ptr<const RopeNode> right_node);

        RopeNode(std::string_view value);

#ifndef W5N_ROPE_UTF8_IGNORE
        std::string at(size_t index) const;
#else
        char at(size_t index) const;
#endif

        std::pair<std::shared_ptr<const RopeNode>, std::shared_ptr<const RopeNode>> split(size_t index) const;

        size_t weight() const;

        size_t depth() const;

        std::vector<std::shared_ptr<const RopeNode>> collectLeaves() const;

        bool isLeaf() const;

        std::string toString() const;
    };

    Rope(std::shared_ptr<const RopeNode> r);

    std::shared_ptr<const RopeNode> root;

    std::pair<std::shared_ptr<const RopeNode>, std::shared_ptr<const RopeNode>> split(size_t index) const;

    std::shared_ptr<const RopeNode> concat(std::shared_ptr<const RopeNode> left,
                                           std::shared_ptr<const RopeNode> right) const;

    bool isBalanced(std::shared_ptr<const RopeNode> node) const;

    std::shared_ptr<const RopeNode> doMerge(const std::vector<std::shared_ptr<const RopeNode>>& leaves,
                                            size_t start,
                                            size_t end) const;
};

} // namespace w5n
