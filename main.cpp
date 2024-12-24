#include <iostream>
#include <fstream>
#include <sstream>
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

    void inOrder(Book* node, bool filterAvailable = false, bool filterIssued = false) {
        if (node != nullptr) {
            inOrder(node->left, filterAvailable, filterIssued);

            if ((filterAvailable && !node->isIssued) || (filterIssued && node->isIssued) || (!filterAvailable && !filterIssued)) {
                cout << "ID: " << node->id << ", Title: " << node->title
                     << ", Author: " << node->author
                     << ", Status: " << (node->isIssued ? "Issued" : "Available") << endl;
            }

            inOrder(node->right, filterAvailable, filterIssued);
        }
    }

    Book* deleteBook(Book* node, int id) {
        if (node == nullptr)
            return nullptr;
        if (id < node->id)
            node->left = deleteBook(node->left, id);
        else if (id > node->id)
            node->right = deleteBook(node->right, id);
        else {
            if (node->left == nullptr) {
                Book* temp = node->right;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                Book* temp = node->left;
                delete node;
                return temp;
            } else {
                Book* successor = node->right;
                while (successor->left != nullptr)
                    successor = successor->left;
                node->id = successor->id;
                node->title = successor->title;
                node->author = successor->author;
                node->isIssued = successor->isIssued;
                node->right = deleteBook(node->right, successor->id);
            }
        }
        return node;
    }

    Book* search(Book* node, int id) {
        if (node == nullptr || node->id == id)
            return node;
        if (id < node->id)
            return search(node->left, id);
        return search(node->right, id);
    }

    void saveBooks(Book* node, ofstream& file) {
        if (node != nullptr) {
            saveBooks(node->left, file);
            file << node->id << "," << node->title << "," << node->author << ","
                 << (node->isIssued ? "Issued" : "Available") << endl;
            saveBooks(node->right, file);
        }
    }

public:
    Library() : root(nullptr) {}

    void loadBooks() {
        ifstream file("books.txt");
        if (!file) {
            cout << "Error: books.txt not found.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            int id;
            string title, author, status;
            getline(ss, line, ',');
            id = stoi(line);
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, status);
            bool isIssued = (status == "Issued");
            root = insert(root, id, title, author, isIssued);
        }
        file.close();
    }

    void saveBooksToFile() {
        ofstream file("books.txt", ios::trunc);
        saveBooks(root, file);
        file.close();
    }

    void addBook(int id, string title, string author, bool isIssued) {
        root = insert(root, id, title, author, isIssued);
        saveBooksToFile();
        cout << "Book added successfully.\n";
    }

    void removeBook(int id) {
        root = deleteBook(root, id);
        saveBooksToFile();
        cout << "Book removed successfully.\n";
    }

    void displayBooks(bool filterAvailable = false, bool filterIssued = false) {
        cout << "Books List:\n";
        inOrder(root, filterAvailable, filterIssued);
    }

    void issueBook(int id) {
        Book* book = search(root, id);
        if (book && !book->isIssued) {
            book->isIssued = true;
            saveBooksToFile();
            cout << "Book issued successfully.\n";
        } else {
            cout << "Book not available for issue.\n";
        }
    }

    void returnBook(int id) {
        Book* book = search(root, id);
        if (book && book->isIssued) {
            book->isIssued = false;
            saveBooksToFile();
            cout << "Book returned successfully.\n";
        } else {
            cout << "Invalid return request.\n";
        }
    }
};

int main() {
    Library library;

    // Load books from the file
    library.loadBooks();

    int roleChoice;
    cout << "Welcome to Library Management System\n";
    cout << "1. Admin\n2. User\n";
    cout << "Enter your role: ";
    cin >> roleChoice;

    if (roleChoice == 1) { // Admin Menu
        int adminChoice;
        while (true) {
            cout << "\nAdmin Menu:\n";
            cout << "1. Add Book\n2. Remove Book\n3. Display All Books\n4. Exit\n";
            cout << "Enter your choice: ";
            cin >> adminChoice;

            if (adminChoice == 1) {
                int id;
                string title, author;
                cout << "Enter book ID: ";
                cin >> id;
                cin.ignore();
                cout << "Enter book title: ";
                getline(cin, title);
                cout << "Enter book author: ";
                getline(cin, author);
                library.addBook(id, title, author, false);
            } else if (adminChoice == 2) {
                int id;
                cout << "Enter book ID to remove: ";
                cin >> id;
                library.removeBook(id);
            } else if (adminChoice == 3) {
                library.displayBooks();
            } else if (adminChoice == 4) {
                cout << "Exiting Admin Menu...\n";
                break;
            } else {
                cout << "Invalid choice. Try again.\n";
            }
        }
    } else if (roleChoice == 2) { // User Menu
        int userChoice;
        while (true) {
            cout << "\nUser Menu:\n";
            cout << "1. Display All Books\n2. Filter by Available Books\n3. Filter by Issued Books\n4. Issue Book\n5. Return Book\n6. Exit\n";
            cout << "Enter your choice: ";
            cin >> userChoice;

            if (userChoice == 1) {
                library.displayBooks();
            } else if (userChoice == 2) {
                library.displayBooks(true, false); // Only available books
            } else if (userChoice == 3) {
                library.displayBooks(false, true); // Only issued books
            } else if (userChoice == 4) {
                int id;
                cout << "Enter book ID to issue: ";
                cin >> id;
                library.issueBook(id);
            } else if (userChoice == 5) {
                int id;
                cout << "Enter book ID to return: ";
                cin >> id;
                library.returnBook(id);
            } else if (userChoice == 6) {
                cout << "Exiting User Menu...\n";
                break;
            } else {
                cout << "Invalid choice. Try again.\n";
            }
        }
    } else {
        cout << "Invalid role selection.\n";
    }

    return 0;
}
