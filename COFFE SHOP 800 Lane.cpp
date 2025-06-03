#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <map>
#include <fstream>
#include <ctime>
#include <sstream>
using namespace std;

struct MenuItem {
    int id;
    string name;
    double price;
    int stock;
};

struct OrderItem {
    MenuItem item;
    int quantity;
};

struct BahanBaku {
    string nama;
    int satuanGram;
    double hargaTotal;
    int jumlah;
    double hargaPerGram;
};

struct Transaction {
    vector<OrderItem> items;
    double total;
    double paid;
    double change;
    string paymentMethod;
    time_t timestamp;
    string costumerName;
};

class CoffeeShop {
private:
    vector<MenuItem> menu;
    vector<BahanBaku> stokBahan;
    vector<Transaction> transactions;
    int nextMenuId;
    const double DISCOUNT_THRESHOLD;
    const double DISCOUNT_RATE;

public:
    CoffeeShop()
        : nextMenuId(1),
          DISCOUNT_THRESHOLD(45000.0),
          DISCOUNT_RATE(0.1)
    {
        loadSampleMenu();
        loadSampleBahan();
    }

    void addMenuItem(const string& name, double price, int stock) {
        MenuItem item;
        item.id = nextMenuId++;
        item.name = name;
        item.price = price;
        item.stock = stock;
        menu.push_back(item);
    }

    void displayMenu() {
    cout << "\n+===============================================+\n";
    cout << "|               ~ DAFTAR MENU ~                 |\n";
    cout << "+===============================================+\n";
    cout << left << setw(5) << "ID"
         << setw(25) << "Nama"
         << setw(12) << "Harga"
         << setw(10) << "Stok" << endl;
    cout << "-------------------------------------------------\n";

    for (size_t i = 0; i < menu.size(); ++i) {
        cout << left << setw(5) << menu[i].id
             << setw(25) << menu[i].name
             << "Rp " << setw(9) << fixed << setprecision(0) << menu[i].price
             << setw(10) << menu[i].stock << endl;
    }

    cout << "=================================================\n";
}


    void takeOrder() {
    string namaCustomer;
    cout << "Masukkan nama customer: ";
    cin.ignore();
    getline(cin, namaCustomer);
    vector<OrderItem> orderList;
    double total = 0;
    int id, qty;
    char more;

    do {
        displayMenu();
        cout << "\nMasukkan ID menu: ";
        cin >> id;
        cout << "Jumlah: ";
        cin >> qty;

        bool itemFound = false;
        for (size_t i = 0; i < menu.size(); ++i) {
            if (menu[i].id == id) {
                if (menu[i].stock >= qty) {
                    OrderItem order;
                    order.item = menu[i];
                    order.quantity = qty;
                    orderList.push_back(order);
                    total += menu[i].price * qty;
                    menu[i].stock -= qty; // Kurangi stok
                    itemFound = true;
                } else {
                    cout << "Stok tidak mencukupi untuk item \"" << menu[i].name << "\". Silakan pilih jumlah yang lebih sedikit.\n";
                }
                break;
            }
        }

        if (!itemFound) {
            cout << "Item dengan ID tersebut tidak ditemukan.\n";
        }

        cout << "Tambah item lain? (y/n): ";
        cin >> more;
    } while (more == 'y' || more == 'Y');

    // Konfirmasi pesanan
    char confirm;
    cout << "\n============ RINGKASAN PESANAN ================\n";
    cout << left << setw(25) << "Item" << setw(10) << "Qty" << setw(15) << "Subtotal" << endl;
    cout << "-----------------------------------------------\n";

    for (size_t i = 0; i < orderList.size(); ++i) {
        double subtotal = orderList[i].item.price * orderList[i].quantity;
        cout << left << setw(25) << orderList[i].item.name
             << setw(10) << orderList[i].quantity
             << "Rp " << fixed << setprecision(2) << subtotal << endl;
    }

    cout << "-----------------------------------------------\n";
    cout << left << setw(35) << "Total" << "Rp " << fixed << setprecision(2) << total << endl;
    cout << "Apakah Anda ingin mengkonfirmasi pesanan ini? (y/n): ";
    cin >> confirm;

    if (confirm == 'n' || confirm == 'N') {
        cout << "Pesanan dibatalkan. Anda dapat mengedit pesanan.\n";
        orderList.clear(); // Menghapus pesanan yang ada
        takeOrder(); // Memanggil kembali fungsi takeOrder untuk mengedit pesanan
        return; // Kembali setelah mengedit
    }

    if (total >= DISCOUNT_THRESHOLD) {
        double discount = total * DISCOUNT_RATE;
        cout << "\nDiskon 10% diterapkan: -Rp " << discount << endl;
        total -= discount;
    }

    cout << "Total belanja: Rp " << total << endl;

    string metode;
    cout << "Metode pembayaran (Tunai/QRIS/ATM/E-Wallet): ";
    cin >> metode;

    if (metode != "Tunai" && metode != "tunai") {
        cout << "Biaya admin Rp 500 diterapkan untuk metode pembayaran ini.\n";
        total += 500;
    }

    double bayar;
    do {
        cout << "Masukkan nominal pembayaran: ";
        cin >> bayar;

        if (bayar < total) {
            cout << "Uang anda tidak cukup, silakan masukkan nominal yang sesuai.\n";
        }
    } while (bayar < total);

    double kembali = bayar - total;
    cout << "Kembalian: Rp " << kembali << endl;

    Transaction trx = {orderList, total, bayar, kembali, metode, time(0),namaCustomer};
    transactions.push_back(trx);

    saveTransaction(trx);
    #ifdef _WIN32
    system("cls");
	#else
    system("clear");
	#endif
    printReceipt(trx);
    cout << "\nTekan Enter untuk kembali ke menu utama...";
	cin.ignore();
	cin.get();
}


    void saveTransaction(const Transaction& trx) {
        ofstream file("transaksi.txt", ios::app);
        file << "Waktu: " << ctime(&trx.timestamp);
        for (size_t i = 0; i < trx.items.size(); ++i) {
            file << trx.items[i].item.name << " x" << trx.items[i].quantity
                 << " = Rp " << trx.items[i].item.price * trx.items[i].quantity << endl;
        }
        file << "Total: Rp " << trx.total
             << ", Bayar: Rp " << trx.paid
             << ", Kembali: Rp " << trx.change
             << ", Metode: " << trx.paymentMethod << endl;
        file << "------------------------------\n";
        file.close();
    }

    void printReceipt(const Transaction& trx) {
        cout << "\n====================================================\n";
        cout << "                STRUK PEMBAYARAN          \n";
        cout << "====================================================\n";
        cout << "Waktu   : " << ctime(&trx.timestamp);
        cout << "Customer: " << trx.costumerName << endl;
        cout << left << setw(20) << "Item"
             << setw(10) << "Qty"
             << setw(15) << "Harga"
             << setw(15) << "Subtotal" << endl;
        cout << "----------------------------------------------------\n";

        for (const auto& item : trx.items) {
            double subtotal = item.item.price * item.quantity;
            cout << left << setw(20) << item.item.name
                 << setw(10) << item.quantity
                 << "Rp " << setw(11) << fixed << setprecision(2) << item.item.price
                 << "Rp " << fixed << setprecision(2) << subtotal << endl;
        }

    cout << "------------------------------------------------------\n";
    cout << left << setw(35) << "Total" << "Rp " << fixed << setprecision(2) << trx.total << endl;
    cout << left << setw(35) << "Dibayar" << "Rp " << fixed << setprecision(2) << trx.paid << endl;
    cout << left << setw(35) << "Kembalian" << "Rp " << fixed << setprecision(2) << trx.change << endl;
    cout << left << setw(35) << "Metode Pembayaran" << trx.paymentMethod << endl;
    cout << "====================================================\n";
    cout << "          Terima Kasih atas kunjungan   \n";
    cout << "====================================================\n";

    ofstream file("struk_coffee_shop.txt", ios::app);
    file << "\n=====================================\n";
    file << "           STRUK PEMBAYARAN          \n";
    file << "=====================================\n";
    file << "Waktu: " << ctime(&trx.timestamp);
    file << left << setw(25) << "Item" << setw(10) << "Qty" << setw(15) << "Subtotal" << endl;
    file << "-------------------------------------\n";

    for (size_t i = 0; i < trx.items.size(); ++i) {
        double subtotal = trx.items[i].item.price * trx.items[i].quantity;
        file << left << setw(25) << trx.items[i].item.name
             << setw(10) << trx.items[i].quantity
             << "Rp " << fixed << setprecision(2) << subtotal << endl;
    }

    file << "-------------------------------------\n";
    file << left << setw(35) << "Total" << "Rp " << fixed << setprecision(2) << trx.total << endl;
    file << left << setw(35) << "Dibayar" << "Rp " << fixed << setprecision(2) << trx.paid << endl;
    file << left << setw(35) << "Kembalian" << "Rp " << fixed << setprecision(2) << trx.change << endl;
    file << left << setw(35) << "Metode Pembayaran" << trx.paymentMethod << endl;
    file << "=====================================\n";
    file << "       Terima Kasih atas kunjungan   \n";
    file << "=====================================\n";
    file.close();
}


    void manageMenu() {
        int pilih;
        do {
            cout << "\n=== Manajemen Menu ===\n"
                 << "1. Tambah Menu\n2. Ubah Harga\n3. Hapus Menu\n4. Kembali\nPilihan: ";
            cin >> pilih;

            if (pilih == 1) {
                string nama;
                double harga;
                int stok;
                cout << "Nama item: ";
                cin.ignore();
                getline(cin, nama);
                cout << "Harga: ";
                cin >> harga;
                cout << "Stok: ";
                cin >> stok;
                addMenuItem(nama, harga, stok);

            } else if (pilih == 2) {
                int id;
                double harga;
                cout << "ID menu: "; cin >> id;
                cout << "Harga baru: "; cin >> harga;

                for (size_t i = 0; i < menu.size(); ++i) {
                    if (menu[i].id == id) {
                        menu[i].price = harga;
                        break;
                    }
                }

            } else if (pilih == 3) {
    int id;
    cout << "ID menu: "; cin >> id;
    bool found = false;

    for (size_t i = 0; i < menu.size(); ++i) {
        if (menu[i].id == id) {
            cout << "Menu \"" << menu[i].name << "\" telah dihapus.\n";
            menu.erase(menu.begin() + i);
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Menu dengan ID tersebut tidak ditemukan.\n";
    }
}

        } while (pilih != 4);
    }

    void loadSampleMenu() {
    addMenuItem("Kopi Susu", 15000, 30);
    addMenuItem("Americano", 20000, 20);
    addMenuItem("Cappuccino", 18000, 25);
    addMenuItem("Kopi Susu Es", 5000, 30);
    addMenuItem("Kopi Saring", 10000, 30);
    addMenuItem("Kopi Susu Panas", 15000, 30);
    addMenuItem("Kopi Tubruk", 10000, 30);
    addMenuItem("Es Teh", 5000, 30);
    addMenuItem("Leci Tea", 15000, 30);
    addMenuItem("Leci Yakult", 15000, 30);
    addMenuItem("Apel Warkop", 10000, 30);
    addMenuItem("Teh Susu", 10000, 30);
    addMenuItem("Mangga Gembira", 15000, 30);
    addMenuItem("Melon Susu", 15000, 30);
    addMenuItem("Kopi Tira", 15000, 30);
    addMenuItem("Supon", 10000, 30);
    addMenuItem("Cappuccino", 18000, 30);
    addMenuItem("Matcha Latte", 20000, 30);
    addMenuItem("Red valvet", 18000, 30);
    addMenuItem("Americano", 15000, 30);
    addMenuItem("Vanilla Latte", 20000, 30);
    addMenuItem("Caramel Macchiato", 22000, 30);
    addMenuItem("Hazelnut Coffee", 21000, 30);
    addMenuItem("Choco Banana", 18000, 30);
    addMenuItem("Strawberry Milk", 17000, 30);
    addMenuItem("Espresso", 12000, 30);
    addMenuItem("Thai Tea", 15000, 30);
    addMenuItem("Roti Bakar Coklat", 12000, 20);
    addMenuItem("Roti Bakar Keju", 13000, 20);
    addMenuItem("Roti Bakar Srikaya", 13000, 20);
    addMenuItem("Indomie Goreng Spesial", 15000, 20);
    addMenuItem("Indomie Kuah Telur", 15000, 20);
    addMenuItem("Dimsum Ayam", 18000, 20);
    addMenuItem("Dimsum Mentai", 23000, 20);
    addMenuItem("Dimsum Udang", 20000, 20);
    addMenuItem("French Fries", 12000, 20);
    addMenuItem("Kentang Goreng Keju", 15000, 20);
    addMenuItem("Mie Goreng Jawa", 17000, 20);
    addMenuItem("Mie Kuah Pedas", 16000, 20);
    addMenuItem("Nasi Goreng Spesial", 20000, 20);
    addMenuItem("Nasi Goreng Seafood", 25000, 20);
    addMenuItem("Nasi Ayam Teriyaki", 22000, 20);
    addMenuItem("Nasi Ayam Katsu", 23000, 20);
    addMenuItem("Sosis Bakar", 10000, 20);
    addMenuItem("Bakso Bakar", 10000, 20);
    addMenuItem("Pisang Coklat Keju", 14000, 20);
    addMenuItem("Pisang Goreng Crispy", 12000, 20);
    addMenuItem("Omelet Keju", 15000, 20);
    addMenuItem("Toast Tuna Mayo", 18000, 20);
    addMenuItem("Pancake Maple", 16000, 20);
    
    }

    void loadSampleBahan() {
    BahanBaku b;
    
    b.nama = "Kopi Giling";
    b.satuanGram = 1000;
    b.hargaTotal = 100000;
    b.jumlah = 1;
    b.hargaPerGram = b.hargaTotal / b.satuanGram;
    stokBahan.push_back(b);

    b.nama = "Kopi Gajah";
    b.satuanGram = 138;
    b.hargaTotal = 14300;
    b.jumlah = 1;
    b.hargaPerGram = 104;
    stokBahan.push_back(b);

    b.nama = "Nescafe";
    b.satuanGram = 100;
    b.hargaTotal = 64000;
    b.jumlah = 1;
    b.hargaPerGram = 640;
    stokBahan.push_back(b);

    b.nama = "Evaporasi";
    b.satuanGram = 380;
    b.hargaTotal = 13500;
    b.jumlah = 1;
    b.hargaPerGram = 36;
    stokBahan.push_back(b);

    b.nama = "SKM Carnation";
    b.satuanGram = 370;
    b.hargaTotal = 13850;
    b.jumlah = 1;
    b.hargaPerGram = 37;
    stokBahan.push_back(b);

    b.nama = "Diamond";
    b.satuanGram = 1000;
    b.hargaTotal = 15500;
    b.jumlah = 1;
    b.hargaPerGram = 15.5;
    stokBahan.push_back(b);

    b.nama = "Teh Poci";
    b.satuanGram = 8;
    b.hargaTotal = 4140;
    b.jumlah = 1;
    b.hargaPerGram = 518;
    stokBahan.push_back(b);

    b.nama = "Roti Prambanan";
    b.satuanGram = 19;
    b.hargaTotal = 14000;
    b.jumlah = 1;
    b.hargaPerGram = 736.8;
    stokBahan.push_back(b);

    b.nama = "Telor";
    b.satuanGram = 1000;
    b.hargaTotal = 28400;
    b.jumlah = 1;
    b.hargaPerGram = 28.4;
    stokBahan.push_back(b);
    
    b.nama = "Susu Full Cream";
    b.satuanGram = 1000;
    b.hargaTotal = 25000;
    b.jumlah = 1;
    b.hargaPerGram = 250;
    stokBahan.push_back(b);
    
    b.nama = "Coklat Bubuk";
    b.satuanGram = 1000;
    b.hargaTotal = 60000;
    b.jumlah = 1;
    b.hargaPerGram = 600;
    stokBahan.push_back(b);

    b.nama = "Selai Coklat";
    b.satuanGram = 250;
    b.hargaTotal = 42550;
    b.jumlah = 1;
    b.hargaPerGram = 170.2;
    stokBahan.push_back(b);
    
    b.nama = "Selai Keju";
    b.satuanGram = 250;
    b.hargaTotal = 50000;
    b.jumlah = 1;
    b.hargaPerGram = 200;
    stokBahan.push_back(b);

    b.nama = "Selai Srikaya";
    b.satuanGram = 170;
    b.hargaTotal = 31000;
    b.jumlah = 1;
    b.hargaPerGram = 182;
    stokBahan.push_back(b);

    b.nama = "Selai Tiramisu";
    b.satuanGram = 250;
    b.hargaTotal = 42550;
    b.jumlah = 1;
    b.hargaPerGram = 170.2;
    stokBahan.push_back(b);

    b.nama = "Indomie";
    b.satuanGram = 1;
    b.hargaTotal = 2990;
    b.jumlah = 1;
    b.hargaPerGram = 2990;
    stokBahan.push_back(b);

    b.nama = "Deenali";
    b.satuanGram = 750;
    b.hargaTotal = 100000;
    b.jumlah = 1;
    b.hargaPerGram = 133;
    stokBahan.push_back(b);

    b.nama = "Marjan";
    b.satuanGram = 450;
    b.hargaTotal = 17000;
    b.jumlah = 1;
    b.hargaPerGram = 38;
    stokBahan.push_back(b);

    b.nama = "Dimsum";
    b.satuanGram = 50;
    b.hargaTotal = 125000;
    b.jumlah = 1;
    b.hargaPerGram = 2500;
    stokBahan.push_back(b);

    b.nama = "Yakult";
    b.satuanGram = 1;
    b.hargaTotal = 2000;
    b.jumlah = 1;
    b.hargaPerGram = 2000;
    stokBahan.push_back(b);

    b.nama = "Es Batu";
    b.satuanGram = 6000;
    b.hargaTotal = 12000;
    b.jumlah = 1;
    b.hargaPerGram = 2;
    stokBahan.push_back(b);

    b.nama = "Gula";
    b.satuanGram = 5000;
    b.hargaTotal = 100000;
    b.jumlah = 1;
    b.hargaPerGram = 20;
    stokBahan.push_back(b);

    b.nama = "Mentega";
    b.satuanGram = 1000;
    b.hargaTotal = 10000;
    b.jumlah = 1;
    b.hargaPerGram = 100;
    stokBahan.push_back(b);

    b.nama = "Cup";
    b.satuanGram = 1;
    b.hargaTotal = 660;
    b.jumlah = 1;
    b.hargaPerGram = 660;
    stokBahan.push_back(b);

    b.nama = "Tutup";
    b.satuanGram = 1;
    b.hargaTotal = 330;
    b.jumlah = 1;
    b.hargaPerGram = 330;
    stokBahan.push_back(b);

    b.nama = "Cup 8oz";
    b.satuanGram = 50;
    b.hargaTotal = 14300;
    b.jumlah = 1;
    b.hargaPerGram = 286;
    stokBahan.push_back(b);

    b.nama = "Tutup 8oz";
    b.satuanGram = 50;
    b.hargaTotal = 7500;
    b.jumlah = 1;
    b.hargaPerGram = 150;
    stokBahan.push_back(b);

    b.nama = "Sedotan";
    b.satuanGram = 500;
    b.hargaTotal = 13300;
    b.jumlah = 1;
    b.hargaPerGram = 26.6;
    stokBahan.push_back(b);

    b.nama = "Sprite";
    b.satuanGram = 1000;
    b.hargaTotal = 10000;
    b.jumlah = 1;
    b.hargaPerGram = 10;
    stokBahan.push_back(b);

    b.nama = "Powder Cappucino";
    b.satuanGram = 1000;
    b.hargaTotal = 130000;
    b.jumlah = 1;
    b.hargaPerGram = 130;
    stokBahan.push_back(b);

    b.nama = "Butterscotch";
    b.satuanGram = 750;
    b.hargaTotal = 75000;
    b.jumlah = 1;
    b.hargaPerGram = 100;
    stokBahan.push_back(b);

    b.nama = "Powder Klepon";
    b.satuanGram = 1000;
    b.hargaTotal = 110000;
    b.jumlah = 1;
    b.hargaPerGram = 110;
    stokBahan.push_back(b);

    b.nama = "Gula Aren";
    b.satuanGram = 1000;
    b.hargaTotal = 50000;
    b.jumlah = 1;
    b.hargaPerGram = 50;
    stokBahan.push_back(b);
    
    b.nama = "Sosis";
    b.satuanGram = 500;
    b.hargaTotal = 35000;
    b.jumlah = 1;
    b.hargaPerGram = 70;
    stokBahan.push_back(b);
    
    b.nama = "Bakso";
    b.satuanGram = 500;
    b.hargaTotal = 25000;
    b.jumlah = 1;
    b.hargaPerGram = 50;
    stokBahan.push_back(b);
    
    b.nama = "Pisang";
    b.satuanGram = 1000;
    b.hargaTotal = 20000;
    b.jumlah = 1;
    b.hargaPerGram = 200;
    stokBahan.push_back(b);
    
    b.nama = "Daging Ayam";
    b.satuanGram = 1000;
    b.hargaTotal = 30000;
    b.jumlah = 1;
    b.hargaPerGram = 300;
    stokBahan.push_back(b);
    
    b.nama = "Daging Sapi";
    b.satuanGram = 1000;
    b.hargaTotal = 130000;
    b.jumlah = 1;
    b.hargaPerGram = 1300;
    stokBahan.push_back(b);
    
    b.nama = "Udang";
    b.satuanGram = 1000;
    b.hargaTotal = 50000;
    b.jumlah = 1;
    b.hargaPerGram = 500;
    stokBahan.push_back(b);
    
    b.nama = "Kerang";
    b.satuanGram = 1000;
    b.hargaTotal = 30000;
    b.jumlah = 1;
    b.hargaPerGram = 300;
    stokBahan.push_back(b);
    
    b.nama = "Ikan Tuna";
    b.satuanGram = 1500;
    b.hargaTotal = 200000;
    b.jumlah = 1;
    b.hargaPerGram = 1300;
    stokBahan.push_back(b);
    
    b.nama = "Mayones";
    b.satuanGram = 500;
    b.hargaTotal = 15000;
    b.jumlah = 1;
    b.hargaPerGram = 30;
    stokBahan.push_back(b);
    
    b.nama = "Saus Pedas";
    b.satuanGram = 1000;
    b.hargaTotal = 25000;
    b.jumlah = 1;
    b.hargaPerGram = 250;
    stokBahan.push_back(b);
    
    b.nama = "Saus Tomat";
    b.satuanGram = 1000;
    b.hargaTotal = 25000;
    b.jumlah = 1;
    b.hargaPerGram = 250;
    stokBahan.push_back(b);
    
    b.nama = "Kecap Manis";
    b.satuanGram = 500;
    b.hargaTotal = 20000;
    b.jumlah = 1;
    b.hargaPerGram = 40;
    stokBahan.push_back(b);
    
    b.nama = "Kecap Asin";
    b.satuanGram = 500;
    b.hargaTotal = 1500;
    b.jumlah = 1;
    b.hargaPerGram = 30;
    stokBahan.push_back(b);
    
    b.nama = "Kecap Inggris";
    b.satuanGram = 600;
    b.hargaTotal = 25000;
    b.jumlah = 1;
    b.hargaPerGram = 41;
    stokBahan.push_back(b);
    
    b.nama = "Kentang";
    b.satuanGram = 1000;
    b.hargaTotal = 15000;
    b.jumlah = 1;
    b.hargaPerGram = 150;
    stokBahan.push_back(b);
    
    b.nama = "Vanilla Essence";
    b.satuanGram = 30;
    b.hargaTotal = 20000;
    b.jumlah = 1;
    b.hargaPerGram = 666;
    stokBahan.push_back(b);
    
    b.nama = "Leci Essence";
    b.satuanGram = 30;
    b.hargaTotal = 10000;
    b.jumlah = 1;
    b.hargaPerGram = 333;
    stokBahan.push_back(b);
    
    b.nama = "Strawberry Essence";
    b.satuanGram = 30;
    b.hargaTotal = 25000;
    b.jumlah = 1;
    b.hargaPerGram = 833;
    stokBahan.push_back(b);
    
    b.nama = "Apel Essence";
    b.satuanGram = 50;
    b.hargaTotal = 30000;
    b.jumlah = 1;
    b.hargaPerGram = 600;
    stokBahan.push_back(b);
    
    b.nama = "Mangga Essence";
    b.satuanGram = 30;
    b.hargaTotal = 20000;
    b.jumlah = 1;
    b.hargaPerGram = 666;
    stokBahan.push_back(b);
    
    b.nama = "Melon Essence";
    b.satuanGram = 30;
    b.hargaTotal = 45000;
    b.jumlah = 1;
    b.hargaPerGram = 1500;
    stokBahan.push_back(b);
    
    b.nama = "Matcha Bubuk";
    b.satuanGram = 100;
    b.hargaTotal = 100000;
    b.jumlah = 1;
    b.hargaPerGram = 1000;
    stokBahan.push_back(b);
    
}


    void tampilkanBahan() {
        cout << "\n=== Stok Bahan ===\n";
        cout << left << setw(20) << "Nama" << setw(10) << "Gram" << setw(15)
             << "Harga Total" << setw(10) << "Jumlah" << setw(15) << "Harga/Gram" << endl;
        for (size_t i = 0; i < stokBahan.size(); ++i) {
            cout << left << setw(20) << stokBahan[i].nama
                 << setw(10) << stokBahan[i].satuanGram
                 << setw(15) << stokBahan[i].hargaTotal
                 << setw(10) << stokBahan[i].jumlah
                 << setw(15) << stokBahan[i].hargaPerGram << endl;
        }
    }

    void cekStokBahan() {
        tampilkanBahan();
        cout << "\n=== PERINGATAN STOK MENIPIS ===\n";
        for (size_t i = 0; i < stokBahan.size(); ++i) {
            if (stokBahan[i].satuanGram < 100) {
                cout << "Stok bahan \"" << stokBahan[i].nama << "\" hanya "
                     << stokBahan[i].satuanGram << " gram\n";
            }
        }
    }

    void mainMenu() {
        int choice;
        do {
            cout << "\n=== COFFEE SHOP SYSTEM ===\n";
            cout << "1. Tampilkan Menu\n2. Pesan\n3. Manajemen Menu\n4. Cek Stok\n5. Keluar\nPilihan: ";
            cin >> choice;
            switch (choice) {
                case 1: displayMenu(); break;
                case 2: takeOrder(); break;
                case 3: manageMenu(); break;
                case 4: cekStokBahan(); break;
            }
        } while (choice != 5);
    }
};

int main() {
    CoffeeShop app;
    app.mainMenu();
    return 0;
}

