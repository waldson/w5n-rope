#include <algorithm>
#include <cwchar>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
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
        auto leaves = collect_leaves(this);

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
            auto leaves = collect_leaves(&*left);

            return std::accumulate(
                std::begin(leaves), std::end(leaves), size_t{0}, [](size_t size, std::shared_ptr<const RopeNode> node) {
                    return size + node->buffer.size();
                });
        }

        return 0;
    }

    size_t depth() const
    {
        return do_depth(this);
    }

    size_t do_depth(const RopeNode* node) const
    {
        if (node == nullptr || node->is_leaf()) {
            return 0;
        }

        return 1 + std::max(do_depth(&*node->left), do_depth(&*node->right));
    }

    std::vector<std::shared_ptr<const RopeNode>> collect_leaves(const RopeNode* root) const
    {
        std::vector<std::shared_ptr<const RopeNode>> children;

        do_collect_leaves(shared_from_this(), children);

        return children;
    }

    void do_collect_leaves(std::shared_ptr<const RopeNode> root,
                           std::vector<std::shared_ptr<const RopeNode>>& children) const
    {
        if (root == nullptr) {
            return;
        }

        if (root->is_leaf()) {
            children.push_back(root);
        }

        if (root->left) {
            do_collect_leaves(root->left, children);
        }

        if (root->right) {
            do_collect_leaves(root->right, children);
        }
    }

    bool is_leaf() const
    {
        return left == nullptr && right == nullptr;
    }

    std::string to_string() const
    {
        std::stringstream stream;
        auto nodes = collect_leaves(this);

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

    std::shared_ptr<const RopeNode> rebalance(std::shared_ptr<const RopeNode> node) const
    {
        if (!is_balanced(node)) {
            auto leaves = root->collect_leaves(&*root);
            return do_merge(leaves, 0, leaves.size());
        }

        return node;
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
        root = rebalance(concat(root, std::make_shared<const RopeNode>(content)));
    }

    void prepend(std::string_view content)
    {
        root = rebalance(concat(std::make_shared<const RopeNode>(content), root));
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

    for (size_t i = 0; i < 32; ++i) {
        data.append(std::to_string(i));
    }

    std::cout << data.to_string() << std::endl;
    std::cout << data.size() << std::endl;

    return 0;
}
