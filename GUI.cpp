#include <wx/wx.h>
#include <wx/listctrl.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct Book {
    int id;
    std::string title;
    std::string author;
    bool isIssued;
};

class Library {
private:
    std::vector<Book> books;

    void saveBooksToFile() {
        std::ofstream file("books.txt", std::ios::trunc);
        for (const auto& book : books) {
            file << book.id << "," << book.title << "," << book.author << ","
                << (book.isIssued ? "Issued" : "Available") << "\n";
        }
    }

    void loadBooksFromFile() {
        std::ifstream file("books.txt");
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string idStr, title, author, status;
            std::getline(ss, idStr, ',');
            std::getline(ss, title, ',');
            std::getline(ss, author, ',');
            std::getline(ss, status);

            Book book = { std::stoi(idStr), title, author, status == "Issued" };
            books.push_back(book);
        }
    }

public:
	Library() { loadBooksFromFile(); }//this is constructor

    std::vector<Book> getBooks() const { return books; }

    void addBook(int id, const std::string& title, const std::string& author) {
        books.push_back({ id, title, author, false });
        saveBooksToFile();
    }

    void removeBook(int id) {
        books.erase(std::remove_if(books.begin(), books.end(),
            [id](const Book& book) { return book.id == id; }),
            books.end());
        saveBooksToFile();
    }

    void issueBook(int id) {
        for (auto& book : books) {
            if (book.id == id) {
                book.isIssued = true;
                break;
            }
        }
        saveBooksToFile();
    }

    void returnBook(int id) {
        for (auto& book : books) {
            if (book.id == id) {
                book.isIssued = false;
                break;
            }
        }
        saveBooksToFile();
    }
};

class LibraryApp : public wxApp {
public:
    virtual bool OnInit();
};

class MainFrame : public wxFrame {
private:
    Library library;
    wxListCtrl* bookList;
    wxCheckBox* showAvailableOnlyCheckBox;

    void PopulateBookList(const std::vector<Book>& books, bool showAvailableOnly) {
        std::vector<Book> filteredBooks = books;

        if (showAvailableOnly) {
            filteredBooks.erase(std::remove_if(filteredBooks.begin(), filteredBooks.end(), [](const Book& book) {
                return book.isIssued;
                }), filteredBooks.end());
        }

        bookList->DeleteAllItems();

        for (const auto& book : filteredBooks) {
            long index = bookList->InsertItem(bookList->GetItemCount(), std::to_string(book.id));
            bookList->SetItem(index, 1, book.title);
            bookList->SetItem(index, 2, book.author);
            bookList->SetItem(index, 3, book.isIssued ? "Issued" : "Available");
        }
    }

    void OnAddBook(wxCommandEvent& event) {
        wxTextEntryDialog idDialog(this, "Enter Book ID:");
        if (idDialog.ShowModal() == wxID_OK) {
            int id = std::stoi(idDialog.GetValue().ToStdString());

            const auto& books = library.getBooks();
            auto it = std::find_if(books.begin(), books.end(), [id](const Book& book) {
                return book.id == id;
                });

            if (it != books.end()) {
                wxMessageBox("A book with this ID already exists!", "Duplicate ID", wxICON_WARNING);
                return;
            }

            wxTextEntryDialog titleDialog(this, "Enter Book Title:");
            if (titleDialog.ShowModal() == wxID_OK) {
                std::string title = titleDialog.GetValue().ToStdString();
                wxTextEntryDialog authorDialog(this, "Enter Book Author:");
                if (authorDialog.ShowModal() == wxID_OK) {
                    std::string author = authorDialog.GetValue().ToStdString();
                    library.addBook(id, title, author);
                    PopulateBookList(library.getBooks(), showAvailableOnlyCheckBox->IsChecked());
                    wxMessageBox("Book added successfully!", "Success");
                }
            }
        }
    }

    void OnRemoveBook(wxCommandEvent& event) {
        wxTextEntryDialog idDialog(this, "Enter Book ID to Remove:");
        if (idDialog.ShowModal() == wxID_OK) {
            int id = std::stoi(idDialog.GetValue().ToStdString());

            // Check if the book with the entered ID exists in the list
            const auto& books = library.getBooks();
            auto it = std::find_if(books.begin(), books.end(), [id](const Book& book) {
                return book.id == id;
                });

            if (it != books.end()) {
                // If the book exists, remove it
                library.removeBook(id);
                PopulateBookList(library.getBooks(), showAvailableOnlyCheckBox->IsChecked());
                wxMessageBox("Book removed successfully!", "Success");
            }
            else {
                // If the book does not exist, show an error message
                wxMessageBox("No book found with this ID!", "Error", wxICON_ERROR);
            }
        }
    }


    void OnIssueBook(wxCommandEvent& event) {
        wxTextEntryDialog idDialog(this, "Enter Book ID to Issue:");
        if (idDialog.ShowModal() == wxID_OK) {
            int id = std::stoi(idDialog.GetValue().ToStdString());

            const auto& books = library.getBooks();
            auto it = std::find_if(books.begin(), books.end(), [id](const Book& book) {
                return book.id == id;
                });

            if (it != books.end()) {
                if (it->isIssued) {
                    wxMessageBox("The book is already issued!", "Book Not Available", wxICON_WARNING);
                }
                else {
                    library.issueBook(id);
                    PopulateBookList(library.getBooks(), showAvailableOnlyCheckBox->IsChecked());
                    wxMessageBox("Book issued successfully!", "Success");
                }
            }
            else {
                wxMessageBox("Book ID not found!", "Error", wxICON_ERROR);
            }
        }
    }

    void OnReturnBook(wxCommandEvent& event) {
        wxTextEntryDialog idDialog(this, "Enter Book ID to Return:");
        if (idDialog.ShowModal() == wxID_OK) {
            int id = std::stoi(idDialog.GetValue().ToStdString());
            library.returnBook(id);
            PopulateBookList(library.getBooks(), showAvailableOnlyCheckBox->IsChecked());
            wxMessageBox("Book returned successfully!", "Success");
        }
    }

    void OnToggleShowAvailableOnly(wxCommandEvent& event) {
        PopulateBookList(library.getBooks(), showAvailableOnlyCheckBox->IsChecked());
    }

public:
    MainFrame(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(600, 400)) {
        wxPanel* panel = new wxPanel(this, wxID_ANY);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        bookList = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxLC_REPORT | wxLC_SINGLE_SEL);
        bookList->InsertColumn(0, "ID", wxLIST_FORMAT_LEFT, 50);
        bookList->InsertColumn(1, "Title", wxLIST_FORMAT_LEFT, 200);
        bookList->InsertColumn(2, "Author", wxLIST_FORMAT_LEFT, 150);
        bookList->InsertColumn(3, "Status", wxLIST_FORMAT_LEFT, 100);
        vbox->Add(bookList, 1, wxEXPAND | wxALL, 10);

        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

        wxButton* addBookBtn = new wxButton(panel, wxID_ANY, "Add Book");
        wxButton* removeBookBtn = new wxButton(panel, wxID_ANY, "Remove Book");
        wxButton* issueBookBtn = new wxButton(panel, wxID_ANY, "Issue Book");
        wxButton* returnBookBtn = new wxButton(panel, wxID_ANY, "Return Book");

        hbox->Add(addBookBtn, 1, wxALL, 5);
        hbox->Add(removeBookBtn, 1, wxALL, 5);
        hbox->Add(issueBookBtn, 1, wxALL, 5);
        hbox->Add(returnBookBtn, 1, wxALL, 5);

        vbox->Add(hbox, 0, wxALIGN_CENTER);

        showAvailableOnlyCheckBox = new wxCheckBox(panel, wxID_ANY, "Show Available Only");
        vbox->Add(showAvailableOnlyCheckBox, 0, wxALIGN_CENTER | wxALL, 5);

        panel->SetSizer(vbox);

        PopulateBookList(library.getBooks(), false);

        addBookBtn->Bind(wxEVT_BUTTON, &MainFrame::OnAddBook, this);
        removeBookBtn->Bind(wxEVT_BUTTON, &MainFrame::OnRemoveBook, this);
        issueBookBtn->Bind(wxEVT_BUTTON, &MainFrame::OnIssueBook, this);
        returnBookBtn->Bind(wxEVT_BUTTON, &MainFrame::OnReturnBook, this);
        showAvailableOnlyCheckBox->Bind(wxEVT_CHECKBOX, &MainFrame::OnToggleShowAvailableOnly, this);
    }
};

wxIMPLEMENT_APP(LibraryApp);

bool LibraryApp::OnInit() {
    MainFrame* frame = new MainFrame("Library Management System");
    frame->Show(true);
    return true;
}
