#include <iostream>
#include <string>
#include <vector>
#include <array>

class PersistentTrie {
public:
    static constexpr int ALPHABET_SIZE = 26;

    struct Node {
        std::array<Node*, ALPHABET_SIZE> children;
        bool is_terminal;

        Node() : is_terminal(false) {
            children.fill(nullptr);
        }

        // Constructor de copia para clonar el nodo en O(ALPHABET_SIZE) = O(1)
        Node(const Node& other) : children(other.children), is_terminal(other.is_terminal) {}
    };

private:
    std::vector<Node*> version_roots;

    // Inserción recursiva basada en el Algoritmo 4 del material de clase
    Node* insert_recursive(Node* curr, const std::string& s, size_t index) {
        // 1. nuevo <- copia de nodo (o nuevo nodo vacío si no existía)
        Node* new_node = (curr != nullptr) ? new Node(*curr) : new Node();

        // 2. Si llegamos al final de la cadena, marcamos fin de palabra
        if (index == s.size()) {
            new_node->is_terminal = true;
            return new_node;
        }

        // 3. Descendemos por el caracter s[index]
        int char_idx = s[index] - 'a';
        Node* old_child = new_node->children[char_idx];

        // 4. Clonamos recursivamente el camino hacia abajo y enlazamos
        new_node->children[char_idx] = insert_recursive(old_child, s, index + 1);

        return new_node;
    }

public:
    PersistentTrie() {
        // La versión 0 representa el Trie vacío
        version_roots.push_back(nullptr);
    }

    // Inserta la cadena 's' a partir de 'base_version' y genera una nueva versión
    int insert(int base_version, const std::string& s) {
        if (base_version < 0 || base_version >= static_cast<int>(version_roots.size())) {
            throw std::out_of_range("ID de versión base inválido");
        }
        Node* new_root = insert_recursive(version_roots[base_version], s, 0);
        version_roots.push_back(new_root);
        return static_cast<int>(version_roots.size()) - 1;
    }

    // Consulta si 's' existe como palabra completa en la versión 'version_id'
    bool search(int version_id, const std::string& s) const {
        if (version_id < 0 || version_id >= static_cast<int>(version_roots.size())) {
            throw std::out_of_range("ID de versión inválido");
        }
        Node* curr = version_roots[version_id];
        for (char ch : s) {
            if (curr == nullptr) return false;
            curr = curr->children[ch - 'a'];
        }
        return curr != nullptr && curr->is_terminal;
    }

    // Consulta si 'prefix' es prefijo de alguna palabra en la versión 'version_id'
    bool starts_with(int version_id, const std::string& prefix) const {
        if (version_id < 0 || version_id >= static_cast<int>(version_roots.size())) {
            throw std::out_of_range("ID de versión inválido");
        }
        Node* curr = version_roots[version_id];
        for (char ch : prefix) {
            if (curr == nullptr) return false;
            curr = curr->children[ch - 'a'];
        }
        return curr != nullptr;
    }
};
