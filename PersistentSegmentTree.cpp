#include <iostream>
#include <vector>
#include <stdexcept>

template <typename T>
class PersistentSegmentTree {
public:
    struct Node {
        T value;
        int l, r;
        Node* left;
        Node* right;

        Node(T val, int left_bound, int right_bound, Node* lc = nullptr, Node* rc = nullptr)
            : value(val), l(left_bound), r(right_bound), left(lc), right(rc) {}

        // Constructor de copia para clonar en O(1)
        Node(const Node& other)
            : value(other.value), l(other.l), r(other.r), left(other.left), right(other.right) {}
    };

private:
    std::vector<Node*> version_roots;

    // Función de combinación (suma por defecto)
    T combine(const T& a, const T& b) const {
        return a + b;
    }

    // Construcción inicial recursiva en O(n)
    Node* build(int l, int r, const std::vector<T>& a) {
        if (l == r) {
            return new Node(a[l], l, r);
        }
        int mid = l + (r - l) / 2;
        Node* left_child = build(l, mid, a);
        Node* right_child = build(mid + 1, r, a);
        return new Node(combine(left_child->value, right_child->value), l, r, left_child, right_child);
    }

    // Update persistente con Path Copying según Algoritmo 3 del PDF
    Node* update_recursive(Node* curr, int pos, const T& new_val) {
        // 1. Clona el nodo actual
        Node* new_node = new Node(*curr);

        // 2. Caso base: reached the leaf
        if (new_node->l == new_node->r) {
            new_node->value = new_val;
            return new_node;
        }

        int mid = new_node->l + (new_node->r - new_node->l) / 2;
        if (pos <= mid) {
            // El hijo derecho se comparte; se clona el camino por la izquierda
            new_node->left = update_recursive(curr->left, pos, new_val);
        } else {
            // El hijo izquierdo se comparte; se clona el camino por la derecha
            new_node->right = update_recursive(curr->right, pos, new_val);
        }

        // 3. Recalcular el valor combinado del nuevo nodo
        new_node->value = combine(new_node->left->value, new_node->right->value);
        return new_node;
    }

    // Consulta estándar en rango [ql, qr] sobre una raíz específica
    T query_recursive(Node* curr, int ql, int qr) const {
        if (curr == nullptr || ql > curr->r || qr < curr->l || ql > qr) {
            return T(0); // Elemento neutro de la suma
        }
        if (ql <= curr->l && curr->r <= qr) {
            return curr->value;
        }
        return combine(query_recursive(curr->left, ql, qr),
                       query_recursive(curr->right, ql, qr));
    }

public:
    // Construcción a partir de un arreglo base (indexado en 0)
    PersistentSegmentTree(const std::vector<T>& a) {
        if (a.empty()) {
            throw std::invalid_argument("El arreglo no puede estar vacío");
        }
        Node* root_v0 = build(0, static_cast<int>(a.size()) - 1, a);
        version_roots.push_back(root_v0);
    }

    // Actualiza la posición 'pos' con 'new_val' partiendo de 'base_version'
    // Retorna el índice identificador de la nueva versión creada
    int update(int base_version, int pos, const T& new_val) {
        if (base_version < 0 || base_version >= static_cast<int>(version_roots.size())) {
            throw std::out_of_range("ID de versión base inválido");
        }
        Node* new_root = update_recursive(version_roots[base_version], pos, new_val);
        version_roots.push_back(new_root);
        return static_cast<int>(version_roots.size()) - 1;
    }

    // Realiza una consulta sobre el rango [ql, qr] en la versión indicada
    T query(int version_id, int ql, int qr) const {
        if (version_id < 0 || version_id >= static_cast<int>(version_roots.size())) {
            throw std::out_of_range("ID de versión inválido");
        }
        return query_recursive(version_roots[version_id], ql, qr);
    }

    int get_version_count() const {
        return static_cast<int>(version_roots.size());
    }
};
