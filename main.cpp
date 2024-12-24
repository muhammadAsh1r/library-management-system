Book* search(Book* node, int id) {
    if (node == nullptr || node->id == id)
        return node;
    if (id < node->id)
        return search(node->left, id);
    return search(node->right, id);
}

void Library::issueBook(int id) {
    Book* book = search(root, id);
    if (book && !book->isIssued) {
        book->isIssued = true;
        saveBooksToFile();
        cout << "Book issued successfully.\n";
    } else {
        cout << "Book not available for issue.\n";
    }
}

void Library::returnBook(int id) {
    Book* book = search(root, id);
    if (book && book->isIssued) {
        book->isIssued = false;
        saveBooksToFile();
        cout << "Book returned successfully.\n";
    } else {
        cout << "Invalid return request.\n";
    }
}
