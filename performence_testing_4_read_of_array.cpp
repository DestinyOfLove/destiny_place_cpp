#include <iostream>
#include <chrono>

class TwoDArrayPerformance {
public:
    TwoDArrayPerformance(int rows, int cols)
            : rows_(rows), cols_(cols) {
        data_ = new int *[rows_];
        for (int i = 0; i < rows_; ++i)
            data_[i] = new int[cols_];
    }

    ~TwoDArrayPerformance() {
        for (int i = 0; i < rows_; ++i)
            delete[] data_[i];
        delete[] data_;
    }

    // 先行后列读
    double ReadRowFirst() {
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < rows_; i++) {
            for (int j = 0; j < cols_; j++) {
                int temp = data_[i][j];
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        return GetElapsedTime(start_time, end_time);
    }

    // 先列后行读
    double ReadColFirst() {
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < cols_; j++) {
            for (int i = 0; i < rows_; i++) {
                int temp = data_[i][j];
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        return GetElapsedTime(start_time, end_time);
    }

// 先行后列写
    double WriteRowFirst() {
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < rows_; i++) {
            for (int j = 0; j < cols_; j++) {
                data_[i][j] = i + j;
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        return GetElapsedTime(start_time, end_time);
    }

// 先列后行写
    double WriteColFirst() {
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < cols_; j++) {
            for (int i = 0; i < rows_; i++) {
                data_[i][j] = i + j;
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        return GetElapsedTime(start_time, end_time);
    }

private:
    int **data_;
    int rows_;
    int cols_;

    static double GetElapsedTime(std::chrono::high_resolution_clock::time_point start_time,
                                 std::chrono::high_resolution_clock::time_point end_time) {
        auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        return diff.count() / 1000000.0; // 返回毫秒单位
    }
};


int main()
{
    const int ROWS = 10000;
    const int COLS = 10000;

    std::cout << "Testing 2D array with " << ROWS << " rows and " << COLS << " cols." << std::endl;

    TwoDArrayPerformance performance(ROWS, COLS);

    double elapsed_time_row_first_read = performance.ReadRowFirst();
    double elapsed_time_col_first_read = performance.ReadColFirst();

    double elapsed_time_row_first_write = performance.WriteRowFirst();
    double elapsed_time_col_first_write = performance.WriteColFirst();

    std::cout << "Elapsed time for reading using row first: " << elapsed_time_row_first_read << "ms" << std::endl;
    std::cout << "Elapsed time for reading using col first: " << elapsed_time_col_first_read << "ms" << std::endl;
    std::cout << "Elapsed time for writing using row first: " << elapsed_time_row_first_write << "ms" << std::endl;
    std::cout << "Elapsed time for writing using col first: " << elapsed_time_col_first_write << "ms" << std::endl;

    return 0;
}
