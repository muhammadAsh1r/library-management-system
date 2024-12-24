#include <iostream>
#include <string>
using namespace std;

struct Book {
    int id;
    string title;
    string author;
    bool isIssued;
    Book* left;
    Book* right;

    Book(int id, string title, string author, bool isIssued)
        : id(id), title(title), author(author), isIssued(isIssued), left(nullptr), right(nullptr) {}
};

class Library {
private:
    Book* root;

    Book* insert(Book* node, int id, string title, string author, bool isIssued) {
        if (node == nullptr)
            return new Book(id, title, author, isIssued);
        if (id < node->id)
            node->left = insert(node->left, id, title, author, isIssued);
        else if (id > node->id)
            node->right = insert(node->right, id, title, author, isIssued);
        return node;
    }

public:
    Library() : root(nullptr) {}

    void addBook(int id, string title, string author, bool isIssued) {
        root = insert(root, id, title, author, isIssued);
        cout << "Book added successfully.\n";
    }
};

int main() {
    Library library;
    return 0;
}
