/* a ^ k mod n */
long long pow_mod(long long a, long long k, long long n)
{
    long long b = 1;
    while (k > 0) {
        if (k % 2 == 0) {
            k /= 2;
            a = (a * a) % n;
        }
        else {
            k--;
            b = (b * a) % n;
        }
    }
    return b;
}
