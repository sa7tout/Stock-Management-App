#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QStyle>
#include <QtSql>
#include <QDebug>
#include <QtWidgets>
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QSqlDatabase>
#include <QSqlQuery>

class DatabaseManager {
public:
    DatabaseManager() {
        connection = QSqlDatabase::addDatabase("QSQLITE");
        connection.setDatabaseName("C:/Users/sa7toute/Desktop/CPPproject/cppproject/sqlite/users.db");
        if (!connection.open()) {
            qDebug() << "Failed to open database!";
        } else {
            qDebug() << "Connected to the database!";
        }
    }

    void closeConnection() {
        connection.close();
    }

    QVariantMap getUserByCredentials(const QString& username, const QString& password) {
        QSqlQuery sqlQuery;
        sqlQuery.prepare("SELECT * FROM user_credentials WHERE username = :username AND password = :password");
        sqlQuery.bindValue(":username", username);
        sqlQuery.bindValue(":password", password);

        if (sqlQuery.exec()) {
            qDebug() << "Query executed successfully";

            if (sqlQuery.next()) {
                QVariantMap result;
                result["id"] = sqlQuery.value("id").toInt();
                result["username"] = sqlQuery.value("username").toString();
                result["password"] = sqlQuery.value("password").toString();

                // Print the retrieved values
                qDebug() << "Retrieved data:";
                qDebug() << "ID: " << result["id"].toInt();
                qDebug() << "Username: " << result["username"].toString();
                qDebug() << "Password: " << result["password"].toString();

                return result;
            }
        } else {
            qDebug() << "Query execution failed: " << sqlQuery.lastError().text();
        }

        return QVariantMap();
    }

private:
    QSqlDatabase connection;
};

class InventoryWindow : public QWidget
{

public:
    InventoryWindow(QWidget *parent = nullptr) : QWidget(parent)
    {
        // Create UI elements
        productTableView = new QTableView(this);
        addButton = new QPushButton("Add Product", this);
        deleteButton = new QPushButton("Delete Product", this);

        // Set up layout
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addWidget(productTableView);
        mainLayout->addWidget(addButton);
        mainLayout->addWidget(deleteButton);

        addButton->setFixedWidth(100);
        deleteButton->setFixedWidth(100);

        QFont font = addButton->font();
        font.setBold(true);
        addButton->setFont(font);
        deleteButton->setFont(font);

        mainLayout->setAlignment(addButton, Qt::AlignCenter);
        mainLayout->setAlignment(deleteButton, Qt::AlignCenter);

        setLayout(mainLayout);

        // Set the size of the table view

        resize(1100, 650);

        // Connect signals and slots
        connect(addButton, &QPushButton::clicked, this, &InventoryWindow::addProduct);
        connect(deleteButton, &QPushButton::clicked, this, &InventoryWindow::deleteProduct);

        // Initialize database connection
        QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("C:/Users/sa7toute/Desktop/CPPproject/cppproject/sqlite/users.db");
        if (!database.open()) {
            qDebug() << "Failed to connect to the database.";
            return;
        }
        else{
            qDebug() << "Connected to the database!";
        }

        loadProducts();
    }

private slots:
    void addProduct()
    {
        bool ok;

        QString productName = QInputDialog::getText(this, "Add Product", "Enter product name:", QLineEdit::Normal, "", &ok);
        if (!ok || productName.isEmpty()) {
            return;
        }

        QString productCategory = QInputDialog::getText(this, "Add Product", "Enter product category:", QLineEdit::Normal, "", &ok);
        if (!ok || productCategory.isEmpty()) {
            return;
        }

        QString productPrice = QInputDialog::getText(this, "Add Product", "Enter product price:", QLineEdit::Normal, "", &ok);
        if (!ok || productPrice.isEmpty()) {
            return;
        }

        int productQuantity = QInputDialog::getInt(this, "Add Product", "Enter product quantity:", 0, 0, 1000000, 1, &ok);
        if (!ok) {
            return;
        }

        QSqlQuery query;
        query.prepare("INSERT INTO products (name, category, price, quantity) VALUES (:name, :category, :price, :quantity)");
        query.bindValue(":name", productName);
        query.bindValue(":category", productCategory);
        query.bindValue(":price", productPrice);
        query.bindValue(":quantity", productQuantity);

        if (query.exec()) {
            loadProducts(); // Refresh the product list
        } else {
            qDebug() << "Failed to add product to the database.";
        }
    }

    void deleteProduct()
    {
        QModelIndexList selectedIndexes = productTableView->selectionModel()->selectedIndexes();
        if (!selectedIndexes.isEmpty()) {
            int row = selectedIndexes.first().row();
            int productId = productTableModel->index(row, 0).data(Qt::UserRole).toInt();

            QSqlQuery query;
            query.prepare("DELETE FROM products WHERE id = :id");
            query.bindValue(":id", productId);

            if (query.exec()) {
                loadProducts();
            } else {
                qDebug() << "Failed to delete product from the database.";
            }
        }
    }

private:
    void loadProducts()
    {
        productTableModel = new QStandardItemModel(this);
        productTableModel->setColumnCount(5);
        productTableModel->setHeaderData(0, Qt::Horizontal, "ID");
        productTableModel->setHeaderData(1, Qt::Horizontal, "Name");
        productTableModel->setHeaderData(2, Qt::Horizontal, "Category");
        productTableModel->setHeaderData(3, Qt::Horizontal, "Price");
        productTableModel->setHeaderData(4, Qt::Horizontal, "Quantity");

        QSqlQuery query("SELECT id, name, category, price, quantity FROM products");
        int row = 0;
        while (query.next()) {
            int productId = query.value("id").toInt();
            QString productName = query.value("name").toString();
            QString productCategory = query.value("category").toString();
            QString productPrice = query.value("price").toString();
            int productQuantity = query.value("quantity").toInt();

            productTableModel->setItem(row, 0, new QStandardItem(QString::number(productId)));
            productTableModel->setItem(row, 1, new QStandardItem(productName));
            productTableModel->setItem(row, 2, new QStandardItem(productCategory));
            productTableModel->setItem(row, 3, new QStandardItem((productPrice)));
            productTableModel->setItem(row, 4, new QStandardItem(QString::number(productQuantity)));

            row++;
        }

        productTableView->setModel(productTableModel);
        productTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        productTableView->setSelectionMode(QAbstractItemView::SingleSelection);
        productTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        productTableView->verticalHeader()->setVisible(false);
        productTableView->setFont(QFont("Arial", 12, QFont::Bold));
        productTableView->resizeRowsToContents();


        productTableView->setFixedSize(800, 500);

        QBoxLayout* mainLayout = qobject_cast<QBoxLayout*>(layout());
        if (mainLayout) {
            mainLayout->setAlignment(productTableView, Qt::AlignCenter);
        }
    }



    QTableView *productTableView;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QStandardItemModel *productTableModel;

};

class LoginWindow : public QWidget {

public:
    LoginWindow(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("Login Window");

        // Configure the style for the black theme
        QStyle* style = qApp->style();
        QFont font("TkDefaultFont", 17, QFont::Bold);
        QPalette palette;
        palette.setColor(QPalette::WindowText, Qt::gray);
        palette.setColor(QPalette::Base, QColor("25212b"));
        palette.setColor(QPalette::Text, Qt::black);
        palette.setColor(QPalette::Button, QColor("25212b"));
        palette.setColor(QPalette::ButtonText, QColor("e4deed"));

        style->unpolish(this);
        style->polish(this);
        setPalette(palette);

        QLabel* titleLabel = new QLabel("LOGIN", this);
        titleLabel->setFont(QFont("TkDefaultFont", 50, QFont::Bold));
        titleLabel->setObjectName("titleLabel");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setGeometry(10, 40, width() - 20, 80);

        QLabel* usernameLabel = new QLabel("Username:", this);
        usernameLabel->setFont(font);
        usernameLabel->setGeometry(10, 130, width() - 20, 30);

        usernameEntry = new QLineEdit(this);
        usernameEntry->setFont(QFont("bold", 14));
        usernameEntry->setGeometry(30, 170, width() - 60, 30);

        QLabel* passwordLabel = new QLabel("Password:", this);
        passwordLabel->setFont(font);
        passwordLabel->setGeometry(10, 220, width() - 20, 30);

        passwordEntry = new QLineEdit(this);
        passwordEntry->setEchoMode(QLineEdit::Password);
        passwordEntry->setFont(QFont("bold", 14));
        passwordEntry->setGeometry(30, 260, width() - 60, 30);

        QPushButton* loginButton = new QPushButton("ENTER", this);
        loginButton->setFont(QFont("bold", 14));
        loginButton->setGeometry(250, 310, width() - 500, 30);

        connect(loginButton, &QPushButton::clicked, this, &LoginWindow::login);

        dbManager = new DatabaseManager();
    }

    virtual ~LoginWindow() {
        delete dbManager;
    }

private slots:
    void login() {
        QString username = usernameEntry->text();
        QString password = passwordEntry->text();

        QVariantMap user = dbManager->getUserByCredentials(username, password);

        if (!user.isEmpty()) {
            qDebug() << "Login successful";
            close();
            InventoryWindow* inventoryWindow = new InventoryWindow;
            inventoryWindow->setWindowTitle("Stock Inventory");
            inventoryWindow->show();
        } else {
            QMessageBox::critical(this, "Login Error", "Invalid credentials");
            usernameEntry->clear();
            passwordEntry->clear();
        }
    }

private:
    QLineEdit* usernameEntry;
    QLineEdit* passwordEntry;
    DatabaseManager* dbManager;
};


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    LoginWindow loginWindow;
    loginWindow.show();

    return app.exec();
}

