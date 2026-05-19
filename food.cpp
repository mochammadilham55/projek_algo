#include <iostream>
#include <string>
#include <queue>
using namespace std;

struct Food {       // untuk menyimpan data makanan
    string nama;
    int porsi;      // Value dalam knapsack
    int berat;      // Weight dalam knapsack
    int expiry;     // Prioritas (semakin kecil, semakin urgent)
};



