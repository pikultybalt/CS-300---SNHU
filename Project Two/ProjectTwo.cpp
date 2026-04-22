#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

//==============================================================
// Project Two - Advising Assistance Program
// Author: Lori Barnett
// Course: CS 300
//
// Description:
// This program reads course data from a CSV file and stores it
// in a binary search tree. The user can load the data, print
// the full course list in alphanumeric order, and look up
// information for an individual course.
//==============================================================

// Structure used to store one course
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Node structure for the binary search tree
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(const Course& aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

//==============================================================
// Binary Search Tree class
//==============================================================
class BinarySearchTree {
private:
    Node* root;

    // Insert a node into the correct location in the tree
    void addNode(Node* node, const Course& course) {
        if (course.courseNumber < node->course.courseNumber) {
            if (node->left == nullptr) {
                node->left = new Node(course);
            }
            else {
                addNode(node->left, course);
            }
        }
        else {
            if (node->right == nullptr) {
                node->right = new Node(course);
            }
            else {
                addNode(node->right, course);
            }
        }
    }

    // In-order traversal prints courses in sorted order
    void inOrder(Node* node) const {
        if (node == nullptr) {
            return;
        }

        inOrder(node->left);
        cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
        inOrder(node->right);
    }

    // Search for a course by course number
    Course search(Node* node, const string& courseNumber) const {
        while (node != nullptr) {
            if (courseNumber == node->course.courseNumber) {
                return node->course;
            }

            if (courseNumber < node->course.courseNumber) {
                node = node->left;
            }
            else {
                node = node->right;
            }
        }

        return Course();
    }

    // Delete all nodes in the tree
    void destroyTree(Node* node) {
        if (node == nullptr) {
            return;
        }

        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    // Copy nodes from another tree
    void copyTree(Node* node) {
        if (node == nullptr) {
            return;
        }

        Insert(node->course);
        copyTree(node->left);
        copyTree(node->right);
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    BinarySearchTree(const BinarySearchTree& other) {
        root = nullptr;
        copyTree(other.root);
    }

    BinarySearchTree& operator=(const BinarySearchTree& other) {
        if (this != &other) {
            destroyTree(root);
            root = nullptr;
            copyTree(other.root);
        }
        return *this;
    }

    ~BinarySearchTree() {
        destroyTree(root);
        root = nullptr;
    }

    // Insert a course into the tree
    void Insert(const Course& course) {
        if (root == nullptr) {
            root = new Node(course);
        }
        else {
            addNode(root, course);
        }
    }

    // Search for a course in the tree
    Course Search(const string& courseNumber) const {
        return search(root, courseNumber);
    }

    // Print all courses in sorted order
    void PrintCourseList() const {
        inOrder(root);
    }

    // Check if the tree is empty
    bool IsEmpty() const {
        return root == nullptr;
    }
};

//==============================================================
// Helper Functions
//==============================================================

// Remove leading and trailing spaces
string trim(const string& text) {
    size_t start = 0;
    while (start < text.length() && isspace(static_cast<unsigned char>(text[start]))) {
        ++start;
    }

    size_t end = text.length();
    while (end > start && isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }

    return text.substr(start, end - start);
}

// Convert a string to uppercase
string toUpperCase(string text) {
    for (size_t i = 0; i < text.length(); ++i) {
        text[i] = static_cast<char>(toupper(static_cast<unsigned char>(text[i])));
    }
    return text;
}

// Split one CSV line into separate values
vector<string> splitLine(const string& line) {
    vector<string> tokens;
    string token;
    stringstream stream(line);

    while (getline(stream, token, ',')) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

//==============================================================
// Load course data from the file into the BST
//==============================================================
bool loadCoursesFromFile(const string& fileName, BinarySearchTree& courseTree) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: Could not open file." << endl;
        return false;
    }

    string line;

    while (getline(inputFile, line)) {
        if (trim(line).empty()) {
            continue;
        }

        vector<string> tokens = splitLine(line);

        // A valid line must contain at least a course number and title
        if (tokens.size() < 2 || tokens[0].empty() || tokens[1].empty()) {
            continue;
        }

        Course course;
        course.courseNumber = toUpperCase(tokens[0]);
        course.courseTitle = tokens[1];

        // Add remaining tokens as prerequisites
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                course.prerequisites.push_back(toUpperCase(tokens[i]));
            }
        }

        courseTree.Insert(course);
    }

    inputFile.close();
    return true;
}

//==============================================================
// Print one course and its prerequisites
//==============================================================
void printCourseInformation(const BinarySearchTree& courseTree, const string& userInput) {
    string courseNumber = toUpperCase(trim(userInput));
    Course course = courseTree.Search(courseNumber);

    if (course.courseNumber.empty()) {
        cout << "Course " << courseNumber << " not found." << endl;
        return;
    }

    cout << course.courseNumber << ", " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            cout << course.prerequisites[i];
            if (i < course.prerequisites.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}

//==============================================================
// Main function
//==============================================================
int main() {
    BinarySearchTree courseTree;
    bool dataLoaded = false;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << endl;
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "What would you like to do? ";

        cin >> choice;

        // Handle non-numeric input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "That is not a valid option." << endl;
            continue;
        }

        switch (choice) {
        case 1: {
            string fileName;
            cout << "Enter the file name: ";
            cin.ignore();
            getline(cin, fileName);

            BinarySearchTree newTree;
            if (loadCoursesFromFile(fileName, newTree)) {
                courseTree = newTree;
                dataLoaded = true;
            }
            break;
        }

        case 2:
            if (!dataLoaded || courseTree.IsEmpty()) {
                cout << "Please load the data structure first." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                courseTree.PrintCourseList();
            }
            break;

        case 3:
            if (!dataLoaded || courseTree.IsEmpty()) {
                cout << "Please load the data structure first." << endl;
            }
            else {
                string courseNumber;
                cout << "What course do you want to know about? ";
                cin >> courseNumber;
                printCourseInformation(courseTree, courseNumber);
            }
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}