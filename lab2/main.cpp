#include <iostream>
#include <string>
#include <cstdint>
#include <algorithm>
#include <fstream>


enum class Color : bool {
    RED = false,
    BLACK = true
};
using enum Color;

std::ostream& operator<<(std::ostream& os, const Color color) {
    switch (color) {
        case RED: return os << "RED";
        case BLACK: return os << "BLACK";
    }
    return os;
}


struct Pair {
    std::string key;
    uint64_t value;

    Pair() = default;

    Pair(std::string&& key, uint64_t value)
     : key(std::move(key)),
    value(value) {}

    Pair(const std::string& key, uint64_t value)
     : key(key),
    value(value) {}

    friend std::ostream& operator<<(std::ostream& os, const Pair& pair) {
        return os << "Pair(" << "key=" << pair.key << ", " << "value=" << pair.value << ")";
    }
};


template <typename T>
class ParentWithColor {
    private:
        uintptr_t data;
        static constexpr uintptr_t MASK = ~static_cast<uintptr_t>(1);

    public:
        explicit ParentWithColor(T* ptr = nullptr, Color color = RED) : data(reinterpret_cast<uintptr_t>(ptr)) {}

        struct ColorProxy {
            uintptr_t& d;

            ColorProxy& operator=(Color color) {
                if (static_cast<bool>(color)) {
                    d |= 1;
                } else {
                    d &= MASK;
                }
                return *this;
            }

            ColorProxy& operator=(const ColorProxy& other) {
                return *this = static_cast<Color>(other);
            }

            operator Color() const {
                return static_cast<Color>(d & 1);
            }

            operator bool() const {
                return static_cast<bool>(static_cast<Color>(d & 1));
            }
        };

        struct ParentProxy {
            uintptr_t& d;

            ParentProxy& operator=(T* ptr) {
                uintptr_t current_color_bit = d & 1;
                d = (reinterpret_cast<uintptr_t>(ptr) & MASK) | current_color_bit;
                return *this;
            }

            ParentProxy& operator=(const ParentProxy& other) {
                if (this != &other) {
                    *this = static_cast<T*>(other);
                }
                return *this;
            }

            T* operator->() const {
                return reinterpret_cast<T*>(d & MASK);
            }
            
            T& operator*() const {
                return *reinterpret_cast<T*>(d & MASK);
            }

            operator T*() const {
                return reinterpret_cast<T*>(d & MASK);
            }
        };

        inline ColorProxy color() {
            return {data};
        }

        inline Color color() const {
            return static_cast<Color>(data & 1);
        }

        inline ParentProxy parent() {
            return {data};
        }

        inline const T* ptr() const {
            return reinterpret_cast<const T*>(data & MASK);
        }
};


struct alignas(alignof(void*)) Node {
    Pair data;
    Node* left;
    Node* right;
    ParentWithColor<Node> pwc;

    Node() = default;

    Node(const Pair& data) : data(data) {}

    friend std::ostream& operator<<(std::ostream& os, const Node& node) {
        return os << node.data << " " << node.pwc.color();
    }
};


class RBTree {
    private:
        Node* root;
        Node* NIL;

    public:

        RBTree() {
            NIL = new Node();
            setUpNIL();

            root = NIL;
        }
    
        ~RBTree() {
            clear(root);
            delete NIL;
        }

        RBTree(const RBTree&) = delete;
        RBTree& operator=(const RBTree&) = delete;

        bool insert(const Pair& data) {
            if (search_impl(to_lower(data.key)) != NIL) {
                return false;
            }

            Node* z = new Node(Pair(to_lower(data.key), data.value));
            Node* y = NIL;
            Node* x = root;

            while (x != NIL) {
                y = x;
                if (z->data.key < y->data.key) {
                    x = x->left;
                } else {
                    x = x->right;
                }
            }

            z->pwc.parent() = y;
            if (y == NIL) {
                root = z;
            } else if (z->data.key < y->data.key) {
                y->left = z;
            } else {
                y->right = z;
            }
        z->left = NIL;
        z->right = NIL;
        z->pwc.color() = RED;
        insertFixUp(z);
        return true;
        }

        bool remove(const std::string& key) {
            Node* z = search_impl(to_lower(key));
            if (z == NIL) {
                return false;
            }

            Node* y;
            if (z->left == NIL || z->right == NIL) {
                y = z;
            } else {
                y = successor(z);
            }

            Node* x;
            if (y->left != NIL) {
                x = y->left;
            } else {
                x = y->right;
            }
            x->pwc.parent() = y->pwc.parent();

            if (y->pwc.parent() == NIL) {
                root = x;
            } else if (y == y->pwc.parent()->left) {
                y->pwc.parent()->left = x;
            } else {
                y->pwc.parent()->right = x;
            }

            if (y != z) {
                z->data = std::move(y->data);
            }

            if (y->pwc.color() == BLACK) {
                removeFixUp(x);
            }
            delete y;
            setUpNIL();
            return true;
        }

        Node* search(const std::string& key) {
            Node* result = search_impl(to_lower(key));
            if (result == NIL) {
                return nullptr;
            }
            return result;
        }

        void save(const std::string& path) {
            std::ofstream out(path, std::ios::binary);
            save_impl(root, out);
        }

        void load(const std::string& path) {
            std::ifstream in(path, std::ios::binary);
            if (!in) {
                clear(root);
                root = NIL;
                return;
            }
            
            Node* new_root;
            try {
                new_root = load_impl(in, NIL);

                if (in.fail() && !in.eof()) {
                    throw std::runtime_error("Invalid file format or corrupted data");
                }

                clear(root);
                root = new_root;
                setUpNIL();

            } catch (...) {
                if (new_root != NIL) {
                    clear(new_root);
                }
                throw;
            }
        }

    private:
        void insertFixUp(Node* z) {
            while (z->pwc.parent()->pwc.color() == RED) {
                if (z->pwc.parent() == z->pwc.parent()->pwc.parent()->left) {
                    Node* y = z->pwc.parent()->pwc.parent()->right;
                    if (y->pwc.color() == RED) {
                        z->pwc.parent()->pwc.color() = BLACK;
                        y->pwc.color() = BLACK;
                        z->pwc.parent()->pwc.parent()->pwc.color() = RED;
                        z = z->pwc.parent()->pwc.parent();

                    } else {
                        if (y->pwc.color() == BLACK && z == z->pwc.parent()->right) {
                            z = z->pwc.parent();
                            leftRotate(z);
                        }
                        z->pwc.parent()->pwc.color() = BLACK;
                        z->pwc.parent()->pwc.parent()->pwc.color() = RED;
                        rightRotate(z->pwc.parent()->pwc.parent());
                    }
                } else if (z->pwc.parent() == z->pwc.parent()->pwc.parent()->right) {
                    Node* y = z->pwc.parent()->pwc.parent()->left;
                    if (y->pwc.color() == RED) {
                        z->pwc.parent()->pwc.color() = BLACK;
                        y->pwc.color() = BLACK;
                        z->pwc.parent()->pwc.parent()->pwc.color() = RED;
                        z = z->pwc.parent()->pwc.parent();

                    } else {
                        if (y->pwc.color() == BLACK && z == z->pwc.parent()->left) {
                            z = z->pwc.parent();
                            rightRotate(z);
                        }
                        z->pwc.parent()->pwc.color() = BLACK;
                        z->pwc.parent()->pwc.parent()->pwc.color() = RED;
                        leftRotate(z->pwc.parent()->pwc.parent());
                    }
                }
            }
            root->pwc.color() = BLACK;
        }
        
        void removeFixUp(Node* x) {
            while (x != root && x->pwc.color() == BLACK) {
                if (x == x->pwc.parent()->left) {
                    Node* w = x->pwc.parent()->right;
                    if (w->pwc.color() == RED) {
                        w->pwc.color() = BLACK;
                        x->pwc.parent()->pwc.color() = RED;
                        leftRotate(x->pwc.parent());
                        w = x->pwc.parent()->right;
                    }
                    if (w->left->pwc.color() == BLACK && w->right->pwc.color() == BLACK) {
                        w->pwc.color() = RED;
                        x = x->pwc.parent();
                    } else {
                        if (w->right->pwc.color() == BLACK) {
                            w->left->pwc.color() = BLACK;
                            w->pwc.color() = RED;
                            rightRotate(w);
                            w = x->pwc.parent()->right;
                        }
                        w->pwc.color() = x->pwc.parent()->pwc.color();
                        x->pwc.parent()->pwc.color() = BLACK;
                        w->right->pwc.color() = BLACK;
                        leftRotate(x->pwc.parent());
                        x = root;
                    }
                } else {
                    Node* w = x->pwc.parent()->left;
                    if (w->pwc.color() == RED) {
                        w->pwc.color() = BLACK;
                        x->pwc.parent()->pwc.color() = RED;
                        rightRotate(x->pwc.parent());
                        w = x->pwc.parent()->left;
                    }
                    if (w->left->pwc.color() == BLACK && w->right->pwc.color() == BLACK) {
                        w->pwc.color() = RED;
                        x = x->pwc.parent();
                    } else {
                            if (w->left->pwc.color() == BLACK) {
                            w->right->pwc.color() = BLACK;
                            w->pwc.color() = RED;
                            leftRotate(w);
                            w = x->pwc.parent()->left;
                        }
                        w->pwc.color() = x->pwc.parent()->pwc.color();
                        x->pwc.parent()->pwc.color() = BLACK;
                        w->left->pwc.color() = BLACK;
                        rightRotate(x->pwc.parent());
                        x = root;
                    }
                }
            }
            x->pwc.color() = BLACK;
        }
    
        void leftRotate(Node* x) {
            Node* y = x->right;
            x->right = y->left;

            if (y->left != NIL) {
                y->left->pwc.parent() = x;
            }
            y->pwc.parent() = x->pwc.parent();

            if (x->pwc.parent() == NIL) {
                root = y;
            } else if (x == x->pwc.parent()->left) {
                x->pwc.parent()->left = y;
            } else {
                x->pwc.parent()->right = y;
            }

            y->left = x;
            x->pwc.parent() = y;
        }

        void rightRotate(Node* y) {
            Node* x = y->left;
            y->left = x->right;
            
            if (x->right != NIL) {
                x->right->pwc.parent() = y;
            }
            x->pwc.parent() = y->pwc.parent();

            if (y->pwc.parent() == NIL) {
                root = x;
            } else if (y == y->pwc.parent()->left) {
                y->pwc.parent()->left = x;
            } else {
                y->pwc.parent()->right = x; 
            }

            x->right = y;
            y->pwc.parent() = x;
        }

        Node* successor(Node* x) {
            if (x->right != NIL) {
                return minimum(x->right);
            }
            Node* y = x->pwc.parent();
            while (x == y->right && y != NIL) {
                x = y;
                y = y->pwc.parent();
            }
            return y;   
        }

        Node* minimum(Node* x) {
            while (x->left != NIL) {
                x = x->left;
            }
            return x;
        }

        Node* search_impl(const std::string& key) {
            Node* current = root;
            while (current != NIL) {
                if (key == current->data.key) {
                    return current;
                } else if (key < current->data.key) {
                    current = current->left;
                } else {
                    current = current->right;
                }
            }
            return NIL;
        }

        void save_impl(Node* node, std::ofstream& out) {
            bool is_nil = (node == NIL);
            out.write(reinterpret_cast<const char*>(&is_nil), sizeof(is_nil));
            if (is_nil) return;

            size_t len = node->data.key.size();
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            out.write(node->data.key.c_str(), len);
            out.write(reinterpret_cast<const char*>(&node->data.value), sizeof(node->data.value));
            bool color = static_cast<bool>(node->pwc.color());
            out.write(reinterpret_cast<const char*>(&color), sizeof(color));

            save_impl(node->left, out);
            save_impl(node->right, out);
        }

        Node* load_impl(std::istream& in, Node* parent) {
            bool is_nil;
            in.read(reinterpret_cast<char*>(&is_nil), sizeof(is_nil));
            if (is_nil) return NIL;

            size_t len;
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            std::string key(len, ' ');
            in.read(&key[0], len);
            uint64_t val;
            in.read(reinterpret_cast<char*>(&val), sizeof(val));
            bool color_bit;
            in.read(reinterpret_cast<char*>(&color_bit), sizeof(color_bit));

            Node* node = new Node(Pair(key, val));
            node->pwc.parent() = parent;
            node->pwc.color() = static_cast<Color>(color_bit);
            
            node->left = load_impl(in, node);
            node->right = load_impl(in, node);
            return node;
        }

        void setUpNIL() const {
            NIL->pwc.color() = BLACK;
            NIL->pwc.parent() = NIL;
            NIL->left = NIL;
            NIL->right = NIL;
        }

        void clear(Node* node) {
            if (node != NIL) {
                clear(node->left);
                clear(node->right);
                delete node;
            }
        }

        std::string to_lower(std::string s) const {
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
            return s;
        }
    };


int main() {
    RBTree tree;
    std::string cmd;

    while (std::cin >> cmd) {
        try {
            if (cmd == "+") {
                std::string key;
                uint64_t val;
                std::cin >> key >> val;
                if (tree.insert(Pair(key, val))) {
                    std::cout << "OK\n";
                }
                else {
                    std::cout << "Exist\n";
                }
            } 
            else if (cmd == "-") {
                std::string key;
                std::cin >> key;
                if (tree.remove(key)) {
                    std::cout << "OK\n";
                }
                else {
                    std::cout << "NoSuchWord\n";
                }
            } 
            else if (cmd == "!") {
                std::string action, path;
                std::cin >> action >> path;
                if (action == "Save") {
                    tree.save(path);
                    std::cout << "OK\n";
                } else {
                    tree.load(path);
                    std::cout << "OK\n";
                }
            } 
            else {
                Node* res = tree.search(cmd);
                if (res) {
                    std::cout << "OK: " << res->data.value << "\n";
                }
                else {
                    std::cout << "NoSuchWord\n";
                }
            }
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << "\n";
        }
    }
    return 0;
}
