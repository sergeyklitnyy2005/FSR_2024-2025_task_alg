#include <iostream>
#include <vector>
#include <cmath>
#include <gmp.h>
#include <gmpxx.h>

using namespace std;

ostream& operator<<(ostream& os, const mpz_class& value) {
    os << value.get_str();
    return os;
}

istream& operator>>(istream& is, mpz_class& value) {
    string input;
    is >> input;
    value.set_str(input, 10);
    return is;
}

void poly_mod(vector<mpz_class>& poly, const vector<mpz_class>& mod, const mpz_class& n) {
    while (poly.size() >= mod.size()) {
        if (poly.back() != 0) {
            mpz_class coef = poly.back();
            for (size_t i = 0; i < mod.size(); ++i) {
                poly[poly.size() - mod.size() + i] -= coef * mod[i];
                poly[poly.size() - mod.size() + i] %= n;
                if (poly[poly.size() - mod.size() + i] < 0)
                    poly[poly.size() - mod.size() + i] += n;
            }
        }
        poly.pop_back();
    }
}

bool aks(mpz_class n) {
    // Шаг 0
    if (n == 2) return true;

    // Шаг 1
    for (mpz_class a = 2; a <= sqrt(n.get_d()); ++a) {
        for (mpz_class b = 2; b <= log(n.get_d()) / log(a.get_d()); ++b) {
            if (n == pow(a.get_d(), b.get_d())) return false;
        }
    }

    // Шаг 2
    mpz_class r = 2;
    for (mpz_class i = 2; i < n; ++i) {
        if (n % i > pow(log2(n.get_d()), 2)) {
            r = i;
            break;
        }
    }

    // Шаг 3
    for (mpz_class a = 2; a <= r; ++a) {
        mpz_class gcd_result;
        mpz_gcd(gcd_result.get_mpz_t(), a.get_mpz_t(), n.get_mpz_t());
        if (gcd_result != 1) return false;
    }

    // Шаг 4
    vector<mpz_class> x_r_minus_1(r.get_ui() + 1, 0);
    x_r_minus_1[r.get_ui()] = 1;
    x_r_minus_1[0] = -1;

    for (mpz_class a = 1; a <= sqrt(r.get_d()) * log(n.get_d()); ++a) {
        vector<mpz_class> lhs = {1};
        for (mpz_class i = 0; i < n; ++i) {
            vector<mpz_class> temp;
            for (size_t j = 0; j < lhs.size(); ++j) {
                temp.push_back((lhs[j] * a) % n);
            }
            temp.push_back(0);
            lhs = temp;
        }

        vector<mpz_class> rhs(n.get_ui() + 1, 0);
        rhs[0] = a;

        poly_mod(lhs, x_r_minus_1, n);
        poly_mod(rhs, x_r_minus_1, n);

        // cout << "lhs: ";
        // for (const auto& val : lhs) {
        //     cout << val << " ";
        // }
        // cout << endl;

        // cout << "rhs: ";
        // for (const auto& val : rhs) {
        //     cout << val << " ";
        // }
        // cout << endl;

        if (!equal(lhs.begin(), lhs.end(), rhs.begin())) return false;
    }

    return true;
}

int main() {
    mpz_class n;
    cout << "Введите число: ";
    cin >> n;

    if (aks(n)) {
        cout << n << " является простым числом." << endl;
    } else {
        cout << n << " не является простым числом." << endl;
    }

    return 0;
}
