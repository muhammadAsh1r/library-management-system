#include <fstream>
#include <sstream>

void saveBooks(Book* node, ofstream& file) {
    if (node != nullptr) {
        saveBooks(node->left, file);
        file << node->id << "," << node->title << "," << node->author << ","
             << (node->isIssued ? "Issued" : "Available") << endl;
        saveBooks(node->right, file);
    }
}

void Library::saveBooksToFile() {
    ofstream file("books.txt", ios::trunc);
    saveBooks(root, file);
    file.close();
}

void Library::loadBooks() {
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
