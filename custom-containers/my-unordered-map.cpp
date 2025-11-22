#include <iostream>

template <typename K, typename V>
class MyUnorderedMap
{
private:
    struct Node
    {
        K key;
        V value;
        Node *next;
        Node(const K &k, const V &v) : key(k), value(v), next(nullptr) {}
    };

    static const size_t TABLE_SIZE = 101;
    Node **table;

    size_t hash(const K &key) const
    {
        return std::hash<K>()(key) % TABLE_SIZE;
    }

public:
    MyUnorderedMap()
    {
        table = new Node *[TABLE_SIZE]();
    }
    ~MyUnorderedMap()
    {
        for (size_t i = 0; i < TABLE_SIZE; ++i)
        {
            Node *current = table[i];
            while (current)
            {
                Node *toDelete = current;
                current = current->next;
                delete toDelete;
            }
        }
        delete[] table;
    }
    void insert(const K &key, const V &value)
    {
        size_t index = hash(key);
        Node *newNode = new Node(key, value);
        newNode->next = table[index];
        table[index] = newNode;
    }
    bool find(const K &key, V &value) const
    {
        size_t index = hash(key);
        Node *current = table[index];
        while (current)
        {
            if (current->key == key)
            {
                value = current->value;
                return true;
            }
            current = current->next;
        }
        return false;
    }
    void erase(const K &key)
    {
        size_t index = hash(key);
        Node *current = table[index];
        Node *prev = nullptr;
        while (current)
        {
            if (current->key == key)
            {
                if (prev)
                {
                    prev->next = current->next;
                }
                else
                {
                    table[index] = current->next;
                }
                delete current;
                return;
            }
            prev = current;
            current = current->next;
        }
    }
    void display() const
    {
        for (size_t i = 0; i < TABLE_SIZE; ++i)
        {
            Node *current = table[i];
            if (current)
            {
                std::cout << "Bucket " << i << ": ";
                while (current)
                {
                    std::cout << "{" << current->key << ": " << current->value << "} -> ";
                    current = current->next;
                }
                std::cout << "nullptr\n";
            }
        }
    }
};

int main()
{
    MyUnorderedMap<std::string, int> myMap;
    myMap.insert("apple", 1);
    myMap.insert("banana", 2);
    myMap.insert("orange", 3);

    myMap.display();

    int value;
    if (myMap.find("banana", value))
    {
        std::cout << "Found banana: " << value << "\n";
    }
    else
    {
        std::cout << "Banana not found\n";
    }

    myMap.erase("banana");
    myMap.display();

    return 0;
}