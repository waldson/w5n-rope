#include "w5n/Rope.hpp"

namespace w5n {

RopeNode::RopeNode() :
    left(nullptr), right(nullptr), size(0), buffer(nullptr)
#ifndef W5N_ROPE_UTF8_IGNORE
    ,
    charCount(0)
#endif
{
}

RopeNode::RopeNode(std::shared_ptr<const RopeNode> left_node, std::shared_ptr<const RopeNode> right_node) :
    left(left_node), right(right_node), size(0), buffer(nullptr)
#ifndef W5N_ROPE_UTF8_IGNORE
    ,
    charCount(0)
#endif
{
    if (left_node != nullptr) {
        size = left_node->size;
#ifndef W5N_ROPE_UTF8_IGNORE
        charCount = left_node->charCount;
#endif
    }

    if (right_node != nullptr) {
        size += right_node->size;
#ifndef W5N_ROPE_UTF8_IGNORE
        charCount += right_node->charCount;
#endif
    }
}

RopeNode::RopeNode(std::string_view value) : RopeNode()
{
    const auto sz = value.size();
    if (sz > 0) {
        size = sz;
        buffer = std::make_shared<std::string>(value.data(), value.size());
#ifndef W5N_ROPE_UTF8_IGNORE
        auto utf8View = uni::views::grapheme::utf8(value);
        charCount = std::distance(utf8View.begin(), utf8View.end());
#endif
    }
}

#ifndef W5N_ROPE_UTF8_IGNORE
std::string RopeNode::at(size_t index) const
#else
char RopeNode::at(size_t index) const
#endif
{
    if (isLeaf()) {
#ifndef W5N_ROPE_UTF8_IGNORE
        if (index >= charCount) {
            return std::string{};
        }

        using uni::views::drop;
        using uni::views::take;

        auto utf8View = uni::views::grapheme::utf8(*buffer);

        auto begin = std::next(utf8View.begin(), index).begin();
        auto end = std::next(utf8View.begin(), index + 1).begin();

        return std::string{begin, end};
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

std::pair<std::shared_ptr<const RopeNode>, std::shared_ptr<const RopeNode>> RopeNode::split(size_t index) const
{
    if (isLeaf()) {
        if (buffer == nullptr) {
            return {std::make_shared<const RopeNode>(std::string_view{}),
                    std::make_shared<const RopeNode>(std::string_view{})};
        }

#ifndef W5N_ROPE_UTF8_IGNORE
        if (index > charCount) {
            index = charCount;
        }
#else
        if (index > buffer->size()) {
            index = buffer->size();
        }

#endif

#ifndef W5N_ROPE_UTF8_IGNORE
        auto utf8View = uni::views::grapheme::utf8(*buffer);

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

size_t RopeNode::weight() const
{
    if (isLeaf()) {
#ifndef W5N_ROPE_UTF8_IGNORE
        return charCount;
#else
        return size;
#endif
    }

#ifndef W5N_ROPE_UTF8_IGNORE
    return left->charCount;
#else
    return left->size;
#endif
}

size_t RopeNode::depth() const
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

std::vector<std::shared_ptr<const RopeNode>> RopeNode::collect_leaves() const
{
    std::vector<std::shared_ptr<const RopeNode>> children;
    std::stack<std::shared_ptr<const RopeNode>> nodes;

    nodes.push(shared_from_this());

    while (nodes.size() > 0) {
        auto node = nodes.top();
        nodes.pop();

        if (node->isLeaf()) {
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

bool RopeNode::isLeaf() const
{
    return left == nullptr && right == nullptr;
}

std::string RopeNode::toString() const
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

Rope::Rope() : root(std::make_shared<const RopeNode>())
{
}

Rope::~Rope()
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

void Rope::rebalance()
{
    if (isBalanced()) {
        return;
    }

    auto leaves = root->collect_leaves();
    root = doMerge(leaves, 0, leaves.size());
}

bool Rope::isBalanced() const
{
    return isBalanced(root);
}

void Rope::append(std::string_view content)
{
    root = concat(root, std::make_shared<const RopeNode>(content));
}

void Rope::prepend(std::string_view content)
{
    root = concat(std::make_shared<const RopeNode>(content), root);
}

void Rope::clear()
{
    root = std::make_shared<const RopeNode>();
}

bool Rope::insert(size_t position, std::string_view content)
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

bool Rope::erase(size_t position, size_t size)
{
    if (size == 0) {
        return false;
    }

    auto parts = split(position);
    auto to_remove_parts = split(position + size);

    root = concat(parts.first, to_remove_parts.second);

    return true;
}

std::string Rope::toString() const
{
    return root->toString();
}

std::string Rope::substring(size_t from) const
{
    auto parts = split(from);

    return parts.second->toString();
}

std::string Rope::substring(size_t from, size_t size) const
{
    auto begin_parts = split(from);
    auto range = begin_parts.second->split(size);

    return range.first->toString();
}

size_t Rope::size() const
{
    return root->size;
}

#ifndef W5N_ROPE_UTF8_IGNORE
size_t Rope::charCount() const
{
    return root->charCount;
}
#endif

#ifndef W5N_ROPE_UTF8_IGNORE
const std::string Rope::at(size_t index) const
#else
char Rope::at(size_t index) const
#endif
{
    return root->at(index);
}

Rope::Rope(std::shared_ptr<const RopeNode> r) : root(r)
{
}

std::pair<std::shared_ptr<const RopeNode>, std::shared_ptr<const RopeNode>> Rope::split(size_t index) const
{
    return root->split(index);
}

std::shared_ptr<const RopeNode> Rope::concat(std::shared_ptr<const RopeNode> left,
                                             std::shared_ptr<const RopeNode> right) const
{
    return std::make_shared<const RopeNode>(left, right);
}

bool Rope::isBalanced(std::shared_ptr<const RopeNode> node) const
{
    long long left_depth = node->left == nullptr ? 0 : node->left->depth();
    long long right_depth = node->right == nullptr ? 0 : node->right->depth();

    return std::abs(left_depth - right_depth) <= 2;
}

std::shared_ptr<const RopeNode> Rope::doMerge(const std::vector<std::shared_ptr<const RopeNode>>& leaves,
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

    return concat(doMerge(leaves, start, mid), doMerge(leaves, mid, end));
}

} // namespace w5n
