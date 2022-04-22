#include <iostream>
#include <map>

using namespace std;
#define DEBUG_MEMORY

void bufferoverrun() {
    int *p = new int[10];
    p[-1] = 10; //Buffer overrun to the negative side
    p[10] = 10; //Bufferx overrun to the positive side
    delete[] p;
}

void memoryleak() {
    int *p = new int[100];

    //No call to delete, so this pointer is a memory leak
}

void invaliddelete_1() {
    int *p;
    delete p;
}

void invaliddelete_2() {
    int *p = new int;
    delete p;

    //invalid delete
    delete p;
}
bool track_new = false;
bool track_delete = false;
map<size_t, int> pointerTrackerState;
map<size_t, size_t> pointerSizeTracker;

void resetState() {
    #ifndef DEBUG_MEMORY
        cout << "This is debug code only " << endl;
    #else
        pointerTrackerState.clear();
        track_new = true;
        track_delete = true;
    #endif
}

void checkForErrors() {
    #ifndef DEBUG_MEMORY
        cout << "This is debug code only " << endl;
    #else
        for(const auto& elem : pointerTrackerState) {
            if (elem.second == 0) {
                cout << "Success! This method has no invalid deletes or memory leaks" << endl;
            } else if (elem.second > 0) {
                cout << "Memory leak! You have not deleted all your memory." << endl;
            } else if (elem.second < 0) {
                cout << "Invalid delete. You have deleted something that doesn't exist." << endl;
            }
        }
    #endif
}


int main() {
    cout << "Start invaliddelete_1()" << endl;
    resetState();
    invaliddelete_1();
    checkForErrors();
    resetState();
    cout << "Start invaliddelete_2()" << endl;
    invaliddelete_2();
    checkForErrors();
    resetState();
    cout << "Start memoryLeak()" << endl;
    memoryleak();
    checkForErrors();
    cout << "Start bufferoverrun()" << endl;
    resetState();
    bufferoverrun();
    checkForErrors();
}
 
// overload global new and global delete
void * operator new(std::size_t n) //throw(std::bad_alloc)
{
    #ifndef DEBUG_MEMORY
    void *new_ptr = malloc(n);
    return new_ptr;
    #else
    void *new_ptr = malloc(n);
    if(track_new) {
        track_new = false;
        if (pointerTrackerState.count(n) > 0) {
            cout << "Error ? " << n << endl;
        } else {
            pointerTrackerState.insert(pair<size_t, int>(size_t(new_ptr), 1));
        }
    }
    return new_ptr;
    #endif
}

void operator delete(void * p) throw()
{
    #ifndef DEBUG_MEMORY
    free(p);
#else
    if (pointerTrackerState.count(size_t(p)) > 0) {
        pointerTrackerState[size_t(p)] = pointerTrackerState[size_t(p)] - 1;
        if (pointerTrackerState[size_t(p)] == 0) {
            free(p);
        }
        
    } else if (track_delete) {
        track_new = false;
        pointerTrackerState.insert(pair<size_t, int>(size_t(p), -1));
        track_delete = false;
    } else {
        free(p);
    }
#endif
}

// over load Global new[] and global delete[]
void * operator new[](size_t sz) {
#ifndef DEBUG_MEMORY
    return malloc(sz)
#else
    if(track_new) {
        char *m = (char *)malloc(sz + 8);
        track_new = false;
        if (pointerTrackerState.count(sz) > 0) {
            cout << "Error ? " << sz << endl;
        } else {
            pointerTrackerState.insert(pair<size_t, int>(size_t(m), 1));
        }
        pointerSizeTracker.insert(pair<size_t, size_t>(size_t(m), sz));
        for (int i = 0; i < 4; i++) {
            *m = '0';
            m = m++;
        }
        for (size_t i = sz; i < sz + 4; i++) {
            *(m+i) = '0';
        }
        return m;
    } else {
        return  malloc(sz);
    }
#endif
}
void operator delete[] (void * p) noexcept {
#ifndef DEBUG_MEMORY
    free(p);
#else
    if (track_delete) {
        char * ptr = (char *) p;
        ptr = ptr--;
        for (int i = 0; i < 4; i++) {
            if (*ptr != '0') {
                cout << "Buffer Overrun detected!" << endl;
            }
            ptr = ptr--;
        }
        size_t sz = pointerSizeTracker[size_t(p)];
        for (size_t i = sz; i < sz+4; i++) {
            if (*(ptr+i) != '0') {
                cout << "Buffer Overrun detected!" << endl;
            }
        }
    }
    if (pointerTrackerState.count(size_t(p)) > 0) {
        pointerTrackerState[size_t(p)] = pointerTrackerState[size_t(p)] - 1;
        if (pointerTrackerState[size_t(p)] == 0) {
            free(p);
        }
        
    } else if (track_delete) {
        track_new = false;
        pointerTrackerState.insert(pair<size_t, int>(size_t(p), -1));
        track_delete = false;
    } else {
        free(p);
    }
#endif
}
