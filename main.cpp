#include <iostream>
#include <forward_list>
#include <vector>
#include <numeric>
#include <chrono>
#include <functional>

using namespace std;
using namespace std::chrono;


// The obvious direct attack, as a baseline comparison.
vector<unsigned long> simplest(unsigned long count)
{
    // Init a ranged vector of integers 1 through count
    vector<unsigned long> l(count);
    iota(l.begin(), l.end(), 1);

    // For each integer n greater than 1 and less or equal to the count
    for (auto n = 2ul; n <= count; ++n) {

        // Remove the value from the vector if it's a multiple of n
        auto v = l.begin();
        while (v != l.end()) {
            if ((*v > n) && (*v % n == 0)) {
                v = l.erase(v);
            } else {
                ++v;
            }
        }
    }

    return l;
}


// A simplified version of the first-try, instead using a forward_list and remove_if() to drop the unwanted values.
//  This one turns out to be slower.
forward_list<unsigned long> using_remove_if(unsigned long count)
{
    // Init a ranged list of integers 1 through count
    forward_list<unsigned long> l(count);
    iota(l.begin(), l.end(), 1);

    // For each integer n greater than 1 and less or equal to the count
    for (auto n = 2ul; n <= count; ++n) {

        // Remove the value from the list if it's a multiple of n
        l.remove_if([&n] (const unsigned long& value) {
            return (value > n) && (value % n == 0);
        });
    }

    return l;
}


// Using a flag and copy strategy, we avoid full-list search loops and
// mod operators in favor of incrementing an iterator.
// In addition, there's some other optimizations that aren't applied above:
//  - Only look for multiples of n that are greater than n (seems obvious...).
//  - only bother culling multiples of n up to count/2, since any integer
//    above that value is a multiple of 2 and an integer lower than that value
//    (both of which will have already been culled).
//  - skip trying to cull multiples of integers that are already themselves
//    flagged as not being prime (since one of their factors has already been
//    tested, and therefore all its multiples are already flagged).
vector<unsigned long> optimal_case(unsigned long count)
{
    // Initialize a vector of booleans, where true is a flag meaning that offset is prime.
    //  We'll increase the size by 1 since we're using iterator offsets that are zero-indexed.
    vector<bool> flags(count + 1, true);

    // For each integer n greater than 1 and less than half the list length
    for (auto n = 2ul; n <= (flags.size() / 2); ++n) {

        // Start at the element in question: (n)
        auto i = flags.begin();
        advance(i, n);

        // If n is already marked as not-prime, don't bother searching for its multiples
        //  (they're already marked)
        if (*i == false) {
            continue;
        }

        // If n is still thought to be prime, advance to it's first multiple
        advance(i, n);

        // And flag all the following multiples of n as not-prime
        while (i < flags.end()) {
            *i = false;
            advance(i, n);
        }
    }

    // Now that we have a set of boolean flags indicating which offsets are prime,
    // copy the offsets into the output array (skipping the first value 0).
    // We'll reserve enough memory to start with for a prime density of 50%.
    vector<unsigned long> l;
    l.reserve(count/2);
    for (auto i = flags.begin(); i != flags.end(); ++i) {
        if (i != flags.begin() && *i == true) {
            l.push_back(distance(flags.begin(), i));
        }
    }

    return l;
}

// Evaluate the passed test function, and print out the resulting collection and the
//  time the function took to execute.
template <typename X>
void time_and_print_result(function<X()> test_function)
{
    // Execute the function under test, keeping track of the start/stop times
    auto t0 = high_resolution_clock::now();
    auto result = test_function();
    auto t1 = high_resolution_clock::now();

    // Print the execution time
    if (duration_cast<milliseconds>(t1 - t0).count() > 0) {
        cout << "  executed in " << duration_cast<milliseconds>(t1 - t0).count() << " ms" << endl;
    } else {
        cout << "  executed in " << duration_cast<nanoseconds>(t1 - t0).count() << " ns" << endl;
    }

    // Print the set size (for testing)
    cout << "  result size: " << distance(result.begin(), result.end()) << " primes" << endl;

    // Print the resulting set of primes
    cout << "  ";
    for (auto i = result.begin(); i != result.end(); ++i) {
        if (i != result.begin()) {
            cout << ", ";
        }
        cout << *i;
    }
    cout << endl;
}

int main()
{
    const unsigned long count {50000};

    cout << endl << "Find all the primes less than " << count << endl;

    cout << endl << "Simplest first try:" << endl;
    time_and_print_result<vector<unsigned long>>([count] () {
        return simplest(count);
    });

    cout << endl << "Revised version with remove_if():" << endl;
    time_and_print_result<forward_list<unsigned long>>([count] () {
        return using_remove_if(count);
    });

    cout << endl << "Optimal version using iterator arithmetic and flag/copy:" << endl;
    time_and_print_result<vector<unsigned long>>([count] () {
        return optimal_case(count);
    });

    return 0;
}

