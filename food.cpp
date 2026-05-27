#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

// ============================================
// STRUKTUR DATA
// ============================================

// Data untuk setiap jenis makanan
struct Makanan {
    string nama;
    int porsi;    // jumlah porsi
    int berat;    // berat dalam gram
    int expire;   // waktu kedaluwarsa dalam jam
    int stok;     // jumlah yang tersedia
};

// Koordinat posisi di peta (baris, kolom)
struct Posisi {
    int baris, kolom;
};

// ============================================
// KONFIGURASI PETA
// ============================================

const int BARIS = 27;
const int KOLOM = 39;

// Peta perumahan: '#' = dinding/blok rumah, '.' = jalan
string peta[BARIS] = {
    "#######################################", // 0
    "#.....................................#", // 1  Jalan utama atas
    "#.##.##.##.##.##.##.##.##.##.##.##.##.#", // 2
    "#.##.##.##.##.##.##.##.##.##.##.##.##.#", // 3
    "#.##.##.##.##.##.##.##.##.##.##.##.##.#", // 4
    "#.##.##.##.##.##.##.##.##.##.##.##.##.#", // 5
    "#.##.##.##.##.##.##.##.##.##.##.##.##.#", // 6
    "#.....................................#", // 7  Jalan tengah
    "#.#################.#################.#", // 8
    "#.#################.#################.#", // 9
    "#.....................................#", // 10
    "#.#################.#################.#", // 11
    "#.#################.#################.#", // 12
    "#.....................................#", // 13
    "#.#################.#################.#", // 14
    "#.#################.#################.#", // 15
    "#.....................................#", // 16
    "#.#################.#################.#", // 17
    "#.#################.#################.#", // 18
    "#.....................................#", // 19
    "#.#################.#################.#", // 20
    "#.#################.#################.#", // 21
    "#.....................................#", // 22
    "#.#################.#################.#", // 23
    "#.#################.#################.#", // 24
    "#.....................................#", // 25 Jalan utama bawah
    "#######################################"  // 26
};

// ============================================
// VARIABEL GLOBAL
// ============================================

vector<Makanan> daftarMakanan;
int kapasitasTas = 0;
Posisi posisiAwal;
vector<Posisi> daftarTujuan;

// ============================================
// FUNGSI PEMBANTU
// ============================================

// Cek apakah posisi (r, c) adalah jalan yang bisa dilewati
bool adalahJalan(int r, int c) {
    return r >= 0 && r < BARIS &&
           c >= 0 && c < KOLOM &&
           peta[r][c] == '.';
}

// ============================================
// 1. SORTING - Urutkan makanan berdasarkan expire (Bubble Sort)
// ============================================

void urutkanMakanan() {
    int n = daftarMakanan.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (daftarMakanan[j].expire > daftarMakanan[j+1].expire) {
                swap(daftarMakanan[j], daftarMakanan[j+1]);
            }
        }
    }
    cout << "\nMakanan diurutkan dari yang paling cepat kedaluwarsa.\n";
}

// ============================================
// 2. KNAPSACK - Pilih makanan terbaik untuk tas kurir
// ============================================

void optimasiTas() {
    // Buat daftar item individual (stok > 1 berarti item duplikat)
    vector<Makanan> items;
    for (size_t i = 0; i < daftarMakanan.size(); i++) {
        for (int s = 0; s < daftarMakanan[i].stok; s++) {
            items.push_back(daftarMakanan[i]);
        }
    }

    int n = items.size();

    // Tabel DP: dp[i][w] = porsi maksimal dari i item pertama dengan kapasitas w
    vector<vector<int> > dp(n + 1, vector<int>(kapasitasTas + 1, 0));

    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= kapasitasTas; w++) {
            // Pilih: ambil item ini atau tidak?
            if (items[i-1].berat <= w)
                dp[i][w] = max(dp[i-1][w],
                               dp[i-1][w - items[i-1].berat] + items[i-1].porsi);
            else
                dp[i][w] = dp[i-1][w];
        }
    }

    // Tampilkan hasil
    cout << "\n--- HASIL OPTIMASI TAS ---\n";
    cout << "Total porsi maksimal: " << dp[n][kapasitasTas] << " porsi\n";
    cout << "Makanan yang dibawa:\n";

    int sisa = kapasitasTas;
    for (int i = n; i > 0 && sisa > 0; i--) {
        if (dp[i][sisa] != dp[i-1][sisa]) {
            cout <<  items[i-1].nama << " (" << items[i-1].berat << " gram)\n";
            sisa -= items[i-1].berat;
        }
    }
    cout << "Sisa kapasitas tas: " << sisa << " gram\n";
}

// ============================================
// 3. BFS - Cari jarak terpendek antara dua titik di peta
// ============================================

int hitungJarak(Posisi asal, Posisi tujuan) {
    if (asal.baris == tujuan.baris && asal.kolom == tujuan.kolom) return 0;

    // Arah gerak: atas, bawah, kiri, kanan
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    vector<vector<bool> > sudahDikunjungi(BARIS, vector<bool>(KOLOM, false));
    queue<pair<Posisi, int> > antrian; // {posisi, jarak}

    // Perbaikan: Gaya inisialisasi push yang kompatibel lama
    antrian.push(make_pair(asal, 0));
    sudahDikunjungi[asal.baris][asal.kolom] = true;

    while (!antrian.empty()) {
        // Perbaikan: Hindari structured binding C++17
        pair<Posisi, int> depan = antrian.front();
        Posisi pos = depan.first;
        int jarak = depan.second;
        antrian.pop();

        if (pos.baris == tujuan.baris && pos.kolom == tujuan.kolom)
            return jarak;

        for (int i = 0; i < 4; i++) {
            int nr = pos.baris + dr[i];
            int nc = pos.kolom + dc[i];

            if (adalahJalan(nr, nc) && !sudahDikunjungi[nr][nc]) {
                sudahDikunjungi[nr][nc] = true;
                
                // Perbaikan: Menggunakan kombinasi struct konseptual & make_pair
                Posisi tetangga;
                tetangga.baris = nr;
                tetangga.kolom = nc;
                antrian.push(make_pair(tetangga, jarak + 1));
            }
        }
    }

    return -1; // Tidak ada jalur ditemukan
}

// ============================================
// 4. RUTE - Tampilkan urutan distribusi dari yang terdekat
// ============================================

struct Tujuan { Posisi pos; int jarak; };

// Helper fungsi comparator untuk sort rute karena compiler lama kadang menolak Lambda
bool bandingkanJarak(const Tujuan& a, const Tujuan& b) {
    return a.jarak < b.jarak;
}

void tampilkanRute() {
    vector<Tujuan> rute;

    for (size_t i = 0; i < daftarTujuan.size(); i++) {
        int j = hitungJarak(posisiAwal, daftarTujuan[i]);
        Tujuan temp;
        temp.pos = daftarTujuan[i];
        temp.jarak = j;
        rute.push_back(temp);
    }

    // Menggunakan fungsi pembanding terpisah agar aman di versi C++ manapun
    sort(rute.begin(), rute.end(), bandingkanJarak);

    // Tampilkan hasil rute
    cout << "\n--- URUTAN RUTE DISTRIBUSI ---\n";
    cout << "Start (" << posisiAwal.baris << "," << posisiAwal.kolom << ")";

    for (size_t i = 0; i < rute.size(); i++) {
        cout << " -> (" << rute[i].pos.baris << "," << rute[i].pos.kolom << ")";
        if (rute[i].jarak == -1)
            cout << " [TIDAK TERJANGKAU]";
        else
            cout << " [" << rute[i].jarak << " langkah]";
    }
    cout << " -> Selesai!\n";
}

// ============================================
// FUNGSI INPUT
// ============================================

void inputMakanan() {
    int n;
    cout << "Jumlah jenis makanan: ";
    cin >> n;

    for (int i = 0; i < n; i++) {
        Makanan m;
        cout << "\nMakanan ke-" << (i+1) << "\n";
        cout << "   Nama        : "; cin >> m.nama;
        cout << "   Porsi       : "; cin >> m.porsi;
        cout << "   Berat (gram): "; cin >> m.berat;
        cout << "   Expire (jam): "; cin >> m.expire;
        cout << "   Stok        : "; cin >> m.stok;
        daftarMakanan.push_back(m);
    }
}

void inputPosisiAwal() {
    cout << "\n=== POSISI AWAL KURIR ===\n";
    while (true) {
        cout << "Koordinat (baris kolom): ";
        cin >> posisiAwal.baris >> posisiAwal.kolom;
        if (adalahJalan(posisiAwal.baris, posisiAwal.kolom)) break;
        cout << "Bukan jalan! Coba lagi.\n";
    }
}

void inputTujuan() {
    int n;
    cout << "\nJumlah lokasi tujuan: ";
    cin >> n;

    for (int i = 0; i < n; i++) {
        Posisi p;
        cout << "Tujuan ke-" << (i+1) << " (baris kolom): ";
        cin >> p.baris >> p.kolom;
        if (adalahJalan(p.baris, p.kolom)) {
            daftarTujuan.push_back(p);
        } else {
            cout << "Bukan jalan! Ulangi.\n";
            i--;
        }
    }
}

// ============================================
// MAIN
// ============================================

int main() {
    cout << "=========================================\n";
    cout << "         SMART FOOD RESCUE SYSTEM         \n";
    cout << "=========================================\n";

    // Langkah 1: Input dan sortir makanan
    inputMakanan();
    urutkanMakanan();

    // Langkah 2: Optimasi isi tas dengan Knapsack
    cout << "\nKapasitas tas (gram): ";
    cin >> kapasitasTas;
    optimasiTas();

    // Langkah 3: Input posisi kurir dan tujuan, lalu tampilkan rute
    inputPosisiAwal();
    inputTujuan();
    tampilkanRute();

    cout << "\n=========================================\n";
    cout << "                PROGRAM SELESAI              \n";
    cout << "=========================================\n";
    return 0;
}
