#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <iostream>

template <typename T>
struct Node
{
    T data;
    Node<T> *next;
};

template <typename T>
class LinkedList
{
public:
    LinkedList();
    LinkedList(const LinkedList<T> &);
    LinkedList<T> &operator=(const LinkedList<T> &);
    ~LinkedList();

    bool empty() const;
    unsigned size() const;
    friend std::ostream &operator<<(std::ostream &out, const LinkedList<T> &list)
    {
        Node<T> *current = list.first;
        while (current != nullptr)
        {
            out << current->data << " ";
            current = current->next;
        }
        return out;
    }
    T &operator[](size_t index);
    const T &operator[](size_t index) const;
    LinkedList<T> &insert_first(const T &);
    LinkedList<T> &insert_last(const T &);
    void remove_first();
    void remove_last();

private:
    Node<T> *first;
    unsigned length;

    using type_of = T;

    void free();
    void copyfrom(const LinkedList<T> &);
};

#endif

template <typename T>
void LinkedList<T>::free()
{
    Node<T> *crr = first;
    Node<T> *save = first;
    while (crr != nullptr)
    {
        save = crr->next;
        delete crr;
        crr = save;
    }
    length = 0;
    first = nullptr;
}

template <typename T>
void LinkedList<T>::copyfrom(const LinkedList<T> &original)
{
    first = nullptr;
    if (original.first == nullptr)
    {
        length = 0;
        return;
    }

    length = original.length;
    first = new Node<T>{original.first->data, nullptr};

    Node<T> *it = original.first->next;
    Node<T> *crr = first;
    while (it != nullptr)
    {
        crr->next = new Node<T>{it->data, nullptr};
        crr = crr->next;
        it = it->next;
    }
}

template <typename T>
LinkedList<T>::LinkedList() : first(nullptr), length(0)
{
}

template <typename T>
LinkedList<T>::LinkedList(const LinkedList<T> &original) : first(nullptr), length(0)
{
    copyfrom(original);
}

template <typename T>
LinkedList<T> &LinkedList<T>::operator=(const LinkedList<T> &rhs)
{
    if (this != &rhs)
    {
        free();
        copyfrom(rhs);
    }
    return *this;
}

template <typename T>
LinkedList<T>::~LinkedList()
{
    free();
}

template <typename T>
bool LinkedList<T>::empty() const
{
    return length == 0;
}

template <typename T>
unsigned LinkedList<T>::size() const
{
    return length;
}

template <typename T>
T &LinkedList<T>::operator[](size_t index)
{
    size_t i = 0;
    Node<T> *current = first;
    while (current != nullptr && i < index)
    {
        current = current->next;
        ++i;
    }

    if (current == nullptr)
        throw "Index out of bonds.";

    return current->data;
}

template <typename T>
const T &LinkedList<T>::operator[](size_t index) const
{
    size_t i = 0;
    Node<T> *current = first;
    while (current != nullptr && i < index)
    {
        current = current->next;
        ++i;
    }

    if (current == nullptr)
        throw "Index out of bonds.";

    return current->data;
}

template <typename T>
LinkedList<T> &LinkedList<T>::insert_first(const T &element)
{
    first = new Node<T>{element, first};
    ++length;

    return *this;
}

template <typename T>
LinkedList<T> &LinkedList<T>::insert_last(const T &element)
{
    if (first == nullptr)
        return insert_first(element);

    Node<T> *current = first;
    while (current->next != nullptr)
    {
        current = current->next;
    }
    current->next = new Node<T>{element, nullptr};
    ++length;

    return *this;
}

template <typename T>
void LinkedList<T>::remove_first()
{
    if (length == 0)
        return;

    Node<T> *save = first->next;
    delete first;
    first = save;
    --length;
}

template <typename T>
void LinkedList<T>::remove_last()
{
    if (length == 0)
        return;

    if (length == 1)
    {
        delete first;
        first = nullptr;
        length = 0;
        return;
    }

    Node<T> *current = first;
    while (current->next->next != nullptr)
    {
        current = current->next;
    }

    delete current->next;
    current->next = nullptr;
    --length;
}