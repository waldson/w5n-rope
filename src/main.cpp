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

    RopeNode() : left(nullptr), right(nullptr)
    {
    }

    ~RopeNode()
    {
    }

    RopeNode(std::shared_ptr<const RopeNode> left_node, std::shared_ptr<const RopeNode> right_node) :
        left(left_node), right(right_node)
    {
    }

    RopeNode(std::string_view value) : RopeNode()
    {
        buffer.resize(value.size());
        std::copy(std::begin(value), std::end(value), buffer.data());
    }

    size_t size() const
    {
        auto leaves = collect_leaves();

        return std::accumulate(
            std::begin(leaves), std::end(leaves), size_t{0}, [](size_t size, std::shared_ptr<const RopeNode> node) {
                return size + node->buffer.size();
            });
    }

    size_t weight() const
    {
        if (is_leaf()) {
            return buffer.size();
        }

        if (left != nullptr) {
            auto leaves = collect_leaves();

            return std::accumulate(
                std::begin(leaves), std::end(leaves), size_t{0}, [](size_t size, std::shared_ptr<const RopeNode> node) {
                    return size + node->buffer.size();
                });
        }

        return 0;
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
    }

    // std::pair<std::shared_ptr<const RopeNode>, std::shared_ptr<const RopeNode>> split(size_t position)
    // {
    // }

    std::shared_ptr<const RopeNode> concat(std::shared_ptr<const RopeNode> left,
                                           std::shared_ptr<const RopeNode> right) const
    {
        return std::make_shared<const RopeNode>(left, right);
    }

    std::string to_string() const
    {
        return root->to_string();
    }

    size_t size() const
    {
        return root->size();
    }
};

int main(int argc, char* argv[])
{
    Rope data;

    for (size_t i = 0; i < 1350000; ++i) {
        data.append(std::to_string(i));
    }

    // data.rebalance();
    std::cout << "Before size" << std::endl;
    std::cout << data.size() << std::endl;
    std::cout << "After size" << std::endl;
    // std::cout << data.to_string() << std::endl;

    return 0;
}
