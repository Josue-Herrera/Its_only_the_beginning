
#include <cmath>
#include <vector>
#include <list>
#include <tuple>
#include <type_traits>
#include <algorithm>

namespace detail{
    template<class T>
    T&& minimum(T&& val) noexcept
    {
        return std::forward<T>(val);
    }

    template<class Compare, class T0, class T1, class... Ts>
    auto minimum(Compare compare, T0&& val1, T1&& val2, Ts&&... vs) noexcept
    {
        return (compare(val1, val2)) ?
        minimum(val1, std::forward<Ts>(vs)...) :
        minimum(val2, std::forward<Ts>(vs)...);
    }
}

template <class T, int max_dimension>
class kdtree {

public:
    using value_type = T;
    using reference  = T&;
    using const_reference  = std::add_const_t<reference>;
    using rvalue_reference = std::add_rvalue_reference_t<value_type>;
    using pointer          = std::add_pointer_t<std::decay_t<T>>;

    kdtree(const kdtree&) = delete;
    kdtree& operator=(const kdtree&) = delete;

    class kdtree_node {
        
    public:
        kdtree_node()  = default;
        ~kdtree_node() = default;

        kdtree_node(const_reference data) noexcept
        : data_{data}, left_{nullptr}, right_{nullptr}, dimension_{0} { }

        kdtree_node(const_reference data, int & dim) noexcept
        : data_{data}, left_{nullptr}, right_{nullptr}, dimension_{dim} { }

        kdtree_node(rvalue_reference data) noexcept
        : data_{std::move(data)}, left_{nullptr}, right_{nullptr}, dimension_{0} { }
       
        kdtree_node(const kdtree_node& rhs) noexcept 
        : data_ {rhs.data_},  left_ {rhs.left_},
          right_{rhs.right_}, dimension_{rhs.dimension_}
        {  }

        kdtree_node(kdtree_node&& rhs) noexcept 
        : data_ {std::move(rhs.data_)}, left_ {std::move(rhs.left_)},
          right_{std::move(rhs.right_)}, dimension_{std::move(rhs.dimension_)}
        {  }
        

        kdtree_node& operator=(const kdtree_node& rhs)noexcept {
            data_ = rhs.data_;
            left_ = rhs.left_;
            right_= rhs.right_;
            dimension_= rhs.dimension_;
            return *this;
        }

        kdtree_node& operator=(kdtree_node&& rhs) noexcept { 
            data_ = std::move(rhs.data_);
            left_ = std::move(rhs.left_);
            right_= std::move(rhs.right_);
            dimension_= std::move(rhs.dimension_);
            return *this;
        }

        kdtree_node * left_;
        kdtree_node * right_;
        value_type data_;
        int dimension_;

    };


    using kdtree_container = std::list<kdtree_node>;
    kdtree_node * root_;
    kdtree_container nodes_; // only used if is constructed with vector;


     auto get_dimension_value (reference data, int current_dimension ) noexcept -> decltype(data[0]) {
        return data[ current_dimension % max_dimension ];
    }

    const auto get_dimension_value ( kdtree_node * node ) noexcept {
        return get_dimension_value (node->data_, node->dimension_);
    }

     const bool compare (reference data, kdtree_node * node) noexcept {
        return std::signbit ( get_dimension_value (data, node->dimension_) - get_dimension_value(node) );
    }

     const auto split_difference (reference data, kdtree_node * node) noexcept {
        return std::abs( get_dimension_value (data, node->dimension_) - get_dimension_value(node) );
    }

     kdtree_node * search (const_reference target, kdtree_node * node ) noexcept {
        if ( !node ) 
            return nullptr;
        else if (node->data_ == target) 
            return node;
        else if (compare(target, node)) {
            kdtree_node* value = search(target, node->left_);
            if (value) 
                return value;
            else
                return search(target, node->right_);
        }
        else {
            kdtree_node* value = search(target, node->right_);
            if (value)
                return value;
            else
                return search(target, node->left_);
        }
    }

     kdtree_node * search (const_reference target) noexcept {
        return search(target, root_);
    }

     kdtree_node * insert (const_reference data, kdtree_node * node, int depth = 0) noexcept {
        if ( !node ) 
            return &nodes_.emplace_back(data, depth);
        else if (node->data_ == data) 
            return node;
        else if (compare(data, node)) {
            // undefinded behavior when new reallocation of std::vector.
            node->left_ = insert(data, node->left_, node->dimension_ + 1); 
            return node;
        }
        else {
             node->right_ = insert(data, node->right_, node->dimension_ + 1);
            return node;
        };
    }

     kdtree_node * insert (const_reference data) noexcept {
        if (nodes_.empty()) {
            root_ = insert(data, nullptr);
            return root_;
        }
        else 
            return insert (data, root_, 1);
    }

     kdtree_node * find_minimum (kdtree_node * node, int index) noexcept {
        using namespace detail; 

        if (!node) 
            return nullptr;
        else if( node->dimension_ == index) {
            if (!node->left_) 
                return node;
            else 
                find_minimum (node->left_, index);
        } else {
            kdtree_node * left_min_node  = find_minimum (node->left_ , index);
            kdtree_node * right_min_node = find_minimum (node->right_, index);
            
            auto node_comparison = [&](auto & node1, auto & node2) { 
                if (node1 && node2 )
                    return node1->data_[index] < node2->data_[index];
                // since one pointer could be null but not both we must do this check.
                if (!node2 && node1) return true; // if node2 is null and node1 exists than node1 is less than node2.
                if (!node1 && node2) return false;// if node1 is null and node2 exists than node2 is less than node1;
             };

            return minimum(node_comparison, node, left_min_node, right_min_node); // variadac minimum 
        }
    }

     kdtree_node * construct (std::size_t begin, std::size_t end, int depth = 0 ) noexcept {
        if (end <= begin ) return nullptr;
    
        size_t n = begin + (end - begin)/2;
        auto i = nodes_.begin();
        auto node_comparison = [&](const auto & node1, const auto & node2) {
            n++;
                                    return node1.data_[depth] < node2.data_[depth];};
        nodes_.sort(node_comparison);
        //std::nth_element(i + begin, i + n, i + end, node_comparison );
        depth = (depth + 1) % max_dimension;
       // nodes_[n].left_  = construct(begin, n, depth);

       // nodes_[n].right_ = construct(n + 1, end, depth);
        
        return &nodes_.front();
    }

    template<class container>
    kdtree (container & cont) noexcept
    : nodes_ { cont.begin(), cont.end() }, root_{nullptr}
    {
        root_ = construct (0, nodes_.size(), 0);
    }
};