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

    RopeNode() :
        left(nullptr), right(nullptr), size(0), buffer(nullptr)
#ifdef W5N_ROPE_UTF8_SUPPORT
        ,
        charCount(0)
#endif
    {
    }

    RopeNode(std::shared_ptr<const RopeNode> left_node, std::shared_ptr<const RopeNode> right_node) :
        left(left_node), right(right_node), size(0), buffer(nullptr)
#ifdef W5N_ROPE_UTF8_SUPPORT
        ,
        charCount(0)
#endif
    {
        if (left_node != nullptr) {
            size = left_node->size;
#ifdef W5N_ROPE_UTF8_SUPPORT
            charCount = left_node->charCount;
#endif
        }

        if (right_node != nullptr) {
            size += right_node->size;
#ifdef W5N_ROPE_UTF8_SUPPORT
            charCount += right_node->charCount;
#endif
        }
    }

    RopeNode(std::string_view value) : RopeNode()
    {
        const auto sz = value.size();
        if (sz > 0) {
            size = sz;
            buffer = std::make_shared<std::string>(value.data(), value.size());
#ifdef W5N_ROPE_UTF8_SUPPORT
            auto utf8View = uni::ranges::utf8_view(value);
            charCount = std::distance(utf8View.begin(), utf8View.end());
#endif
        }
    }

#ifdef W5N_ROPE_UTF8_SUPPORT
    std::string at(size_t index) const
#else
    char at(size_t index) const
#endif
    {
        if (is_leaf()) {
#ifdef W5N_ROPE_UTF8_SUPPORT
            if (index >= charCount) {
                return std::string{};
            }

            using uni::views::drop;
            using uni::views::take;

            auto utf8View = uni::ranges::utf8_view(*buffer);
            return utf8View | drop(index) | take(1) | uni::ranges::to_utf8<std::string>();
#else
            if (index >= size) {
                return '\0';
            }

            return (*buffer)[index];
#endif
        }

        if (index < weight()) {
            return left->at(index);
        } else {
            return right->at(index - weight());
        }
    }

    std::pair<std::shared_ptr<const RopeNode>, std::shared_ptr<const RopeNode>> split(size_t index) const
    {
        if (is_leaf()) {
            if (buffer == nullptr) {
                return {std::make_shared<const RopeNode>(std::string_view{}),
                        std::make_shared<const RopeNode>(std::string_view{})};
            }

#ifdef W5N_ROPE_UTF8_SUPPORT
            if (index > charCount) {
                index = charCount;
            }
#else
            if (index > buffer->size()) {
                index = buffer->size();
            }

#endif

#ifdef W5N_ROPE_UTF8_SUPPORT
            auto utf8View = uni::ranges::utf8_view(*buffer);

            auto begin_first = std::begin(*buffer);
            auto end_first = std::next(utf8View.begin(), index).begin();
            auto begin_second = end_first;
            auto end_second = std::end(*buffer);
#else
            auto begin_first = std::begin(*buffer);
            auto end_first = begin_first + index;
            auto begin_second = end_first;
            auto end_second = std::end(*buffer);
#endif

            auto test = std::make_shared<const RopeNode>(std::string_view{begin_second, end_second});

            return {std::make_shared<const RopeNode>(std::string_view{begin_first, end_first}),
                    std::make_shared<const RopeNode>(std::string_view{begin_second, end_second})};
        }

        if (index < weight()) {
            auto parts = left->split(index);

            return {parts.first, std::make_shared<const RopeNode>(parts.second, right)};
        } else if (index > weight()) {
            auto parts = right->split(index - weight());

            return {std::make_shared<const RopeNode>(left, parts.first), parts.second};
        }

        return {left, right};
    }

    size_t weight() const
    {
        if (is_leaf()) {
#ifdef W5N_ROPE_UTF8_SUPPORT
            return charCount;
#else
            return size;
#endif
        }

#ifdef W5N_ROPE_UTF8_SUPPORT
        return left->charCount;
#else
        return left->size;
#endif
    }

    size_t depth() const
    {
        std::stack<const RopeNode*> queue;
        std::stack<const RopeNode*> path;

        size_t depth = 0;
        queue.emplace(this);

        while (!queue.empty()) {
            auto r = queue.top();

            if (!path.empty() && r == path.top()) {
                if (path.size() > depth) {
                    depth = path.size();
                }

                path.pop();
                queue.pop();
            } else {
                path.emplace(r);

                if (r->left != nullptr) {
                    queue.emplace(&*r->left);
                }

                if (r->right != nullptr) {
                    queue.emplace(&*r->right);
                }
            }
        }

        return depth;
    }

    std::vector<std::shared_ptr<const RopeNode>> collect_leaves() const
    {
        std::vector<std::shared_ptr<const RopeNode>> children;
        std::stack<std::shared_ptr<const RopeNode>> nodes;

        nodes.push(shared_from_this());

        while (nodes.size() > 0) {
            auto node = nodes.top();
            nodes.pop();

            if (node->is_leaf()) {
                if (node->buffer != nullptr) {
                    children.push_back(node);
                }
                continue;
            }

            if (node->right != nullptr) {
                nodes.push(node->right);
            }

            if (node->left != nullptr) {
                nodes.push(node->left);
            }
        }

        return children;
    }

    bool is_leaf() const
    {
        return left == nullptr && right == nullptr;
    }

    std::string to_string() const
    {
        std::stringstream stream;
        auto nodes = collect_leaves();

        std::for_each(std::begin(nodes), std::end(nodes), [&stream](const auto node) {
            if (node->size > 0 && node->buffer != nullptr) {
                std::string tmp{std::begin(*node->buffer), std::end(*node->buffer)};
                stream << tmp;
            }
        });

        return stream.str();
    }
};

struct Rope
{
  public:
    Rope() : root(std::make_shared<const RopeNode>())
    {
    }

    Rope(const Rope&) = default;
    Rope& operator=(const Rope&) = default;

    ~Rope()
    {
        std::stack<std::shared_ptr<const RopeNode>> queue;
        std::stack<std::shared_ptr<const RopeNode>> path;

        queue.emplace(root);

        while (!queue.empty()) {
            auto r = queue.top();

            if (!path.empty() && r == path.top()) {
                // all children processed
                path.pop();
                queue.pop();

                // 2 = on the tree and on the r variable only
                if (r.use_count() > 2) {
                    continue;
                }

                auto node = const_cast<RopeNode*>(r.get());

                if (node->left.use_count() <= 1) {
                    node->left.reset();
                }

                if (node->right.use_count() <= 1) {
                    node->right.reset();
                }

                r.reset();
            } else {
                path.emplace(r);

                if (r->right != nullptr) {
                    queue.emplace(r->right);
                }

                if (r->left != nullptr) {
                    queue.emplace(r->left);
                }
            }
        }

        root = nullptr;
    }

    void rebalance()
    {
        if (is_balanced()) {
            return;
        }

        auto leaves = root->collect_leaves();
        root = do_merge(leaves, 0, leaves.size());
    }

    bool is_balanced() const
    {
        return is_balanced(root);
    }

    void append(std::string_view content)
    {
        root = concat(root, std::make_shared<const RopeNode>(content));
    }

    void prepend(std::string_view content)
    {
        root = concat(std::make_shared<const RopeNode>(content), root);
    }

    void clear()
    {
        root = std::make_shared<const RopeNode>();
    }

    bool insert(size_t position, std::string_view content)
    {
        if (position == 0) {
            prepend(content);

            return true;
        }

        auto sz = size();

        if (position == sz) {
            append(content);

            return true;
        } else if (position > sz) {
            return false;
        }

        auto parts = split(position);

        root = concat(concat(parts.first, std::make_shared<const RopeNode>(content)), parts.second);

        return true;
    }

    bool erase(size_t position, size_t size)
    {
        if (size == 0) {
            return false;
        }

        auto parts = split(position);
        auto to_remove_parts = split(position + size);

        root = concat(parts.first, to_remove_parts.second);

        return true;
    }

    std::string to_string() const
    {
        return root->to_string();
    }

    std::string substring(size_t from) const
    {
        auto parts = split(from);

        return parts.second->to_string();
    }

    std::string substring(size_t from, size_t size) const
    {
        auto begin_parts = split(from);
        auto range = begin_parts.second->split(size);

        return range.first->to_string();
    }

    size_t size() const
    {
        return root->size;
    }

#ifdef W5N_ROPE_UTF8_SUPPORT
    size_t charCount() const
    {
        return root->charCount;
    }

#endif

#ifdef W5N_ROPE_UTF8_SUPPORT
    const std::string at(size_t index) const
#else
    const char at(size_t index) const
#endif
    {
        return root->at(index);
    }

  private:
    Rope(std::shared_ptr<const RopeNode> r) : root(r)
    {
    }

    std::shared_ptr<const RopeNode> root;

    std::pair<std::shared_ptr<const RopeNode>, std::shared_ptr<const RopeNode>> split(size_t index) const
    {
        return root->split(index);
    }

    std::shared_ptr<const RopeNode> concat(std::shared_ptr<const RopeNode> left,
                                           std::shared_ptr<const RopeNode> right) const
    {
        return std::make_shared<const RopeNode>(left, right);
    }

    bool is_balanced(std::shared_ptr<const RopeNode> node) const
    {
        long long left_depth = node->left == nullptr ? 0 : node->left->depth();
        long long right_depth = node->right == nullptr ? 0 : node->right->depth();

        return std::abs(left_depth - right_depth) <= 2;
    }

    std::shared_ptr<const RopeNode> do_merge(const std::vector<std::shared_ptr<const RopeNode>>& leaves,
                                             size_t start,
                                             size_t end) const
    {
        auto range = end - start;

        if (range == 1) {
            return leaves[start];
        }

        if (range == 2) {
            return concat(leaves[start], leaves[start + 1]);
        }

        auto mid = start + range / 2;

        return concat(do_merge(leaves, start, mid), do_merge(leaves, mid, end));
    }
};
