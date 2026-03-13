#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

template <typename T>
class Matrix{
    public:
        Matrix(std::vector<std::vector<T>> data) : 
            m_matrix(std::move(data)), 
            m_m(m_matrix.size()),
            m_n(m_matrix.empty() ? 0 : m_matrix[0].size()) {
            
            // Check if matrix rows are consistant
            for (const auto& row : m_matrix) {
                if (row.size() != m_n) {
                    throw std::runtime_error("Matrix rows have inconsistent sizes");
                }
            }
        }
        ~Matrix() = default;

        // Multiply operator overloading
        Matrix operator*(const Matrix& other) const{
            // Check if multiplable
            if (m_n != other.m_m) {
                std::ostringstream oss;
                oss << "Matrix multiplication not possible\n";
                oss << "Left matrix:\n" << *this << "\n";
                oss << "Right matrix:\n" << other;
                throw std::runtime_error(oss.str());
            }
            
            std::vector<std::vector<T>> matrix_data(
                m_m,
                std::vector<T>(other.m_n, 0)
            );

            auto threads = std::max(1u, std::thread::hardware_concurrency());
            boost::asio::thread_pool pool(threads);

            for(size_t  i=0; i< m_m; i++){
                boost::asio::post(pool, [&, i]() {
                    for (size_t j = 0; j < other.m_n; ++j) {
                        T sum{};
                        for (size_t k = 0; k < m_n; ++k) {
                            sum += m_matrix[i][k] * other.m_matrix[k][j];
                        }
                        result[i][j] = sum;
                    }
                });
            }
            pool.join();
            return Matrix(std::move(matrix_data), m_m, other.m_n);
        }

        friend std::ostream& operator<<(std::ostream& os, const Matrix& matrix){
            for (const auto& row : matrix.m_matrix) {
                os << "[ ";
                for (const auto& n : row) {
                    os << n << " ";
                }
                os << "]\n";
            }
            os << "( " << matrix.m_m << " x " << matrix.m_n << " )";
            return os;
        }

    private:
        std::vector<std::vector<T>> m_matrix;
        std::mutex m_mutex;
        size_t m_n;
        size_t m_m;
};

int main(){

    Matrix<int> m1({{1,2,3}, {1,2,3}, {1,2,3}}, 3, 3);
    Matrix<int> m2({{1,2,3}, {1,2,3}, {1,2,3}}, 3, 3);
    Matrix<int> m3 = m1*m2;
    
    std::cout<< m3 <<std::endl;
    return 0;
}