#include <iostream>
#include <vector>
#include <stdexcept>

template <typename T>
class PersistentStack {
public:
    // Nodo inmutable según el modelo de máquina de punteros
    struct Node {
        T data;
        Node* next;
        Node(const T& val, Node* nxt) : data(val), next(nxt) {}
    };

private:
    // Guarda el puntero raíz (nodo tope) de cada versión creada
    std::vector<Node*> version_roots;

public:
    // La versión 0 representa el stack vacío
    PersistentStack() {
        version_roots.push_back(nullptr);
    }

    // PUSH persistente: deriva una nueva versión a partir de 'version_id'
    // Retorna el ID de la nueva versión creada
    int push(int version_id, const T& value) {
        if (version_id < 0 || version_id >= static_cast<int>(version_roots.size())) {
            throw std::out_of_range("ID de versión inválido");
        }
        // Creamos un nodo nuevo apuntando a la raíz de la versión base (O(1))
        version_roots.emplace_back(new Node(value, version_roots[version_id]));
        return static_cast<int>(version_roots.size()) - 1;
    }

    // POP persistente: deriva una nueva versión cuyo tope es el nodo siguiente
    // Retorna el ID de la nueva versión creada
    int pop(int version_id) {
        if (version_id < 0 || version_id >= static_cast<int>(version_roots.size())) {
            throw std::out_of_range("ID de versión inválido");
        }
        Node* current_top = version_roots[version_id];
        if (current_top == nullptr) {
            // El stack ya está vacío en esa versión; la nueva versión sigue vacía
            version_roots.push_back(nullptr);
        } else {
            // La nueva versión comparte la estructura apuntando a current_top->next
            version_roots.push_back(current_top->next);
        }
        return static_cast<int>(version_roots.size()) - 1;
    }

    // Consulta el elemento superior en una versión específica
    const T& top(int version_id) const {
        if (version_id < 0 || version_id >= static_cast<int>(version_roots.size())) {
            throw std::out_of_range("ID de versión inválido");
        }
        Node* current_top = version_roots[version_id];
        if (current_top == nullptr) {
            throw std::underflow_error("El stack está vacío en esta versión");
        }
        return current_top->data;
    }

    // Comprueba si la versión especificada está vacía
    bool empty(int version_id) const {
        if (version_id < 0 || version_id >= static_cast<int>(version_roots.size())) {
            throw std::out_of_range("ID de versión inválido");
        }
        return version_roots[version_id] == nullptr;
    }

    // Retorna el total de versiones registradas
    int get_version_count() const {
        return static_cast<int>(version_roots.size());
    }
};
