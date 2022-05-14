#include <algorithm>
#include <cwchar>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <stack>
#include <string>
#include <utility>

struct RopeNode : std::enable_shared_from_this<RopeNode>
{
    std::shared_ptr<const RopeNode> left;
    std::shared_ptr<const RopeNode> right;

    std::vector<char> buffer;
    size_t size;
    size_t weight;

    RopeNode() : left(nullptr), right(nullptr), size(0), weight(0)
    {
    }

    ~RopeNode()
    {
    }

    RopeNode(std::shared_ptr<const RopeNode> left_node, std::shared_ptr<const RopeNode> right_node) :
        left(left_node), right(right_node), size(0), weight(0)
    {
        if (left_node != nullptr) {
            weight = left_node->size;
            size = weight;
        }

        if (right_node != nullptr) {
            size += right_node->size;
        }
    }

    RopeNode(std::string_view value) : RopeNode()
    {
        const auto sz = value.size();
        weight = sz;
        size = sz;
        buffer.resize(sz);
        std::copy(std::begin(value), std::end(value), buffer.data());
    }

    char at(size_t index) const
    {
        if (is_leaf()) {
            if (index >= buffer.size()) {
                // TODO: throw
                return '\0';
            }
            return buffer[index];
        }

        if (index < weight) {
            return left->at(index);
        } else {
            return right->at(index - weight);
        }
    }

    std::pair<std::shared_ptr<const RopeNode>, std::shared_ptr<const RopeNode>> split(size_t index) const
    {
        if (is_leaf()) {
            auto begin_first = std::begin(buffer);
            auto end_first = begin_first + index;
            auto begin_second = end_first;
            auto end_second = std::end(buffer);
            return {std::make_shared<const RopeNode>(std::string_view{begin_first, end_first}),
                    std::make_shared<const RopeNode>(std::string_view{begin_second, end_second})};
        }

        if (index < weight) {
            auto parts = left->split(index);
            return {parts.first, std::make_shared<const RopeNode>(parts.second, right)};
        } else if (index > weight) {
            auto parts = right->split(index - weight);
            return {std::make_shared<const RopeNode>(left, parts.first), parts.second};
        }

        return {left, right};
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
                children.push_back(node);
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
            if (node->buffer.size() > 0) {
                std::string tmp{std::begin(node->buffer), std::end(node->buffer)};
                stream << tmp;
            }
        });

        return stream.str();
    }
};

struct Rope
{
    std::shared_ptr<const RopeNode> root;

    Rope() : root(std::make_shared<const RopeNode>())
    {
    }

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

                auto node = const_cast<RopeNode*>(r.get());
                node->left.reset();
                node->right.reset();

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

    std::shared_ptr<const RopeNode> rebalance() const
    {
        if (!is_balanced(root)) {
            auto leaves = root->collect_leaves();
            return do_merge(leaves, 0, leaves.size());
        }

        return root;
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

    void append(std::string_view content)
    {
        root = concat(root, std::make_shared<const RopeNode>(content));
    }

    void prepend(std::string_view content)
    {
        root = concat(std::make_shared<const RopeNode>(content), root);
    }

    void insert(size_t position, std::string_view content)
    {
        if (position == 0) {
            prepend(content);
            return;
        }

        auto sz = size();

        if (position == sz) {
            append(content);
        } else if (position > sz) {
            // TODO: throw
            return;
        }

        auto parts = split(position);

        root = concat(concat(parts.first, std::make_shared<const RopeNode>(content)), parts.second);
    }

    void erase(size_t position, size_t size)
    {
        if (size == 0) {
            return;
        }

        auto parts = split(position);
        auto to_remove_parts = split(position + size);

        root = concat(parts.first, to_remove_parts.second);
    }

    std::pair<std::shared_ptr<const RopeNode>, std::shared_ptr<const RopeNode>> split(size_t index) const
    {
        return root->split(index);
    }

    std::shared_ptr<const RopeNode> concat(std::shared_ptr<const RopeNode> left,
                                           std::shared_ptr<const RopeNode> right) const
    {
        return std::make_shared<const RopeNode>(left, right);
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

    const char at(size_t index) const
    {
        return root->at(index);
    }
};

int main(int argc, char* argv[])
{
    Rope data;
    data.append("Waldson");
    data.append("Patricio");
    data.append("Nascimento");
    data.append("leandro");

    std::cout << data.substring(7, 5) << std::endl;
    return 0;
}
